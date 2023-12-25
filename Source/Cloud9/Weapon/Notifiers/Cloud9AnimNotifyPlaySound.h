// Copyright (c) 2023 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Cloud9AnimNotifyWeapon.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Cloud9AnimNotifyPlaySound.generated.h"

UCLASS(Blueprintable)
class CLOUD9_API UCloud9AnimNotifyPlaySound : public UCloud9AnimNotifyWeapon
{
	GENERATED_BODY()

	UCloud9AnimNotifyPlaySound();

public:
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Settings, meta=(ExposeOnSpawn))
	bool bIsEnabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Settings, meta=(ExposeOnSpawn))
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Preview, meta=(ExposeOnSpawn))
	USoundBase* PreviewSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Preview, meta=(ExposeOnSpawn))
	float PreviewVolume;
};
