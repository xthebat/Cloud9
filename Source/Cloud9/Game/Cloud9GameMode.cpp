#include "Cloud9GameMode.h"

#include "Cloud9DeveloperSettings.h"
#include "Cloud9PlayerController.h"
#include "Cloud9/Character/Cloud9Character.h"
#include "UObject/ConstructorHelpers.h"

ACloud9GameMode::ACloud9GameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PlayerControllerClass = ACloud9PlayerController::StaticClass();
	DefaultPawnClass = ACloud9Character::StaticClass();
}

ACloud9Character* ACloud9GameMode::GetCharacter() const
{
	return Cast<ACloud9Character>(GetWorld()->GetFirstPlayerController()->GetCharacter());
}

void ACloud9GameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const auto Settings = UCloud9DeveloperSettings::GetCloud9DeveloperSettings();

	if (Settings->NetGraph > 0)
	{
		const auto Fps = 1.0f / DeltaSeconds;

		const auto Location = GetCharacter()->GetActorLocation();
		const auto Velocity = GetCharacter()->GetVelocity();
		const auto Text = FString::Printf(
			TEXT("Location = %s Velocity = %.0f fps = %.1f"),
			*Location.ToString(),
			Velocity.Size(),
			Fps
		);
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, Text);
	}
}
