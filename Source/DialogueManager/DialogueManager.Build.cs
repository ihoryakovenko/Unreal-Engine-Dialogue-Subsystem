// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DialogueManager : ModuleRules
{
	public DialogueManager(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
	
        PublicDependencyModuleNames.AddRange(new string[] { "Projects", "Core", "CoreUObject",
				"Engine", "InputCore", "GameplayTags", "UMG", });

        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
    }
}
