SET BUILDROOT=c:\builds\mg22x64
SET MGSOURCE=%BUILDROOT%\mgdev
SET INSTALLROOT=%BUILDROOT%\install
SET PROJECTCONFIG="Release|x64"

rmdir /S /Q %BUILDROOT%
mkdir %INSTALLROOT%

call "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86

rem TODO get build number from Subversion

svn export svn://svn.bld.mgproto.net/mapguide/trunk/mgdev %MGSOURCE%
svn export svn://svn.bld.mgproto.net/metacrs/csmap/trunk/CsMapDev %MGSOURCE%\Oem\CsMap
svn export svn://svn.bld.mgproto.net/fusion/trunk %MGSOURCE%\Oem\fusion

cd %MGSOURCE%\Oem
devenv /Build oem.sln /ProjectConfig %PROJECTCONFIG%

cd %MGSOURCE%\Oem\CsMap
devenv /Build OpenSource.sln /ProjectConfig %PROJECTCONFIG%

cd %MGSOURCE%\Server
devenv /Build Server.sln /ProjectConfig %PROJECTCONFIG%

cd %MGSOURCE%\Web\src
devenv /Build WebTier.sln /ProjectConfig %PROJECTCONFIG%
