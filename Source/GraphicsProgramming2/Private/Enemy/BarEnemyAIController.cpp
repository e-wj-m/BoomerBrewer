// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BarEnemyAIController.h"

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
	/*CanSeePlayerTimerDelegate.BindUFunction(this, FName("ResetCanSeePlayer"));*/
	GetWorldTimerManager().SetTimer(CanSeePlayerTimerHandle, CanSeePlayerTimerDelegate, PawnSensing->SensingInterval * 2.0f, false);
}

void ABarEnemyAIController::ResetCanSeePlayer()
{
	SetCanSeePlayer(false, nullptr);
}
