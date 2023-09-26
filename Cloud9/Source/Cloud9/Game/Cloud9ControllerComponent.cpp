#include "Cloud9ControllerComponent.h"

#include "Cloud9PlayerController.h"
#include "Cloud9/Character/Cloud9Character.h"

ACloud9Character* ICloud9ControllerComponent::GetPawn() const
{
	if (const auto Controller = GetPlayerController())
		return Controller->GetPawn<ACloud9Character>();

	return nullptr;
}

ACloud9PlayerController* ICloud9ControllerComponent::GetPlayerController() const
{
	if (const auto Component = Cast<UActorComponent>(this))
		return Component->GetOwner<ACloud9PlayerController>();
	
	return nullptr;
}
