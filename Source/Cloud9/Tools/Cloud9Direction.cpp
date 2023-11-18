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

#include "Cloud9Direction.h"

#include "Cloud9/Cloud9.h"

FVector UCloud9Direction::DirectionToActorVector(const AActor* Actor, const EDirection Direction)
{
	if (Direction == EDirection::Right)
	{
		return Actor->GetActorRightVector();
	}

	if (Direction == EDirection::Left)
	{
		return -Actor->GetActorRightVector();
	}

	if (Direction == EDirection::Up)
	{
		return Actor->GetActorUpVector();
	}

	if (Direction == EDirection::Down)
	{
		return -Actor->GetActorUpVector();
	}

	if (Direction == EDirection::Forward)
	{
		return Actor->GetActorForwardVector();
	}

	if (Direction == EDirection::Backward)
	{
		return -Actor->GetActorForwardVector();
	}

	UE_LOG(LogCloud9, Fatal, TEXT("Invalid value for Actor = '%s' Direction = '%d'"), *Actor->GetName(), Direction);
	return {};
}
