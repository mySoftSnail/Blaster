// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"

ABlasterGameMode::ABlasterGameMode()
{
	bDelayedStart = true;
	/*
	Now, when we set this to true, the game mode will stay in the WaitingToStart state and it'll actually spawn a default pawn for all players and they can use that pawn to fly around the level.
	There won't be a mesh or anything like that. and the plyaers cna fly until we call StartMatch, at which point the game mode will spawn the pawns that we specified as the default pawn class in the game mode.
	*/
}

void ABlasterGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();
	/*
	And now we know how much time it took from the timie of launching the game to actually entering the BlasterMap.
	as BlasterGameMode is only used in the BlasterMap and not in the GameStartupMap where we have the game menu.
	*/
}

void ABlasterGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime; // So this gives us an accurate representation of countdown time.
		/*
		Now keep in mind that GetWorld()->GetTimeSeconds() starts counting as soon as the game is launched up.
		That means if we launch up in the opening menu, that time is already ticking away.
		So by the time we acually click 'Host' and go into the BlasterMap, this value is no longer zero.
		So what we should do is get our LevelStartingTime, which is the current time as soon as we've entered the BlasterMap.
		*/

		if (CountdownTime <= 0.f)
		{
			StartMatch();
			/*
			StartMatch will result in the game mode transitioning to the InProgress state, thus spawning all player characters and allowing all the players to actually control their characters, run around, pick up weapons and start shooting each other.
			*/
		}
	}
}

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
	ABlasterPlayerState* AttackerPlayerState = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
	ABlasterPlayerState* VictimPlayerState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;

	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.f);
	}

	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}

	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
	}
}

void ABlasterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset(); // it detaches the character from the controller and calls UnPossess for the controller
		ElimmedCharacter->Destroy();
	}

	if (ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}

