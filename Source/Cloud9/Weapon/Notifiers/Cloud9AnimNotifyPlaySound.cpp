// Copyright (c) 2023 Alexei Gladkikh

#include "Cloud9AnimNotifyPlaySound.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Extensions/USoundBase.h"
#include "Cloud9/Game/Cloud9DeveloperSettings.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponFirearm.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponMelee.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponGrenade.h"
#include "Cloud9/Weapon/Sounds/Cloud9SoundPlayer.h"

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

bool UCloud9AnimNotifyPlaySound::PlayMeleeSound(USkeletalMeshComponent* MeshComp, float Volume) const
{
	let SelectedWeapon = GetSelectedWeapon<ACloud9WeaponMelee>(MeshComp);
	AssertOrReturn(
		IsValid(SelectedWeapon) and SelectedWeapon->IsWeaponDefined(), false,
		Error, "Invalid or undefined weapon for play sound");

	let WeaponInfo = SelectedWeapon->GetWeaponInfo();
	let Location = SelectedWeapon->GetActorLocation();

	switch (SoundType)
	{
	case EWeaponSoundType::Deploy:
		return UCloud9SoundPlayer::PlaySingleSound(WeaponInfo->Sounds.DeploySound, Location, Volume);
	case EWeaponSoundType::Primary:
		return UCloud9SoundPlayer::PlayRandomSound(WeaponInfo->Sounds.SlashSounds, Location, Volume);
	case EWeaponSoundType::Secondary:
		return UCloud9SoundPlayer::PlayRandomSound(WeaponInfo->Sounds.StabSounds, Location, Volume);
	default:
		log(Error, "[%s] Invalid sound type for melee sound", *GetName());
		return false;
	}
}

bool UCloud9AnimNotifyPlaySound::PlayFirearmSound(USkeletalMeshComponent* MeshComp, float Volume) const
{
	let SelectedWeapon = GetSelectedWeapon<ACloud9WeaponFirearm>(MeshComp);
	AssertOrReturn(
		IsValid(SelectedWeapon) and SelectedWeapon->IsWeaponDefined(), false,
		Error, "Invalid or undefined weapon for play sound");

	let WeaponInfo = SelectedWeapon->GetWeaponInfo();
	let CommonData = SelectedWeapon->GetWeaponCommonData();
	let Location = SelectedWeapon->GetActorLocation();

	switch (SoundType)
	{
	case EWeaponSoundType::Deploy:
		return UCloud9SoundPlayer::PlaySingleSound(WeaponInfo->Sounds.DeploySound, Location, Volume);
	case EWeaponSoundType::Reload:
		return UCloud9SoundPlayer::PlaySoundByName(WeaponInfo->Sounds.ReloadSounds, Name, Location, Volume);
	case EWeaponSoundType::Primary:
		return UCloud9SoundPlayer::PlayRandomSound(WeaponInfo->Sounds.FireSounds, Location, Volume);
	case EWeaponSoundType::Secondary:
		return UCloud9SoundPlayer::PlaySoundByName(WeaponInfo->Sounds.ZoomSounds, Name, Location, Volume);
	case EWeaponSoundType::LowAmmo:
		if (SelectedWeapon->GetCurrentAmmo() <= WeaponInfo->LowAmmoCount)
		{
			let LowAmmoVolume = CommonData->Firearm.LowAmmoVolumeMultiplier * Volume;
			return UCloud9SoundPlayer::PlaySingleSound(CommonData->Firearm.LowAmmoSound, Location, LowAmmoVolume);
		}
		return false;
	default:
		log(Error, "[%s] Invalid sound type for firearm sound SoundType=%d", *GetName(), SoundType);
		return false;
	}
}

bool UCloud9AnimNotifyPlaySound::PlayGrenadeSound(USkeletalMeshComponent* MeshComp, float Volume) const
{
	let SelectedWeapon = GetSelectedWeapon<ACloud9WeaponGrenade>(MeshComp);
	AssertOrReturn(
		IsValid(SelectedWeapon) and SelectedWeapon->IsWeaponDefined(), false,
		Error, "Invalid or undefined weapon for play sound");

	let WeaponInfo = SelectedWeapon->GetWeaponInfo();
	let Location = SelectedWeapon->GetActorLocation();

	switch (SoundType)
	{
	case EWeaponSoundType::Deploy:
		return UCloud9SoundPlayer::PlaySingleSound(WeaponInfo->Sounds.DeploySound, Location, Volume);
	case EWeaponSoundType::Pinpull:
		return UCloud9SoundPlayer::PlaySingleSound(WeaponInfo->Sounds.PinpullSound, Location, Volume);
	case EWeaponSoundType::Primary:
		return UCloud9SoundPlayer::PlaySingleSound(WeaponInfo->Sounds.ThrowSound, Location, Volume);
	default:
		log(Error, "[%s] Invalid sound type for firearm sound SoundType=%d", *GetName(), SoundType);
		return false;
	}
}

void UCloud9AnimNotifyPlaySound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AssertOrVoid(bIsEnabled, Verbose, "Notify is disabled");

#if WITH_EDITORONLY_DATA
	// See AnimNotify_PlaySound (only variant that's work)
	let World = MeshComp->GetWorld();
	AssertOrVoid(IsValid(World), Error, "Game world is invalid");

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
			log(Error, "[%s] Invalid class name", *GetName());
		}
	}
}
