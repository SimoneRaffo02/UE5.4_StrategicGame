// Fill out your copyright notice in the Description page of Project Settings.

#include "UEG_GamemodeBase.h"
#include "UEG_HUD.h"
#include "UEG_GameInstance.h"
#include "RandomPlayer.h"
#include "UEG_PlayerController.h"
#include "HumanPlayer.h"
#include "SmartPlayer.h"
#include "EngineUtils.h"
#include <Kismet/GameplayStatics.h>

AUEG_GamemodeBase::AUEG_GamemodeBase()
{
	PlayerControllerClass = AUEG_PlayerController::StaticClass();
	DefaultPawnClass = AHumanPlayer::StaticClass();
	
	FieldSize = 25;
	ObstaclesPercentage = 40;
	TroopPlacingHeight = 20;

	bTroopInMotion = false;
	StepTime = 0.01;
	StepsForTile = 10;

	CurrentTurn = 0;
}

void AUEG_GamemodeBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GameInstance = Cast<UUEG_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
}

int32 AUEG_GamemodeBase::GetFieldSize()
{
	return FieldSize;
}

AGameField* AUEG_GamemodeBase::GetGameField()
{
	return GField;
}

void AUEG_GamemodeBase::BeginPlay()
{
	Super::BeginPlay();

	OnReset.AddDynamic(this, &AUEG_GamemodeBase::ResetGamemodeBase);

	ArchersNumber = 1;
	KnightsNumber = 1;

	AHumanPlayer* HumanPlayer = GetWorld()->GetFirstPlayerController()->GetPawn<AHumanPlayer>();

	if (!IsValid(HumanPlayer))
	{
		UE_LOG(LogTemp, Error, TEXT("No player pawn of type '%s' was found."), *AHumanPlayer::StaticClass()->GetName());
		return;
	}

	if (GameFieldClass != nullptr)
	{
		GField = GetWorld()->SpawnActor<AGameField>(GameFieldClass);
		GField->SetFieldSize(FieldSize);
		GField->SetObstaclesPercentage(ObstaclesPercentage);
		GField->GenerateObstacles();
		GField->SetTilesMaterial();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Game Field is null"));
		return;
	}

	float CameraPosX = (GField->GetTileSize() * FieldSize - GField->GetTileSize()) / 2;
	float Zposition = (GField->GetTileSize() * FieldSize * 0.6);
	FVector CameraPos(CameraPosX, CameraPosX, Zposition);
	HumanPlayer->SetActorLocationAndRotation(CameraPos, FRotator(-90, -90, 0));

	//Random Player
	//ARandomPlayer* AI = GetWorld()->SpawnActor<ARandomPlayer>(FVector(), FRotator());
	//OpponentType = EOpponentType::RANDOM;

	//Smart Player
	ASmartPlayer* AI = GetWorld()->SpawnActor<ASmartPlayer>(FVector(), FRotator());
	OpponentType = EOpponentType::SMART;

	Players.Add(HumanPlayer);
	Players.Add(AI);

	ChoosePlayerAndStartGame();
}

void AUEG_GamemodeBase::ChoosePlayerAndStartGame()
{
	GameInstance->SetMessage(TEXT("Sorteggio..."));

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			AUEG_PlayerController* PlayerController = Cast<AUEG_PlayerController>(GetWorld()->GetFirstPlayerController());
			if (PlayerController)
			{
				AUEG_HUD* HUD = Cast<AUEG_HUD>(PlayerController->GetHUD());
				if (HUD)
				{
					UMyUserWidget* UserWidget = HUD->GetHUDWidget();
					if (UserWidget)
					{
						CurrentPlayer = FMath::RandRange(0, Players.Num() - 1);

						for (int32 IndexI = 0; IndexI < Players.Num(); IndexI++)
						{
							Players[IndexI]->PlayerNumber = IndexI;
						}

						Players[CurrentPlayer]->OnTurn();
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Non esiste UserWidget"));
						return;
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Non esiste HUD"));
					return;
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Non esiste PlayerController"));
				return;
			}
		}, 3, false);
}

