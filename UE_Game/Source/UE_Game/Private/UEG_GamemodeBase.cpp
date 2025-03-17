// Fill out your copyright notice in the Description page of Project Settings.

#include "UEG_GamemodeBase.h"
#include "UEG_PlayerController.h"
#include "HumanPlayer.h"
#include "EngineUtils.h"

AUEG_GamemodeBase::AUEG_GamemodeBase()
{
	PlayerControllerClass = AUEG_PlayerController::StaticClass();
	DefaultPawnClass = AHumanPlayer::StaticClass();
	FieldSize = 25;
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
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Game Field is null"));
	}

	float CameraPosX = (GField->GetTileSize() * FieldSize - GField->GetTileSize()) / 2;
	float Zposition = (GField->GetTileSize() * FieldSize - GField->GetTileSize()) / 2 + GField->GetTileSize() * 1.5f;
	FVector CameraPos(CameraPosX, CameraPosX, Zposition);
	HumanPlayer->SetActorLocationAndRotation(CameraPos, FRotator(-90, 0, 0));

	Players.Add(HumanPlayer);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("BeginPlay!"));
	this->ChoosePlayerAndStartGame();
}

void AUEG_GamemodeBase::ChoosePlayerAndStartGame()
{
	//Da rendere randomico
	CurrentPlayer = 0;

	for (int32 IndexI = 0; IndexI < Players.Num(); IndexI++)
	{
		Players[IndexI]->PlayerNumber = IndexI;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("ChoosePlayerAndStartGame!"));

	//FVector SpawnLocation = FVector(0, 0, 20);
	//PlaceTroop(0, SpawnLocation);

	Players[CurrentPlayer]->OnTurn();
}

void AUEG_GamemodeBase::PlaceTroop(const int32 PlayerNumber, FVector& SpawnLocation)
{
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
			Troop->SetAttackType(TEXT("Attacco a distanza"));
			Troop->SetAttackRange(10);
			Troop->SetMinAttackDamage(4);
			Troop->SetMaxAttackDamage(8);
			Troop->SetHealth(20);

			Players[PlayerNumber]->GetTroops().Add(Troop);
			Players[PlayerNumber]->UpdatePlacedArchers();
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
			Troop->SetAttackType(TEXT("Attacco a corto raggio"));
			Troop->SetAttackRange(1);
			Troop->SetMinAttackDamage(1);
			Troop->SetMaxAttackDamage(6);
			Troop->SetHealth(40);

			Players[PlayerNumber]->GetTroops().Add(Troop);
			Players[PlayerNumber]->UpdatePlacedArchers();
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
	//Players[CurrentPlayer]->OnTurn();
}

