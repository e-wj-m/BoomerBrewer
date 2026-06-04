// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "CoreMinimal.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTTaskNode.h"
#include "NavigationSystem.h"
#include "BarEnemyGetRandomPatrolLocation.generated.h"

/**
 * 
 */
UCLASS()
class GRAPHICSPROGRAMMING2_API UBarEnemyGetRandomPatrolLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	FVector PatrolLocation;
	UNavigationSystemV1* NavSystem;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
