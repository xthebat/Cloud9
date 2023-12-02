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

#include "Cloud9WeaponMelee.h"

#include "Cloud9/Weapon/Enums/Cloud9MeleeActions.h"
#include "Cloud9/Weapon/Tables/Cloud9WeaponTableMelee.h"

const FName ACloud9WeaponMelee::WeaponMeshComponentName = TEXT("WeaponMeshComponent");

ACloud9WeaponMelee::ACloud9WeaponMelee()
{
	Class = EWeaponClass::Melee;
	Actions = StaticEnum<EMeleeAction>();
	bCanBeDropped = false;

	if (WeaponMesh = CreateMesh(WeaponMeshComponentName); not IsValid(WeaponMesh))
	{
		TRACE(Error, "Failed to create WeaponMeshComponent");
		return;
	}
}

void ACloud9WeaponMelee::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	let MyWeaponInfo = GetWeaponInfo<FMeleeWeaponInfo>();
	let Skin = MyWeaponInfo | EFWeaponInfo::GetSkinByName<FMeleeWeaponSkin>();
	WeaponMesh->SetStaticMesh(Skin->Weapon);
}
