#pragma once

#include "CoreMinimal.h"
#include "Cloud9WeaponBase.h"
#include "Cloud9WeaponPistol.generated.h"

UCLASS()
class CLOUD9_API ACloud9WeaponPistol : public ACloud9WeaponBase
{
	GENERATED_BODY()

public:
	ACloud9WeaponPistol();

protected:
	virtual void BeginPlay() override;
};
