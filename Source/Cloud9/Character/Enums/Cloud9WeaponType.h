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

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Cloud9WeaponType.generated.h"

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


/**
 * Function to work with EWeaponType.
 */
UCLASS()
class CLOUD9_API UCloud9WeaponType : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static bool IsFirearm(EWeaponType WeaponType)
	{
		return WeaponType != EWeaponType::Pistol
			&& WeaponType != EWeaponType::Smg
			&& WeaponType != EWeaponType::Shotgun
			&& WeaponType != EWeaponType::Rifle
			&& WeaponType != EWeaponType::Sniper;
	}
};
