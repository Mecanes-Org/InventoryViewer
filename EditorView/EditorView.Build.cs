using UnrealBuildTool;

public class EditorView : ModuleRules
{
    public EditorView(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "InputCore",
                "ProInventorySystem", 
                "Slate"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "SlateCore",
                "EditorStyle",
                "UnrealEd"
                
            }
        );
    }
}