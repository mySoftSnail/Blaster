// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocket.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Sound/SoundCue.h"
#include "Components/BoxComponent.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "RocketMovementComponent.h"

AProjectileRocket::AProjectileRocket()
{
	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rocket Mesh"));
	RocketMesh->SetupAttachment(RootComponent);
	RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // RocketMesh is purely cosmetic.

	RocketMovementComponent = CreateDefaultSubobject<URocketMovementComponent>(TEXT("RocketMovementComponent"));
	RocketMovementComponent->bRotationFollowsVelocity = true;
	RocketMovementComponent->SetIsReplicated(true);
}

void AProjectileRocket::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) // now we're binding all of the client projectiles OnHit functions
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectileRocket::OnHit);
	}

	if (TrailSystem)
	{
		TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TrailSystem,
			GetRootComponent(),
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false // the reason is i like to deactivate system manually
		);
	}

	if (ProjectileLoop && LoopingSoundAttenuation)
	{
		ProjectileLoopComponent = UGameplayStatics::SpawnSoundAttached(
			ProjectileLoop,
			GetRootComponent(),
			FName(),
			GetActorLocation(),
			EAttachLocation::KeepWorldPosition,
			false,
			1.f,
			1.f,
			0.f,
			LoopingSoundAttenuation,
			(USoundConcurrency*)nullptr,
			false
		);
	}
}

void AProjectileRocket::DestroyTimerFinished()
{
	Destroy();
}

void AProjectileRocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == GetOwner())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Hit self"));
		return; // ProjectileMovementComponent는 OnHit 이벤트 발생 시 멈추게 되어 있으므로 제자리에 로켓이 멈추게 됨 
		// 그래서 로켓 프로젝타일을 위한, OnHit 이벤트에도 멈추지 않는 커스텀 무브먼트 컴포넌트를 만들 것임
	}

	// now, when applying damage, we need to have the controller of the player who fired the rocket as that is the controller of our instigator. GetInstigator() returns us the pawn that owns this rocket.
	// and just before we call SpawnActor in ProjectileWeapon.cpp, we create FActorSpawnParameters and we set SpawnParams.Owner = GetOwner() (The owner of the weapon) and SpawnParams.Instigator = InstigatorPawn (Cast of GetOwner())

	APawn* FiringPawn = GetInstigator();
	if (FiringPawn)
	{
		AController* FiringController = FiringPawn->GetController();
		if (FiringController && HasAuthority())
		{
			// ApplyRadialDamageWithFalloff: this way our rocket will damage anything within the vicinity, and we can have a falloff for that damage.
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				this, // World context object
				Damage, // BaseDamage
				10.f, // MinimumDamage
				// Max: BaseDamage, Min: MinimumDamage
				GetActorLocation(), // Origin: Center of two radii
				200.f, // DamageInnerRadius
				500.f, // DamageOuterRadius
				1.f, // DamageFalloff: exponent of the falloff function. we can simply use 1.f and we'll have a linear damage falloff. this just means that the damage will decrease steadily.
				UDamageType::StaticClass(), // Damage Type Class
				TArray<AActor*>(), // IgnoreActors: we have an array of actors to ignore. e.g. you could make it so that this rocket does not damage the character that fired it. But i'd like this rocket to damage anything that's close enough, including the player who fired the rocket. so i'm going to satisfy this input by passing in an empty array of actor pointers. 
				this, // damage causer
				FiringController // InstigatorController: That way in ReceiveDamage, we'll have access to that controller. as we're passing that information on to the GameMode. 
				
				);
			/*
			ApplyRadialDamageWithFalloff is going to have two radii(DamageInnerRadius, DamageOuterRadius).
			Any actors that are damaged within the OuterRadius are going to at least receive the MinimumDamage amount that we specify here. and any actors within the InnerDamageRadius will receive the full amount of BaseDamage. and there will be a falloff between these two radii.
			*/
		}
	}

	GetWorldTimerManager().SetTimer(
		DestroyTimer,
		this,
		&AProjectileRocket::DestroyTimerFinished,
		DestroyTime
	);

	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
	}

	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if (RocketMesh)
	{
		RocketMesh->SetVisibility(false);
	}
	if (CollisionBox)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (TrailSystemComponent && TrailSystemComponent->GetSystemInstanceController())
	{
		TrailSystemComponent->GetSystemInstanceController()->Deactivate();
	}
	if (ProjectileLoopComponent && ProjectileLoopComponent->IsPlaying())
	{
		ProjectileLoopComponent->Stop();
	}

	// 로켓 프로젝타일의 스모크트레일이 바로 지워지지 않고 딜레이를 갖게 하기 위해 부모 프로젝타일 클래스와는 Destroy 과정을 다르게 구현함
}

void AProjectileRocket::Destroyed()
{
	// here we can simply NOT call Super::Destroyed()

}