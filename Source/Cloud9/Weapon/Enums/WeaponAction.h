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

#pragma once

UENUM(BlueprintType)
enum class EWeaponAction : uint8
{
	Deploy = 0 UMETA(DisplayName = "Deploy"),

	ReloadStart = 1 UMETA(DisplayName="Reload"),
	ReloadLoop = 2 UMETA(DisplayName="ReloadLoop"),
	ReloadEnd = 3 UMETA(DisplayName="ReloadEnd"),

	PrimaryStart = 4 UMETA(DisplayName="Primary"),
	PrimaryLoop = 5 UMETA(DisplayName="PrimaryLoop"),
	PrimaryEnd = 6 UMETA(DisplayName="PrimaryEnd"),

	Secondary = 7 UMETA(DisplayName="Secondary"),
	Third = 8 UMETA(DisplayName="Third"),
};

// HotReload won't work if try to use StaticEnum in constructor
inline static let WeaponActionCount = 9;
