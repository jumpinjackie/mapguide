SET BUILDROOT=c:\builds\mg22win32
SET FDOBUILD=c:\fdo32-5667
SET MGSOURCE=%BUILDROOT%\mgdev
SET MGINSTALL=%BUILDROOT%\installer
SET INSTALLROOT=%MGSOURCE%\Release
SET SOLUTIONCONFIG="Release|Win32"
SET OEMFDO=%MGSOURCE%\Oem\FDO
SET STARTDIR=%CD%

rmdir /S /Q %MGSOURCE%
rmdir /S /Q %MGINSTALL%

call "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86

svn info svn://svn.bld.mgproto.net/mapguide/branches/2.2/MgDev | perl revnum.pl > revision.txt
set /p REVISION= < revision.txt

svn export -r %REVISION%  svn://svn.bld.mgproto.net/mapguide/branches/2.2/MgDev %MGSOURCE%
svn export svn://svn.bld.mgproto.net/mapguide/branches/2.2/Installer %MGINSTALL% 

cd %MGSOURCE%
cscript updateversion.vbs /major:2 /minor:2 /point:0 /build:%REVISION%


mkdir %OEMFDO%\Bin\Release
xcopy /Y /E %FDOBUILD%\Bin\* %OEMFDO%\Bin\Release
xcopy /E /I %FDOBUILD%\Inc %OEMFDO%\Inc
xcopy /E /I %FDOBUILD%\Lib %OEMFDO%\Lib

cd %MGSOURCE%\Oem
devenv oem.sln /build %SOLUTIONCONFIG%

cd %MGSOURCE%\Oem\CsMap
devenv OpenSource.sln /build %SOLUTIONCONFIG%

cd %MGSOURCE%\Server
devenv Server.sln /build %SOLUTIONCONFIG%

cd %MGSOURCE%\Web\src
devenv WebTier.sln /build %SOLUTIONCONFIG%

cd %MGSOURCE%
call build.bat
call build.bat -a=install 

cd %MGINSTALL%
call build.bat -source=%INSTALLROOT% -a=prepare
call build.bat -source=%INSTALLROOT% -a=generate 
call build.bat -source=%INSTALLROOT% -version=2.2.0.%REVISION% -name=MapGuideOpenSource-2.2.0.%REVISION%-RC3 -title="MapGuide Open Source 2.2 RC3"

cd %STARTDIR%
