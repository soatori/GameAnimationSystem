// Copyright Soatori Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GameAnimationSystem : ModuleRules
{
    public GameAnimationSystem(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
            }
        );
    }
}