// 


#include "Cloud9ControllerBase.h"

#include "Cloud9PlayerController.h"
#include "Cloud9/Character/Cloud9Character.h"

ACloud9Character* UCloud9ControllerBase::GetPawn() const
{
	return GetOwner<ACloud9PlayerController>()->GetPawn<ACloud9Character>();
}
