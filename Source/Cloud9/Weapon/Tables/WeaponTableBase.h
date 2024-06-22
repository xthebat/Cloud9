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

#include "Cloud9/Tools/Extensions/TOptional.h"
#include "Cloud9/Weapon/Enums/WeaponType.h"

#include "WeaponTableBase.generated.h"

USTRUCT(BlueprintType)
struct FWeaponSkin
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

	/**
	 * Skin material to be used
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Visual)
	UMaterialInstance* Material;
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

	/**
	 * Weapon icon in HUD
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Icon)
	UTexture2D* Icon;

	/**
	 * Weapon preview in GUI
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Icon)
	UTexture2D* Preview;
	
	/**
	 * Maximum running speed with the weapon equipped (aka Mobility)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Mobility,
		meta=(UIMin="0", UIMax="250", ClampMin="0", ClampMax="250"))
	float MaxPlayerSpeed = 250.0f;

	/**
	 * Money award for kill an enemy with this gun
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Economy,
		meta=(UIMin="0", UIMax="20000", ClampMin="0", ClampMax="20000"))
	int KillAward;
};

namespace EFWeaponInfo
{
	struct GetSkinByNameOrNull
	{
		FName SkinName = FWeaponSkin::Default;

		template <typename WeaponInfoType>
		TOptional<FWeaponSkin> operator()(WeaponInfoType&& Self) const
		{
			let Skin = Self->Skins.FindByPredicate([this](let& It) { return It.Name == SkinName; });

			if (Skin == nullptr)
			{
				if (SkinName == FWeaponSkin::Default)
				{
					// Crash if we have no even default skin
					FUNCTION_FATAL("Can't get default skin");
				}

				return {};
			}

			return *Skin;
		}

		OPERATOR_BODY(GetSkinByNameOrNull)
	};

	struct GetSkinByNameOrThrow
	{
		FName SkinName = FWeaponSkin::Default;

		template <typename WeaponInfoType>
		FWeaponSkin operator()(WeaponInfoType&& Self) const
		{
			using namespace ETOptional;

			return Self
				| GetSkinByNameOrNull(SkinName)
				| Get([&]
					{
						FUNCTION_ERROR("Weapon skin '%s' not found -> try to get default skin", *SkinName.ToString());
						return Self
							| GetSkinByNameOrNull()
							| Get();
					}
				);
		}

		OPERATOR_BODY(GetSkinByNameOrThrow)
	};
}
