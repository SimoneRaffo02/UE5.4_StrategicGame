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

void IPlayerInterface::ResetPlayer()
{
	PlacedArchers = 0;
	PlacedKnights = 0;
	Troops.Empty();
}
