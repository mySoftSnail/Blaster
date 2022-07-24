// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int NumberOfPlayers = GameState.Get()->PlayerArray.Num();

	if (NumberOfPlayers == 2)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			/*
			Now, server travel takes a address to travel to. And if we're calling this from the server and as we'll learn later on,
			the game mode only exists on the server. So we are definitely on the server if we're in the game mode class.
			we can simply pass a path ot the level we'd like to travel to and all connected clients will travel to that level.

			this is going to be BlasterMap, but we neeed to travel to this level and designate it to be a listen server.
			So we use a question mark to add additional options to the address. And the option we need is listen.
			*/

			// We'd like to travel seamlessly, and we can trabel seamlessly by setting the boolean variable that exists on the game mode class called bUseSeamlessTravel.
			bUseSeamlessTravel = true;
			World->ServerTravel(FString("/Game/Maps/BlasterMap?listen"));
		}
	}
}
