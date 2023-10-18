#pragma once

UENUM(BlueprintType)
enum class EWeaponClass : uint8
{
	NoClass UMETA(DisplayName = "NoClass"),
	Melee UMETA(DisplayName = "Melee"),
	Firearm UMETA(DisplayName = "Firearm"),
	Grenade UMETA(DisplayName = "Grenade"),
};
