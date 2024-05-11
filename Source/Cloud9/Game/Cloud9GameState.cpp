// Copyright (c) 2024 Alexei Gladkikh

#include "Cloud9GameState.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"

#include "Cloud9/Character/Cloud9Character.h"
#include "Cloud9/Game/Cloud9DeveloperSettings.h"

ACloud9GameState::ACloud9GameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACloud9GameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	static let Settings = UCloud9DeveloperSettings::Get();

	if (Settings->NetGraph > 0)
	{
		let MyWorld = GetWorld();
		AssertOrVoid(IsValid(MyWorld), Error, "World isn't exist");

		// TODO: Make valid for any player
		let FirstPlayerController = MyWorld->GetFirstPlayerController();
		AssertOrVoid(IsValid(FirstPlayerController), Error, "World isn't exist");

		let Character = Cast<ACloud9Character>(FirstPlayerController->GetCharacter());
		AssertOrVoid(IsValid(Character), Error, "Character isn't exist");

		let Fps = 1.0f / DeltaSeconds;
		let Location = Character->GetActorLocation();
		let Velocity = Character->GetVelocity();
		let Text = FString::Printf(
			TEXT("Location = %s Scaled Velocity = %.2f fps = %.1f"),
			*Location.ToString(),
			Velocity.Size() / UCloud9CharacterMovement::SpeedScaleCoefficient,
			Fps
		);

		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, Text);
	}
}
