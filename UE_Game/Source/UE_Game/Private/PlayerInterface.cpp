// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInterface.h"

// Add default functionality here for any IPlayerInterface functions that are not pure virtual.

IPlayerInterface::IPlayerInterface()
{
}

int32 IPlayerInterface::GetPlacedArchers()
{
	return PlacedArchers;
}

int32 IPlayerInterface::GetPlacedKnights()
{
	return PlacedKnights;
}

TArray<ATroop*>& IPlayerInterface::GetTroops()
{
	return Troops;
}

void IPlayerInterface::UpdatePlacedArchers()
{
	PlacedArchers++;
}

void IPlayerInterface::UpdatePlacedKnights()
{
	PlacedKnights++;
}

bool IPlayerInterface::EveryTroopMadeAction()
{
	for (int32 I = 0; I < bTroopMadeAction.Num(); I++)
	{
		if (bTroopMadeAction[I] == false && Troops[I]->GetHealth() > 0)
		{
			return false;
		}
	}
	return true;
}

bool IPlayerInterface::EveryTroopAttacked()
{
	for (ATroop* Troop : Troops)
	{
		if (!Troop->GetAttacked() && Troop->GetHealth() > 0)
		{
			return false;
		}
	}
	return true;
}

void IPlayerInterface::IncreaseTroopActionArrayLength()
{
	bTroopMadeAction.Add(false);
}

void IPlayerInterface::ResetTroopActionArray()
{
	for (int32 I = 0; I < bTroopMadeAction.Num(); I++)
	{
		bTroopMadeAction[I] = false;
	}
}

void IPlayerInterface::ResetPlayer()
{
	PlacedArchers = 0;
	PlacedKnights = 0;
	Troops.Empty();
}
