@echo off
pushd nuget
nuget pack mapguide-api-base-x86.nuspec -BasePath x86\api-base
nuget pack mg-desktop-net40-x86.nuspec -BasePath x86\api-desktop
nuget pack mg-desktop-viewer-net40-x86.nuspec -BasePath x86\api-desktop-viewer
nuget pack mapguide-api-web-x86.nuspec -BasePath x86\api-web
nuget pack mapguide-api-base-x64.nuspec -BasePath x64\api-base
nuget pack mg-desktop-net40-x64.nuspec -BasePath x64\api-desktop
nuget pack mg-desktop-viewer-net40-x64.nuspec -BasePath x64\api-desktop-viewer
nuget pack mapguide-api-web-x64.nuspec -BasePath x64\api-web
popd