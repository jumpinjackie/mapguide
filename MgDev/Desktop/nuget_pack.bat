@echo off
pushd nuget
nuget pack mapguide-api-base.nuspec -BasePath api-base
nuget pack mg-desktop-net40.nuspec -BasePath api-desktop
nuget pack mg-desktop-viewer-net40.nuspec -BasePath api-desktop-viewer
popd