// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UEG_GamemodeBase.h"
#include "UEG_GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "MyUserWidget.generated.h"

UCLASS()
class UE_GAME_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UMyUserWidget(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "UI")
	UTextBlock* TurnCounter;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "UI")
	UTextBlock* MessageText;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "UI")
	UScrollBox* ActionsHistoryScrollBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "UI")
	UTextBlock* ActionsHistory;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "UI")
	UButton* EndTurnButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "UI")
	UButton* ResetGameButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UUEG_GameInstance* GameInstance;

	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void OnEndTurnButtonClick();

	UFUNCTION()
	void OnResetGameButtonClick();

	UFUNCTION()
	void SetMessageText();

	UFUNCTION()
	void UpdateTurnCounter();

	UFUNCTION()
	void AddToHistory();

	UFUNCTION()
	void ResetUI();
};
