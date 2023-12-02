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

#include "Cloud9/Cloud9.h"

namespace EUWorld
{
	template <typename TClass, typename TInitializer>
	struct SpawnActorInitialized : TOperator<SpawnActorInitialized<TClass, TInitializer>>
	{
	public:
		SpawnActorInitialized(
			UClass* Class,
			TInitializer Initializer,
			const FTransform& Transform = {},
			AActor* Owner = nullptr,
			APawn* Instigator = nullptr
		)
			: Class(Class)
			, Initializer(Initializer)
			, Transform(Transform)
			, Owner(Owner)
			, Instigator(Instigator) { }

		TClass* operator()(UWorld* World) const
		{
			let Actor = World->SpawnActorDeferred<TClass>(Class, Transform, Owner, Instigator);
			if (IsValid(Actor) && Initializer(Actor))
			{
				Actor->FinishSpawning(Transform);
				return Actor;
			}
			return nullptr;
		}

	private:
		UClass* Class;
		TInitializer Initializer;
		const FTransform& Transform;
		AActor* Owner = nullptr;
		APawn* Instigator = nullptr;
	};

	template <template<typename> typename TClassInfo, typename TClass, typename TInit>
	SpawnActorInitialized(
		TClassInfo<TClass> Class,
		TInit Init,
		...) -> SpawnActorInitialized<TClass, TInit>;

	template <typename FunctionType>
	struct AsyncAfter : TOperator<AsyncAfter<FunctionType>>
	{
		explicit AsyncAfter(FunctionType Function, float InRate, bool bInLoop = false)
			: Function(Function)
			, InRate(InRate)
			, bInLoop(bInLoop) {}

		FORCEINLINE FTimerHandle operator()(const UWorld* Self) const
		{
			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindLambda(Function);
			Self->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, InRate, bInLoop);
			return TimerHandle;
		}

	private:
		FunctionType Function;
		float InRate;
		bool bInLoop;
	};

	struct IsTimerActive : TOperator<IsTimerActive>
	{
		explicit IsTimerActive(const FTimerHandle& TimerHandle)
			: TimerHandle(TimerHandle) {}

		FORCEINLINE bool operator()(const UWorld* Self) const
		{
			return Self->GetTimerManager().IsTimerActive(TimerHandle);
		}

	private:
		FTimerHandle TimerHandle;
	};
}
