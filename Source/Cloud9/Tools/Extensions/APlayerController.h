// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"
#include "Cloud9/Tools/Macro/Operator.h"

namespace EAPlayerController
{
	struct GetHitUnderCursor
	{
		ECollisionChannel TraceChannel;
		bool bTraceComplex;
		const TArray<AActor*>& ActorsToIgnore{};

		TOptional<FHitResult> operator()(const APlayerController* Self) const
		{
			CRASH_IF_FAIL(Self != nullptr, "APlayerController should not be nullptr");

			let LocalPlayer = Cast<ULocalPlayer>(Self->Player);
			if (IsValid(LocalPlayer) and LocalPlayer->ViewportClient)
			{
				FVector2D MousePosition;
				if (LocalPlayer->ViewportClient->GetMousePosition(MousePosition))
				{
					FHitResult HitResult;
					FCollisionQueryParams Params;
					Params.bTraceComplex = bTraceComplex;
					Params.AddIgnoredActors(ActorsToIgnore);
					if (Self->GetHitResultAtScreenPosition(MousePosition, TraceChannel, Params, HitResult))
					{
						return HitResult;
					}
				}
			}

			return {};
		}

		OPERATOR_BODY(GetHitUnderCursor)
	};
}
