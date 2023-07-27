// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class DefconEditorTarget : TargetRules
{
	public DefconEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
		ExtraModuleNames.Add("Defcon");

		// Uncomment these to build without Unity and PCH to detect missing #include directives
		//bUseUnityBuild = false;
		//bUsePCHFiles = false;
    }
}
