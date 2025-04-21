// Fill out your copyright notice in the Description page of Project Settings.


#include "HumanPlayer.h"
#include "Tile.h"
#include "UEG_GamemodeBase.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AHumanPlayer::AHumanPlayer() : Super()
{

	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetRelativeRotation(FRotator(0, 0, 0));
	Camera->FieldOfView = 120;

	SetRootComponent(Camera);

	PlayerNumber = 0;

	SelectedTroop = nullptr;
}

void AHumanPlayer::OnTurn()
{
	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());
	GamemodeBase->GetGameField()->RefreshGameField();

	IsMyTurn = true;
	ResetTroopsActions();
	ResetTroopActionArray();

	if (GameInstance)
	{
		if (GamemodeBase->bArcherPlacingTurn)
		{
			GameInstance->SetMessage(TEXT("Piazzamento Arciere del Giocatore"));
		}
		else if (GamemodeBase->bKnightPlacingTurn)
		{
			GameInstance->SetMessage(TEXT("Piazzamento Cavaliere del Giocatore"));
		}
		else if (GamemodeBase->IsGameStarted)
		{
			GameInstance->SetMessage(TEXT("Turno Giocatore"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Human Game Instance non creata"));
	}
}

void AHumanPlayer::OnWin()
{
	if (GameInstance)
	{
		GameInstance->SetMessage(TEXT("Vince Giocatore!"));
		GameInstance->IncreasePlayerScore();
	}
}

void AHumanPlayer::OnLose()
{
}

void AHumanPlayer::Attack(ATroop& PlayerTroop, ATroop& EnemyTroop)
{
	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());
	int32 Damage = PlayerTroop.Attack(EnemyTroop);
	GameInstance->AddAttackToHistory(0, PlayerTroop.GetAttackType(), GamemodeBase->GetGameField()->GetTileName(GamemodeBase->GetGameField()->GetTileByRelativeLocation(EnemyTroop.GetActorLocation())), Damage);
	GamemodeBase->GetGameField()->RefreshGameField();
	GamemodeBase->OnTroopHealthChange.Broadcast();
	bTroopMadeAction[PlayerTroop.GetTroopIndex()] = true;
	PlayerTroop.SetMoved(true);
	PlayerTroop.SetAttacked(true);
}

void AHumanPlayer::Move(ATroop* Troop, ATile* Tile)
{
	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());
	TArray<pair<int32, int32>> Path;
	pair<int32, int32> Src = make_pair(GamemodeBase->GetGameField()->GetGridLocationByRelativeLocation(SelectedTroop->GetActorLocation()).X, GamemodeBase->GetGameField()->GetGridLocationByRelativeLocation(SelectedTroop->GetActorLocation()).Y);
	pair<int32, int32> Dest = make_pair(Tile->GetGridLocation().X, Tile->GetGridLocation().Y);
	GamemodeBase->GetGameField()->AStar(Src, Dest, Path);
	GamemodeBase->GetGameField()->DrawPath(Path);
	SelectedTroop->SetCurrentPath(Path);
	GamemodeBase->GetGameField()->RefreshGameField();
	GameInstance->AddMoveToHistory(0, SelectedTroop->GetAttackType(), GamemodeBase->GetGameField()->GetTileName(GamemodeBase->GetGameField()->GetTile(Src.first, Src.second)), GamemodeBase->GetGameField()->GetTileName(GamemodeBase->GetGameField()->GetTile(Dest.first, Dest.second)));

	bTroopMadeAction[SelectedTroop->GetTroopIndex()] = true;
}

void AHumanPlayer::EndTurn()
{
	SelectedTroop = nullptr;
	IsMyTurn = false;
}

int32 AHumanPlayer::NextTroop()
{
	return -1;
}

void AHumanPlayer::Action()
{
}

void AHumanPlayer::ResetPlayer()
{
	IPlayerInterface::ResetPlayer();

	bTroopMadeAction.Empty();
	SelectedTroop = nullptr;
}