void AUEG_GamemodeBase::PlaceTroop(const int32 PlayerNumber, FVector& SpawnLocation)
{
	SpawnLocation.Z += TroopPlacingHeight;
	if (bArcherPlacingTurn)
	{
		ATroop* Troop = nullptr;
		if (PlayerNumber == 0)
		{
			Troop = GetWorld()->SpawnActor<ATroop>(HumanArcherClass, SpawnLocation, FRotator::ZeroRotator);
		}
		else
		{
			Troop = GetWorld()->SpawnActor<ATroop>(IAArcherClass, SpawnLocation, FRotator::ZeroRotator);
		}
		if (Troop != nullptr)
		{
			Troop->SetAsArcher(CurrentPlayer);
			Troop->SetTroopIndex(Players[PlayerNumber]->GetTroops().Num());

			Players[PlayerNumber]->GetTroops().Add(Troop);
			Players[PlayerNumber]->UpdatePlacedArchers();
			Players[PlayerNumber]->IncreaseTroopActionArrayLength();
			GField->GetTileByRelativeLocation(SpawnLocation)->SetTroop(Troop);
			bArcherPlacingTurn = false;
			bKnightPlacingTurn = true;
			for (int32 IndexI = 0; IndexI < Players.Num(); IndexI++)
			{
				if (Players[IndexI]->GetPlacedArchers() < ArchersNumber)
				{
					bArcherPlacingTurn = true;
					bKnightPlacingTurn = false;
				}
			}
			OnTroopHealthChange.Broadcast();
			this->TurnNextPlayer();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Troop has not been spawned."));
			return;
		}
	}
	else if (bKnightPlacingTurn)
	{
		ATroop* Troop;
		if (PlayerNumber == 0)
		{
			Troop = GetWorld()->SpawnActor<ATroop>(HumanKnightClass, SpawnLocation, FRotator::ZeroRotator);
		}
		else
		{
			Troop = GetWorld()->SpawnActor<ATroop>(IAKnightClass, SpawnLocation, FRotator::ZeroRotator);
		}
		if (Troop != nullptr)
		{
			Troop->SetAsKnight(CurrentPlayer);
			Troop->SetTroopIndex(Players[PlayerNumber]->GetTroops().Num());

			Players[PlayerNumber]->GetTroops().Add(Troop);
			Players[PlayerNumber]->UpdatePlacedKnights();
			Players[PlayerNumber]->IncreaseTroopActionArrayLength();
			GField->GetTileByRelativeLocation(SpawnLocation)->SetTroop(Troop);
			bKnightPlacingTurn = false;
			IsGameStarted = true;
			for (int32 IndexI = 0; IndexI < Players.Num(); IndexI++)
			{
				if (Players[IndexI]->GetPlacedKnights() < KnightsNumber)
				{
					bKnightPlacingTurn = true;
					IsGameStarted = false;
				}
			}
			OnTroopHealthChange.Broadcast();
			this->TurnNextPlayer();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Troop has not been spawned."));
			return;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Placing turn is over."));
		return;
	}
}

int32 AUEG_GamemodeBase::GetNextPlayer(int32 Player)
{
	Player++;
	if (!Players.IsValidIndex(Player))
	{
		Player = 0;
	}
	return Player;
}

void AUEG_GamemodeBase::TurnNextPlayer()
{
	CurrentPlayer = GetNextPlayer(CurrentPlayer);
	Players[CurrentPlayer]->OnTurn();
	CurrentTurn++;
	GameInstance->OnTurnChange.Broadcast();
}

bool AUEG_GamemodeBase::IsWinCondition()
{
	UE_LOG(LogTemp, Warning, TEXT("Turno %d: Win Condition Check"), CurrentTurn);

	//Array che segna chi perde (può finire in pareggio la partita se una truppa uccide l'avversaria ma muore per il contraccolpo)
	TArray<bool> IsLoser;
	IsLoser.Init(true, Players.Num());

	//Scorro i giocatori
	for (IPlayerInterface* Player : Players)
	{
		//Scorro le truppe del giocatore Player
		for (ATroop* Troop : Player->GetTroops())
		{
			//Se ha almeno una truppa in vita non è perdente
			if (Troop->GetHealth() > 0)
			{
				IsLoser[Player->PlayerNumber] = false;
				break;
			}
		}
	}

	//Contatore dei perdenti
	int32 LosersCounter = 0;
	//Scorro per contare i perdenti
	for (int32 I = 0; I < IsLoser.Num(); I++)
	{
		if (IsLoser[I] == true)
		{
			LosersCounter++;
		}
	}

	//Se è un pareggio
	if (LosersCounter == IsLoser.Num())
	{
		IsGameStarted = false;
		IsGameOver = true;
		GameInstance->SetMessage(TEXT("Pareggio!"));
	}
	//Se c'è uno sconfitto (e quindi un vincitore)
	else if (LosersCounter == 1)
	{
		IsGameStarted = false;
		IsGameOver = true;
		for (int32 I = 0; I < Players.Num(); I++)
		{
			if (IsLoser[I] == false)
			{
				Players[I]->OnWin();
			}
		}
	}
	//Se non ci sono perdenti
	else if (LosersCounter == 0)
	{
		return false;
	}

	return true;
}

void AUEG_GamemodeBase::SetTroopInMotion(bool Value)
{
	bTroopInMotion = Value;
}

bool AUEG_GamemodeBase::IsTroopInMotion()
{
	return bTroopInMotion;
}

void AUEG_GamemodeBase::ResetGamemodeBase()
{
	CurrentTurn = 0;
	bArcherPlacingTurn = true;
	bKnightPlacingTurn = false;
	IsGameStarted = false;
	IsGameOver = false;
}

