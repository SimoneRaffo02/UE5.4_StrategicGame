// Fill out your copyright notice in the Description page of Project Settings.


#include "TroopReachableTileDetails.h"
#include "UEG_GamemodeBase.h"

void ITroopReachableTileDetails::Init(ATile& NewTile, ATroop& CurrentTroop, AUEG_GamemodeBase& GamemodeBase)
{
	AddTile(NewTile, CurrentTroop, GamemodeBase);
	SearchAttackableEnemies(CurrentTroop, GamemodeBase);
}

int32 ITroopReachableTileDetails::CountInAttackRangeOfEnemy()
{
	int32 Count = 0;
	
	for (int32 I = 0; I < IsInAttackRangeOfTroop.Num(); I++)
	{
		if (IsInAttackRangeOfTroop[I])
		{
			Count++;
		}
	}

	return Count;
}

int32 ITroopReachableTileDetails::GetMinEnemyDistance()
{
	int32 MinDistance = MAX_int32;

	for (int32 I = 0; I < EnemyDistance.Num(); I++)
	{
		if (EnemyDistance[I] < MinDistance)
		{
			MinDistance = EnemyDistance[I];
		}
	}

	return MinDistance;
}

//Riordino i nemici in base ai seguenti criteri
//- nemico con più vita uccidibile in un colpo sicuro
//- nemico con meno vita
//- nemico con più danno medio
void ITroopReachableTileDetails::SortAttackableEnemies(ATroop& Troop)
{
	for (int32 I = 0; I < AttackableEnemies.Num() - 1; I++)
	{
		for (int32 J = I + 1; J < AttackableEnemies.Num(); J++)
		{
			if (AttackableEnemies[J]->GetHealth() <= Troop.GetMinAttackDamage() && AttackableEnemies[I]->GetHealth() > Troop.GetMinAttackDamage())
			{
				ATroop* tmpVar = AttackableEnemies[J];
				AttackableEnemies[J] = AttackableEnemies[I];
				AttackableEnemies[I] = tmpVar;
			}
			else if (!(AttackableEnemies[J]->GetHealth() > Troop.GetMinAttackDamage() && AttackableEnemies[I]->GetHealth() <= Troop.GetMinAttackDamage()))
			{
				if (AttackableEnemies[J]->GetHealth() < AttackableEnemies[I]->GetHealth())
				{
					ATroop* tmpVar = AttackableEnemies[J];
					AttackableEnemies[J] = AttackableEnemies[I];
					AttackableEnemies[I] = tmpVar;
				}
				else if (AttackableEnemies[J]->GetHealth() == AttackableEnemies[I]->GetHealth())
				{
					float MeanEnemyJAttack = (AttackableEnemies[J]->GetMinAttackDamage() + AttackableEnemies[J]->GetMaxAttackDamage()) / 2.f;
					float MeanEnemyIAttack = (AttackableEnemies[I]->GetMinAttackDamage() + AttackableEnemies[I]->GetMaxAttackDamage()) / 2.f;
					if (MeanEnemyJAttack < MeanEnemyIAttack)
					{
						ATroop* tmpVar = AttackableEnemies[J];
						AttackableEnemies[J] = AttackableEnemies[I];
						AttackableEnemies[I] = tmpVar;
					}
				}
			}
		}
	}
}
