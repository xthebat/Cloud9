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

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"
#include "Cloud9/Tools/Macro/Operator.h"

namespace Private_EUWorld
{
	extern TSet<FTimerHandle> ActiveTimers;
}

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
		BlockType Block;
		float InRate;
		bool bInLoop = false;

		FTimerHandle TimerHandle{};

		FORCEINLINE FTimerHandle operator()(const UWorld* Self)
		{
			if (InRate == 0.0f)
			{
				Block();
				return {};
			}

			assertf(Self != nullptr, "World should not be nullptr to start timer");

			let Delegate = FTimerDelegate::CreateStatic(&AsyncAfter::Execute, this);
			Self->GetTimerManager().SetTimer(TimerHandle, Delegate, InRate, bInLoop);
			Private_EUWorld::ActiveTimers.Add(TimerHandle);
			return TimerHandle;
		}

		static void Execute(AsyncAfter* What)
		{
			What->Block();
			Private_EUWorld::ActiveTimers.Remove(What->TimerHandle);
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

	struct ClearTimer
	{
		FTimerHandle& TimerHandle;

		FORCEINLINE void operator()(const UWorld* Self) const
		{
			Self->GetTimerManager().ClearTimer(TimerHandle);
			Private_EUWorld::ActiveTimers.Remove(TimerHandle);
		}

		OPERATOR_BODY(ClearTimer)
	};

	struct ClearAllTimers
	{
		FORCEINLINE void operator()(const UWorld* Self) const
		{
			for (var It : Private_EUWorld::ActiveTimers)
			{
				Self->GetTimerManager().ClearTimer(It);
			}

			Private_EUWorld::ActiveTimers.Empty();
		}

		OPERATOR_BODY(ClearAllTimers)
	};
}
