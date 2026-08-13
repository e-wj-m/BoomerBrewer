// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/BarEnemyCharacter.h"
#include "Enemy/BarEnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABarEnemyCharacter::ABarEnemyCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABarEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	DefaultMeshRelativeLocation = GetMesh()->GetRelativeLocation();
	DefaultMeshRelativeRotation = GetMesh()->GetRelativeRotation();
}

void ABarEnemyCharacter::RestoreMeshTransform()
{
	GetMesh()->SetRelativeLocationAndRotation(DefaultMeshRelativeLocation, DefaultMeshRelativeRotation);
}

// Called every frame
void ABarEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABarEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABarEnemyCharacter::KnockDown_Implementation(const FVector& Impulse)
{
	UE_LOG(LogTemp, Warning, TEXT("Knockdown Called On %s with Impulse %s"),
		*GetName(), *Impulse.ToString());

	bIsKnockedDown = true;

	if (ABarEnemyAIController* AICon = Cast<ABarEnemyAIController>(GetController()))
	{
		AICon->ForceReleaseChaseSlot();

		if (UBrainComponent* Brain = AICon->GetBrainComponent())
		{
			Brain->StopLogic(TEXT("KnockedDown"));
		}
	}

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->AddImpulse(Impulse, NAME_None, true);

	if (KnockdownSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, KnockdownSound, GetActorLocation());
	}

	GetWorldTimerManager().SetTimer(RecoveryTimerHandle, this, &ABarEnemyCharacter::Recover, RecoveryDelay, false);
}

void ABarEnemyCharacter::Recover()
{
	const FVector PelvisLocation = GetMesh()->GetBoneLocation(PelvisBoneName);

	const float HalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector TargetLocation = PelvisLocation;

	FHitResult Hit;
	const FVector TraceStart = PelvisLocation + FVector(0.f, 0.f, HalfHeight);
	const FVector TraceEnd = PelvisLocation - FVector(0.f, 0.f, 500.f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()-> LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params))
	{
		TargetLocation = Hit.ImpactPoint + FVector(0.f, 0.f, HalfHeight);
	}

	else
	{
		TargetLocation = PelvisLocation + FVector(0.f, 0.f, HalfHeight);
	}

	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetAllBodiesSimulatePhysics(false);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));

	SetActorLocation(TargetLocation, false, nullptr, ETeleportType::TeleportPhysics);
	GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	GetMesh()->SetRelativeLocationAndRotation(GetMesh()->GetRelativeLocation(), FRotator::ZeroRotator);

	RestoreMeshTransform();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	bIsKnockedDown = false;

	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		if (UBrainComponent* Brain = AICon->GetBrainComponent())
		{
			Brain->RestartLogic();
		}
	}
}