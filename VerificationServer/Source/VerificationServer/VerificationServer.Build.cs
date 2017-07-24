// Copyright (c) 2017, Vadim Petrov - MIT License

using UnrealBuildTool;
using System.IO;
using System;

public class VerificationServer : ModuleRules
{
	private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
    }
	
	public VerificationServer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.Default;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Networking", "Sockets" });

		string IncludesPath = Path.Combine(ThirdPartyPath, "Protobuf", "Includes");
        PublicIncludePaths.Add(IncludesPath);
		
		string LibrariesPath = Path.Combine(ThirdPartyPath, "Protobuf", "Libs");
		PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libprotobuf.lib"));
	}
}
