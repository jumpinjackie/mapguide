SET BUILDROOT=c:\builds\mg22win32
rem SET FDOBUILD=c:\builds\fdo35win32\install\Fdo
SET FDOBUILD=c:\fdo32rc1
SET MGSOURCE=%BUILDROOT%\mgdev
SET MGINSTALL=%BUILDROOT%\installer
SET INSTALLROOT=%MGSOURCE%\Release
SET SOLUTIONCONFIG="Release|Win32"
SET OEMFDO=%MGSOURCE%\Oem\FDO
SET STARTDIR=%CD%

rmdir /S /Q %BUILDROOT%
rmdir /S /Q %INSTALLROOT%

call "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86

svn info svn://svn.bld.mgproto.net/mapguide/trunk/MgDev | perl revnum.pl > revision.txt
set /p REVISION= < revision.txt

svn export -r %REVISION%  svn://svn.bld.mgproto.net/mapguide/trunk/MgDev %MGSOURCE%
svn export svn://svn.bld.mgproto.net/metacrs/csmap/trunk/CsMapDev %MGSOURCE%\Oem\CsMap
svn export svn://svn.bld.mgproto.net/fusion/trunk %MGSOURCE%\Oem\fusion
svn export svn://svn.bld.mgproto.net/mapguide/trunk/Installer %MGINSTALL% 

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
call build.bat -source=%INSTALLROOT% -version=2.2.0.%REVISION% -name=MapGuideOpenSource-2.2.0.%REVISION%-Beta1 -title="MapGuide Open Source 2.2 Beta 1"

cd %STARTDIR%
