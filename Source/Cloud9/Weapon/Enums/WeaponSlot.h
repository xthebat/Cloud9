// Copyright (c) 2023 Alexei Gladkikh
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "Cloud9/Tools/Extensions/FName.h"
#include "Cloud9/Tools/Extensions/UEnum.h"

#include "WeaponSlot.generated.h"

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	NotSelected = 0 UMETA(DisplayName = "Not Selected"),

	Main = 1 UMETA(DisplayName = "Main Weapon Slot"),
	Pistol = 2 UMETA(DisplayName = "Pistol Slot"),
	Knife = 3 UMETA(DisplayName = "Knife Slot"),
	Grenade = 4 UMETA(DisplayName = "Grenage Slot"),
	Stuff = 5 UMETA(DisplayName = "Stuff Slot"),
};

/**
 * Function to work with EWeaponType.
 */
UCLASS()
class CLOUD9_API UWeaponSlot : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static FName HolsteredSocket(EWeaponSlot Slot)
	{
		if (Slot == EWeaponSlot::Main) { return TEXT("HolsteredMainWeaponSocket"); }
		if (Slot == EWeaponSlot::Pistol) { return TEXT("HolsteredPistolWeaponSocket"); }
		if (Slot == EWeaponSlot::Knife) { return TEXT("HolsteredKnifeWeaponSocket"); }
		if (Slot == EWeaponSlot::Grenade) { return TEXT("HolsteredGrenadeWeaponSocket"); }
		return NAME_None;
	}

	UFUNCTION(BlueprintCallable)
	static FName EquippedSocket() { return TEXT("EquippedWeaponSocket"); }

	UFUNCTION(BlueprintCallable)
	static FName ReloadWeaponSocket() { return TEXT("ReloadWeaponSocket"); }

	UFUNCTION(BlueprintCallable)
	static FName ReloadPistolSocket() { return TEXT("ReloadPistolSocket"); }

	UFUNCTION(BlueprintCallable)
	static FString ToString(EWeaponSlot Slot) { return Slot | EUEnum::GetEnumFullValueName{} | EFName::ToString{}; }
};
