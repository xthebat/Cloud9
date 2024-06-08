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
#include "Cloud9ControllerComponent.h"

#include "Cloud9KeyboardController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveDelegate);

UCLASS()
class CLOUD9_API UCloud9KeyboardController
	: public UActorComponent
	, public ICloud9ControllerComponent
{
	GENERATED_BODY()

public:
	UCloud9KeyboardController();

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	void OnPrimaryActionPressed();
	void OnPrimaryActionReleased();

	void OnSecondaryActionPressed();
	void OnSecondaryActionReleased();

	void OnUseAction();

	FOnMoveDelegate OnMoveDelegate;

	void OnMoveForward(float Value);
	void OnMoveBackward(float Value);
	void OnMoveRight(float Value);
	void OnMoveLeft(float Value);

	void OnWalkPressed();
	void OnWalkReleased();

	void OnCrouchPressed();
	void OnCrouchReleased();

	void OnJumpPressed();
	void OnJumpReleased();

	void OnSlot1();
	void OnSlot2();
	void OnSlot3();
	void OnSlot4();
	void OnSlot5();
	void OnReloadPressed();
	void OnReloadReleased();

	void OnCursorSelfAim();

protected:
	float ForwardScale;
	float BackwardScale;
	float LeftScale;
	float RightScale;

	template <typename GetDirectionType>
	FORCEINLINE void UpdateMove(GetDirectionType GetDirection, float Scale);

	template <class FunctionType>
	FORCEINLINE void WeaponAction(FunctionType Function);
};
