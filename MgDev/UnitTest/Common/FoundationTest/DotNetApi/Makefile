
cleandebug:
  del /Q Mg*.cs
  del ..\..\bin\debug\MapGuideDotNetApid.dll

cleanrelease:
  del /Q Mg*.cs
  del ..\..\bin\release\MapGuideDotNetApi.dll

debug: ..\..\bin\debug\MapGuideDotNetApid.dll

release: ..\..\bin\release\MapGuideDotNetApi.dll

..\..\bin\debug\MapGuideDotNetApid.dll: 
  csc.exe /out:..\..\bin\debug\MapGuideDotNetApid.dll /target:library /debug+ /debug:full *.cs

..\..\bin\release\MapGuideDotNetApi.dll: 
  csc.exe /out:..\..\bin\release\MapGuideDotNetApi.dll /target:library *.cs
