// Copyright (c) 2023 Alexei Gladkikh


#include "Cloud9SoundPlayer.h"

#include "Cloud9/Game/Cloud9GameInstance.h"
#include "Cloud9/Tools/Extensions/TContainer.h"
#include "Cloud9/Tools/Extensions/USoundBase.h"

bool UCloud9SoundPlayer::PlaySingleSound(USoundBase* Sound, FVector Location, float Volume)
{
	RETURN_IF_FAIL(Sound, false, Error, "Invalid Sound");

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
	RETURN_IF_FAIL(not Name.IsNone(), false, Error, "Name not specified");
	let Sound = Sounds.Find(Name);
	RETURN_IF_FAIL(Sound and *Sound, false, Error, "No sound specified");
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
		| OnNull{[] { FUNCTION_ERROR("No sound specified"); }}
		| OnSet{[Location, Volume](var It) { PlaySingleSound(It, Location, Volume); }}
		| IsSet{};
}
