// Copyright (c) 2023 Alexei Gladkikh
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#include "ItemSpawner.h"

#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Game/Cloud9DeveloperSettings.h"

#include "Cloud9/Weapon/Classes/Cloud9WeaponBase.h"
#include "Cloud9/Weapon/Sounds/Cloud9SoundPlayer.h"

FName AItemSpawner::RootComponentName = "RootComponent";
FName AItemSpawner::TriggerBoxComponentName = "TriggerBox";
FName AItemSpawner::ItemSampleComponentName = "ItemSample";
FName AItemSpawner::GlowingEffectComponentName = "GlowingEffect";
FName AItemSpawner::SpriteComponentName = "Sprite";

AItemSpawner::AItemSpawner()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(RootComponentName);

	RootComponent->SetMobility(EComponentMobility::Movable);

	TriggerBoxComponent = CreateDefaultSubobject<UBoxComponent>(TriggerBoxComponentName);
	ItemSampleComponent = CreateDefaultSubobject<UChildActorComponent>(ItemSampleComponentName);
	GlowingEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(GlowingEffectComponentName);

	TriggerBoxComponent->SetupAttachment(RootComponent);
	ItemSampleComponent->SetupAttachment(RootComponent);
	GlowingEffectComponent->SetupAttachment(RootComponent);

	log(Error, "ATTACHEMENT DONE %s", *GetName())

	bIsDestroyOnActivation = false;
	GlowingEffect = nullptr;
	bIsGlowingEffectPreview = true;
	SampleOffset = FVector::ZeroVector;
	SampleRotator = FRotator::ZeroRotator;
	SampleScale = 1.5f;
	RotationSpeedInDegree = 0.0f;
	bIsRandomInitialRotation = false;
	ZoneSize = {20.0f, 20.0f, 20.0f};

	TriggerBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemSpawner::OnBeginOverlap);
}

AActor* AItemSpawner::CreateChildActor() { return nullptr; }

bool AItemSpawner::ActivateSpawner(ACloud9Character* Character) { return false; }

void AItemSpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	let ItemSample = CreateChildActor();

	if (not IsValid(ItemSample))
	{
		log(Error, "[ItemSpawner=%s] Can't create spawner sample", *GetName());
		return;
	}

	ItemSample->SetActorTickEnabled(false);
	ItemSample->UpdateComponentTransforms();
	ItemSample->SetActorEnableCollision(false);
	ItemSample->SetActorRelativeLocation(SampleOffset);
	ItemSample->SetActorRelativeRotation(SampleRotator);
	ItemSample->SetActorScale3D({SampleScale, SampleScale, SampleScale});

	TriggerBoxComponent->SetBoxExtent(ZoneSize);

	// Remove previous set glowing effect (required if preview disabled)
	GlowingEffectComponent->SetAsset(nullptr);

	if (IsValid(GlowingEffect) and bIsGlowingEffectPreview)
	{
		GlowingEffectComponent->SetAsset(GlowingEffect);
	}
}

void AItemSpawner::BeginPlay()
{
	Super::BeginPlay();

	let ItemSample = ItemSampleComponent->GetChildActor();

	if (not IsValid(ItemSample))
	{
		log(Error, "[AItemSpawnerBase=%s] Invalid spawner", *GetName());
		return;
	}

	if (bIsRandomInitialRotation)
	{
		var Rotation = ItemSample->GetActorRotation();
		Rotation.Yaw = FMath::RandRange(0.0f, 360.0f);
		ItemSample->SetActorRotation(Rotation);
	}

	if (IsValid(GlowingEffect) and not bIsGlowingEffectPreview)
	{
		GlowingEffectComponent->SetAsset(GlowingEffect);
	}
}

void AItemSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (let ItemSample = ItemSampleComponent->GetChildActor(); IsValid(ItemSample))
	{
		var Rotation = ItemSample->GetActorRotation();
		Rotation.Yaw += DeltaTime * RotationSpeedInDegree;
		ItemSample->SetActorRotation(Rotation);
	}
}

void AItemSpawner::OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& Hit)
{
	if (let Character = Cast<ACloud9Character>(OtherActor);
		IsValid(Character) and IsValid(ItemSampleComponent->GetChildActor()))
	{
		if (ActivateSpawner(Character))
		{
			let Settings = UCloud9DeveloperSettings::Get();

			UCloud9SoundPlayer::PlayRandomSound(ActivationSounds, GetActorLocation(), Settings->Volume);

			if (bIsDestroyOnActivation)
			{
				Destroy();
			}
		}
	}
}
