#include "Cloud9ToolsLibrary.h"
#include "Cloud9/Game/Cloud9GameMode.h"
#include "Kismet/GameplayStatics.h"

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

UWorld* UCloud9ToolsLibrary::GetWorld()
{
	return GEngine->GameViewport->GetWorld();
}

ACloud9GameMode* UCloud9ToolsLibrary::GetGameMode()
{
	const auto MyWorld = GetWorld();
	return Cast<ACloud9GameMode>(UGameplayStatics::GetGameMode(MyWorld));
}
