// GravityGunComponent (Private). This is the meat of the Gravity Gun's Logic, handling the Pull, Fire, and Punch logic the Player can utilize. Links to the Character's IA's. -E.M


#include "Components/GravityGunComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealInterfaces/Knockbackable.h"

// Sets default values for this component's properties
UGravityGunComponent::UGravityGunComponent()
{
	// Ticking is disabled at the component level. Per-frame work (TickHold) is driven externally by the owning actor/pawn, which keeps this component passive and avoids paying tick cost while the gun is Idle.
	PrimaryComponentTick.bCanEverTick = false;
}

// Casts a ray straight out from the camera and returns the first physics-simulating primitive it hits within TraceRange. Returns nullptr if nothing valid is found. Only components with IsSimulatingPhysics() == true are grabbable.
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

// Handles the "pull" input being pressed. While Holding, this acts as a drop: it restores the object's physics, plays the drop SFX, and returns to Idle.
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

// Handles the "pull" input being released. Clears the latch so the next press is allowed to start a fresh pull. 
void UGravityGunComponent::OnPullReleased()
{
	bPullConsumed = false;
}

// Core state machine for grabbing and reeling in an object.

// Idle = trace for a target; if found and under MaxGrabMass, disable its gravity and begin Pulling.          
// Pulling = each call, drive the object toward the hold point. Once it's within PullCatchDistance it transitions to Holding; otherwise it keeps applying a constant-speed velocity toward the hold point.          
// Holding = TickHold() maintains the held position.

// Physics notes:
//  - Gravity is disabled the moment an object is grabbed so it doesn't fight the pull velocity / fall mid-reel.   
//  - Velocity is set directly (SetPhysicsLinearVelocity) rather than via impulse, giving a consistent reel-in speed independent of the object's mass.  
//  - Angular velocity is zeroed every step so the object doesn't tumble while being pulled, which keeps the motion readable.
void UGravityGunComponent::Pull(UCameraComponent* Camera)
{
	if (!Camera) return;

	// Latch gate: ignore Pull while the press that just dropped an object is still held.
	if (bPullConsumed) return;

	switch (CurrentState)
	{
	case EGravityGunState::Idle:
	{
		FHitResult Hit;
		UPrimitiveComponent* HitComp = TraceForPhysicsObject(Camera, Hit);

		// Mass gate: only objects at or below MaxGrabMass can be picked up, so the gun can't yank heavy world geometry around (set at 200kg as a default, can be changed in public file).
		if (HitComp && HitComp->GetMass() <= MaxGrabMass)
		{
			GrabbedObject = HitComp;
			GrabbedObject->SetEnableGravity(false);
			CurrentState = EGravityGunState::Pulling;
			PullElapsed = 0.0f;

			if (PullSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, PullSound, GetOwner()->GetActorLocation());
			}
		}
		break;
	}
	case EGravityGunState::Pulling:
	{
		// Safety: object may have been destroyed mid-pull; bail back to Idle.
		if (!GrabbedObject)
		{
			CurrentState = EGravityGunState::Idle;
			break;
		}

		PullElapsed += GetWorld()->GetDeltaSeconds();
		if (PullElapsed >= MaxPullTime)
		{
			ReleaseObjectPhysics(GrabbedObject);
			GrabbedObject = nullptr;
			CurrentState = EGravityGunState::Idle;
			break;
		}

		// Hold point sits a fixed GrabDistance in front of the camera. Measure how far the object still is from that point each call.
		const FVector HoldPoint = Camera->GetComponentLocation() + Camera->GetForwardVector() * GrabDistance;
		const FVector ToHold = HoldPoint - GrabbedObject->GetComponentLocation();
		const float Distance = ToHold.Size();

		if (Distance <= PullCatchDistance)
		{
			// Close enough to "catch" — hand off to the Holding state.
			SetGrabbedObject(GrabbedObject);
			CurrentState = EGravityGunState::Holding;

			if (PickupSound && GrabbedObject)
			{
				UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GrabbedObject->GetComponentLocation());
			}
		}
		break;
	}
	case EGravityGunState::Holding:
	{
		// Already holding, so just maintain the held position.
		TickPull(Camera);
		break;
	}

	}
}

