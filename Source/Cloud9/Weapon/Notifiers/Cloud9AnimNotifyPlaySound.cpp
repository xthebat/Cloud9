// Copyright (c) 2023 Alexei Gladkikh


#include "Cloud9AnimNotifyPlaySound.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Extensions/USoundBase.h"
#include "Cloud9/Game/Cloud9DeveloperSettings.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponFirearm.h"

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
		if (let SelectedWeapon = GetSelectedWeapon<ACloud9WeaponFirearm>(MeshComp))
		{
			let WeaponInfo = SelectedWeapon->GetWeaponInfo();
			if (let Sound = WeaponInfo->Sounds.ReloadSounds.Find(Name); Sound and *Sound)
			{
				SelectedWeapon->PlaySound(*Sound, Settings->Volume);
			}
		}
	}
}
