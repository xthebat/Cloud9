﻿// Copyright (c) 2023 Alexei Gladkikh
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

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Operator.h"

namespace EUWorld
{
	template <typename ClassType>
	struct SpawnActor
	{
		using FInitializerType = TFunction<bool(ClassType*)>;

		FInitializerType Initializer;
		const FTransform& Transform = {};
		AActor* Owner = nullptr;
		APawn* Instigator = nullptr;

		FORCEINLINE ClassType* operator()(UWorld* Self) const
		{
			let Actor = Self->SpawnActorDeferred<ClassType>(ClassType::StaticClass(), Transform, Owner, Instigator);
			if (IsValid(Actor) and Initializer(Actor))
			{
				Actor->FinishSpawning(Transform);
				return Actor;
			}
			return nullptr;
		}

		OPERATOR_BODY(SpawnActor)
	};

	template <typename BlockType>
	struct AsyncAfter
	{
		const BlockType& Block;
		float InRate;
		bool bInLoop = false;

		FORCEINLINE FTimerHandle operator()(const UWorld* Self) const
		{
			if (InRate == 0.0f)
			{
				Block();
				return {};
			}

			assertf(Self != nullptr, "World should not be nullptr to start timer");
			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindLambda(Block);
			Self->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, InRate, bInLoop);
			return TimerHandle;
		}

		OPERATOR_BODY(AsyncAfter)
	};

	struct IsTimerActive
	{
		FTimerHandle TimerHandle;

		FORCEINLINE bool operator()(const UWorld* Self) const
		{
			return Self->GetTimerManager().IsTimerActive(TimerHandle);
		}

		OPERATOR_BODY(IsTimerActive)
	};
}
