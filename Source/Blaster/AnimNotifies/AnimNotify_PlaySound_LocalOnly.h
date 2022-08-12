// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify_PlaySound.h"
#include "AnimNotify_PlaySound_LocalOnly.generated.h"

/**
 * 
 */
UCLASS(const, hidecategories = Object, collapsecategories, Config = Game, meta = (DisplayName = "Play Sound LocalOnly"))
class BLASTER_API UAnimNotify_PlaySound_LocalOnly : public UAnimNotify_PlaySound
{
	GENERATED_BODY()
	
public:
	UAnimNotify_PlaySound_LocalOnly();

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
};
