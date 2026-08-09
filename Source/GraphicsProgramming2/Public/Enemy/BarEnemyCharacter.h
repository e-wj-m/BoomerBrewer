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

	// Read by the AI controller to gate perception-driven behavior (e.g. spotted sound).
	UFUNCTION(BlueprintPure, Category = "Knockback")
	bool IsKnockedDown() const { return bIsKnockedDown; }

protected:
	UPROPERTY(EditAnywhere, Category = "Knockback")
	float RecoveryDelay = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio")
	class USoundBase* KnockdownSound = nullptr;

	// True once KnockDown has been applied; latches the enemy out of "active" behavior.
	UPROPERTY(BlueprintReadOnly, Category = "Knockback")
	bool bIsKnockedDown = false;
};