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

#include "UWorld.h"
#include "FTextBuilder.h"
#include "Cloud9/Tools/Macro/Logging.h"
#include "Cloud9/Tools/Macro/Operator.h"

namespace EUObject
{
	struct Stringify
	{
	public:
		const UStruct* Type = nullptr;

		FORCEINLINE FString operator()(const UObject* Object) const
		{
			using namespace EFTextBuilder;

			let Text = FTextBuilder()
				| AppendObject{Object, Object->GetClass()}
				| ToText{};
			return Text.ToString();
		}

		OPERATOR_BODY(Stringify)
	};

	template <typename BlockType>
	struct AsyncAfter
	{
		const BlockType& Block;
		float InRate;
		bool bInLoop = false;

		FORCEINLINE FTimerHandle operator()(const UObject* Self) const
		{
			let MyWorld = Self->GetWorld();

			if (not IsValid(MyWorld))
			{
				// Should we crash or not crash in this case?
				log(Fatal, "Timer not set due to game World not exists")
				return {};
			}

			return MyWorld | EUWorld::AsyncAfter{Block, InRate, bInLoop};
		}

		OPERATOR_BODY(AsyncAfter)
	};

	struct IsTimerActive
	{
		FTimerHandle TimerHandle;

		FORCEINLINE bool operator()(const UObject* Self) const
		{
			let MyWorld = Self->GetWorld();

			if (not IsValid(MyWorld))
			{
				// Should we crash or not crash in this case?
				log(Fatal, "Timer not set due to game World not exists")
				return false;
			}

			return MyWorld | EUWorld::IsTimerActive{TimerHandle};
		}

		OPERATOR_BODY(IsTimerActive)
	};
}
