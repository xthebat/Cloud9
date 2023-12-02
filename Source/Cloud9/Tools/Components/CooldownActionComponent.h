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
#include "Cloud9/Tools/Extensions/UObject.h"
#include "Components/ActorComponent.h"
#include "CooldownActionComponent.generated.h"

namespace CooldownAction
{
	template <typename FunctionType>
	concept callable = std::invocable<FunctionType> && std::same_as<bool, TInvokeResult_T<FunctionType>>;
}


UCLASS(BlueprintType)
class CLOUD9_API UCooldownActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCooldownActionComponent();

	/**
	 * Function setups default values for CooldownActionComponent
	 */
	UCooldownActionComponent* Initialize(float NewDefaultCooldownTime);

public:
	/**
	 * Function executes new action with specified function if cooldown
	 * finished; also setups timer to wait new cooldown and returns true. 
	 * Otherwise (if cooldown not finished yet) just return false.
	 */
	template <CooldownAction::callable FunctionType>
	FORCEINLINE bool Execute(FunctionType&& Function, float OverrideCooldownTime = -1)
	{
		if (not bIsActionInProcess)
		{
			if (not Function())
			{
				TRACE(Warning, "Failed to execute cooldown function in '%s'", *GetName());
				return false;
			}

			if (let Time = OverrideCooldownTime < 0.0f ? DefaultCooldownTime : OverrideCooldownTime; Time > 0.0f)
			{
				bIsActionInProcess = true;
				ActionTimerHandle = this | EUObject::AsyncAfter([=] { bIsActionInProcess = false; }, Time);
			}

			return true;
		}

		return false;
	}

	bool IsExecuting() const;

	// TODO: Add blueprint bindings

protected:
	/**
	 * Cooldown time - next action will be executed only after it
	 */
	UPROPERTY(Category=Timer, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	float DefaultCooldownTime;

	/**
	 * Action timer cooldown
	 */
	UPROPERTY(Category=Implementation, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	FTimerHandle ActionTimerHandle;

	/**
	 * Whether or not action currently in process
	 */
	UPROPERTY(Category=Implementation, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	bool bIsActionInProcess;
};
