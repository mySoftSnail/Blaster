// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_PlaySound_LocalOnly.h"
#include "Blaster/Character/BlasterCharacter.h"

UAnimNotify_PlaySound_LocalOnly::UAnimNotify_PlaySound_LocalOnly()
{
}

void UAnimNotify_PlaySound_LocalOnly::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(MeshComp->GetOwner());
	UWorld* World = MeshComp->GetWorld();
	if (BlasterCharacter && BlasterCharacter->IsLocallyControlled()||
		World && World->WorldType == EWorldType::EditorPreview)
	{
		Super::Notify(MeshComp, Animation, EventReference);
	}
}
