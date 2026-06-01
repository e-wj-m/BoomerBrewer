// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraComponent.h"
#include "GravityGunComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GRAPHICSPROGRAMMING2_API UGravityGunComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGravityGunComponent();

	void TryGrab(UCameraComponent* Camera);

	void ThrowOrRelease();

	void TickHold(UCameraComponent* Camera);

	bool IsHoldingObject() const { return GrabbedObject != nullptr; }

	UPROPERTY(EditAnywhere, Category = "Gravity Gun")
	float GrabDistance = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Gravity Gun")
	float GrabStiffness = 15.0f;

	UPROPERTY(EditAnywhere, Category = "Gravity Gun")
	float ThrowImpulse = 2500.0f;

	UPROPERTY(EditAnywhere, Category = "Gravity Gun")
	float MaxGrabMass = 200.0f;

private: 

	UPROPERTY()
	UPrimitiveComponent* GrabbedObject = nullptr;

	void SetGrabbedObject(UPrimitiveComponent* ObjectToGrab);

};
