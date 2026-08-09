// Knockbackable Component (Public). Setup for linking to the BarEnemyCharacter to allow for ragdolling when Punched by the gravity gun. -E.M

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Knockbackable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UKnockbackable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GRAPHICSPROGRAMMING2_API IKnockbackable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	UFUNCTION(BlueprintNativeEvent, Category = "Knockback")
	void KnockDown(const FVector& Impulse);
};
