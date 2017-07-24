// Copyright (c) 2017, Vadim Petrov - MIT License

using UnrealBuildTool;
using System.Collections.Generic;

public class Spiky_ClientTarget : TargetRules
{
	public Spiky_ClientTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "Spiky_Client" } );
	}
}