void UGravityGunComponent::TickPull(UCameraComponent* Camera)
{
	if (CurrentState != EGravityGunState::Pulling || !Camera) return;

	// Object may have been destroyed mid-pull.
	if (!GrabbedObject)
	{
		CurrentState = EGravityGunState::Idle;
		return;
	}

	// Fail-safe runs every frame now, not just while the button is held.
	PullElapsed += GetWorld()->GetDeltaSeconds();
	if (PullElapsed >= MaxPullTime)
	{
		ReleaseObjectPhysics(GrabbedObject);
		GrabbedObject = nullptr;
		CurrentState = EGravityGunState::Idle;
		return;
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
		const FVector Desired = ToHold * PullStiffness;
		const FVector PullVelocity = Desired.SizeSquared() > FMath::Square(PullSpeed)
			? Desired.GetSafeNormal() * PullSpeed
			: Desired;
		GrabbedObject->SetPhysicsLinearVelocity(PullVelocity);
		GrabbedObject->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	}
}

// Handles the "fire" input. Two distinct behaviours depending on state:

// If currently Holding an object, THROW it: clear hold state, restore gravity and normal damping, then apply a forward impulse scaled by ThrowImpulse.  
// Linear velocity is zeroed first so the throw impulse is the only force acting, making throw strength predictable regardless of the object's prior motion.  

// If NOT holding, PUNCH it: trace forward and either knock down a Knockbackable actor (via the interface) or apply a one-off impulse to a loose physics object.

// Physics notes:
//  - AddImpulse with bVelChange=false means the impulse is mass-scaled (heavier objects move less for the same impulse) — realistic throw/punch feel.
//  - Damping is reset to 0.01 on throw because the held object was given high damping (5.0 in SetGrabbedObject) to keep it steady; that high damping must be cleared or the thrown object would decelerate unnaturally fast.
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

	// Punch path: nothing held, so do a forward trace to punch that shizzle homeslice. Now whip it. Whip it good.
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

		// Prefer the gameplay interface: if the actor knows how to be knocked down, let it handle the reaction (ragdoll on enemies) via Execute_KnockDown.
		if (HitActor && HitActor->Implements<UKnockbackable>())
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit actor implements Knockbackable!"));
			IKnockbackable::Execute_KnockDown(HitActor, Camera->GetForwardVector() * PunchImpulse);
		}

		// Fallback: a plain physics object just gets a mass-scaled impulse.
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

// Per-frame maintenance of a held object's position. Called externally while Holding. Rather than teleporting the object (which breaks collision), it computes the gap between the object and the target hold point and converts that gap into a velocity.

// Physics notes:
//  - Displacement * GrabStiffness is a proportional ("spring-like") controller: the further the object is from where it should be, the faster it moves to catch up. GrabStiffness tunes how snappy vs. floaty the hold feels. 
//  - Because it sets velocity rather than position, the object still collides with the world correctly and won't tunnel through walls while being carried.
//  - Angular velocity is zeroed so the held object stays visually stable.
void UGravityGunComponent::TickHold(UCameraComponent* Camera)
{
	if (CurrentState != EGravityGunState::Holding || !GrabbedObject || !Camera) return;

	const FVector TargetLocation = Camera->GetComponentLocation() + Camera->GetForwardVector() * GrabDistance;

	const FVector Displacement = TargetLocation - GrabbedObject->GetComponentLocation();

	GrabbedObject->SetPhysicsLinearVelocity(Displacement * GrabStiffness);
	GrabbedObject->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
}

// Sets (or clears) the currently grabbed object and applies the "carry" physics state.

// When grabbing: gravity is disabled and high linear damping (5.0) is applied so the object settles quickly and doesn't oscillate/drift around the hold point.
// When clearing (nullptr): nothing is restored here on purpose — the caller that releases the object (OnPullPressed/Fire via ReleaseObjectPhysics) owns restoring gravity and damping, so defaults aren't double-applied.
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

// Restores a released object's physics to sensible defaults: gravity back on, low linear damping (so it moves freely again), and zeroed spin so it doesn't fly off rotating from leftover angular velocity accumulated while held.
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
