@echo off
SET PATH=%PATH%;%CD%\BuildTools\WebTools\SetAssemblyVersion\SetAssemblyVersion\bin\Release
SET VERSION=%1
echo Stamping version %VERSION% to all instances of AssemblyInfo.cs
pushd "%CD%\Web\src\DotNetApi"
SetAssemblyVersion.exe -set:%1 AssemblyInfo.cs
popd
pushd "%CD%\Web\src\DotNetApi\Foundation"
SetAssemblyVersion.exe -set:%1 AssemblyInfo.cs
popd
pushd "%CD%\Web\src\DotNetApi\Geometry"
SetAssemblyVersion.exe -set:%1 AssemblyInfo.cs
popd
pushd "%CD%\Web\src\DotNetApi\MapGuideCommon"
SetAssemblyVersion.exe -set:%1 AssemblyInfo.cs
popd
pushd "%CD%\Web\src\DotNetApi\PlatformBase"
SetAssemblyVersion.exe -set:%1 AssemblyInfo.cs
popd
pushd "%CD%\Web\src\DotNetApi\Web"
SetAssemblyVersion.exe -set:%1 AssemblyInfo.cs
popd
pushd "%CD%\Desktop\DesktopUnmanagedApi\DotNet\Partials"
SetAssemblyVersion.exe -set:%1 AssemblyInfo.cs
popd
pushd "%CD%\Desktop\MgAppLayout\Properties"
SetAssemblyVersion.exe -set:%1 AssemblyInfo.cs
popd
pushd "%CD%\Desktop\MapViewer\Properties"
SetAssemblyVersion.exe -set:%1 AssemblyInfo.cs
popd
pushd "%CD%\Desktop\MapViewer.Desktop\Properties"
SetAssemblyVersion.exe -set:%1 AssemblyInfo.cs
popd