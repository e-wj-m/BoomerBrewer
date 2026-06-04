// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GUI/GameMenuWidget.h"
#include "GravityGunGameMode.generated.h"

//class GEngine;

/**
 * 
 */
UCLASS()
class GRAPHICSPROGRAMMING2_API AGravityGunGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	virtual void StartPlay() override;

public:
    UFUNCTION(BlueprintCallable)
    void AddScore(int32 Points);

    UFUNCTION(BlueprintPure)
    int32 GetScore() const { return CurrentScore; }

    void SetGameMenuWidget(UGameMenuWidget* Widget) { GameMenuWidget = Widget; }

    UGameMenuWidget* GetGameMenuWidget() const { return GameMenuWidget; }

private:
    int32 CurrentScore = 0;

    UPROPERTY()
    UGameMenuWidget* GameMenuWidget = nullptr;
};
