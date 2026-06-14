// Fill out your copyright notice in the Description page of Project Settings.


#include "GUI/GameMenuWidget.h"
#include "Kismet/KismetSystemLibrary.h"

void UGameMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UpdatePatienceBar(1.0f);
	UpdateScore(0);
}

void UGameMenuWidget::UpdatePatienceBar(float PatiencePercent)
{
	if (!PatienceBar)
	{
		return;
	}

	PatienceBar->SetPercent(PatiencePercent);
}

void UGameMenuWidget::UpdateScore(int32 Score)
{
	if (!ScoreText)
	{
		return;
	}
	FString ScoreString = FString::Printf(TEXT("Score: %d"), Score);
	ScoreText->SetText(FText::FromString(ScoreString));
}

void UGameMenuWidget::UpdateTimer(int Time)
{
	if (!TimerText)
	{
		return;
	}
	FString TimerString = FString::Printf(TEXT("Time: %d"), Time);
	TimerText->SetText(FText::FromString(TimerString));
}

//void UGameMenuWidget::OnPauseClicked()
//{
//	//Leaving empty for now in case I decide to add a pause later
//}	
