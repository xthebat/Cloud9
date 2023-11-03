// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Cloud9 : ModuleRules
{
	public Cloud9(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"HeadMountedDisplay",
			"NavigationSystem",
			"DeveloperSettings",
			"AIModule",
			"Niagara"
		});

		CppStandard = CppStandardVersion.Cpp17;
	}
}