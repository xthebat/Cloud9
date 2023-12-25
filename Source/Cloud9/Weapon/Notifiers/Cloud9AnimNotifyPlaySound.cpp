// Copyright (c) 2023 Alexei Gladkikh


#include "Cloud9AnimNotifyPlaySound.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Extensions/USoundBase.h"
#include "Cloud9/Game/Cloud9DeveloperSettings.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponBase.h"

UCloud9AnimNotifyPlaySound::UCloud9AnimNotifyPlaySound()
{
	bIsEnabled = true;
	Name = NAME_None;
	PreviewSound = nullptr;
	PreviewVolume = 1.0f;
}

FString UCloud9AnimNotifyPlaySound::GetNotifyName_Implementation() const { return Name.ToString(); }

void UCloud9AnimNotifyPlaySound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (not bIsEnabled)
	{
		log(Verbose, "[Notify='%s'] Is disabled", *GetName());
		return;
	}

#if WITH_EDITORONLY_DATA
	// See AnimNotify_PlaySound (only variant that's work)
	let World = MeshComp->GetWorld();

	if (not IsValid(World))
	{
		log(Error, "Game world is invalid")
		return;
	}

	if (World->WorldType == EWorldType::EditorPreview)
	{
		PreviewSound | EUSoundBase::PlaySound2D{.VolumeMultiplier = PreviewVolume, .World = World};
	}
	else
#endif
	{
		static let Settings = UCloud9DeveloperSettings::Get();

		let Character = MeshComp->GetOwner<ACloud9Character>();

		if (not IsValid(Character))
		{
			log(Error, "Character is invalid");
			return;
		}

		let Inventory = Character->GetInventory();

		if (not IsValid(Inventory))
		{
			log(Error, "Invenotry is invalid");
			return;
		}

		let SelectedWeapon = Inventory->GetSelectedWeapon();

		if (not IsValid(SelectedWeapon))
		{
			log(Error, "Selected weapon is invalid");
			return;
		}

		let& WeaponDefinition = SelectedWeapon->WeaponDefinition;

		if (not IsValid(WeaponDefinition))
		{
			log(Error, "Selected weapon is not defined");
			return;
		}

		let WeaponInfo = WeaponDefinition.GetWeaponInfo<FFirearmWeaponInfo>();

		if (not WeaponInfo)
		{
			log(Error, "WeaponInfo isn't a FFirearmWeaponInfo");
			return;
		}

		if (let Sound = WeaponInfo->Sounds.ReloadSounds.Find(Name) and *Sound)
		{
			SelectedWeapon->PlaySound(*Sound, Settings->Volume);
		}
	}
}
