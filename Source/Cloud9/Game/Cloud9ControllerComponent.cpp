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

#include "Cloud9ControllerComponent.h"

#include "Cloud9/Game/Cloud9PlayerController.h"
#include "Cloud9/Character/Cloud9Character.h"

ACloud9Character* ICloud9ControllerComponent::GetCloud9Pawn() const
{
	if (let Controller = GetCloud9Controller(); IsValid(Controller))
	{
		return Controller->GetPawn<ACloud9Character>();
	}

	return nullptr;
}

ACloud9PlayerController* ICloud9ControllerComponent::GetCloud9Controller() const
{
	if (let Component = Cast<UActorComponent>(this))
	{
		return Component->GetOwner<ACloud9PlayerController>();
	}

	return nullptr;
}
