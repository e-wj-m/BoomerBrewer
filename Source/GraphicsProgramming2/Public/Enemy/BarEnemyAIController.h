// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "FPSCharacter.h"
#include "BarEnemyAIController.generated.h"
/**
 *
 */
UCLASS()
class GRAPHICSPROGRAMMING2_API ABarEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABarEnemyAIController();
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UBehaviorTree* StartingBehaviorTree;

	UPROPERTY(EditAnywhere)
	UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio")
	class USoundBase* SpottedPlayerSound = nullptr;

	// Minimum seconds between SpottedPlayerSound plays, regardless of how often the pawn is seen.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio")
	float SpottedSoundCooldown = 5.0f;

	UFUNCTION()
	void OnSeePawn(APawn* PlayerPawn);

	UFUNCTION()
	void RunRetriggerableTimer();

	UFUNCTION()
	void SetCanSeePlayer(bool bCanSee, UObject* PlayerObject);

	FTimerHandle CanSeePlayerTimerHandle;

	FTimerDelegate CanSeePlayerTimerDelegate;

	void ResetCanSeePlayer();

	// Timestamp (world seconds) of the last time SpottedPlayerSound played; -BIG so the first sighting always plays.
	float LastSpottedSoundTime = -1000.0f;
};