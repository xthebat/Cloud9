#pragma once

#include "CoreMinimal.h"
#include "Cloud9ControllerBase.h"
#include "Cloud9KeyboardController.generated.h"


UCLASS()
class CLOUD9_API UCloud9KeyboardController : public UCloud9ControllerBase
{
	GENERATED_BODY()

public:
	void OnMoveForward(float Value);
	void OnMoveRight(float Value);

	void OnWalkPressed();
	void OnWalkReleased();

	void OnCrouchPressed();
	void OnCrouchReleased();
	
	void OnJumpPressed();
	void OnJumpReleased();
};
