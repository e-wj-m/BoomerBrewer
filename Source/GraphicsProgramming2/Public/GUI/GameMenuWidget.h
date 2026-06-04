// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "GameMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class GRAPHICSPROGRAMMING2_API UGameMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* PatienceBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreText;
		
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TimerText;

	//UPROPERTY(meta = (BindWidget))
	//UButton* PauseButton;

	UFUNCTION()
	void UpdatePatienceBar(float PatiencePercent);

	UFUNCTION()
	void UpdateScore(int32 Score);

	UFUNCTION()
	void UpdateTimer(int Time);

	//UFUNCTION()
	//void OnPauseClicked();

};
