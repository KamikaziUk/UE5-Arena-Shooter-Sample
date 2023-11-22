// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright @ MGGames 2023

using UnrealBuildTool;

public class BoomerShooter : ModuleRules
{
	public BoomerShooter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "Niagara" });
	}
}
