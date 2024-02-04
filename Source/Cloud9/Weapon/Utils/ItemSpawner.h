// Copyright (c) 2024 Alexei Gladkikh
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
#include "GameFramework/Actor.h"
#include "Cloud9/Tools/Macro/Common.h"
#include "ItemSpawner.generated.h"

class ISpawnerDelegateComponent;
class ACloud9Character;
class UNiagaraSystem;
class UNiagaraComponent;
class UBoxComponent;

UCLASS()
class CLOUD9_API AItemSpawner : public AActor
{
	GENERATED_BODY()

public:
	static FName RootComponentName;
	static FName TriggerBoxComponentName;
	static FName ItemSampleComponentName;
	static FName GlowingEffectComponentName;
	static FName SpriteComponentName;

public:
	AItemSpawner();

protected:
	virtual AActor* CreateChildActor();

	virtual bool ActivateSpawner(ACloud9Character* Character);

	virtual void OnConstruction(const FTransform& Transform) override final;

	virtual void BeginPlay() override final;

	virtual void Tick(float DeltaTime) override final;

	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& Hit);

	/**
 	 * Function creates child actor of ItemSampleComponent or recreate it
 	 * when child has been destroyed during OnConstruction call.
 	 * Function targets to be somewhere safe to call in OnConstruction.
 	 * 
 	 * @tparam ActorType Return type or if StaticClass is nullptr use it ::StaticClass for SetChildActorClass
 	 * @param StaticClass Class for SetChildActorClass if nullptr ActorType::StaticClass will be used
 	 * @param Template An Actor to use as the template when spawning a child actor using this component
 	 */
	template <typename ActorType = AActor>
	ActorType* CreateItemSample(UClass* StaticClass = nullptr, ActorType* Template = nullptr)
	{
		StaticClass = StaticClass ? StaticClass : ActorType::StaticClass();

		if (Template != nullptr)
		{
			ItemSampleComponent->SetChildActorClass(StaticClass, Template);
		}
		else
		{
			ItemSampleComponent->SetChildActorClass(StaticClass);
		}

		var ItemSample = ItemSampleComponent->GetChildActor();

		if (not IsValid(ItemSample))
		{
			ItemSampleComponent->CreateChildActor();
			ItemSample = ItemSampleComponent->GetChildActor();
		}

		return Cast<ActorType>(ItemSample);
	}

protected:
	UPROPERTY(BlueprintReadOnly, Category=Implementation)
	UBoxComponent* TriggerBoxComponent;

	UPROPERTY(BlueprintReadOnly, Category=Implementation)
	UChildActorComponent* ItemSampleComponent;

	UPROPERTY(BlueprintReadOnly, Category=Implementation)
	UNiagaraComponent* GlowingEffectComponent;

	UPROPERTY(EditAnywhere, Category=Config)
	bool bIsDestroyOnActivation;

	UPROPERTY(EditAnywhere, Category=Glowing)
	UNiagaraSystem* GlowingEffect;

	UPROPERTY(EditAnywhere, Category=Glowing)
	bool bIsGlowingEffectPreview;

	UPROPERTY(EditDefaultsOnly, Category=Config)
	TArray<USoundBase*> ActivationSounds;

	UPROPERTY(EditAnywhere, Category=Sample)
	FVector SampleOffset;

	UPROPERTY(EditAnywhere, Category=Sample)
	FRotator SampleRotator;

	UPROPERTY(EditAnywhere, Category=Sample)
	float SampleScale;

	UPROPERTY(EditAnywhere, Category=Sample)
	float RotationSpeedInDegree;

	UPROPERTY(EditAnywhere, Category=Sample)
	bool bIsRandomInitialRotation;

	UPROPERTY(EditAnywhere, Category=Zone)
	FVector ZoneSize;
};
