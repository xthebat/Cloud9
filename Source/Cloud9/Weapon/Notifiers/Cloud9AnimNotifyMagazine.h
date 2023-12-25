// Copyright (c) 2023 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"

#include "Cloud9AnimNotifyWeapon.h"

#include "Cloud9AnimNotifyMagazine.generated.h"

UENUM(BlueprintType)
enum class EMagazineAction : uint8
{
	Nothing = 0 UMETA(DisplayName = "Nothing"),
	Detach = 1 UMETA(DisplayName = "Detach"),
	Attach = 2 UMETA(DisplayName = "Attach"),
	Drop = 3 UMETA(DisplayName = "Drop"),
};

UCLASS(Blueprintable)
class CLOUD9_API UCloud9AnimNotifyMagazine : public UCloud9AnimNotifyWeapon
{
	GENERATED_BODY()

	UCloud9AnimNotifyMagazine();

public:
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Settings, meta=(ExposeOnSpawn))
	EMagazineAction Action;
};
