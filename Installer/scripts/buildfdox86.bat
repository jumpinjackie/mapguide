set BUILDROOT=c:\builds\fdo35win32
set FDOSOURCE=%BUILDROOT%\fdo
set INSTALLROOT=%BUILDROOT%\install
set PLATFORM=Win32
SET FDO_VERSION=5667
SET PROVIDERS_VERSION=5667

rmdir /S /Q %BUILDROOT%
mkdir %INSTALLROOT%

rem TODO get build number from Subversion

svn export -r %FDO_VERSION% svn://svn.bld.mgproto.net/fdo/branches/3.5 %FDOSOURCE%

SET FDOMYSQL=c:\mysql-5.0.86-win32
SET FDOORACLE=C:\instantclient_10_2-win32\sdk

cd %FDOSOURCE%
call c:\script\setenvironment.bat
call build_thirdparty.bat -p=%PLATFORM% -a=buildinstall -o=%INSTALLROOT%
call build.bat  -p=%PLATFORM% -a=buildinstall -o=%INSTALLROOT%
