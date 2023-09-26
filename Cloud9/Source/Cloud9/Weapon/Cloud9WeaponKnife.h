// 

#pragma once

#include "CoreMinimal.h"
#include "Cloud9WeaponBase.h"
#include "Cloud9WeaponKnife.generated.h"

UCLASS()
class CLOUD9_API ACloud9WeaponKnife : public ACloud9WeaponBase
{
	GENERATED_BODY()

public:
	ACloud9WeaponKnife();

protected:
	virtual void BeginPlay() override;
};
