// 


#include "Cloud9ControllerBase.h"

#include "Cloud9PlayerController.h"
#include "Cloud9/Character/Cloud9Character.h"

ACloud9PlayerController* UCloud9ControllerBase::GetPlayerController() const
{
	return GetOwner<ACloud9PlayerController>();
}

ACloud9Character* UCloud9ControllerBase::GetPawn() const
{
	if (const auto Controller = GetOwner<ACloud9PlayerController>())
		return Controller->GetPawn<ACloud9Character>();

	return nullptr;
}
