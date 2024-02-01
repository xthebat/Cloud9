// Copyright (c) 2023 Alexei Gladkikh


#include "WeaponSpawner.h"

#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"

#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponBase.h"

FName AWeaponSpawner::RootComponentName = "RootComponent";
FName AWeaponSpawner::TriggerBoxComponentName = "TriggerBox";
FName AWeaponSpawner::WeaponSampleComponentName = "WeaponInitializer";
FName AWeaponSpawner::GlowingEffectComponentName = "GlowingEffect";
FName AWeaponSpawner::SpriteComponentName = "Sprite";

AWeaponSpawner::AWeaponSpawner()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(RootComponentName);

	TriggerBoxComponent = CreateDefaultSubobject<UBoxComponent>(TriggerBoxComponentName);
	WeaponSampleComponent = CreateDefaultSubobject<UChildActorComponent>(WeaponSampleComponentName);
	GlowingEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(GlowingEffectComponentName);

	TriggerBoxComponent->SetupAttachment(RootComponent);
	WeaponSampleComponent->SetupAttachment(RootComponent);
	GlowingEffectComponent->SetupAttachment(RootComponent);

	bIsDestroyOnActivation = false;
	GlowingEffect = nullptr;
	bIsGlowingEffectPreview = true;
	SampleScale = 1.5f;
	RotationSpeedInDegree = 0.0f;
	bIsRandomInitialRotation = false;
	ZoneSize = {20.0f, 20.0f, 20.0f};

	TriggerBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AWeaponSpawner::OnBeginOverlap);
}

void AWeaponSpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	TriggerBoxComponent->SetBoxExtent(ZoneSize);

	// Remove previous set glowing effect (required if preview disabled)
	GlowingEffectComponent->SetAsset(nullptr);

	if (IsValid(GlowingEffect) and bIsGlowingEffectPreview)
	{
		GlowingEffectComponent->SetAsset(GlowingEffect);
	}

	if (IsValid(WeaponConfig))
	{
		WeaponSampleComponent->SetChildActorClass(WeaponConfig.GetWeaponStaticClass());

		var WeaponSample = WeaponSampleComponent->GetChildActor();

		if (not IsValid(WeaponSample))
		{
			WeaponSampleComponent->CreateChildActor();
			WeaponSample = WeaponSampleComponent->GetChildActor();
		}

		if (not WeaponConfig.Initialize(WeaponSample))
		{
			SetActorTickEnabled(false);
			return;
		}

		WeaponSample->SetActorTickEnabled(false);
		WeaponSample->UpdateComponentTransforms();
		WeaponSample->SetActorEnableCollision(false);
		WeaponSample->SetActorScale3D({SampleScale, SampleScale, SampleScale});
	}
}

void AWeaponSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(WeaponConfig))
	{
		if (IsValid(GlowingEffect) and not bIsGlowingEffectPreview)
		{
			GlowingEffectComponent->SetAsset(GlowingEffect);
		}

		let WeaponSample = WeaponSampleComponent->GetChildActor();

		if (not IsValid(WeaponSample))
		{
			log(Error, "WeaponSampleComponent child actor is invalid");
			return;
		}

		if (bIsRandomInitialRotation)
		{
			var Rotation = WeaponSample->GetActorRotation();
			Rotation.Yaw = FMath::RandRange(0.0f, 360.0f);
			WeaponSample->SetActorRotation(Rotation);
		}
	}
}

void AWeaponSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(WeaponConfig))
	{
		let WeaponSample = WeaponSampleComponent->GetChildActor();

		if (not IsValid(WeaponSample))
		{
			log(Error, "WeaponSampleComponent child actor is invalid");
			return;
		}

		var Rotation = WeaponSample->GetActorRotation();
		Rotation.Yaw += DeltaTime * RotationSpeedInDegree;
		WeaponSample->SetActorRotation(Rotation);
	}
}

void AWeaponSpawner::OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& Hit)
{
	if (let Character = Cast<ACloud9Character>(OtherActor); IsValid(Character) and IsValid(WeaponConfig))
	{
		let Inventory = Character->GetInventory();

		if (not IsValid(Inventory))
		{
			log(Error, "Inventory is invalid");
			return;
		}

		if (Inventory->AddWeapon(WeaponConfig, true, true) and bIsDestroyOnActivation)
		{
			Destroy();
		}
	}
}
