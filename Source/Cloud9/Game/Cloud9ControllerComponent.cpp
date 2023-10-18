#include "Cloud9ControllerComponent.h"

#include "Cloud9PlayerController.h"
#include "Cloud9/Character/Cloud9Character.h"

ACloud9Character* ICloud9ControllerComponent::GetCloud9Pawn() const
{
	if (const auto Controller = GetCloud9Controller())
		return Controller->GetPawn<ACloud9Character>();

	return nullptr;
}

ACloud9PlayerController* ICloud9ControllerComponent::GetCloud9Controller() const
{
	if (const auto Component = Cast<UActorComponent>(this))
		return Component->GetOwner<ACloud9PlayerController>();
	
	return nullptr;
}
