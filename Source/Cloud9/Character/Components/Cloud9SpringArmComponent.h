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
#include "GameFramework/SpringArmComponent.h"

#include "Cloud9SpringArmComponent.generated.h"

UCLASS(ClassGroup=(Camera), meta=(BlueprintSpawnableComponent))
class CLOUD9_API UCloud9SpringArmComponent : public USpringArmComponent
{
	GENERATED_BODY()

public:
	/**
	 * If bEnableCameraLag is true, controls how quickly camera reaches target position by each axis.
	 * Values of this vector multiplies with @CameraLagSpeed.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Lag, meta=(editcondition="bEnableCameraLag"))
	FVector CameraLagVector;

public:
	UCloud9SpringArmComponent();

	virtual void UpdateDesiredArmLocation(
		bool bDoTrace,
		bool bDoLocationLag,
		bool bDoRotationLag,
		float DeltaTime
	) override;

	virtual void OnRegister() override;

private:
	float CachedCameraVerticalSpeedLag;
};
