#pragma once

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	NotSelected = 0 UMETA(DisplayName = "Not Selected"),

	Main = 1 UMETA(DisplayName = "Slot1"),
	Pistol = 2 UMETA(DisplayName = "Pistol Slot"),
	Knife = 3 UMETA(DisplayName = "Knife Slot"),
	Grenade = 4 UMETA(DisplayName = "Grenage Slot"),
	Stuff = 5 UMETA(DisplayName = "Stuff Slot"),
};
