// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "UEG_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GIOCOUE_API AUEG_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AUEG_PlayerController();

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* UEG_Context;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ClickAction;

	void ClickOnGrid();

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;
};
