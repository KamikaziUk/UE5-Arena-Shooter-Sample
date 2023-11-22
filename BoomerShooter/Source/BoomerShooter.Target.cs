// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright @ MGGames 2023

using UnrealBuildTool;
using System.Collections.Generic;

public class BoomerShooterTarget : TargetRules
{
	public BoomerShooterTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
		ExtraModuleNames.Add("BoomerShooter");
	}
}
