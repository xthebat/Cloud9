// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9GameState.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"

#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Game/Cloud9DeveloperSettings.h"

void ACloud9GameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (let Settings = UCloud9DeveloperSettings::Get(); Settings->NetGraph > 0)
	{
		let MyWorld = GetWorld();

		if (not IsValid(MyWorld))
		{
			log(Error, "World isn't exist")
			return;
		}

		// TODO: Make valid for any player
		let FirstPlayerController = MyWorld->GetFirstPlayerController();

		if (not IsValid(FirstPlayerController))
		{
			log(Error, "FirstPlayerController isn't exist")
			return;
		}

		let Character = Cast<ACloud9Character>(FirstPlayerController->GetCharacter());

		if (not IsValid(FirstPlayerController))
		{
			log(Error, "Character isn't exist")
			return;
		}

		let Fps = 1.0f / DeltaSeconds;
		let Location = Character->GetActorLocation();
		let Velocity = Character->GetVelocity();
		let Text = FString::Printf(
			TEXT("Location = %s Velocity = %.0f fps = %.1f"),
			*Location.ToString(),
			Velocity.Size(),
			Fps
		);

		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, Text);
	}
}
