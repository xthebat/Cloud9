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
#include "Components/ActorComponent.h"
#include "Cloud9/Tools/Extensions/UObject.h"
#include "CooldownActionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCooldownActionCompleteDelegate);

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
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	/**
	 * Function executes new action with specified function if cooldown
	 * finished; also setups timer to wait new cooldown and returns true. 
	 * Otherwise (if cooldown not finished yet) just return false.
	 */
	template <typename FunctionType>
	FORCEINLINE UCooldownActionComponent* Execute(FunctionType&& Function, float OverrideCooldownTime = -1)
	{
		if (not bIsActionInProcess)
		{
			if (not Function())
			{
				log(Warning, "Failed to execute cooldown function in '%s'", *GetName());
				bIsSuccess = false;
				return this;
			}

			bIsSuccess = true;
			RemainTime = OverrideCooldownTime < 0.0f ? DefaultCooldownTime : OverrideCooldownTime;

			if (RemainTime > 0.0f)
			{
				bIsActionInProcess = true;
				SetComponentTickEnabled(true);
				return this;
			}
		}

		return this;
	}

	template <typename FunctionType>
	void OnComplete(FunctionType&& Function)
	{
		OnCompleteDelegate = TDelegate<void()>::CreateLambda(Function);
	}

	bool IsSuccess() const { return bIsSuccess; }

	bool IsExecuting() const;

protected:
	/**
	 * Cooldown time - next action will be executed only after it
	 */
	UPROPERTY(Category=Timer, BlueprintReadOnly, meta=(AllowPrivateAccess))
	float DefaultCooldownTime;

	/**
	 * Remain cooldown time
	 */
	UPROPERTY(Category=Implementation, BlueprintReadOnly, meta=(AllowPrivateAccess))
	float RemainTime;

	UPROPERTY(Category=Implementation, BlueprintReadOnly, meta=(AllowPrivateAccess))
	bool bIsSuccess;

	/**
	 * Whether or not action currently in process
	 */
	UPROPERTY(Category=Implementation, BlueprintReadOnly, meta=(AllowPrivateAccess))
	bool bIsActionInProcess;

	UPROPERTY(Category=Implementation, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FOnCooldownActionCompleteDelegate OnCompleteDelegate;
};
