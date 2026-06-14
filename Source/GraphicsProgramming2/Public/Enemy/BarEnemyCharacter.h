// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UnrealInterfaces/Knockbackable.h"
#include "BarEnemyCharacter.generated.h"

UCLASS()
class GRAPHICSPROGRAMMING2_API ABarEnemyCharacter : public ACharacter, public IKnockbackable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABarEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void KnockDown_Implementation(const FVector& Impulse) override;

protected:

	UPROPERTY(EditAnywhere, Category = "Knockback")
	float RecoveryDelay = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio")
	class USoundBase* KnockdownSound = nullptr;
};
