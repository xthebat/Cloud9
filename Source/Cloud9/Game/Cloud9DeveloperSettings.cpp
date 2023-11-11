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

#include "Cloud9DeveloperSettings.h"

#include "Cloud9/Cloud9.h"
#include "Cloud9/Tools/Extensions/UObject.h"
#include "Cloud9/Tools/Extensions/WhenOrNone.h"

// ReSharper disable once CppPossiblyUninitializedMember
UCloud9DeveloperSettings::UCloud9DeveloperSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {}

const UCloud9DeveloperSettings* UCloud9DeveloperSettings::Get()
{
	static let Settings = StaticClass()->GetDefaultObject<UCloud9DeveloperSettings>();
	Settings->InitializeCVars();
	return Settings;
}

void UCloud9DeveloperSettings::Save()
{
	UpdateDefaultConfigFile();
	Log();
}

template <typename TObject, typename TProperty>
auto RegisterConsoleVariable(TObject* Object, TProperty* Property)
{
	using TCppType = typename TProperty::TCppType;

	let ConsoleVariable = Property->GetMetaData(TEXT("ConsoleVariable"));
	let ToolTip = Property->GetMetaData(TEXT("ToolTip"));

	let ValuePtr = Property->template ContainerPtrToValuePtr<TCppType>(Object);

	let ConsoleManager = &IConsoleManager::Get();

	// FAutoConsoleVariableSink
	let CVar = ConsoleManager->RegisterConsoleVariableRef(*ConsoleVariable, *ValuePtr, *ToolTip);

	CVar->AsVariable()->SetOnChangedCallback(
		FConsoleVariableDelegate::CreateLambda([Object](auto Arg) { Object->Save(); })
	);

	return CVar;
}

void UCloud9DeveloperSettings::InitializeCVars()
{
	static var bIsConsoleInitialized = false;

	if (!bIsConsoleInitialized)
	{
		bIsConsoleInitialized = true;

		for (TFieldIterator<FProperty> It(GetClass()); It; ++It)
		{
			if (let Property = *It; Property->HasMetaData(TEXT("ConsoleVariable")))
			{
				// TODO: Refactor with EachOrNone
				let CVar = Property | WhenOrNone{
					[this](FBoolProperty* Arg) { return RegisterConsoleVariable(this, Arg); },
					[this](FIntProperty* Arg) { return RegisterConsoleVariable(this, Arg); },
					[this](FFloatProperty* Arg) { return RegisterConsoleVariable(this, Arg); },
				};

				if (!CVar)
				{
					UE_LOG(
						LogCloud9,
						Warning,
						TEXT("Property with meta key 'ConsoleVariable' %s wasn't registered"
							" as console variable due to unsupported type '%s'"),
						*Property->GetName(),
						*Property->GetCPPType()
					)
				}
			}
		}

		Log();
	}
}

void UCloud9DeveloperSettings::Log() const
{
	let String = this | EUObject::Stringify();
	UE_LOG(LogCloud9, Display, TEXT("%s"), *String);
}

void UCloud9DeveloperSettings::PostInitProperties()
{
	InitializeCVars();
	Super::PostInitProperties();
#if WITH_EDITOR
	if (IsTemplate())
	{
		ImportConsoleVariableValues();
	}
#endif
}

#if WITH_EDITOR
void UCloud9DeveloperSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	InitializeCVars();
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.Property)
	{
		ExportValuesToConsoleVariables(PropertyChangedEvent.Property);
	}
}
#endif
