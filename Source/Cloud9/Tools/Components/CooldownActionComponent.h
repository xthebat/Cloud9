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

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"
#include "Cloud9/Tools/Extensions/UObject.h"
#include "Cloud9/Tools/Macro/Template.h"

#include "CooldownActionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCooldownActionCompleteDelegate);

UCLASS(BlueprintType)
class CLOUD9_API UCooldownActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCooldownActionComponent()
	{
		bIsExecuting = false;
	}

public:
	/**
	 * Function executes new action with specified function if cooldown
	 * finished; also setups timer to wait new cooldown and returns true. 
	 * Otherwise (if cooldown not finished yet) just return false.
	 */
	template <typename OnExecuteType, typename OnCompleteType>
	FORCEINLINE bool Execute(
		float CooldownTime,
		OnExecuteType&& OnExecute,
		OnCompleteType&& OnComplete)
	{
		if (not bIsExecuting)
		{
			if (not OnExecute())
			{
				log(Error, "[Action='%s'] Failed to execute function", *GetName());
				OnComplete();
				return false;
			}

			bIsExecuting = true;

			// lambda should be copied otherwise GC fuck up objects inside it
			TimerHandle = GetWorld() | EUWorld::AsyncAfter{
				[this, OnComplete]
				{
					OnComplete();
					bIsExecuting = false;
				},
				CooldownTime
			};
		}

		return false;
	}

	template <typename OnExecuteType>
	FORCEINLINE bool Execute(float CooldownTime, OnExecuteType&& OnExecute)
	{
		if (not bIsExecuting)
		{
			if (not OnExecute())
			{
				log(Error, "[Action='%s'] Failed to execute function", *GetName());
				return false;
			}
			bIsExecuting = true;
			TimerHandle = GetWorld() | EUWorld::AsyncAfter{[this] { bIsExecuting = false; }, CooldownTime};
		}

		return false;
	}

	bool IsExecuting() const { return bIsExecuting; }

protected:
	/**
	 * Whether or not action currently in process
	 */
	UPROPERTY(Category=Implementation, BlueprintReadOnly, meta=(AllowPrivateAccess))
	bool bIsExecuting;

	UPROPERTY(Category=Implementation, BlueprintReadOnly)
	FTimerHandle TimerHandle;
};
