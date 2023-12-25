// Copyright 2023 (c) 
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include "Kismet/GameplayStatics.h"

#include "Cloud9/Tools/Macro/Operator.h"
#include "Cloud9/Tools/Cloud9ToolsLibrary.h"

namespace EUSoundBase
{
	struct PlaySoundAtLocation
	{
		FVector Location;
		float VolumeMultiplier = 1.f;
		float PitchMultiplier = 1.f;
		float StartTime = 0.f;
		const UObject* World = nullptr;

		FORCEINLINE void operator()(USoundBase* Self) const
		{
			let MyWorld = World == nullptr ? UCloud9ToolsLibrary::GetGameWorld() : World;
			UGameplayStatics::PlaySoundAtLocation(
				MyWorld,
				Self,
				Location,
				VolumeMultiplier,
				PitchMultiplier,
				StartTime);
		}

		OPERATOR_BODY(PlaySoundAtLocation)
	};

	struct PlaySound2D
	{
		float VolumeMultiplier = 1.f;
		const UObject* World = nullptr;

		FORCEINLINE void operator()(USoundBase* Self) const
		{
			let MyWorld = World == nullptr ? UCloud9ToolsLibrary::GetGameWorld() : World;
			UGameplayStatics::PlaySound2D(MyWorld, Self, VolumeMultiplier);
		}

		OPERATOR_BODY(PlaySound2D)
	};
}
