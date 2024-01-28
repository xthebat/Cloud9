// Copyright (c) 2023 Alexei Gladkikh


#include "WeaponSpawner.h"

#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"

#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponBase.h"
#include "Components/BillboardComponent.h"

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

	GlowingEffect = nullptr;
	SampleScale = 1.0f;
	RotationSpeedInDegree = 0.0f;
	bIsRandomInitialRotation = false;
	ZoneSize = {20.0f, 20.0f, 20.0f};

	TriggerBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AWeaponSpawner::OnBeginOverlap);

#if WITH_EDITORONLY_DATA
	// SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(SpriteComponentName);
	// SpriteComponent->SetupAttachment(RootComponent);
	//
	// static ConstructorHelpers::FObjectFinderOptional<UTexture2D>
	// 	TriggerTextureObject(TEXT("/Engine/EditorResources/S_Trigger"));
	//
	// SpriteComponent->Sprite = TriggerTextureObject.Get();
	// SpriteComponent->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	// SpriteComponent->bHiddenInGame = true;
	// SpriteComponent->bIsScreenSizeScaled = true;
#endif
}

void AWeaponSpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	TriggerBoxComponent->SetBoxExtent(ZoneSize);

	if (IsValid(GlowingEffect))
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
			// WeaponSampleComponent->DestroyChildActor();
			SetActorTickEnabled(false);
			return;
		}

		if (not IsValid(WeaponSample))
		{
			log(Error, "WeaponSampleComponent child actor is invalid");
			return;
		}

		WeaponSample->UpdateComponentTransforms();
		WeaponSample->SetActorEnableCollision(false);
		WeaponSample->SetActorScale3D({SampleScale, SampleScale, SampleScale});

		if (bIsRandomInitialRotation)
		{
			var Rotation = WeaponSample->GetActorRotation();
			Rotation.Yaw = FMath::RandRange(0.0f, 360.0f);
			WeaponSample->SetActorRotation(Rotation);
		}
	}
}

void AWeaponSpawner::BeginPlay()
{
	Super::BeginPlay();
}

void AWeaponSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(WeaponConfig))
	{
		let Actor = WeaponSampleComponent->GetChildActor();

		if (not IsValid(Actor))
		{
			log(Error, "WeaponSampleComponent child actor is invalid");
			return;
		}

		var Rotation = Actor->GetActorRotation();
		Rotation.Yaw += DeltaTime * RotationSpeedInDegree;
		Actor->SetActorRotation(Rotation);
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

		Inventory->AddWeapon(WeaponConfig, true, true);
	}
}

void AWeaponSpawner::Initialize() {}
