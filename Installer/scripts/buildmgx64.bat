SET BUILDROOT=c:\builds\mg22x64
rem SET FDOBUILD=c:\builds\fdo35x64\install\Fdo
SET FDOBUILD=c:\fdo64rc1
SET MGSOURCE=%BUILDROOT%\mgdev
SET MGINSTALL=%BUILDROOT%\installer
SET INSTALLROOT=%MGSOURCE%\Release64
SET OEMFDO=%MGSOURCE%\Oem\FDO
SET STARTDIR=%CD%

rmdir /S /Q %MGSOURCE%
rmdir /S /Q %MGINSTALL%

call "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86

svn info svn://svn.bld.mgproto.net/mapguide/branches/2.2/MgDev | perl revnum.pl > revision.txt
set /p REVISION= < revision.txt

svn export -r %REVISION%  svn://svn.bld.mgproto.net/mapguide/branches/2.2/MgDev %MGSOURCE%
svn export svn://svn.bld.mgproto.net/mapguide/trunk/Installer %MGINSTALL% 

cd %MGSOURCE%
cscript updateversion.vbs /major:2 /minor:2 /point:0 /build:%REVISION%


mkdir %OEMFDO%\Bin\Release64
mkdir %OEMFDO%\Bin\Lib64
xcopy /Y /E %FDOBUILD%\Bin\* %OEMFDO%\Bin\Release64
xcopy /Y /E /I %FDOBUILD%\Inc %OEMFDO%\Inc
xcopy /Y /E /I %FDOBUILD%\Lib %OEMFDO%\Lib64

cd %MGSOURCE%
call build64.bat
call build64.bat -a=install 

cd %MGINSTALL%
call build64.bat -source=%INSTALLROOT% -a=prepare
call build64.bat -source=%INSTALLROOT% -a=generate 
call build64.bat -source=%INSTALLROOT% -version=2.2.0.%REVISION% -name=MapGuideOpenSource-2.2.0.%REVISION%-Beta2-x64 -title="MapGuide Open Source 2.2 x64 Beta 2"

cd %STARTDIR%
