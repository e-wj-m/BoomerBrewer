// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Projectile/FPSProjectile.h"
#include "Components/GravityGunComponent.h"
#include "FPSCharacter.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class GRAPHICSPROGRAMMING2_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Enhanced Input assets
	UPROPERTY(EditDefaultsOnly, Category = "Input") // Very similar to serializeable fields in Unity, but with more options for when the value can be edited and where it can be edited from
	UInputMappingContext* PlayerMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* FireAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* PullAction;

	//UPROPERTY(EditAnywhere, Category = "Gravity Gun")
	//float GrabDistance = 200.0f;

	// GRAVITY GUN - Keeping track of the currently grabbed object, if there is one. This will be set to nullptr when there is no object currently grabbed, ensuring we manage our memory proper.
	//UPROPERTY()
	//UPrimitiveComponent* GrabbedObject;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* FPSCameraComponent;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* FPSMeshComponent;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* FP_Gun;

	UPROPERTY(EditAnywhere)
	FVector MuzzleOffset;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AFPSProjectile> ProjectileClass;

	//UPROPERTY(EditAnywhere, Category = "Gravity Gun")
	//float GrabStiffness = 15.0f;

	//UPROPERTY(EditAnywhere, Category = "Gravity Gun")
	//float ThrowImpulse = 2500.0f;

	//UPROPERTY(Editanywhere, Category = "Gravity Gun")
	//float MaxGrabMass = 200.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gravity Gun")
	UGravityGunComponent* GravityGunComponent;

	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()

	void Look(const FInputActionValue& Value);

	UFUNCTION()

	void StartJump();



	UFUNCTION()

	void EndJump();



	UFUNCTION()

	void Fire();

	UFUNCTION()
	void StartPull();

	UFUNCTION()
	void Pull();

	UFUNCTION()
	void EndPull();

	UFUNCTION()
	void OnHurtPlayer(float DamageAmount);

	UFUNCTION()
	void OnPlayerHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	//UFUNCTION()
	//void EndFire();

	//UFUNCTION()
	//void SetGrabbedObject(UPrimitiveComponent* ObjectToGrab);

private:
	float Health = 100.0f;
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float DamagePerHit = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float DamageCooldown = 1.0f;
	float LastDamageTime = -1.0f;

};
