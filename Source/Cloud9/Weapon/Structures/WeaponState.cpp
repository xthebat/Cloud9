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

#include "WeaponState.h"

FWeaponState::FWeaponState() { Reset(); }

void FWeaponState::Reset()
{
	Slot = EWeaponSlot::NotSelected;
	Bond = EWeaponBond::Dropped;
	bIsBoltCycled = false;
	bIsMagazineDetached = false;
	ClearAllActions();
}

void FWeaponState::ClearAllActions() { Actions.SetNumZeroed(EUEnum::Count<EWeaponAction>()); }

bool FWeaponState::ActivateAction(EWeaponAction Action, bool IsReleased)
{
	var& Flag = (*this)[Action];

	let Previous = Flag;

	if (Flag and IsReleased)
	{
		Flag = false;
	}
	else if (not Flag and not IsReleased)
	{
		Flag = true;
	}

	return Previous;
}

void FWeaponState::OnUpdateWeaponAttachment(EWeaponSlot NewSlot, EWeaponBond NewBond, bool Instant)
{
	Slot = NewSlot;
	Bond = NewBond;
	(*this)[EWeaponAction::Deploy] = not Instant and NewBond == EWeaponBond::Armed;
}

void FWeaponState::OnRemovedFromInventory()
{
	Slot = EWeaponSlot::NotSelected;
	Bond = EWeaponBond::Dropped;
	ClearAllActions();
}

EWeaponSlot FWeaponState::GetWeaponSlot() const { return Slot; }

void FWeaponState::BoltCycled(bool State) { bIsBoltCycled = State; }

bool FWeaponState::IsBoltCycled() const { return bIsBoltCycled; }

void FWeaponState::DetachMagazine(bool State) { bIsMagazineDetached = State; }

bool FWeaponState::IsMagazineDetached() const { return bIsMagazineDetached; }
