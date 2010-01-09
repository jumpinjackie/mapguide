
cleandebug:
  del /Q Mg*.cs
  del ..\..\bin\debug\MapGuideDotNetApid.dll

cleanbebug64:
  del /Q Mg*.cs
  del ..\..\bin\debug64\MapGuideDotNetApid.dll

cleanrelease:
  del /Q Mg*.cs
  del ..\..\bin\release\MapGuideDotNetApi.dll

cleanrelease64:
  del /Q Mg*.cs
  del ..\..\bin\release64\MapGuideDotNetApi.dll

debug: ..\..\bin\debug\MapGuideDotNetApid.dll

debug64: ..\..\bin\debug64\MapGuideDotNetApid.dll

release: ..\..\bin\release\MapGuideDotNetApi.dll

release64: ..\..\bin\release64\MapGuideDotNetApi.dll

..\..\bin\debug\MapGuideDotNetApid.dll:
  csc.exe /out:..\..\bin\debug\MapGuideDotNetApid.dll /target:library /debug+ /debug:full *.cs

..\..\bin\debug64\MapGuideDotNetApid.dll:
  csc.exe /out:..\..\bin\debug64\MapGuideDotNetApid.dll /target:library /debug+ /debug:full *.cs

..\..\bin\release\MapGuideDotNetApi.dll:
  csc.exe /out:..\..\bin\release\MapGuideDotNetApi.dll /target:library *.cs

..\..\bin\release64\MapGuideDotNetApi.dll:
  csc.exe /out:..\..\bin\release64\MapGuideDotNetApi.dll /target:library *.cs
