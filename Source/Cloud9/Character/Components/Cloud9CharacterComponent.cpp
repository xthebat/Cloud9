#include "Cloud9CharacterComponent.h"

#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Game/Cloud9PlayerController.h"


ACloud9Character* ICloud9CharacterComponent::GetPawn() const
{
	if (const auto Component = Cast<UActorComponent>(this))
		return Component->GetOwner<ACloud9Character>();

	return nullptr;
}

ACloud9PlayerController* ICloud9CharacterComponent::GetPlayerController() const
{
	if (const auto Pawn = GetPawn())
		return Cast<ACloud9PlayerController>(Pawn->GetController());

	return nullptr;
}
