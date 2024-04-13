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

#include "Cloud9/Tools/Macro/Operator.h"
#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Concepts.h"

namespace EFVector
{
	struct Normalize
	{
		template <typename SelfType> requires Concepts::convertiable<SelfType, FVector>
		FORCEINLINE FVector operator()(SelfType&& Self) const
		{
			FVector Normalized = Self;
			Normalized.Normalize();
			return Normalized;
		}

		OPERATOR_BODY(Normalize)
	};

	FVector VInterpTo(
		const FVector Current,
		const FVector Target,
		float DeltaTime,
		const FVector InterpSpeed)
	{
		let ClampLerp = [](auto Current, auto Dist, auto Alpha, auto Target)
		{
			return Alpha <= 0.0f ? Target : Current + Dist * FMath::Clamp(Alpha, 0.0f, 1.0f);
		};

		// Distance to reach
		let Dist = Target - Current;

		// If distance is too small, just set the desired location
		if (Dist.SizeSquared() < KINDA_SMALL_NUMBER)
		{
			return Target;
		}

		let Alpha = DeltaTime * InterpSpeed;

		return {
			ClampLerp(Current.X, Dist.X, Alpha.X, Target.X),
			ClampLerp(Current.Y, Dist.Y, Alpha.Y, Target.Y),
			ClampLerp(Current.Z, Dist.Z, Alpha.Z, Target.Z),
		};
	}

	inline FVector Random(FVector Min, FVector Max, FVector Grid)
	{
		let Vector = FMath::RandPointInBox({Min, Max});
		return {
			FMath::GridSnap(Vector.X, Grid.X),
			FMath::GridSnap(Vector.Y, Grid.Y),
			FMath::GridSnap(Vector.Z, Grid.Z),
		};
	}
}
