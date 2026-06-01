// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/GravityGunComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UGravityGunComponent::UGravityGunComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UGravityGunComponent::TryGrab(UCameraComponent* Camera)
{
	if (!Camera) return;

	if (GrabbedObject) return;

	const FVector Start = Camera->GetComponentLocation();
	const FVector End = Start + Camera->GetForwardVector() * GrabDistance * 25.0f;

	FHitResult Hit;
	FCollisionQueryParams Params(TEXT("Gravity Gun Trace"), false, GetOwner());

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		UPrimitiveComponent* HitComp = Hit.GetComponent();

		if (HitComp && HitComp->IsSimulatingPhysics())
		{
			if (HitComp->GetMass() <= MaxGrabMass)
			{
				SetGrabbedObject(HitComp);
			}
		}
	}
}

void UGravityGunComponent::ThrowOrRelease()
{
	if (!GrabbedObject) return;

	UPrimitiveComponent* ObjectToThrow = GrabbedObject;

	SetGrabbedObject(nullptr);

	AActor* Owner = GetOwner();
	if (!Owner) return;

	UCameraComponent* Camera = Owner->FindComponentByClass<UCameraComponent>();
	if (!Camera) return;

	ObjectToThrow->SetPhysicsLinearVelocity(FVector::ZeroVector);
	ObjectToThrow->SetEnableGravity(true);
	ObjectToThrow->SetLinearDamping(0.01f);
	ObjectToThrow->AddImpulse(Camera->GetForwardVector() * ThrowImpulse, NAME_None, false);
}

void UGravityGunComponent::TickHold(UCameraComponent* Camera)
{
	if (!GrabbedObject || !Camera) return;
	
	const FVector TargetLocation = Camera->GetComponentLocation() + Camera->GetForwardVector() * GrabDistance;

	const FVector Displacement = TargetLocation - GrabbedObject->GetComponentLocation();

	GrabbedObject->SetPhysicsLinearVelocity(Displacement * GrabStiffness);
	GrabbedObject->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
}

void UGravityGunComponent::SetGrabbedObject(UPrimitiveComponent* ObjectToGrab)
{
	GrabbedObject = ObjectToGrab;

	if (GrabbedObject)
	{
		GrabbedObject->SetEnableGravity(false);
		GrabbedObject->SetLinearDamping(5.0f);
	}

	else
	{
		//Stuff goes here... What kind of stuff? Restoring defaults for releasing objects, like a ThrowOrRelease check. 
	}

}