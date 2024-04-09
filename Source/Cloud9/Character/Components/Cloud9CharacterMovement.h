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

#include "Cloud9CharacterMovement.generated.h"

class ACloud9Character;

UCLASS(Blueprintable)
class CLOUD9_API UCloud9CharacterMovement : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UCloud9CharacterMovement();

	ACloud9Character* GetCloud9CharacterOwner() const;

	bool IsSneaking() const;
	void Sneak() const;
	void UnSneak() const;

	void Rotate(FRotator Rotator, bool Instant = false);
	bool IsOnLadder() const;

	UPROPERTY(Category="Character Movement: Sneaking", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float MaxSneakSpeed;

	virtual float GetMaxSpeed() const override;

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

protected:
	/** Target rotator of character*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=State)
	FRotator TargetRotator;

private:
	static constexpr float RotationLagScale = 360.0f;

	/** Character rotation lag*/
	UPROPERTY(EditDefaultsOnly, Category=Config)
	float RotationLag;
};
