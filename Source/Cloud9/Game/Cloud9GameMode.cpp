// Copyright (c) 2023 Alexei Gladkikh
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#include "Cloud9GameMode.h"

#include "Cloud9DeveloperSettings.h"
#include "Cloud9PlayerController.h"
#include "Cloud9/Cloud9.h"
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

	let Settings = UCloud9DeveloperSettings::GetCloud9DeveloperSettings();

	if (Settings->NetGraph > 0)
	{
		let Fps = 1.0f / DeltaSeconds;
		let Location = GetCharacter()->GetActorLocation();
		let Velocity = GetCharacter()->GetVelocity();
		let Text = FString::Printf(
			TEXT("Location = %s Velocity = %.0f fps = %.1f"),
			*Location.ToString(),
			Velocity.Size(),
			Fps
		);
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, Text);
	}
}
