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

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Cloud9/Structures/SavedInfo.h"
#include "Cloud9/Weapon/Structures/WeaponConfig.h"

#include "Cloud9GameInstance.generated.h"

class ACloud9GameMode;

UCLASS()
class CLOUD9_API UCloud9GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	void SaveCharacterInfo(const ACloud9Character* Character);

	void LoadCharacterInfo(ACloud9Character* Character);

	bool HasCharacterInfo(const ACloud9Character* Character) const;

	USoundAttenuation* GetAttenuationSettings() const;

protected:
	UPROPERTY()
	FSavedInfo SavedInfo;

	UPROPERTY(EditDefaultsOnly, Category=Sounds)
	USoundAttenuation* AttenuationSettings;
};
