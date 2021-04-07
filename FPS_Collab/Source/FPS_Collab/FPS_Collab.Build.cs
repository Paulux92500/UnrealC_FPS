// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FPS_Collab : ModuleRules
{
	public FPS_Collab(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
