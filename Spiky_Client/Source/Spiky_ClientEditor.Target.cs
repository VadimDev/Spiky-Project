// Copyright (c) 2017, Vadim Petrov - MIT License

using UnrealBuildTool;
using System.Collections.Generic;

public class Spiky_ClientEditorTarget : TargetRules
{
	public Spiky_ClientEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "Spiky_Client" } );
	}
}
