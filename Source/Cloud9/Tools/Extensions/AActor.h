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

#include "EDirection.h"
#include "UWorld.h"
#include "Cloud9/Tools/Macro/Logging.h"
#include "Cloud9/Tools/Macro/Operator.h"

namespace EAActor
{
	struct ToDirectionVector
	{
		EDirection Direction;

		FVector operator()(const AActor* Self) const
		{
			CRASH_IF_FAIL(Self != nullptr, "Actor should not be nullptr");

			if (Direction == EDirection::Right)
			{
				return Self->GetActorRightVector();
			}

			if (Direction == EDirection::Left)
			{
				return -Self->GetActorRightVector();
			}

			if (Direction == EDirection::Up)
			{
				return Self->GetActorUpVector();
			}

			if (Direction == EDirection::Down)
			{
				return -Self->GetActorUpVector();
			}

			if (Direction == EDirection::Forward)
			{
				return Self->GetActorForwardVector();
			}

			if (Direction == EDirection::Backward)
			{
				return -Self->GetActorForwardVector();
			}

			FUNCTION_FATAL("Invalid value Actor = '%s' Direction = '%d'", *Self->GetName(), Direction);
			return {};
		}

		OPERATOR_BODY(ToDirectionVector)
	};

	struct DestroyAfter
	{
		float Delay;

		FTimerHandle operator()(AActor* Self) const
		{
			CRASH_IF_FAIL(Self != nullptr, "Actor should not be nullptr");

			// May be not needed?
			if (Self->IsPendingKill())
			{
				FUNCTION_WARNING("Object '%s' already waiting it's death", *Self->GetName());
				return {};
			}

			if (Delay > 0)
			{
				return Self->GetWorld() | EUWorld::AsyncAfter{[Self] { Self->Destroy(); }, Delay};
			}

			if (Delay == 0.0f)
			{
				Self->Destroy();
				return {};
			}

			FUNCTION_VERBOSE("[%s] Set to be destroyed with delay but won't be because Delay<0.0f", *Self->GetName());
			return {};
		}

		OPERATOR_BODY(DestroyAfter)
	};
}
