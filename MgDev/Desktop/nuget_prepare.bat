@echo off
rem Top-level build script for mg-desktop
rem
rem Author: Jackie Ng (jumpinjackie@gmail.com)
rem
rem You must call setenvironment[64].bat first with any appropriate
rem parameters before calling this script

IF "%NUGET_TARGET%" == "" SET NUGET_TARGET=net40
SET NUGET_CONTENT=%CD%\nuget\%NUGET_TARGET%\content\%NUGET_TARGET%\mg-desktop
SET NUGET_LIB=%CD%\nuget\%NUGET_TARGET%\lib\%NUGET_TARGET%

echo [install]: Copy files to nuget staging
%XCOPY% "bin\%TYPEBUILD%\FDO" "%NUGET_CONTENT%\FDO" /EXCLUDE:package_excludes.txt
%XCOPY% "bin\%TYPEBUILD%\Dictionaries" "%NUGET_CONTENT%\Dictionaries" /EXCLUDE:package_excludes.txt
%XCOPY% "bin\%TYPEBUILD%\Resources" "%NUGET_CONTENT%\Resources" /EXCLUDE:package_excludes.txt
%XCOPY% "bin\%TYPEBUILD%\Schema" "%NUGET_CONTENT%\Schema" /EXCLUDE:package_excludes.txt
xcopy /y "bin\%TYPEBUILD%\*.dll" "%NUGET_CONTENT%" /EXCLUDE:dll_excludes.txt
copy /y "bin\%TYPEBUILD%\Platform.ini" "%NUGET_CONTENT%"
copy /y "bin\%TYPEBUILD%\OSGeo.*.dll" "%NUGET_LIB%"
copy /y "bin\%TYPEBUILD%\OSGeo.MapGuide.Viewer.xml" "%NUGET_LIB%"
echo mg-desktop is ready for nuget packaging and publishing
goto quit


:error
echo [ERROR]: There was an error building the component
exit /B 1

:quit