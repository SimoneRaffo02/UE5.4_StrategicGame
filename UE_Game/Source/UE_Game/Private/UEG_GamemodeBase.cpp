// Fill out your copyright notice in the Description page of Project Settings.

#include "UEG_GamemodeBase.h"
#include "RandomPlayer.h"
#include "UEG_PlayerController.h"
#include "HumanPlayer.h"
#include "EngineUtils.h"

AUEG_GamemodeBase::AUEG_GamemodeBase()
{
	PlayerControllerClass = AUEG_PlayerController::StaticClass();
	DefaultPawnClass = AHumanPlayer::StaticClass();
	
	FieldSize = 25;
	ObstaclesPercentage = 50;
	TroopPlacingHeight = 20;

	StepTime = 0.01;
	StepsForTile = 10;

	CurrentTurn = 0;
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
	}

	float CameraPosX = (GField->GetTileSize() * FieldSize - GField->GetTileSize()) / 2;
	float Zposition = (GField->GetTileSize() * FieldSize - GField->GetTileSize()) / 2 + GField->GetTileSize() * 1.5f;
	FVector CameraPos(CameraPosX, CameraPosX, Zposition);
	HumanPlayer->SetActorLocationAndRotation(CameraPos, FRotator(-90, -90, 0));

	//Random Player
	ARandomPlayer* AI = GetWorld()->SpawnActor<ARandomPlayer>(FVector(), FRotator());

	Players.Add(HumanPlayer);
	Players.Add(AI);
	this->ChoosePlayerAndStartGame();
}

void AUEG_GamemodeBase::ChoosePlayerAndStartGame()
{
	CurrentPlayer = 0;
	//CurrentPlayer = FMath::RandRange(0, Players.Num() - 1);

	for (int32 IndexI = 0; IndexI < Players.Num(); IndexI++)
	{
		Players[IndexI]->PlayerNumber = IndexI;
	}

	Players[CurrentPlayer]->OnTurn();
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
			Troop->SetMovement(3);
			Troop->SetAttackType(EAttackType::RANGED);
			Troop->SetAttackRange(10);
			Troop->SetMinAttackDamage(4);
			Troop->SetMaxAttackDamage(8);
			Troop->SetHealth(20);

			Players[PlayerNumber]->GetTroops().Add(Troop);
			Players[PlayerNumber]->UpdatePlacedArchers();
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
			Troop->SetMovement(6);
			Troop->SetAttackType(EAttackType::MELEE);
			Troop->SetAttackRange(1);
			Troop->SetMinAttackDamage(1);
			Troop->SetMaxAttackDamage(6);
			Troop->SetHealth(40);

			Players[PlayerNumber]->GetTroops().Add(Troop);
			Players[PlayerNumber]->UpdatePlacedKnights();
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
}

