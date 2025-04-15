// Fill out your copyright notice in the Description page of Project Settings.


#include "UEG_HUD.h"

AUEG_HUD::AUEG_HUD()
{
	
}

UMyUserWidget* AUEG_HUD::GetHUDWidget()
{
	return HUDWidget;
}

void AUEG_HUD::BeginPlay()
{
	Super::BeginPlay();

	if (UserWidgetClass)
	{
		HUDWidget = CreateWidget<UMyUserWidget>(GetWorld(), UserWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}
}
