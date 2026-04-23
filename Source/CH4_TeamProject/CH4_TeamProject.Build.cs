// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CH4_TeamProject : ModuleRules
{
	public CH4_TeamProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
			"UMG",
      	    "NavigationSystem",
        	"AIModule",
     		"GameplayTasks",
      		"Niagara",
			"InterchangeCore",
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
