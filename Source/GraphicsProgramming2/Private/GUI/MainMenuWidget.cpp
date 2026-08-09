#include "GUI/MainMenuWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (StartGameButton)
    {
        StartGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnStartGameClicked);
    }

    if (ExitGameButton)
    {
        ExitGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnExitGameClicked);
    }
}

void UMainMenuWidget::OnStartGameClicked()
{
    UGameplayStatics::OpenLevel(this, FName("/Game/Scenes/BarLevel"));
}

void UMainMenuWidget::OnExitGameClicked()
{
    UKismetSystemLibrary::QuitGame(
        this,
        GetWorld()->GetFirstPlayerController(),
        EQuitPreference::Quit,
        true
    );
}