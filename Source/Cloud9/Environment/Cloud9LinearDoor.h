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
#include "Engine/StaticMeshActor.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"
#include "Cloud9/Tools/Extensions/EDirection.h"

#include "Cloud9LinearDoor.generated.h"

UCLASS()
class CLOUD9_API ACloud9LinearDoor : public AStaticMeshActor
{
	GENERATED_BODY()

public: // functions
	ACloud9LinearDoor();

	UFUNCTION(BlueprintCallable)
	void Open();

	UFUNCTION(BlueprintCallable)
	void Close();

	UFUNCTION(BlueprintCallable)
	void Toggle();

protected: // functions
	virtual void BeginPlay() override;

public: // functions
	virtual void Tick(float DeltaTime) override;

protected: // variables
	bool bIsMoving;

	FVector OriginPosition;
	FVector TargetPosition;
	FVector Shift;
	FVector DirectionVector;

	/**
	 * Whether door is currently open or closed
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Door, meta=(AllowPrivateAccess))
	bool bIsOpen;

	/**
	 * Direction where to move door
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Door, meta=(AllowPrivateAccess))
	EDirection Direction;

	/**
	 * Door moving speed when open or close
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Door, meta=(AllowPrivateAccess))
	float Speed;

	/**
	 * How far move door from current position
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Door, meta=(AllowPrivateAccess))
	float Distance;

	/**
	 * Extent when move distance to make door not open on full size (if distance not specified)  
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Door, meta=(AllowPrivateAccess))
	float Extent;
};
