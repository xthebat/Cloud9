#pragma once

#include "CoreMinimal.h"
#include "Cloud9ControllerComponent.h"
#include "Cloud9KeyboardController.generated.h"


UCLASS()
class CLOUD9_API UCloud9KeyboardController
	: public UActorComponent
	, public ICloud9ControllerComponent
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
	
	void OnSlot1();
	void OnSlot2();
	void OnSlot3();
	void OnSlot4();
	void OnSlot5();
	void Reload();
};
