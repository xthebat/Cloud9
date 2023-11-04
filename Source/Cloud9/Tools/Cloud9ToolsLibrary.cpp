#include "Cloud9ToolsLibrary.h"

#include "Cloud9/Cloud9.h"
#include "Cloud9/Game/Cloud9GameMode.h"

void UCloud9ToolsLibrary::SetCollisionComplexity(UStaticMesh* StaticMesh, uint8 CollisionTraceFlag)
{
	StaticMesh->GetBodySetup()->CollisionTraceFlag = static_cast<ECollisionTraceFlag>(CollisionTraceFlag);
}

float UCloud9ToolsLibrary::CalculateCollisionVolumeScale(UStaticMesh* StaticMesh)
{
	const auto Scale = FVector{1.0f, 1.0f, 1.0f};
	const auto BoundingBoxVolume = StaticMesh->GetBoundingBox().GetVolume();
	const auto SimpleCollisionVolume = StaticMesh->GetBodySetup()->AggGeom.GetVolume(Scale);
	return SimpleCollisionVolume / BoundingBoxVolume;
}

UWorld* UCloud9ToolsLibrary::GetWorld() { return GEngine->GameViewport->GetWorld(); }

ACloud9GameMode* UCloud9ToolsLibrary::GetGameMode()
{
	const auto MyWorld = GetWorld();
	return Cast<ACloud9GameMode>(UGameplayStatics::GetGameMode(MyWorld));
}

/**
 * https://dev.epicgames.com/community/snippets/13w/skeletal-mesh-get-accurate-reference-pose-bounds
 */
FBox UCloud9ToolsLibrary::GetAccurateReferencePoseBounds(const USkeletalMesh* Mesh)
{
	auto Box = FBox(ForceInitToZero);

	if (!Mesh || !Mesh->GetPhysicsAsset()) { return {}; }

	for (const auto BodySetups : Mesh->GetPhysicsAsset()->SkeletalBodySetups)
	{
		const FReferenceSkeleton& RefSkeleton = Mesh->GetSkeleton()->GetReferenceSkeleton();
		auto ComponentSpaceBoneTransform = FTransform::Identity;

		auto BoneIndex = RefSkeleton.FindBoneIndex(BodySetups->BoneName);
		while (BoneIndex != INDEX_NONE)
		{
			const FTransform& BoneLocalTM = RefSkeleton.GetRefBonePose()[BoneIndex];
			ComponentSpaceBoneTransform = ComponentSpaceBoneTransform * BoneLocalTM;

			BoneIndex = RefSkeleton.GetParentIndex(BoneIndex);
		}

		Box += BodySetups->AggGeom.CalcAABB(ComponentSpaceBoneTransform);
	}

	return Box;
}

/**
 * Use CS:GO models import Width/Height/Depth convention 
 */
void UCloud9ToolsLibrary::GetWidthHeightDepth(const FBox& Box, float& Width, float& Height, float& Depth)
{
	const auto Size = Box.GetExtent();
	Width = Size.Y;
	Height = Size.X;
	Depth = Size.Z;
}

FRotator UCloud9ToolsLibrary::RadiansToDegrees(const FRotator Rotator)
{
	return {
		FMath::RadiansToDegrees(Rotator.Pitch),
		FMath::RadiansToDegrees(Rotator.Yaw),
		FMath::RadiansToDegrees(Rotator.Roll)
	};
}

FVector UCloud9ToolsLibrary::VInterpTo(
	const FVector Current,
	const FVector Target,
	float DeltaTime,
	const FVector InterpSpeed)
{
	const auto ClampLerp = [](auto Current, auto Dist, auto Alpha, auto Target)
	{
		return Alpha <= 0.0f ? Target : Current + Dist * FMath::Clamp(Alpha, 0.0f, 1.0f);
	};

	// Distance to reach
	const auto Dist = Target - Current;

	// If distance is too small, just set the desired location
	if (Dist.SizeSquared() < KINDA_SMALL_NUMBER)
	{
		return Target;
	}

	const auto Alpha = DeltaTime * InterpSpeed;

	return {
		ClampLerp(Current.X, Dist.X, Alpha.X, Target.X),
		ClampLerp(Current.Y, Dist.Y, Alpha.Y, Target.Y),
		ClampLerp(Current.Z, Dist.Z, Alpha.Z, Target.Z),
	};
}

template <typename TType>
class FPropertyToString
{
public:
	using TCppType = typename TType::TCppType;
	using TGetter = TFunction<TCppType(const UObject*, const TType*)>;

	FPropertyToString(const UObject* Container, const FProperty* ToConvert)
	{
		static_assert(TIsDerivedFrom<TType, FProperty>::Value, "Type should be inherited from FProperty");
		Object = Container;
		Property = ToConvert;
	}
	
	FPropertyToString& UseGetter(TGetter NewGetter)
	{
		Getter = NewGetter;
		return *this;
	}
	
	TOptional<FString> Convert()
	{
		if (const auto TypedProperty = CastField<TType>(Property))
		{
			const auto Name = Property->GetFName();
			const auto TypeName = Property->GetCPPType();
			const auto ActualGetter = Getter.Get(&FPropertyToString::DefaultGetter);
			const auto Value = ActualGetter(Object, TypedProperty);
			return FString::Printf(TEXT("\t%s: %s = %s\n"), *Name.ToString(), *TypeName, *LexToString(Value));
		}
	
		return {};
	}
	
	bool AppendTo(FString& String)
	{
		if (const auto Converted = Convert(); Converted.IsSet())
		{
			String += Converted.GetValue();
			return true;
		}
	
		return false;
	}

private:
	static TCppType DefaultGetter(const UObject* Object, const TType* Property)
	{
		if constexpr (!TIsSame<FBoolProperty, TType>::Value)
		{
			const auto ValuePtr = Property->template ContainerPtrToValuePtr<void>(Object);
			return TType::GetPropertyValue(ValuePtr);
		}

		UE_LOG(LogCloud9, Fatal, TEXT("Default getter not exists for FBoolProperty"))
		return false;
	}

	const UObject* Object;
	const FProperty* Property;
	TOptional<TGetter> Getter;
};


FString UCloud9ToolsLibrary::UObjectToString(const UObject* Object)
{
	FString String;
	const auto Class = Object->GetClass();

	String += FString::Printf(TEXT("class %s {\n"), *Class->GetName());
	for (TFieldIterator<FProperty> PropertyIterator(Class); PropertyIterator; ++PropertyIterator)
	{
		const auto Property = *PropertyIterator;

		FPropertyToString<FInt16Property>(Object, Property).AppendTo(String)
			|| FPropertyToString<FIntProperty>(Object, Property).AppendTo(String)
			|| FPropertyToString<FInt64Property>(Object, Property).AppendTo(String)
			|| FPropertyToString<FUInt16Property>(Object, Property).AppendTo(String)
			|| FPropertyToString<FUInt32Property>(Object, Property).AppendTo(String)
			|| FPropertyToString<FUInt64Property>(Object, Property).AppendTo(String)
			|| FPropertyToString<FFloatProperty>(Object, Property).AppendTo(String)
			|| FPropertyToString<FDoubleProperty>(Object, Property).AppendTo(String)
			|| FPropertyToString<FBoolProperty>(Object, Property)
			   .UseGetter([](auto Object, auto Property) { return true; })
			   .AppendTo(String);
	}
	String += FString::Printf(TEXT("}"));

	return String;
}
