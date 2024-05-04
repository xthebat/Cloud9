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
#include "GameFramework/CharacterMovementComponent.h"

#include "Cloud9/Tools/Macro/Common.h"

#include "Cloud9CharacterMovement.generated.h"

class ACloud9Character;

UCLASS(Blueprintable)
class CLOUD9_API UCloud9CharacterMovement : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	static constexpr let RotationLagScale = 360.0f;

	/**
	 * Coefficient to correct import scale of all models and assets from CS:GO
	 */
	static constexpr let SpeedScaleCoefficient = 2.0f;

	static constexpr let AccelerationWalkScaleCoefficient = 6.0f;

	static constexpr let AccelerationDuckScaleCoefficient = 4.0f;

	UCloud9CharacterMovement();

	ACloud9Character* GetCloud9CharacterOwner() const;

	bool IsSneaking() const;
	void Sneak() const;
	void UnSneak() const;

	void Rotate(FRotator Rotator, bool Instant = false);
	bool IsOnLadder() const;

	// cstrike15

	/**
	 * Maximum speed of character (WARN! This variable OVERRIDE behaviour of MaxWalkSpeed)
	 */
	UPROPERTY(Category="Character Movement: Base", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="500"))
	float SpeedRun;

	/**
	 * Maximum speed of character if being VIP (Not Implemented)
	 */
	UPROPERTY(Category="Character Movement: Base", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="500"))
	float SpeedVip;

	/**
	 * Maximum speed of character when carrying shield (Not Implemented)
	 */
	UPROPERTY(Category="Character Movement: Base", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="500"))
	float SpeedShield;

	/**
	 * Maximum speed of character when carrying hostage (Not Implemented)
	 */
	UPROPERTY(Category="Character Movement: Base", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="500"))
	float SpeedHasHostage;

	UPROPERTY(Category="Character Movement: Base", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="500"))
	float SpeedStopped;

	/**
	 * Maximum speed of character if being observer in free mode
	 */
	UPROPERTY(Category="Character Movement: Base", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="500"))
	float SpeedObserver;

	/**
	 * Character movement modifier for crouch/duck
	 */
	UPROPERTY(Category="Character Movement: Modifier", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="1"))
	float SpeedDuckModifier;

	/**
	 * Character movement modifier for sneaking
	 */
	UPROPERTY(Category="Character Movement: Modifier", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="1"))
	float SpeedWalkModifier;

	/**
	 * Character movement modifier for climbing(?) (Not Implemented)
	 */
	UPROPERTY(Category="Character Movement: Modifier", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="1"))
	float SpeedClimbModifier;

	void UpdateFlinchVelocityModifier(float FlinchModSmall, float FlinchModLarge);

	virtual float GetMaxSpeed() const override
	{
		var MaxCharacterSpeed = GetMovementValue(SpeedScaleCoefficient, SpeedScaleCoefficient);

		if (IsWalking())
		{
			MaxCharacterSpeed *= FlinchVelocityModifier;
		}

		return MaxCharacterSpeed;
	}

	virtual float GetMaxAcceleration() const override
	{
		return GetMovementValue(
			AccelerationWalkScaleCoefficient,
			AccelerationDuckScaleCoefficient
		);
	}

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

protected:
	float GetMovementValue(float WalkScale, float DuckScale) const;

	UPROPERTY(Category="Character Movement: Modifier", BlueprintReadOnly)
	float FlinchVelocityModifier;

	/** Target rotator of character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=State)
	FRotator TargetRotator;

private:
	/** Character rotation lag */
	UPROPERTY(EditDefaultsOnly, Category=Config)
	float RotationLag;
};
