set BUILDROOT=c:\builds\fdo35x64
set FDOSOURCE=%BUILDROOT%\fdo
set INSTALLROOT=%BUILDROOT%\install
set PLATFORM=x64

rmdir /S /Q %BUILDROOT%
mkdir %INSTALLROOT%

rem TODO get build number from Subversion

svn export svn://svn.bld.mgproto.net/fdo/branches/3.5 %FDOSOURCE%

SET FDOMYSQL="c:\mysql-5.0.86-winx64"
SET FDOORACLE="C:\instantclient_10_2-win-x86-64\sdk"

cd %FDOSOURCE%
call c:\scripts\setenvironment.bat x64
call build_thirdparty.bat -p=%PLATFORM% -a=buildinstall -o=%INSTALLROOT%
call build.bat -p=%PLATFORM% -a=buildinstall -o=%INSTALLROOT%
