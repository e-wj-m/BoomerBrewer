// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/FPSGameHUD.h"
#include "GUI/GameEndWidget.h"
#include "GravityGunGameMode.h"
#include "Widgets/SWeakWidget.h"

void AFPSGameHUD::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("FPSGameHUD::BeginPlay() called"));

	// 2. Slates Method of creating a User Interface
	// ShowSettingsWidget();

	// 3. UMG Function to create a User Interface
	SpawnGameMenuWidget();

	TimeRemaining = StartingTime;

	GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &AFPSGameHUD::CountdownTick, 1.0f, true);
}

void AFPSGameHUD::CountdownTick()
{
	TimeRemaining--;

	if (GameMenuWidgetContainer)
	{
		GameMenuWidgetContainer->UpdateTimer(TimeRemaining);
	}

	if (TimeRemaining <= 0)
	{
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);

		ShowGameEndScreen(false);
	}
}

void AFPSGameHUD::ShowGameEndScreen(bool bPlayerWon)
{
	if (bGameEnded) return;
	bGameEnded = true;

	GetWorldTimerManager().ClearTimer(CountdownTimerHandle);

	if (PlayerOwner)
	{
		if (APawn* PlayerPawn = PlayerOwner->GetPawn())
		{
			PlayerPawn->DisableInput(PlayerOwner);
		}
	}

	if (GameMenuWidgetContainer)
	{
		GameMenuWidgetContainer->RemoveFromParent();
		GameMenuWidgetContainer = nullptr;
	}

	if (!GameEndWidgetClass) return;

	UGameEndWidget* EndWidget = CreateWidget<UGameEndWidget>(GetWorld(), GameEndWidgetClass);

	if (bPlayerWon)
	{
		EndWidget->SetResultMessage(FText::FromString("You Won!"));
	}

	else
	{
		EndWidget->SetResultMessage(FText::FromString("You Lost!"));
	}

	EndWidget->AddToViewport();

	PlayerOwner->bShowMouseCursor = true;
	PlayerOwner->SetInputMode(FInputModeUIOnly());
}

void AFPSGameHUD::DrawHUD()
{
	Super::DrawHUD();
	
	if (!CrosshairTexture) return;

	// Canvas Dimensions
	float CanvasWidth = Canvas->ClipX;
	float CanvasHeight = Canvas->ClipY;
	FVector2D Center(CanvasWidth * 0.5f, CanvasHeight * 0.5f);

	// Crosshair Dimensions 2
	float CrosshairWidth = CrosshairTexture->GetSurfaceWidth();
	float CrosshairHeight = CrosshairTexture->GetSurfaceHeight();

	float AlignmentX = 1;
	float AlignmentY = 1;
	FVector2D CrosshairPosOffset(CrosshairWidth * AlignmentX, CrosshairHeight * AlignmentY);

	// Draw/Render Settings
	FVector2D FinalCrosshairPos = Center - CrosshairPosOffset;

	float CrosshairScale = 5.0f;
	FVector2D FinalCrosshairSize(CrosshairWidth * CrosshairScale, CrosshairHeight * CrosshairScale);

	// Draw/Render the crosshair
	FCanvasTileItem TileItem(FinalCrosshairPos, CrosshairTexture->GetResource(), FinalCrosshairSize, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);
}

void AFPSGameHUD::SpawnGameMenuWidget()
{
	if (GameMenuWidgetContainer)
	{
		GameMenuWidgetContainer->RemoveFromParent();
		GameMenuWidgetContainer = nullptr;
	}

	GameMenuWidgetContainer = CreateWidget<UGameMenuWidget>(GetWorld(), GameMenuWidgetClass);
	GameMenuWidgetContainer->AddToViewport();

	GameMenuWidgetContainer->UpdateTimer(StartingTime);

	if (AGravityGunGameMode* GM = Cast<AGravityGunGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->SetGameMenuWidget(GameMenuWidgetContainer);
	}

	PlayerOwner->bShowMouseCursor = false;
	PlayerOwner->SetInputMode(FInputModeGameOnly());
}

void AFPSGameHUD::ShowSettingsWidget()
{
	FPSSettingsWidget = SNew(SFPSSettingsWidget).FPSGameHUD(this).MyInt(69);
	GEngine->GameViewport->AddViewportWidgetContent(SAssignNew(FPSSettingsWidgetContainer, SWeakWidget).PossiblyNullContent(FPSSettingsWidget.ToSharedRef()));

	PlayerOwner->bShowMouseCursor = true;
	PlayerOwner->SetInputMode(FInputModeUIOnly());
}

void AFPSGameHUD::HideSettingsWidget()
{
	GEngine->GameViewport->RemoveViewportWidgetContent(FPSSettingsWidgetContainer.ToSharedRef());

	PlayerOwner->bShowMouseCursor = false;
	PlayerOwner->SetInputMode(FInputModeGameOnly());
}
