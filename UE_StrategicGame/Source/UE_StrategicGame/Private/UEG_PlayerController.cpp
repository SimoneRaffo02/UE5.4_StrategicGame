// Fill out your copyright notice in the Description page of Project Settings.


#include "UEG_PlayerController.h"
#include "HumanPlayer.h"

AUEG_PlayerController::AUEG_PlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
}

void AUEG_PlayerController::ClickOnGrid()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("ClickOnGrid!"));
	const auto HumanPlayer = Cast<AHumanPlayer>(GetPawn());
	if (IsValid(HumanPlayer))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Valid Human!"));
		HumanPlayer->OnClick();
	}
}

void AUEG_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(UEG_Context, 0);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Valid Subsystem!"));
	}
}

void AUEG_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Triggered, this, &AUEG_PlayerController::ClickOnGrid);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Valid BindAction!"));
	}
}

