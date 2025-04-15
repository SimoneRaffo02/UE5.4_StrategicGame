// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MyUserWidget.h"
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UEG_HUD.generated.h"

/**
 * 
 */
UCLASS()
class UE_GAME_API AUEG_HUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AUEG_HUD();

	UMyUserWidget* GetHUDWidget();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UMyUserWidget> UserWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UMyUserWidget* HUDWidget;
};
