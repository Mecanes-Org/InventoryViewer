using UnrealBuildTool;

public class EditorInventoryViewer : ModuleRules
{
    public EditorInventoryViewer(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "InputCore",
                "ProInventorySystem"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "EditorStyle",
                "ToolMenus",
                "UnrealEd", 
                "ApplicationCore",
                "EditorView"
            }
        );
    }
}