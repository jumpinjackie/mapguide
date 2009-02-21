
cleandebug:
  del /Q Mg*.cs
  if exist $(OUTDIR)\MapGuideDotNetApid.dll del $(OUTDIR)\MapGuideDotNetApid.dll
  if exist $(OUTDIR)\MapGuideDotNetApid.pdb del $(OUTDIR)\MapGuideDotNetApid.pdb

cleanrelease:
  del /Q Mg*.cs
  if exist $(OUTDIR)\MapGuideDotNetApi.dll del $(OUTDIR)\MapGuideDotNetApi.dll
  if exist $(OUTDIR)\MapGuideDotNetApi.pdb del $(OUTDIR)\MapGuideDotNetApi.pdb

debug: $(OUTDIR)\MapGuideDotNetApid.dll

release: $(OUTDIR)\MapGuideDotNetApi.dll

$(OUTDIR)\MapGuideDotNetApid.dll: 
  csc.exe /out:$(OUTDIR)\MapGuideDotNetApid.dll /target:library /debug+ /debug:full *.cs

$(OUTDIR)\MapGuideDotNetApi.dll: 
  csc.exe /out:$(OUTDIR)\MapGuideDotNetApi.dll /target:library *.cs