void AHumanPlayer::ResetTroopsActions()
{
	for (ATroop* Troop : Troops)
	{
		Troop->SetMoved(false);
		Troop->SetAttacked(false);
	}
}

ATroop* AHumanPlayer::GetSelectedTroop()
{
	return SelectedTroop;
}

// Called when the game starts or when spawned
void AHumanPlayer::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UUEG_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());

	if (GamemodeBase)
	{
		GamemodeBase->OnReset.AddDynamic(this, &AHumanPlayer::ResetPlayer);
	}
}

// Called every frame
void AHumanPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AHumanPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AHumanPlayer::OnClick()
{
	FHitResult Hit = FHitResult(ForceInit);

	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit);

	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());

	if (Hit.bBlockingHit && IsMyTurn && !GamemodeBase->IsTroopInMotion())
	{
		if (GamemodeBase->bArcherPlacingTurn || GamemodeBase->bKnightPlacingTurn)
		{
			if (ATile* CurrentTile = Cast<ATile>(Hit.GetActor()))
			{
				if (CurrentTile->GetTileStatus() == ETileStatus::FREE)
				{
					IsMyTurn = false;
					CurrentTile->SetTileStatus(ETileStatus::OCCUPIED);
					FVector SpawnLocation = CurrentTile->GetActorLocation();
					GamemodeBase->PlaceTroop(PlayerNumber, SpawnLocation);
				}
			}
		}
		else if (GamemodeBase->IsGameStarted)
		{
			//Selezione truppa
			if (ATroop* CurrentTroop = Cast<ATroop>(Hit.GetActor()))
			{
				if (Troops.Contains(CurrentTroop) && !CurrentTroop->GetAttacked())
				{
					if (CurrentTroop == SelectedTroop)
					{
						CurrentTroop->SetSelected(false);
						SelectedTroop = nullptr;
						GamemodeBase->GetGameField()->RefreshGameField();
						UE_LOG(LogTemp, Log, TEXT("Cliccato due volte"));
					}
					else
					{
						if (SelectedTroop != nullptr)
						{
							for (ATroop* Troop : Troops)
							{
								if (Troop->IsSelected())
								{
									Troop->SetSelected(false);
								}
							}
							GamemodeBase->GetGameField()->ResetTilesMoveType();
						}
						CurrentTroop->SetSelected(true);
						SelectedTroop = CurrentTroop;
						GamemodeBase->GetGameField()->EvaluatePossibleMoves(CurrentTroop);
						if (SelectedTroop->GetMoved())
						{
							GamemodeBase->GetGameField()->FilterAttackTiles();
						}
						GamemodeBase->GetGameField()->SetTilesMaterial();
					}
				}
				else if (GamemodeBase->Players[GamemodeBase->GetNextPlayer(GamemodeBase->CurrentPlayer)]->GetTroops().Contains(CurrentTroop) && GamemodeBase->GetGameField()->GetTileByRelativeLocation(CurrentTroop->GetActorLocation())->GetMoveType() == EMoveType::ATTACK)
				{
					Attack(*SelectedTroop, *CurrentTroop);
					if (!GamemodeBase->IsWinCondition() && EveryTroopAttacked())
					{
						GamemodeBase->TurnNextPlayer();
						EndTurn();
					}
				}
			}
			else if (ATile* CurrentTile = Cast<ATile>(Hit.GetActor()))
			{
				if (SelectedTroop != nullptr)
				{
					if (CurrentTile->GetMoveType() == EMoveType::MOVE && !SelectedTroop->GetMoved())
					{
						SelectedTroop->SetMoved(true);
						Move(nullptr, CurrentTile);
					}
					if (CurrentTile->GetMoveType() == EMoveType::ATTACK && !SelectedTroop->GetAttacked())
					{
						ATroop* EnemyTroop = CurrentTile->GetTroop();
						Attack(*SelectedTroop, *EnemyTroop);
						if (!GamemodeBase->IsWinCondition() && EveryTroopAttacked())
						{
							GamemodeBase->TurnNextPlayer();
							EndTurn();
						}
					}
				}
			}
		}
	}
}

