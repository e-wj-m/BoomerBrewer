// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BarEnemyAttack.h"

EBTNodeResult::Type UBarEnemyAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Calling Player Directly through code
	//AFPSCharacter* PlayerCharacter = Cast<AFPSCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	// Calling Player through Blackboard
	AFPSCharacter* PlayerCharacter = Cast<AFPSCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("PlayerTarget"));

	// Calling Enemy through code
	//AAIController* BarEnemyAIController = OwnerComp.GetAIOwner();
	//ABarEnemyCharacter* EnemyCharacter = Cast<ABarEnemyCharacter>(BarEnemyAIController->GetPawn());

	// AFPSCharacter* PlayerCharacter = Cast<AFPSCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	// Using the built in call system
	// AFPSCharacter* PlayerCharacter = Cast<AFPSCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	return EBTNodeResult::Succeeded;
}
