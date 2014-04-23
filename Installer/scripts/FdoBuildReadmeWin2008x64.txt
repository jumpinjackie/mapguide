Notes for Fdo Build Setup on a 64 bit Win2008 machine
-----------------------------------------------------
Update March 15, 2010
By Trevor Wekel

The following change should be make to setenvironment.bat to support "(x86)" in the path:

:setvcvarsall
SET ACTIVENAMECHECK="Microsoft Visual Studio 9"
SET ACTIVEPATHCHECK="C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC"


The 32 bit MySQL and Oracle sdks are installed to:

SET FDOMYSQL=c:\mysql-5.0.86-win32
SET FDOORACLE=C:\instantclient_10_2-win32\sdk

The 64 bit MySQL and Oracle sdks are installed to:


SET FDOMYSQL=c:\mysql-5.0.86-winx64
SET FDOORACLE=C:\instantclient_10_2-win-x86-64\sdk

Under ThirdParty\util the stampver and UpdateVersion binaries will generate "ok to run"
popups on the build machine.  Running as Administrator will avoid these popups.

Note: Boost libraries included in Fdo 3.5.0 will fail to build on a 64 bit operating system.
This has been corrected in Fdo 3.6.0.



