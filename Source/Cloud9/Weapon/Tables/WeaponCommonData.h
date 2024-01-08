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

#include "Niagara/Classes/NiagaraSystem.h"

#include "WeaponCommonData.generated.h"

USTRUCT(BlueprintType)
struct FFirearmCommonData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage)
	float ImpulseMultiplier = 50.0f;

	/**
	 * Alpha coefficient to fixup hit-scan registration.
	 * 
	 * GetHitResultUnderCursor can return value slightly upper then surface.
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Settings)
	float LineTraceAlpha = 5.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Visual)
	float MagazineLifetime = 10.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* Tracer;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* Squib;
};

USTRUCT(BlueprintType)
struct FCaseCommonData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Eject)
	float EjectImpulse = 200.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Eject)
	float EjectAngle = 30.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Eject)
	float MaxEjectRotation = 25.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Visual)
	float Scale = 1.2f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Visual)
	float Lifetime = 3.0f;
};

USTRUCT(BlueprintType)
struct FGrenadeCommonData
{
	GENERATED_BODY()

	/**
	 * Only relevant for frag (explosive)
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Damage)
	float ImpulseMultiplier = 10.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Throw)
	float MaxThrowImpulse = 800.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Throw)
	float MaxThrowAngle = 30.0f;

	/**
	 * Distance multiplier when making run throw
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Throw,
		meta=(UIMin="0", UIMax="100", ClampMin="0", ClampMax="100"))
	float RunDistanceMultiplier = 1.0f;

	/**
	 * Distance multiplier when making jump throw
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category=Throw,
		meta=(UIMin="0", UIMax="100", ClampMin="0", ClampMax="100"))
	float JumpDistanceMultiplier = 1.0f;
};

USTRUCT(BlueprintType)
struct FWeaponCommonData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FFirearmCommonData Firearm;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FCaseCommonData Case;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGrenadeCommonData Grenade;
};
