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
#include "Cloud9/Tools/Extensions/FName.h"
#include "Cloud9/Tools/Extensions/UEnum.h"
#include "WeaponType.generated.h"

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
class CLOUD9_API UWeaponType : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static bool IsFirearm(EWeaponType Type)
	{
		return Type == EWeaponType::Pistol
			or Type == EWeaponType::Smg
			or Type == EWeaponType::Shotgun
			or Type == EWeaponType::Rifle
			or Type == EWeaponType::Sniper;
	}

	UFUNCTION(BlueprintCallable)
	static bool IsMelee(EWeaponType Type) { return Type == EWeaponType::Knife; }

	UFUNCTION(BlueprintCallable)
	static bool IsGrenade(EWeaponType Type) { return Type == EWeaponType::Grenade; }

	UFUNCTION(BlueprintCallable)
	static FString ToString(EWeaponType Type) { return Type | EUEnum::GetEnumFullValueName() | EFName::ToString(); }
};
