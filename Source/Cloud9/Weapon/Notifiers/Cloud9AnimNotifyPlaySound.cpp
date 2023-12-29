// Copyright (c) 2023 Alexei Gladkikh

#include "Cloud9AnimNotifyPlaySound.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Extensions/USoundBase.h"
#include "Cloud9/Game/Cloud9DeveloperSettings.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponFirearm.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponMelee.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponGrenade.h"

UCloud9AnimNotifyPlaySound::UCloud9AnimNotifyPlaySound()
{
	bIsEnabled = true;
	WeaponClass = EWeaponClass::Firearm;
	SoundType = EWeaponSoundType::Reload;
	Name = NAME_None;
	PreviewSound = nullptr;
	PreviewVolume = 1.0f;
}

FString UCloud9AnimNotifyPlaySound::GetNotifyName_Implementation() const
{
	var Output = SoundType
		| EUEnum::GetValueName{}
		| EFName::ToString{};

	if (not Name.IsNone())
	{
		Output += ":";
		Output += Name | EFName::ToString{};
	}

	return Output;
}

bool UCloud9AnimNotifyPlaySound::PlaySingleSound(USoundBase* Sound, FVector Location, float Volume) const
{
	if (not IsValid(Sound))
	{
		log(Error, "[Notify='%s'] Invalid sound", *GetName());
		return false;
	}

	Sound | EUSoundBase::PlaySoundAtLocation{Location, Volume};
	return true;
}

bool UCloud9AnimNotifyPlaySound::PlaySoundByName(
	const TMap<FName, USoundBase*>& Sounds,
	FVector Location,
	float Volume) const
{
	if (Name.IsNone())
	{
		log(Error, "[Notify='%s'] Name not specified", *GetName());
		return false;
	}

	let Sound = Sounds.Find(Name);

	if (not Sound or not*Sound)
	{
		log(Error, "[Notify='%s'] No sound specified", *GetName());
		return false;
	}

	return PlaySingleSound(*Sound, Location, Volume);
}

bool UCloud9AnimNotifyPlaySound::PlayRandomSound(
	const TArray<USoundBase*>& Sounds,
	FVector Location,
	float Volume) const
{
	using namespace ETContainer;
	using namespace ETOptional;

	return Sounds
		| Random()
		| OnNull{[this] { log(Error, "[Notify='%s'] No sound specified", *GetName()); }}
		| OnSet{[this, Location, Volume](var It) { PlaySingleSound(It, Location, Volume); }}
		| IsSet{};
}

bool UCloud9AnimNotifyPlaySound::PlayMeleeSound(USkeletalMeshComponent* MeshComp, float Volume) const
{
	let SelectedWeapon = GetSelectedWeapon<ACloud9WeaponMelee>(MeshComp);
	if (not IsValid(SelectedWeapon) or not SelectedWeapon->IsWeaponDefined())
	{
		log(Error, "[Notify='%s'] Invalid or undefined weapon for play sound", *GetName());
		return false;
	}

	let WeaponInfo = SelectedWeapon->GetWeaponInfo();
	let Location = SelectedWeapon->GetActorLocation();

	switch (SoundType)
	{
	case EWeaponSoundType::Deploy:
		return PlaySingleSound(WeaponInfo->Sounds.DeploySound, Location, Volume);
	case EWeaponSoundType::Primary:
		return PlayRandomSound(WeaponInfo->Sounds.SlashSounds, Location, Volume);
	case EWeaponSoundType::Secondary:
		return PlayRandomSound(WeaponInfo->Sounds.StabSounds, Location, Volume);
	default:
		log(Error, "[Notify='%s'] Invalid sound type for melee sound", *GetName());
		return false;
	}
}

bool UCloud9AnimNotifyPlaySound::PlayFirearmSound(USkeletalMeshComponent* MeshComp, float Volume) const
{
	let SelectedWeapon = GetSelectedWeapon<ACloud9WeaponFirearm>(MeshComp);
	if (not IsValid(SelectedWeapon) or not SelectedWeapon->IsWeaponDefined())
	{
		log(Error, "[Notify='%s'] Invalid or undefined weapon for play sound", *GetName());
		return false;
	}

	let WeaponInfo = SelectedWeapon->GetWeaponInfo();
	let Location = SelectedWeapon->GetActorLocation();

	switch (SoundType)
	{
	case EWeaponSoundType::Deploy:
		return PlaySingleSound(WeaponInfo->Sounds.DeploySound, Location, Volume);
	case EWeaponSoundType::Reload:
		return PlaySoundByName(WeaponInfo->Sounds.ReloadSounds, Location, Volume);
	case EWeaponSoundType::Primary:
		return PlayRandomSound(WeaponInfo->Sounds.FireSounds, Location, Volume);
	case EWeaponSoundType::Secondary:
		return PlaySoundByName(WeaponInfo->Sounds.ZoomSounds, Location, Volume);
	default:
		log(Error, "[Notify='%s'] Invalid sound type for firearm sound", *GetName());
		return false;
	}
}

bool UCloud9AnimNotifyPlaySound::PlayGrenadeSound(USkeletalMeshComponent* MeshComp, float Volume) const
{
	// TODO: Implement PlayGrenadeSound

	let SelectedWeapon = GetSelectedWeapon<ACloud9WeaponGrenade>(MeshComp);
	if (not IsValid(SelectedWeapon) or not SelectedWeapon->IsWeaponDefined())
	{
		log(Error, "[Notify='%s'] Invalid or undefined weapon for play sound", *GetName());
		return false;
	}

	// let Location = SelectedWeapon->GetActorLocation();

	return false;
}

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

		let Volume = Settings->Volume;

		switch (WeaponClass)
		{
		case EWeaponClass::Melee:
			PlayMeleeSound(MeshComp, Volume);
			break;
		case EWeaponClass::Firearm:
			PlayFirearmSound(MeshComp, Volume);
			break;
		case EWeaponClass::Grenade:
			PlayGrenadeSound(MeshComp, Volume);
			break;
		default:
			log(Error, "[Notify='%s'] Invalid class name", *GetName());
		}
	}
}
