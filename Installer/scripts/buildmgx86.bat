SET BUILDROOT=c:\builds\mg22win32
SET FDOBUILD=c:\builds\fdo35win32\install\Fdo
SET MGSOURCE=%BUILDROOT%\mgdev
SET INSTALLROOT=%BUILDROOT%\install
SET SOLUTIONCONFIG="Release|Win32"
SET OEMFDO=%MGSOURCE%\Oem\FDO

rmdir /S /Q %BUILDROOT%
mkdir %INSTALLROOT%

call "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86

rem TODO get build number from Subversion

svn export svn://svn.bld.mgproto.net/mapguide/trunk/MgDev %MGSOURCE%
svn export svn://svn.bld.mgproto.net/metacrs/csmap/trunk/CsMapDev %MGSOURCE%\Oem\CsMap
svn export svn://svn.bld.mgproto.net/fusion/trunk %MGSOURCE%\Oem\fusion

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
