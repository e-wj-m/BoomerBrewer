// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Canvas.h"
#include "GameFramework/HUD.h"
#include "GUI/GameMenuWidget.h"
#include "Slate/SFPSSettingsWidget.h"
#include "FPSGameHUD.generated.h"

/**
 * 
 */
UCLASS()
class GRAPHICSPROGRAMMING2_API AFPSGameHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 1. HUD Draw Method
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere)
	UTexture2D* CrosshairTexture;

	// 2. Slates Method of Creating a User Interface
	TSharedPtr<class SFPSSettingsWidget> FPSSettingsWidget;
	TSharedPtr<class SWidget> FPSSettingsWidgetContainer;

	// 3. UMG Method of Creating a User Interface
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGameMenuWidget> GameMenuWidgetClass;
	UGameMenuWidget* GameMenuWidgetContainer;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UGameEndWidget> GameEndWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Gameplay")
	int32 StartingTime = 60;

	void SpawnGameMenuWidget();
	void ShowSettingsWidget();
	void HideSettingsWidget();
	void ShowGameEndScreen(bool bPlayerWon);

private:
	int32 TimeRemaining;

	FTimerHandle CountdownTimerHandle;

	bool bGameEnded = false;

	void CountdownTick();
};
