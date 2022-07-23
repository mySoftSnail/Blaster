// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * It needs to see how many players have connected to the lobby level or travled to it.
 * And once a cretain number have come in, then we can travel to the actual game map that we're going to create.
 */
UCLASS()
class BLASTER_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	virtual void PostLogin(APlayerController* NewPlayer) override;

};
