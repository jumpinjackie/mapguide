cleandebug:
  if exist $(OUTDIR)\MapGuideDotNetApid.dll del $(OUTDIR)\MapGuideDotNetApid.dll
  if exist $(OUTDIR)\MapGuideDotNetApid.pdb del $(OUTDIR)\MapGuideDotNetApid.pdb

cleanrelease:
  if exist $(OUTDIR)\MapGuideDotNetApi.dll del $(OUTDIR)\MapGuideDotNetApi.dll
  if exist $(OUTDIR)\MapGuideDotNetApi.pdb del $(OUTDIR)\MapGuideDotNetApi.pdb

debug: $(OUTDIR)\MapGuideDotNetApid.dll

release: $(OUTDIR)\MapGuideDotNetApi.dll

$(OUTDIR)\MapGuideDotNetApid.dll:
  csc.exe /debug+ /debug:full /optimize- /out:$(OUTDIR)\MapGuideDotNetApid.dll /platform:$(PLATFORM) /target:library /lib:$(OUTDIR) /reference:OSGeo.MapGuide.Foundation.dll;OSGeo.MapGuide.Geometry.dll;OSGeo.MapGuide.PlatformBase.dll;OSGeo.MapGuide.MapGuideCommon.dll;OSGeo.MapGuide.Web.dll .\AssemblyInfo.cs

$(OUTDIR)\MapGuideDotNetApi.dll:
  csc.exe /debug+ /debug:pdbonly /optimize+ /out:$(OUTDIR)\MapGuideDotNetApi.dll /platform:$(PLATFORM) /target:library /lib:$(OUTDIR) /reference:OSGeo.MapGuide.Foundation.dll;OSGeo.MapGuide.Geometry.dll;OSGeo.MapGuide.PlatformBase.dll;OSGeo.MapGuide.MapGuideCommon.dll;OSGeo.MapGuide.Web.dll .\AssemblyInfo.cs
