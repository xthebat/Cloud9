#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Cloud9KeyboardController.generated.h"


UCLASS()
class CLOUD9_API UCloud9KeyboardController : public UActorComponent
{
	GENERATED_BODY()

public:
	UCloud9KeyboardController();
	
	void MoveForward(float Value);
	void MoveRight(float Value);

	void WalkPressed();
	void CrouchPressed();

	void WalkReleased();
	void CrouchReleased();
};
