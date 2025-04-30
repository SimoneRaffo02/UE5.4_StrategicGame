// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcherReachableTileDetails.h"
#include "UEG_GamemodeBase.h"

UArcherReachableTileDetails::UArcherReachableTileDetails()
{
}

UArcherReachableTileDetails::~UArcherReachableTileDetails()
{
}

void UArcherReachableTileDetails::AddTile(ATile& NewTile, ATroop& CurrentTroop, AUEG_GamemodeBase& GamemodeBase)
{
	ITroopReachableTileDetails::AddTile(NewTile, CurrentTroop, GamemodeBase);

	//Memorizzo la tile passata come parametro
	Tile = &NewTile;

	//Mi memorizzo la posizione della truppa
	FVector TroopLocation = CurrentTroop.GetActorLocation();

	//Posiziono la truppa sulla tile che sto analizzando
	FVector TmpLocation = Tile->GetActorLocation();
	TmpLocation.Z = GamemodeBase.TroopPlacingHeight;
	GamemodeBase.GetGameField()->GetTileByRelativeLocation(TroopLocation)->SetTroop(nullptr);
	GamemodeBase.GetGameField()->GetTileByRelativeLocation(TroopLocation)->SetTileStatus(ETileStatus::FREE);
	CurrentTroop.SetActorLocation(TmpLocation);
	Tile->SetTroop(&CurrentTroop);
	Tile->SetTileStatus(ETileStatus::OCCUPIED);

	for (int32 I = 0; I < GamemodeBase.Players[0]->GetTroops().Num(); I++)
	{
		GamemodeBase.GetGameField()->EvaluatePossibleMoves(GamemodeBase.Players[0]->GetTroops()[I]);
		
		if (NewTile.GetMoveType() == EMoveType::ATTACK)
		{
			IsInAttackRangeOfTroop.Add(true);
		}
		else
		{
			IsInAttackRangeOfTroop.Add(false);
		}
		GamemodeBase.GetGameField()->RefreshGameField();
		
	}
	//Riposiziono la pedina nella posizione iniziale
	Tile->SetTroop(nullptr);
	Tile->SetTileStatus(ETileStatus::FREE);
	CurrentTroop.SetActorLocation(TroopLocation);
	GamemodeBase.GetGameField()->GetTileByRelativeLocation(TroopLocation)->SetTroop(&CurrentTroop);
	GamemodeBase.GetGameField()->GetTileByRelativeLocation(TroopLocation)->SetTileStatus(ETileStatus::OCCUPIED);
}

void UArcherReachableTileDetails::SearchAttackableEnemies(ATroop& AttackingTroop, AUEG_GamemodeBase& GamemodeBase)
{
	//Mi memorizzo la posizione della truppa
	FVector TroopLocation = AttackingTroop.GetActorLocation();

	//Posiziono la truppa sulla tile che sto analizzando
	FVector TmpLocation = Tile->GetActorLocation();
	TmpLocation.Z = GamemodeBase.TroopPlacingHeight;
	GamemodeBase.GetGameField()->GetTileByRelativeLocation(TroopLocation)->SetTroop(nullptr);
	GamemodeBase.GetGameField()->GetTileByRelativeLocation(TroopLocation)->SetTileStatus(ETileStatus::FREE);
	AttackingTroop.SetActorLocation(TmpLocation);
	Tile->SetTroop(&AttackingTroop);
	Tile->SetTileStatus(ETileStatus::OCCUPIED);

	//Trovo le caselle su cui può attaccare
	GamemodeBase.GetGameField()->EvaluatePossibleMoves(&AttackingTroop);

	//Vedo quali truppe avversarie sono in pedine attaccabili e memorizzo la distanza tra la casella e il nemico
	for (ATroop* Troop : GamemodeBase.Players[0]->GetTroops())
	{
		if (Troop->GetHealth() > 0)
		{
			if (GamemodeBase.GetGameField()->GetTileByRelativeLocation(Troop->GetActorLocation())->GetMoveType() == EMoveType::ATTACK)
			{
				AttackableEnemies.Add(Troop);
				int32 Col = Tile->GetGridLocation().X;
				int32 Row = Tile->GetGridLocation().Y;
				pair<int32, int32> Dest = make_pair(GamemodeBase.GetGameField()->GetGridLocationByRelativeLocation(Troop->GetActorLocation()).X, GamemodeBase.GetGameField()->GetGridLocationByRelativeLocation(Troop->GetActorLocation()).Y);
				EnemyDistance.Add(GamemodeBase.GetGameField()->GetManhattanDistance(Col, Row, Dest));
			}
		}
		
	}
	GamemodeBase.GetGameField()->RefreshGameField();

	//Riposiziono la pedina nella posizione iniziale
	Tile->SetTroop(nullptr);
	Tile->SetTileStatus(ETileStatus::FREE);
	AttackingTroop.SetActorLocation(TroopLocation);
	GamemodeBase.GetGameField()->GetTileByRelativeLocation(TroopLocation)->SetTroop(&AttackingTroop);
	GamemodeBase.GetGameField()->GetTileByRelativeLocation(TroopLocation)->SetTileStatus(ETileStatus::OCCUPIED);
}
