// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BarEnemyCharacter.h"
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
		* GetName(), * Impulse.ToString());

	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		if (UBrainComponent* Brain = AICon->GetBrainComponent())
		{
			Brain->StopLogic(TEXT("Knocked Down"));
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
}