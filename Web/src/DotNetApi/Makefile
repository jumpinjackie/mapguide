
cleandebug:
  del /Q .\Mg*.cs
  del /Q .\*Constants.cs  
  del /Q .\*PINVOKE.cs    
  del /Q .\*Api.cs       
  if exist $(OUTDIR)\MapGuideDotNetApid.dll del $(OUTDIR)\MapGuideDotNetApid.dll
  if exist $(OUTDIR)\MapGuideDotNetApid.pdb del $(OUTDIR)\MapGuideDotNetApid.pdb

cleanrelease:
  del /Q .\Mg*.cs
  del /Q .\*Constants.cs  
  del /Q .\*PINVOKE.cs    
  del /Q .\*Api.cs       
  if exist $(OUTDIR)\MapGuideDotNetApi.dll del $(OUTDIR)\MapGuideDotNetApi.dll
  if exist $(OUTDIR)\MapGuideDotNetApi.pdb del $(OUTDIR)\MapGuideDotNetApi.pdb

debug: $(OUTDIR)\MapGuideDotNetApid.dll

release: $(OUTDIR)\MapGuideDotNetApi.dll

$(OUTDIR)\MapGuideDotNetApid.dll: 
  csc.exe /out:$(OUTDIR)\MapGuideDotNetApid.dll /target:library /addmodule:$(OUTDIR)\OSGeo.MapGuide.Foundation.netmodule;$(OUTDIR)\OSGeo.MapGuide.Geometry.netmodule;$(OUTDIR)\OSGeo.MapGuide.PlatformBase.netmodule;$(OUTDIR)\OSGeo.MapGuide.MapGuideCommon.netmodule;$(OUTDIR)\OSGeo.MapGuide.Web.netmodule

$(OUTDIR)\MapGuideDotNetApi.dll: 
  csc.exe /out:$(OUTDIR)\MapGuideDotNetApi.dll /target:library /addmodule:$(OUTDIR)\OSGeo.MapGuide.Foundation.netmodule;$(OUTDIR)\OSGeo.MapGuide.Geometry.netmodule;$(OUTDIR)\OSGeo.MapGuide.PlatformBase.netmodule;$(OUTDIR)\OSGeo.MapGuide.MapGuideCommon.netmodule;$(OUTDIR)\OSGeo.MapGuide.Web.netmodule
