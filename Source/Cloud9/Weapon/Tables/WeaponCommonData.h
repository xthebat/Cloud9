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

#include "WeaponCommonData.generated.h"

USTRUCT(BlueprintType)
struct FWeaponCommonData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage)
	float ImpulseMultiplier = 50.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Case)
	float CaseImpulse = 500.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Case)
	float CaseEjectAngle = 45.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Case)
	float CaseMaxEjectRotation = 30.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Case)
	float CaseScale = 2.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Case)
	float CaseLifetime = 2.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Grenade)
	float GrenadeExplosionRadius = 200.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Grenade)
	float GrenadeImpulseStrength = 1000.0f;

	/**
     * Maximum fire weapon range (no any impact after this distance)
     */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Grenade)
	float GrenadeMaxThrowImpulse = 800.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Grenade)
	float GrenadeMaxThrowAngle = 30.0f;

	/**
	 * Distance multiplier when making run throw
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Grenade,
		meta=(UIMin="0", UIMax="100", ClampMin="0", ClampMax="100"))
	float GrenadeRunDistanceMultiplier = 1.0f;

	/**
	 * Distance multiplier when making jump throw
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Grenade,
		meta=(UIMin="0", UIMax="100", ClampMin="0", ClampMax="100"))
	float GrenadeJumpDistanceMultiplier = 1.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Weapon)
	float MagazineLifetime = 10.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Weapon)
	UParticleSystem* FirearmTracer;
};
