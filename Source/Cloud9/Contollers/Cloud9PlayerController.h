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

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "Cloud9PlayerController.generated.h"

class UCloud9KeyboardController;
class UCloud9MouseController;
class UCloud9ConsoleComponent;
class ACloud9Character;

UCLASS()
class ACloud9PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	static inline const FName ConsoleName = TEXT("Console");
	static inline const FName KeyboardControllerName = TEXT("KeyboardController");
	static inline const FName MouseControllerName = TEXT("MouseController");

	ACloud9PlayerController();

	UCloud9MouseController* GetMouseControllerComponent() const;
	UCloud9KeyboardController* GetKeyboardControllerComponent() const;

	void ShowMouseCursor(bool NewValue);

protected:
	virtual void SetupInputComponent() override;
	virtual bool ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Console)
	UCloud9ConsoleComponent* ConsoleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Controllers)
	UCloud9MouseController* MouseController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Controllers)
	UCloud9KeyboardController* KeyboardController;
};
