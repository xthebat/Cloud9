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

#include "Cloud9WeaponGrenade.h"

#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Tools/Extensions/TVariant.h"
#include "Cloud9/Weapon/Tables/WeaponTableGrenade.h"

FWeaponId ACloud9WeaponGrenade::GetWeaponId() const { return ETVariant::Convert<FWeaponId>(WeaponId); }

bool ACloud9WeaponGrenade::OnInitialize(const FWeaponId& NewWeaponId, FName NewWeaponSkin)
{
	return Super::OnInitialize(NewWeaponId, NewWeaponSkin);
}

void ACloud9WeaponGrenade::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	WEAPON_IS_DEFINED_GUARD();
	WEAPON_IS_DISARMED_GUARD();
	WEAPON_IS_ACTION_IN_PROGRESS_GUARD();

	let Character = GetOwner<ACloud9Character>();
	let WeaponInfo = WeaponDefinition.GetWeaponInfo<FGrenadeWeaponInfo>();
	let PoseMontages = WeaponDefinition.GetPoseMontages(Character->bIsCrouched);

	if (WeaponState.IsActionActive(EWeaponAction::Deploy))
	{
		ExecuteAction(
			EWeaponAction::Deploy,
			WeaponInfo->DeployTime,
			[&] { return PlayAnimMontage(PoseMontages->DeployMontage); },
			[this] { WeaponState.ClearAction(EWeaponAction::Deploy); }
		);
	}
	else if (WeaponState.IsActionActive(EWeaponAction::Primary))
	{
		ExecuteAction(
			EWeaponAction::Primary,
			WeaponInfo->PinpullTime,
			[&] { return PlayAnimMontage(PoseMontages->PrimaryActionMontage); }
		);
	}
	else if (WeaponState.IsActionActive(EWeaponAction::Secondary))
	{
		ExecuteAction(
			EWeaponAction::Secondary,
			WeaponInfo->PinpullTime,
			[&] { return PlayAnimMontage(PoseMontages->SecondaryActionMontage); }
		);

		WeaponState.ClearAction(EWeaponAction::Secondary);
	}
	else if (WeaponState.IsActionActive(EWeaponAction::Third))
	{
		ExecuteAction(
			EWeaponAction::Third,
			WeaponInfo->PinpullTime,
			[&] { return PlayAnimMontage(PoseMontages->SecondaryActionMontage); }
		);

		WeaponState.ClearAction(EWeaponAction::Third);
	}
}
