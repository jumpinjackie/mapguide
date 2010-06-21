cleandebug:
  if exist $(OUTDIR)\MapGuideDotNetApid.dll del $(OUTDIR)\MapGuideDotNetApid.dll
  if exist $(OUTDIR)\MapGuideDotNetApid.pdb del $(OUTDIR)\MapGuideDotNetApid.pdb

cleanrelease:
  if exist $(OUTDIR)\MapGuideDotNetApi.dll del $(OUTDIR)\MapGuideDotNetApi.dll
  if exist $(OUTDIR)\MapGuideDotNetApi.pdb del $(OUTDIR)\MapGuideDotNetApi.pdb

debug: $(OUTDIR)\MapGuideDotNetApid.dll

release: $(OUTDIR)\MapGuideDotNetApi.dll

$(OUTDIR)\MapGuideDotNetApid.dll:
  csc.exe /debug+ /debug:full /optimize- /out:$(OUTDIR)\MapGuideDotNetApid.dll /target:library /reference:$(OUTDIR)\OSGeo.MapGuide.Foundation.dll;$(OUTDIR)\OSGeo.MapGuide.Geometry.dll;$(OUTDIR)\OSGeo.MapGuide.PlatformBase.dll;$(OUTDIR)\OSGeo.MapGuide.MapGuideCommon.dll;$(OUTDIR)\OSGeo.MapGuide.Web.dll .\AssemblyInfo.cs

$(OUTDIR)\MapGuideDotNetApi.dll:
  csc.exe /debug+ /debug:pdbonly /optimize+ /out:$(OUTDIR)\MapGuideDotNetApi.dll /target:library /reference:$(OUTDIR)\OSGeo.MapGuide.Foundation.dll;$(OUTDIR)\OSGeo.MapGuide.Geometry.dll;$(OUTDIR)\OSGeo.MapGuide.PlatformBase.dll;$(OUTDIR)\OSGeo.MapGuide.MapGuideCommon.dll;$(OUTDIR)\OSGeo.MapGuide.Web.dll .\AssemblyInfo.cs
