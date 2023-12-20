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
#include "UObject/Object.h"
#include "Cloud9/Weapon/Classes/Cloud9WeaponBase.h"
#include "Cloud9/Weapon/Structures/WeaponConfig.h"
#include "WeaponConfigUtils.generated.h"

UCLASS()
class CLOUD9_API UWeaponConfigUtils : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static bool IsValid(const FWeaponConfig& Config)
	{
		return Config.bIsEnabled and Config.WeaponClass != EWeaponClass::NoClass;
	}

	UFUNCTION(BlueprintCallable)
	static EWeaponSlot GetWeaponSlot(const FWeaponConfig& Config);

	UFUNCTION(BlueprintCallable)
	static FName GetClassName(const FWeaponConfig& Config);

	UFUNCTION(BlueprintCallable)
	static FName GetWeaponName(const FWeaponConfig& Config);

	UFUNCTION(BlueprintCallable)
	static FName GetSkinName(const FWeaponConfig& Config);

	UFUNCTION(BlueprintCallable)
	static ACloud9WeaponBase* Spawn(const FWeaponConfig& Config, UWorld* World);

	UFUNCTION(BlueprintCallable)
	static TSubclassOf<ACloud9WeaponBase> GetWeaponStaticClass(const FWeaponConfig& Config);

	UFUNCTION(BlueprintCallable)
	static FString ToString(const FWeaponConfig& Config);

	static bool IsMeleeWeapon(const FWeaponConfig& Config) { return Config.WeaponClass == EWeaponClass::Melee; }

	static bool IsFirearmWeapon(const FWeaponConfig& Config) { return Config.WeaponClass == EWeaponClass::Firearm; }

	static bool IsGrenadeWeapon(const FWeaponConfig& Config) { return Config.WeaponClass == EWeaponClass::Grenade; }

private:
	template <typename WeaponClassType, typename WeaponNameType>
	static WeaponClassType* SpawnIntern(UWorld* World, WeaponNameType WeaponName, FName WeaponSkin)
	{
		return World | EUWorld::SpawnActor<WeaponClassType>{
			.Initializer = [&](let It) { return It->Configure(WeaponName, WeaponSkin) and It->Initialize(); },
		};
	}
};
