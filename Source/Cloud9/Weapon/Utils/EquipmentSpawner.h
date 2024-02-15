// Copyright (c) 2024 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "ItemSpawner.h"
#include "EquipmentSpawner.generated.h"

UCLASS()
class CLOUD9_API AEquipmentSpawner : public AItemSpawner
{
	GENERATED_BODY()

public:
	AEquipmentSpawner();

	virtual AActor* CreateChildActor() override;

	virtual bool ActivateSpawner(ACloud9Character* Character) override;

	virtual bool CanBeDestroyed() const override;

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	UPROPERTY(EditDefaultsOnly, Category=Config)
	UStaticMesh* SampleMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Overrides,
		meta=(PinHiddenByDefault, InlineEditConditionToggle))
	bool bChangeHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Config,
		meta=(UIMin="0", UIMax="100.0", ClampMin="0", ClampMax="20000.0", EditCondition="bChangeHealth"))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Overrides,
		meta=(PinHiddenByDefault, InlineEditConditionToggle))
	bool bChangeArmor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Config,
		meta=(UIMin="0", UIMax="100.0", ClampMin="0", ClampMax="100.0", EditCondition="bChangeArmor"))
	float Armor;

	UPROPERTY(EditAnywhere, Category=Config, meta=(EditCondition="bChangeArmor"))
	bool bHelmet;

	UPROPERTY(BlueprintReadOnly, Category=State)
	bool bHealthActivated;

	UPROPERTY(BlueprintReadOnly, Category=State)
	bool bArmorActivated;

	UPROPERTY(BlueprintReadOnly, Category=State)
	bool bHelmetActivated;
};
