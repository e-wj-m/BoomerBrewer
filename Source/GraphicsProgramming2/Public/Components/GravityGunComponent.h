// GravityGunComponent (Public). This component of the Gravity Gun handles specific values such as the Max Grab Mass, Trace Range, Impulse strength etc. Also handles Camera linking for where the trace/hold points should live. -E.M

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraComponent.h"
#include "GravityGunComponent.generated.h"

UENUM(BlueprintType)
enum class EGravityGunState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Pulling UMETA(DisplayName = "Pulling"),
	Holding UMETA(DisplayName = "Holding")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GRAPHICSPROGRAMMING2_API UGravityGunComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGravityGunComponent();

	void OnPullPressed();

	void OnPullReleased();

	void Pull(UCameraComponent* Camera);

	void Fire(UCameraComponent* Camera);

	void TickHold(UCameraComponent* Camera);

	bool IsHoldingObject() const { return CurrentState == EGravityGunState::Holding; }

	UPROPERTY(EditAnywhere, Category = "Gravity Gun")
	float GrabDistance = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Gravity Gun")
	float GrabStiffness = 15.0f;

	UPROPERTY(EditAnywhere, Category = "Gravity Gun")
	float ThrowImpulse = 2500.0f;

	UPROPERTY(EditAnywhere, Category = "Gravity Gun")
	float MaxGrabMass = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Gravity Gun")
	float TraceRange = 5000.0f;

	UPROPERTY(EditAnywhere, Category = "Gravity Gun")
	float PullSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, Category = "Gravity Gun")
	float PullCatchDistance = 75.0f;

	UPROPERTY(EditAnywhere, Category = "Gravity Gun")
	float PunchImpulse = 50000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio")
	class USoundBase* PullSound = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio")
	class USoundBase* PickupSound = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio")
	class USoundBase* ThrowSound = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio")
	class USoundBase* DropSound = nullptr;

private: 

	UPROPERTY()
	UPrimitiveComponent* GrabbedObject = nullptr;

	UPROPERTY()
	EGravityGunState CurrentState = EGravityGunState::Idle;

	bool bPullConsumed = false;

	UPrimitiveComponent* TraceForPhysicsObject(UCameraComponent* Camera, FHitResult& OutHit) const;

	void SetGrabbedObject(UPrimitiveComponent* ObjectToGrab);

	void ReleaseObjectPhysics(UPrimitiveComponent* Object);
};
