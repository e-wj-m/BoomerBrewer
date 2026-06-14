#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ScoreTriggerZone.generated.h"

UCLASS()
class GRAPHICSPROGRAMMING2_API AScoreTriggerZone : public AActor
{
    GENERATED_BODY()

public:
    AScoreTriggerZone();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UBoxComponent* TriggerBox;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
    int32 PointsPerObject = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")

    //Swap out tags for using the can itself as an AC - DO THIS NEXT UPDATE
    FName RequiredObjectTag = "Pickup";

    UFUNCTION()
    void OnTriggerBeginOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

private:
    void PushScoreToWidget(int32 NewScore);
};