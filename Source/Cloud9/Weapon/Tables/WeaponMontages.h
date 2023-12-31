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

#include "WeaponMontages.generated.h"

USTRUCT(BlueprintType)
struct FWeaponActionMontages
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Action, meta=(InlineCategoryProperty))
	EWeaponClass WeaponClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Action)
	UAnimMontage* PrimaryActionMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Action,
		meta=(EditCondition="WeaponClass == EWeaponClass::Melee", EditConditionHides))
	bool bHasPrimaryBackAction = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Action,
		meta=(EditCondition="WeaponClass == EWeaponClass::Melee && bHasPrimaryBackAction", EditConditionHides))
	UAnimMontage* PrimaryBackActionMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Action, meta=(InlineEditConditionToggle))
	bool bHasSecondaryAction = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Action, meta=(EditCondition="bHasSecondaryAction"))
	UAnimMontage* SecondaryActionMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Deploy)
	UAnimMontage* DeployMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Reload,
		meta=(EditCondition="WeaponClass == EWeaponClass::Firearm", EditConditionHides))
	UAnimMontage* ReloadMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Reload,
		meta=(EditCondition="WeaponClass == EWeaponClass::Firearm", EditConditionHides))
	bool bHasReloadLoop = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Reload, AdvancedDisplay,
		meta=(EditCondition="WeaponClass == EWeaponClass::Firearm && bHasReloadLoop", EditConditionHides))
	UAnimMontage* ReloadLoopMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Reload, AdvancedDisplay,
		meta=(EditCondition="WeaponClass == EWeaponClass::Firearm && bHasReloadLoop", EditConditionHides))
	UAnimMontage* ReloadEndMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Pinpull,
		meta=(EditCondition="WeaponClass == EWeaponClass::Grenade", EditConditionHides))
	UAnimMontage* PinpullPrimaryActionMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Pinpull,
		meta=(EditCondition="WeaponClass == EWeaponClass::Grenade && bHasSecondaryAction", EditConditionHides))
	UAnimMontage* PinpullSecondaryActionMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Pinpull,
		meta=(EditCondition="WeaponClass == EWeaponClass::Grenade", EditConditionHides))
	float PinpullPrimaryActionHoldTiming = 0.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Pinpull,
		meta=(EditCondition="WeaponClass == EWeaponClass::Grenade && bHasSecondaryAction", EditConditionHides))
	float PinpullSecondaryActionHoldTiming = 0.0f;
};

USTRUCT(BlueprintType)
struct FWeaponPosesMontages
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FWeaponActionMontages OnStand;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FWeaponActionMontages OnCrouch;
};
