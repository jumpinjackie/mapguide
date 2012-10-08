@echo off
SET MG_CPU=x86
IF NOT "%1" == "" SET MG_CPU=%1
pushd nuget
nuget pack mapguide-api-base-%MG_CPU%.nuspec -BasePath %MG_CPU%\api-base
nuget pack mg-desktop-net40-%MG_CPU%.nuspec -BasePath %MG_CPU%\api-desktop
nuget pack mg-desktop-viewer-net40-%MG_CPU%.nuspec -BasePath %MG_CPU%\api-desktop-viewer
nuget pack mapguide-api-web-%MG_CPU%.nuspec -BasePath %MG_CPU%\api-web
nuget pack cs-map-dictionaries.nuspec -BasePath cs-map-dicts
popd