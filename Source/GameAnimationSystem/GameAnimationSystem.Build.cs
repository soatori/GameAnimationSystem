// Copyright Soatori Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GameAnimationSystem : ModuleRules
{
    public GameAnimationSystem(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
                // ... add public include paths required here ...
            }
        );
				
		
        PrivateIncludePaths.AddRange(
            new string[] {
                // ... add other private include paths required here ...
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "Mover",
                "MotionWarping",
                "PoseSearch",
                "AnimationWarpingRuntime",
                "Chooser",
                "GameplayCameras"
            });

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "AnimationCore",
                "GameplayCameras"
            });
    }
}