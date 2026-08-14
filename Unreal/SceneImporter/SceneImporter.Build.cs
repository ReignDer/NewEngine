// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SceneImporter : ModuleRules
{
	public SceneImporter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput", 
			"InterchangeCore", 
			"InterchangeEngine",
			"InterchangeImport", 
			"InterchangeNodes",
			"InterchangeFactoryNodes",
			"InterchangePipelines",
			"UnrealEd",
			"Json", 
			"JsonUtilities" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		if ((Target.Type == TargetType.Editor))
		{
			PrivateDependencyModuleNames.AddRange(new string[] { "InterchangeEditorPipelines","DesktopPlatform","Slate","SlateCore",});
		}

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
