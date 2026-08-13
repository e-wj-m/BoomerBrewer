// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GravityGunGameMode.h"
#include "GUI/GameMenuWidget.h"
#include "HUD/FPSGameHUD.h"
//#include "Kismet/GameplayStatistics.h"


// Sets default values. THIS IS OUR CONSTRUCTOR!
AFPSCharacter::AFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// This handles creating the rules of the Camera and setting it's state in the editor manually. If we didn't have this, we would have to add the camera component in the editor and set up all of its properties manually, which is more time consuming and error prone. By doing it in code, we can ensure that the camera is always set up correctly and consistently across all instances of the character.
	if (!FPSCameraComponent) // This if check quickly ensures we only create a FPSCameraComponent if we DO NOT have one already. This is important because the constructor can be called multiple times in the editor when we make changes to the class, and we don't want to create multiple camera components if we already have one. This is similar to how we would check if a component already exists in Unity before adding it, but with more options for when the component can be created and where it can be created from.
	{
		FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera")); // Very similar to AddComponent/CreateObject/CreateComponent in Unity, but with more options for when the component can be created and where it can be created from
		FPSCameraComponent->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent())); // Similar to setting the parent of a transform in Unity, but with more options for how the attachment works
		FPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight)); // Similar to setting the local position of a transform in Unity, but with more options for how the location is set
		FPSCameraComponent->bUsePawnControlRotation = true; // Similar to setting the "Use Local Rotation" option of a camera in Unity, but with more options for how the rotation is used
	}

	if (!FPSMeshComponent)
	{
		FPSMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
		FPSMeshComponent->SetupAttachment(FPSCameraComponent); // Similar to setting the parent of a transform in Unity, but with more options for how the attachment works
		FPSMeshComponent->SetOnlyOwnerSee(true); // Similar to setting the "Culling Mask" of a camera in Unity, but with more options for how the visibility is set
		FPSMeshComponent->bCastDynamicShadow = false; // Similar to setting the "Cast Shadows" option of a mesh renderer in Unity, but with more options for how the shadows are cast
		FPSMeshComponent->CastShadow = false; // Similar to setting the "Cast Shadows" option of a mesh renderer in Unity, but with more options for how the shadows are cast
	}

	if (!FP_Gun)
	{
		FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
		FP_Gun->SetupAttachment(FPSCameraComponent); // Similar to setting the parent of a transform in Unity, but with more options for how the attachment works
		FPSMeshComponent->SetOnlyOwnerSee(true);
	}

	if (!GravityGunComponent)
	{
		GravityGunComponent = CreateDefaultSubobject<UGravityGunComponent>(TEXT("GravityGunComponent"));
	}

	GetMesh()->SetOwnerNoSee(true); // Similar to setting the "Culling Mask" of a camera in Unity, but with more options for how the visibility is set

	UE_LOG(LogTemp, Warning, TEXT("FPSCharacter Constructor Called"));
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AFPSCharacter::OnPlayerHit);
	
	if(APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if(PlayerMappingContext)
		{
			UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
			if(Subsystem)
			{
				Subsystem->AddMappingContext(PlayerMappingContext, 0);
			}
		}
	}
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GravityGunComponent->TickPull(FPSCameraComponent);
	GravityGunComponent->TickHold(FPSCameraComponent);
}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if(MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Move);
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Look);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AFPSCharacter::StartJump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AFPSCharacter::EndJump);
			EnhancedInputComponent->BindAction(PullAction, ETriggerEvent::Started, this, &AFPSCharacter::StartPull);
			EnhancedInputComponent->BindAction(PullAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Pull);
			EnhancedInputComponent->BindAction(PullAction, ETriggerEvent::Completed, this, &AFPSCharacter::EndPull);
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AFPSCharacter::Fire);
			/*EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AFPSCharacter::EndFire);*/
			/*EnhancedInputComponent->BindAction(PullAction, ETriggerEvent::Triggered, this, &AFPSCharacter::EndPull);*/
		}
	}
}

void AFPSCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	AddMovementInput(GetActorRightVector(), MovementVector.X);
}

void AFPSCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddControllerPitchInput(LookAxisVector.Y);
	AddControllerYawInput(LookAxisVector.X);
}

void AFPSCharacter::StartJump()
{
	bPressedJump = true;
}

void AFPSCharacter::EndJump()
{
	bPressedJump = false;
}

void AFPSCharacter::Fire()
{
	GravityGunComponent->Fire(FPSCameraComponent);

	if (GravityGunComponent->IsHoldingObject()) return;

	// Init relevant infomration for where the projectile will be
	FVector CameraLocation;
	FRotator CameraRotation;
	GetActorEyesViewPoint(CameraLocation, CameraRotation);

	if (!ProjectileClass) return;
	MuzzleOffset.Set(100.0f, 0.0f, 0.0f);
	FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);

	UWorld* World = GetWorld();
	if (!World) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	AFPSProjectile* Projectile = World->SpawnActor<AFPSProjectile>(ProjectileClass, MuzzleLocation, CameraRotation, SpawnParams);
	if (!Projectile) return;

	Projectile->FireInDirection(CameraRotation.Vector());

	FRotator MuzzleRotation = CameraRotation;

	// GRAVITY GUN - Try to play an Animation for the firing action.
	//if (FireAnimation != NULL)
	//{
	//	// Get the animation object for the arms mesh
	//	UAnimInstance* AnimInstance = FPSMeshComponent->GetAnimInstance();

	//	if (AnimInstance != NULL)
	//	{
	//		AnimInstance->Montage_Play(FireAnimation, 1.f);
	//	}
	//}
}

void AFPSCharacter::StartPull()
{
	GravityGunComponent->OnPullPressed();
}

void AFPSCharacter::Pull()
{
	GravityGunComponent->Pull(FPSCameraComponent);
}

void AFPSCharacter::EndPull()
{
	GravityGunComponent->OnPullReleased();
}



void AFPSCharacter::OnPlayerHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this) return;
	if (!OtherActor->ActorHasTag("Enemy")) return;

	const float Now = GetWorld()->GetTimeSeconds();

	if (Now - LastDamageTime < DamageCooldown) return;
	LastDamageTime = Now;

	OnHurtPlayer(DamagePerHit);
}

void AFPSCharacter::OnHurtPlayer(float DamageAmount)
{
	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);

	if (AGravityGunGameMode* GM = Cast<AGravityGunGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (UGameMenuWidget* Menu = GM->GetGameMenuWidget())
		{
			Menu->UpdatePatienceBar(Health / MaxHealth);
		}
	}

	if (Health <= 0.0f)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			if (AFPSGameHUD* HUD = Cast<AFPSGameHUD>(PC->GetHUD()))
			{
				HUD->ShowGameEndScreen(false);
			}
		}
	}
}

// GRAVITY GUN - End Fire Function for Throwing Objects after the Fire input for Picking Up Objects has Finished.
//void AFPSCharacter::EndFire()
//{
//	GravityGunComponent->ThrowOrRelease();
//}


// GRAVITY GUN - Function for Attaching fired objects to a point in front of the Player. This is called in the Fire function after we have determined that we have hit an object that can be picked up, and we want to attach it to the GrabbedObjectLocation so that it follows the player around until we release it.
//void AFPSCharacter::SetGrabbedObject(UPrimitiveComponent* ObjectToGrab)
//{
//	GrabbedObject = ObjectToGrab;
//
//	if (GrabbedObject)
//	{
//		if (!GrabbedObject->IsSimulatingPhysics()) return;
//		GrabbedObject->SetEnableGravity(false);
//
//		GrabbedObject->SetLinearDamping(5.0f);
//	}
//}



//void AFPSCharacter::EndPull()
//{
//
//}