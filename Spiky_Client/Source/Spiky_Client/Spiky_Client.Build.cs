// Copyright (c) 2017, Vadim Petrov - MIT License

using UnrealBuildTool;
using System.IO;
using System;

public class Spiky_Client : ModuleRules
{
	private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
    }
	
	public Spiky_Client(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.Default;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Networking", "Sockets", "ImageWrapper" });

        PrivateDependencyModuleNames.AddRange(new string[] { "UMG", "Slate", "SlateCore" });

        string IncludesPath = Path.Combine(ThirdPartyPath, "Protobuf", "Includes");
        PublicIncludePaths.Add(IncludesPath);

        IncludesPath = Path.Combine(ThirdPartyPath, "OpenSSL", "Includes");
        PublicIncludePaths.Add(IncludesPath);

        if ((Target.Platform == UnrealTargetPlatform.Win64))
        {
			string LibrariesPath = Path.Combine(ThirdPartyPath, "Protobuf", "Libs");
			PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libprotobuf.lib"));

            LibrariesPath = Path.Combine(ThirdPartyPath, "OpenSSL", "Libs");
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libeay32.lib"));
        }

        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            string BuildPath = Utils.MakePathRelativeTo(ModuleDirectory, BuildConfiguration.RelativeEnginePath);

            AdditionalPropertiesForReceipt.Add(new ReceiptProperty("AndroidPlugin", Path.Combine(BuildPath, "APL.xml")));

            PublicAdditionalLibraries.Add(BuildPath + "/armv7/libprotobuf.so");
            PublicAdditionalLibraries.Add(BuildPath + "/armv7/libcrypto_1_0_0.so");
        }
    }
}
