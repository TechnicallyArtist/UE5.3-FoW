// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FogOfWarProject : ModuleRules
{
	public FogOfWarProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput" });

        /* We must include plugin here to be able to use ITrackedInterface in main project**/
        PrivateDependencyModuleNames.AddRange(new string[] { "FogOfWar" });
    }
}
