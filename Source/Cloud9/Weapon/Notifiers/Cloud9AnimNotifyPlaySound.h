// Copyright (c) 2023 Alexei Gladkikh

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Cloud9AnimNotifyPlaySound.generated.h"

UCLASS(Blueprintable)
class CLOUD9_API UCloud9AnimNotifyPlaySound : public UAnimNotify
{
	GENERATED_BODY()

public:
	// virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	int Index;
};
