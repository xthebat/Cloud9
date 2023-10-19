#pragma once

#include "CoreMinimal.h"
#include "Cloud9ControllerComponent.h"
#include "Cloud9KeyboardController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveDelegate);

UCLASS()
class CLOUD9_API UCloud9KeyboardController
	: public UActorComponent
	, public ICloud9ControllerComponent
{
	GENERATED_BODY()

public:
	UCloud9KeyboardController();
	
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	FOnMoveDelegate OnMoveDelegate;

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

protected:
	float ForwardScale;
	float RightScale;

	template <typename TGetDirection>
	void UpdateMove(TGetDirection GetDirection, float Scale);
};
