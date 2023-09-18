#pragma once

#include "CoreMinimal.h"
#include "Cloud9ControllerBase.h"
#include "Components/ActorComponent.h"
#include "Cloud9KeyboardController.generated.h"


UCLASS()
class CLOUD9_API UCloud9KeyboardController : public UCloud9ControllerBase
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
