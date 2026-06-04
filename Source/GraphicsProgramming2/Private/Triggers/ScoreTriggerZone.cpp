#include "Triggers/ScoreTriggerZone.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GUI/GameMenuWidget.h"
#include "GravityGunGameMode.h"

AScoreTriggerZone::AScoreTriggerZone()
{
    PrimaryActorTick.bCanEverTick = false;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    RootComponent = TriggerBox;

    TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
    TriggerBox->SetGenerateOverlapEvents(true);
}

void AScoreTriggerZone::BeginPlay()
{
    Super::BeginPlay();

    TriggerBox->OnComponentBeginOverlap.AddDynamic(
        this, &AScoreTriggerZone::OnTriggerBeginOverlap
    );
}

void AScoreTriggerZone::OnTriggerBeginOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!OtherActor || !OtherActor->ActorHasTag(RequiredObjectTag))
    {
        return;
    }

    if (AGameModeBase* GM = UGameplayStatics::GetGameMode(this))
    {
        if (AGravityGunGameMode* GGGM = Cast<AGravityGunGameMode>(GM))
        {
            GGGM->AddScore(PointsPerObject);
            PushScoreToWidget(GGGM->GetScore());
        }
    }

    OtherActor->Destroy();
}

void AScoreTriggerZone::PushScoreToWidget(int32 NewScore)
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC) return;

    UGameViewportClient* Viewport = GetWorld()->GetGameViewport();
    if (!Viewport) return;

    if (AGameModeBase* GM = UGameplayStatics::GetGameMode(this))
    {
        if (AGravityGunGameMode* GGGM = Cast<AGravityGunGameMode>(GM))
        {
            if (UGameMenuWidget* MenuWidget = GGGM->GetGameMenuWidget())
            {
                MenuWidget->UpdateScore(NewScore);
            }
        }
    }
}