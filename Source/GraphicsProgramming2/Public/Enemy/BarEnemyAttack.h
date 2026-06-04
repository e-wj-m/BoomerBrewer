// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTTaskNode.h"
#include "FPSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "BarEnemyAttack.generated.h"

/**
 * 
 */
UCLASS()
class GRAPHICSPROGRAMMING2_API UBarEnemyAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
