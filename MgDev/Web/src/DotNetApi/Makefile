
cleandebug:
  del /Q Mg*.cs
  del $(OUTDIR)\MapGuideDotNetApid.dll

cleanrelease:
  del /Q Mg*.cs
  del $(OUTDIR)\MapGuideDotNetApi.dll

debug: $(OUTDIR)\MapGuideDotNetApid.dll

release: $(OUTDIR)\MapGuideDotNetApi.dll

$(OUTDIR)\MapGuideDotNetApid.dll: 
  csc.exe /out:$(OUTDIR)\MapGuideDotNetApid.dll /target:library /debug+ /debug:full *.cs

$(OUTDIR)\MapGuideDotNetApi.dll: 
  csc.exe /out:$(OUTDIR)\MapGuideDotNetApi.dll /target:library *.cs
