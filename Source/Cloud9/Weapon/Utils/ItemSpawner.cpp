// Copyright (c) 2023 Alexei Gladkikh


#include "ItemSpawner.h"

#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Cloud9/Character/Cloud9Character.h"

#include "Cloud9/Weapon/Classes/Cloud9WeaponBase.h"

FName AItemSpawner::RootComponentName = "RootComponent";
FName AItemSpawner::TriggerBoxComponentName = "TriggerBox";
FName AItemSpawner::WeaponSampleComponentName = "WeaponInitializer";
FName AItemSpawner::GlowingEffectComponentName = "GlowingEffect";
FName AItemSpawner::SpriteComponentName = "Sprite";

AItemSpawner::AItemSpawner()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(RootComponentName);

	TriggerBoxComponent = CreateDefaultSubobject<UBoxComponent>(TriggerBoxComponentName);
	ItemSampleComponent = CreateDefaultSubobject<UChildActorComponent>(WeaponSampleComponentName);
	GlowingEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(GlowingEffectComponentName);

	TriggerBoxComponent->SetupAttachment(RootComponent);
	ItemSampleComponent->SetupAttachment(RootComponent);
	GlowingEffectComponent->SetupAttachment(RootComponent);

	bIsDestroyOnActivation = false;
	GlowingEffect = nullptr;
	bIsGlowingEffectPreview = true;
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
		if (ActivateSpawner(Character) and bIsDestroyOnActivation)
		{
			Destroy();
		}
	}
}
