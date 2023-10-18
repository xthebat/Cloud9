#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	NoWeapon UMETA(DisplayName = "NoWeapon"),

	Knife UMETA(DisplayName = "Knife"),
	Pistol UMETA(DisplayName = "Pistol"),
	
	Smg UMETA(DisplayName = "Smg"),
	Shotgun UMETA(DisplayName = "Shotgun"),
	Rifle UMETA(DisplayName = "Rifle"),
	Sniper UMETA(DisplayName = "Sniper"),
	
	Heavy UMETA(DisplayName = "Heavy"),
	Grenade UMETA(DisplayName = "Grenade"),
	C4 UMETA(DisplayName = "C4"),
};
