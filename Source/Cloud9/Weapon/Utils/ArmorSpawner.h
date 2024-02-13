// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "ItemSpawner.h"
#include "ArmorSpawner.generated.h"

UCLASS()
class CLOUD9_API AArmorSpawner : public AItemSpawner
{
	GENERATED_BODY()

public:
	AArmorSpawner();

	virtual AActor* CreateChildActor() override;

	virtual bool ActivateSpawner(ACloud9Character* Character) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category=Config)
	UStaticMesh* SampleMesh;

	UPROPERTY(EditAnywhere, Category=Config)
	int ArmorIncrement;

	UPROPERTY(EditAnywhere, Category=Config)
	bool bIsAddHelmet;
};
