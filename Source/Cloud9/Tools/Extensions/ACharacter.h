// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"
#include "Cloud9/Tools/Macro/Operator.h"

namespace EACharacter
{
	constexpr int InvalidPlayerId = 0xCAFEAFFE;

	struct GetPlayerId
	{
		int operator()(const ACharacter* Self) const
		{
			CRASH_IF_FAIL(Self != nullptr, "ACharacter should not be nullptr");

			if (let PlayerState = Self->GetPlayerState())
			{
				return PlayerState->GetPlayerId();
			}

			return InvalidPlayerId;
		}

		OPERATOR_BODY(GetPlayerId)
	};

	constexpr bool IsPlayerIdValid(int PlayerId) { return PlayerId != InvalidPlayerId; }
}
