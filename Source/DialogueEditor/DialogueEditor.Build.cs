// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DialogueEditor : ModuleRules
{
	public DialogueEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"DialogueManager",
				"AssetTools",
				"UnrealEd",
				"PropertyEditor",
				"Projects",
				"ToolMenus",
				"GraphEditor"
			}
			);


	}
}
