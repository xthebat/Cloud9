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

UENUM(BlueprintType)
enum class EFirearm : uint8
{
	Deagle UMETA(DisplayName="Deagle"),
	Revolver UMETA(DisplayName="Revolver"),
	Elite UMETA(DisplayName="Elite"),
	Fiveseven UMETA(DisplayName="Fiveseven"),
	Glock UMETA(DisplayName="Glock"),
	P2000 UMETA(DisplayName="P2000"),
	Usp UMETA(DisplayName="Usp"),
	P250 UMETA(DisplayName="P250"),
	Cz75A UMETA(DisplayName="Cz75A"),
	Tec9 UMETA(DisplayName="Tec9"),
	Mag7 UMETA(DisplayName="Mag7"),
	Nova UMETA(DisplayName="Nova"),
	SawedOff UMETA(DisplayName="SawedOff"),
	Xm1014 UMETA(DisplayName="Xm1014"),
	Bizon UMETA(DisplayName="Bizon"),
	Mac10 UMETA(DisplayName="Mac10"),
	MP7 UMETA(DisplayName="MP7"),
	MP5 UMETA(DisplayName="MP5"),
	MP9 UMETA(DisplayName="MP9"),
	P90 UMETA(DisplayName="P90"),
	Ump45 UMETA(DisplayName="Ump45"),
	Ak47 UMETA(DisplayName="Ak47"),
	Aug UMETA(DisplayName="Aug"),
	Famas UMETA(DisplayName="Famas"),
	Galilar UMETA(DisplayName="Galilar"),
	M4A4 UMETA(DisplayName="M4A4"),
	M4A1S UMETA(DisplayName="M4A1S"),
	SG556 UMETA(DisplayName="SG556"),
	M249 UMETA(DisplayName="M249"),
	Negev UMETA(DisplayName="Negev"),
	Awp UMETA(DisplayName="Awp"),
	G3SG1 UMETA(DisplayName="G3SG1"),
	Scar20 UMETA(DisplayName="Scar20"),
	Ssg08 UMETA(DisplayName="Ssg08"),
};
