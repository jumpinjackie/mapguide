@echo off
SET EXTRA_NUGET_ARGS=
IF NOT "%1" == "" SET EXTRA_NUGET_ARGS=-Version %1
pushd nuget
nuget pack mapguide-api-base.nuspec -BasePath api-base %EXTRA_NUGET_ARGS%
nuget pack mg-desktop-net40.nuspec -BasePath api-desktop %EXTRA_NUGET_ARGS%
nuget pack mg-desktop-viewer-net40.nuspec -BasePath api-desktop-viewer %EXTRA_NUGET_ARGS%
nuget pack mapguide-api-web.nuspec -BasePath api-web %EXTRA_NUGET_ARGS%
popd