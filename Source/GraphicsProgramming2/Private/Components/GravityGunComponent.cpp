// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/GravityGunComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealInterfaces/Knockbackable.h"

// Sets default values for this component's properties
UGravityGunComponent::UGravityGunComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

UPrimitiveComponent* UGravityGunComponent::TraceForPhysicsObject(UCameraComponent* Camera, FHitResult& OutHit) const
{
	if (!Camera || !GetWorld()) return nullptr;

	const FVector Start = Camera->GetComponentLocation();
	const FVector End = Start + Camera->GetForwardVector() * TraceRange;

	FCollisionQueryParams Params(TEXT("Gravity Gun Trace"), false, GetOwner());

	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, Params))
	{
		UPrimitiveComponent* HitComp = OutHit.GetComponent();

		if (HitComp && HitComp->IsSimulatingPhysics())
		{
			return HitComp;
		}
	}
	return nullptr;
}

void UGravityGunComponent::OnPullPressed()
{
	if (CurrentState == EGravityGunState::Holding)
	{
		ReleaseObjectPhysics(GrabbedObject);

		if (DropSound && GrabbedObject)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DropSound, GrabbedObject->GetComponentLocation());
		}

		SetGrabbedObject(nullptr);
		CurrentState = EGravityGunState::Idle;
		bPullConsumed = true;
	}
}

void UGravityGunComponent::OnPullReleased()
{
	bPullConsumed = false;
}

void UGravityGunComponent::Pull(UCameraComponent* Camera)
{
	if (!Camera) return;

	if (bPullConsumed) return;

	switch (CurrentState)
	{
	case EGravityGunState::Idle:
	{
		FHitResult Hit;
		UPrimitiveComponent* HitComp = TraceForPhysicsObject(Camera, Hit);

		if (HitComp && HitComp->GetMass() <= MaxGrabMass)
		{
			GrabbedObject = HitComp;
			GrabbedObject->SetEnableGravity(false);
			CurrentState = EGravityGunState::Pulling;

			if (PullSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, PullSound, GetOwner()->GetActorLocation());
			}
		}
		break;
	}
	case EGravityGunState::Pulling:
	{
		if (!GrabbedObject)
		{
			CurrentState = EGravityGunState::Idle;
			break;
		}

		const FVector HoldPoint = Camera->GetComponentLocation() + Camera->GetForwardVector() * GrabDistance;
		const FVector ToHold = HoldPoint - GrabbedObject->GetComponentLocation();
		const float Distance = ToHold.Size();

		if (Distance <= PullCatchDistance)
		{
			SetGrabbedObject(GrabbedObject);
			CurrentState = EGravityGunState::Holding;

			if (PickupSound && GrabbedObject)
			{
				UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GrabbedObject->GetComponentLocation());
			}
		}

		else
		{
			const FVector PullVelocity = ToHold.GetSafeNormal() * PullSpeed;
			GrabbedObject->SetPhysicsLinearVelocity(PullVelocity);
			GrabbedObject->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		}
		break;
	}
	case EGravityGunState::Holding:

		break;
	}
}

void UGravityGunComponent::Fire(UCameraComponent* Camera)
{
	if (!Camera) return;

	if (CurrentState == EGravityGunState::Holding && GrabbedObject)
	{
		UPrimitiveComponent* ObjectToThrow = GrabbedObject;
		SetGrabbedObject(nullptr);
		CurrentState = EGravityGunState::Idle;

		ObjectToThrow->SetPhysicsLinearVelocity(FVector::ZeroVector);
		ObjectToThrow->SetEnableGravity(true);
		ObjectToThrow->SetLinearDamping(0.01f);
		ObjectToThrow->AddImpulse(Camera->GetForwardVector() * ThrowImpulse, NAME_None, false);

		if (ThrowSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ThrowSound, GetOwner()->GetActorLocation());
		}
		return;
	}

	FHitResult Hit;
	const FVector Start = Camera->GetComponentLocation();
	const FVector End = Start + Camera->GetForwardVector() * TraceRange;
	FCollisionQueryParams Params(TEXT("Gravity Gun Punch"), false, GetOwner());

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		AActor* HitActor = Hit.GetActor();
		UE_LOG(LogTemp, Warning, TEXT("Punch Hit: %s (component: %s)"),
			HitActor ? *HitActor->GetName() : TEXT("null"),
			Hit.GetComponent() ? *Hit.GetComponent()->GetName() : TEXT("null"));

		if (HitActor && HitActor->Implements<UKnockbackable>())
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit actor implements Knockbackable!"));
			IKnockbackable::Execute_KnockDown(HitActor, Camera->GetForwardVector() * PunchImpulse);
		}

		else if (UPrimitiveComponent* HitComp = Hit.GetComponent())
		{
			if (HitComp->IsSimulatingPhysics())
			{
				HitComp->AddImpulse(Camera->GetForwardVector() * PunchImpulse, NAME_None, false);
			}
		}
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Punch trace hit nothing"));
	}
}

void UGravityGunComponent::TickHold(UCameraComponent* Camera)
{
	if (CurrentState != EGravityGunState::Holding || !GrabbedObject || !Camera) return;

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
		//Stuff goes here... What kind of stuff? Restoring defaults for releasing objects, like a ThrowOrRelease check. Don't need it right now though. 
	}

}

void UGravityGunComponent::ReleaseObjectPhysics(UPrimitiveComponent* Object)
{
	if (!Object) return;

	Object->SetEnableGravity(true);
	Object->SetLinearDamping(0.01f);
	Object->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
}

//void UGravityGunComponent::TryGrab(UCameraComponent* Camera)
//{
//	if (!Camera) return;
//
//	if (GrabbedObject) return;
//
//	const FVector Start = Camera->GetComponentLocation();
//	const FVector End = Start + Camera->GetForwardVector() * GrabDistance * 25.0f;
//
//	FHitResult Hit;
//	FCollisionQueryParams Params(TEXT("Gravity Gun Trace"), false, GetOwner());
//
//	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
//	{
//		UPrimitiveComponent* HitComp = Hit.GetComponent();
//
//		if (HitComp && HitComp->IsSimulatingPhysics())
//		{
//			if (HitComp->GetMass() <= MaxGrabMass)
//			{
//				SetGrabbedObject(HitComp);
//			}
//		}
//	}
//}

//void UGravityGunComponent::ThrowOrRelease()
//{
//	if (!GrabbedObject) return;
//
//	UPrimitiveComponent* ObjectToThrow = GrabbedObject;
//
//	SetGrabbedObject(nullptr);
//
//	AActor* Owner = GetOwner();
//	if (!Owner) return;
//
//	UCameraComponent* Camera = Owner->FindComponentByClass<UCameraComponent>();
//	if (!Camera) return;
//
//	ObjectToThrow->SetPhysicsLinearVelocity(FVector::ZeroVector);
//	ObjectToThrow->SetEnableGravity(true);
//	ObjectToThrow->SetLinearDamping(0.01f);
//	ObjectToThrow->AddImpulse(Camera->GetForwardVector() * ThrowImpulse, NAME_None, false);
//}