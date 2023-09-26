#pragma once

#include "CoreMinimal.h"
#include "Cloud9WeaponBase.h"
#include "Cloud9WeaponSniper.generated.h"

UCLASS()
class CLOUD9_API ACloud9WeaponSniper : public ACloud9WeaponBase
{
	GENERATED_BODY()

public:
	ACloud9WeaponSniper();

protected:
	virtual void BeginPlay() override;
};
