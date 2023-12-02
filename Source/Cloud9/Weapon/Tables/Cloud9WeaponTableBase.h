﻿// Copyright (c) 2023 Alexei Gladkikh
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

#include "Engine/DataTable.h"
#include "Cloud9/Cloud9.h"
#include "Cloud9/Weapon/Enums/Cloud9WeaponType.h"
#include "Cloud9WeaponTableBase.generated.h"

USTRUCT(BlueprintType)
struct FBaseWeaponSkin
{
	GENERATED_BODY()

	inline static const FName Default = TEXT("default");

	/**
	 * Unique name of skin
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Base)
	FName Name = Default;

	/**
	 * Localized label to show in UI and etc
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Base)
	FText Label;

	/**
	 * Localized description to show in UI and etc
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Base)
	FText Description;
};

USTRUCT(BlueprintType)
struct FBaseWeaponInfo : public FTableRowBase
{
	GENERATED_BODY()

	/**
	 * Weapon type
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Base)
	EWeaponType Type = EWeaponType::NoWeapon;

	/**
	 * Localized label to show in UI and etc
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Base)
	FText Label;

	/**
	 * Localized description to show in UI and etc
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Base, AdvancedDisplay)
	FText Description;
};

namespace EFWeaponInfo
{
	template <typename SkinInfoType>
	struct GetSkinByName : TOperator<GetSkinByName<SkinInfoType>>
	{
		explicit GetSkinByName(FName SkinName = FBaseWeaponSkin::Default) : SkinName(SkinName) { }

		template <typename WeaponInfoType>
		TOptional<SkinInfoType> operator()(WeaponInfoType Self) const
		{
			let Skin = Self->Skins.FindByPredicate([this](let& It) { return It.Name == SkinName; });

			if (not Skin and SkinName == FBaseWeaponSkin::Default)
			{
				TRACE(Fatal, "Can't get default skin");
				return {};
			}

			return *static_cast<const SkinInfoType*>(Skin);
		}

	private:
		FName SkinName;
	};
}
