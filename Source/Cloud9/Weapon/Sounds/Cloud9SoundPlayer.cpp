// Copyright (c) 2023 Alexei Gladkikh


#include "Cloud9SoundPlayer.h"

#include "Cloud9/Game/Cloud9GameInstance.h"
#include "Cloud9/Tools/Extensions/TContainer.h"
#include "Cloud9/Tools/Extensions/USoundBase.h"

bool UCloud9SoundPlayer::PlaySingleSound(USoundBase* Sound, FVector Location, float Volume)
{
	CheckIsValid(Sound, Error, "Invalid Sound", false);

	USoundAttenuation* AttenuationSettings = nullptr;

	if (let GameWorld = UCloud9ToolsLibrary::GetGameWorld(); IsValid(GameWorld))
	{
		if (let GameInstance = GameWorld->GetGameInstance<UCloud9GameInstance>(); IsValid(GameInstance))
		{
			AttenuationSettings = GameInstance->GetAttenuationSettings();
		}
	}

	Sound | EUSoundBase::PlaySoundAtLocation{
		.Location = Location,
		.VolumeMultiplier = Volume,
		.AttenuationSettings = AttenuationSettings
	};

	return true;
}

bool UCloud9SoundPlayer::PlaySoundByName(
	const TMap<FName, USoundBase*>& Sounds,
	FName Name,
	FVector Location,
	float Volume)
{
	if (Name.IsNone())
	{
		log(Error, "Name not specified");
		return false;
	}

	let Sound = Sounds.Find(Name);

	if (not Sound or not*Sound)
	{
		log(Error, "No sound specified");
		return false;
	}

	return PlaySingleSound(*Sound, Location, Volume);
}

bool UCloud9SoundPlayer::PlayRandomSound(
	const TArray<USoundBase*>& Sounds,
	FVector Location,
	float Volume)
{
	using namespace ETContainer;
	using namespace ETOptional;

	return Sounds
		| Random()
		| OnNull{[] { log(Error, "No sound specified"); }}
		| OnSet{[Location, Volume](var It) { PlaySingleSound(It, Location, Volume); }}
		| IsSet{};
}
