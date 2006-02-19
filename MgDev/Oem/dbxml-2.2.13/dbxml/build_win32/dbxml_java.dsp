# Microsoft Developer Studio Project File - Name="dbxml_java" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=dbxml_java - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dbxml_java.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dbxml_java.mak" CFG="dbxml_java - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dbxml_java - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dbxml_java - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dbxml_java - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../lib"
# PROP BASE Intermediate_Dir "Release\dbxml_java"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../lib"
# PROP Intermediate_Dir "Release\dbxml_java"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "." /I "../include" /I "../../db-4.3.29" /I "../../db-4.3.29/build_win32" /I "../../db-4.3.29/dbinc" /I "../../pathan/include" /I "../../pathan/src/config/win32" /I "../../xquery-1.2.0/include" /I "../../xerces-c-src/include" /I "../../xerces-c-src/src" /I "../../include" /D "DBXML_CREATE_DLL" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /libpath:../../lib xerces-c_2.lib libxquery12.lib /libpath:../../xquery-1.2.0/build_win32/Release /libpath:../../pathan/lib pathan.lib /libpath:../../db-4.3.29/build_win32/Release libdb43.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /debug /machine:I386 /out:"../../bin/libdbxml_java22.dll" /implib:"../../lib/libdbxml_java22.lib"  /libpath:../../xerces-c-src/lib /libpath:../../xerces-c-src/build/win32/vc6/release
# Begin Custom Build
ProjDir=.
InputPath=../../bin/libdbxml_java22.dll
SOURCE="$(InputPath)"

"force_compilation.txt" : $(SOURCE) "$(INTDIR)"
	mkdir ".\Release\classes\dummy" ".\../../jar/debug\dummy"
	echo compiling Berkeley DB XML classes
	javac -O -d "Release/classes" -classpath "Release/classes;../../jar/db.jar;Release/classes;../../db-4.3.29/build_win32/Release/db.jar" ../src/java/com/sleepycat/dbxml/*.java ../src/java/com/sleepycat/dbxml/release/*.java ../src/java/com/sleepycat/db/*.java
	echo creating jar files
	cd "./Release/classes"
	jar cf ../../../../jar/dbxml.jar com/sleepycat/dbxml/*.class com/sleepycat/db/*.class
	cd "../.."
	echo compiling Berkeley DB XML examples
	javac -O -d "Release/classes" -classpath "Release/classes;../../jar/dbxml.jar;../../jar/db.jar;../../db-4.3.29/build_win32/Release/db.jar;Release/classes" ../examples/java/gettingStarted/*.java
	echo creating example jar file
	cd "./Release/classes"
	jar cf ../../../../jar/dbxmlexamples.jar dbxml/gettingStarted/*.class
	cd "../.."
	rmdir ".\Release\classes\dummy" ".\../../jar/debug\dummy"
	echo Java build finished
# End Custom Build

!ELSEIF  "$(CFG)" == "dbxml_java - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../lib"
# PROP BASE Intermediate_Dir "Debug\dbxml_java"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../lib"
# PROP Intermediate_Dir "Debug\dbxml_java"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "." /I "../include" /I "../../db-4.3.29" /I "../../db-4.3.29/build_win32" /I "../../db-4.3.29/dbinc" /I "../../pathan/include" /I "../../pathan/src/config/win32" /I "../../xquery-1.2.0/include" /I "../../xerces-c-src/include" /I "../../xerces-c-src/src" /I "../../include" /D "DBXML_CREATE_DLL" /D "CONFIG_TEST" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX"config.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /libpath:../../lib xerces-c_2d.lib libxquery12d.lib /libpath:../../xquery-1.2.0/build_win32/Debug /libpath:../../pathan/lib pathand.lib /libpath:../../db-4.3.29/build_win32/Debug libdb43d.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:../../bin/debug/dbxml_java.pdb /debug /machine:I386 /out:"../../bin/debug/libdbxml_java22d.dll" /implib:"../../lib/libdbxml_java22d.lib" /libpath:../../xerces-c-src/lib /libpath:../../xerces-c-src/build/win32/vc6/debug
# Begin Custom Build
ProjDir=.
InputPath=../../bin/debug/libdbxml_java22d.dll
SOURCE="$(InputPath)"

"force_compilation.txt" : $(SOURCE) "$(INTDIR)"
	mkdir ".\Debug\classes\dummy" ".\../../jar/debug\dummy"
	echo compiling Berkeley DB XML classes
	javac -O -d "Debug/classes" -classpath "Debug/classes;../../jar/debug/db.jar;Debug/classes;../../db-4.3.29/build_win32/Debug/db.jar" ../src/java/com/sleepycat/dbxml/*.java ../src/java/com/sleepycat/dbxml/debug/*.java ../src/java/com/sleepycat/db/*.java
	cd "./Debug/classes"
	echo creating jar files
	jar cf ../../../../jar/debug/dbxml.jar com/sleepycat/dbxml/*.class com/sleepycat/db/*.class
	cd "../.."
	echo compiling Berkeley DB XML examples
	javac -O -d "Debug/classes" -classpath "Debug/classes;../../jar/debug/dbxml.jar;../../jar/debug/db.jar;Debug/classes;../../db-4.3.29/build_win32/Debug/db.jar" ../examples/java/gettingStarted/*.java
	cd "Debug/classes"
	echo creating example jar file
	jar cf ../../../../jar/debug/dbxmlexamples.jar dbxml/gettingStarted/*.class
	cd "../.."
	rmdir ".\Debug\classes\dummy" ".\../../jar/debug\dummy"
	echo Java build finished
# End Custom Build


!ENDIF 

# Begin Target

# Name "dbxml_java - Win32 Release"
# Name "dbxml_java - Win32 Debug"
# Begin Source File

SOURCE=..\src\java\dbxml_java_wrap.cpp
# End Source File
# End Target
# End Project
