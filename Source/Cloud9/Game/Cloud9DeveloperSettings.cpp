#include "Cloud9DeveloperSettings.h"

#include "Cloud9/Cloud9.h"
#include "Cloud9/Tools/Cloud9ReflectionLibrary.h"

const UCloud9DeveloperSettings* UCloud9DeveloperSettings::GetCloud9DeveloperSettings()
{
	static var bIsInitialized = false;
	static let Settings = GetDefault<UCloud9DeveloperSettings>();
	
	if (!bIsInitialized)
	{
		Cast<UCloud9DeveloperSettings>(Settings)->Log();
		bIsInitialized = true;
	}

	return Settings;
}

void UCloud9DeveloperSettings::Save()
{
	UpdateDefaultConfigFile();
	Log();
}

void UCloud9DeveloperSettings::Log() const
{
	UE_LOG(LogCloud9, Display, TEXT("%s"), *UCloud9ReflectionLibrary::UObjectToString(this));
}
