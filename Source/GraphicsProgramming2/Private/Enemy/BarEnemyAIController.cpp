// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/BarEnemyAIController.h"
#include "Enemy/BarEnemyCharacter.h"
#include "Kismet/GameplayStatics.h"

ABarEnemyAIController::ABarEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	if (!PawnSensing)
	{
		PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
		PawnSensing->OnSeePawn.AddDynamic(this, &ABarEnemyAIController::OnSeePawn);
	}
}

void ABarEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	RunBehaviorTree(StartingBehaviorTree);
}

void ABarEnemyAIController::OnSeePawn(APawn* PlayerPawn)
{
	AFPSCharacter* PlayerCharacter = Cast<AFPSCharacter>(PlayerPawn);

	if (!PlayerCharacter) return;

	// Don't react to the player at all if this enemy has been knocked down.
	if (ABarEnemyCharacter* MyEnemy = Cast<ABarEnemyCharacter>(GetPawn()))
	{
		if (MyEnemy->IsKnockedDown())
		{
			return;
		}
	}

	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	const bool bWasSeeing = BlackboardComp && BlackboardComp->GetValueAsBool("CanSeePlayer");

	const float Now = GetWorld()->GetTimeSeconds();
	const bool bCooldownElapsed = (Now - LastSpottedSoundTime) >= SpottedSoundCooldown;

	if (!bWasSeeing && bCooldownElapsed && SpottedPlayerSound)
	{
		if (APawn* MyPawn = GetPawn())
		{
			UGameplayStatics::PlaySoundAtLocation(this, SpottedPlayerSound, MyPawn->GetActorLocation());
			LastSpottedSoundTime = Now;
		}
	}

	SetCanSeePlayer(true, PlayerCharacter);
	RunRetriggerableTimer();
}

void ABarEnemyAIController::SetCanSeePlayer(bool bCanSee, UObject* PlayerObject)
{
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	BlackboardComp->SetValueAsBool("CanSeePlayer", bCanSee);
	if (bCanSee)
	{
		BlackboardComp->SetValueAsObject("PlayerTarget", PlayerObject);
	}
}

void ABarEnemyAIController::RunRetriggerableTimer()
{
	GetWorldTimerManager().ClearTimer(CanSeePlayerTimerHandle);
	CanSeePlayerTimerDelegate.BindUFunction(this, FName("SetCanSeePlayer"), false, nullptr);

	// CanSeePlayerTimerDelegate.BindUFunction(this, FName("ResetCanSeePlayer"));
	GetWorldTimerManager().SetTimer(CanSeePlayerTimerHandle, CanSeePlayerTimerDelegate, PawnSensing->SensingInterval * 2.0f, false);
}

void ABarEnemyAIController::ResetCanSeePlayer()
{
	SetCanSeePlayer(false, nullptr);
}