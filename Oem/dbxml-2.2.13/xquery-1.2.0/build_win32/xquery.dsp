# Microsoft Developer Studio Project File - Name="xquery" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=xquery - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xquery.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xquery.mak" CFG="xquery - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xquery - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xquery - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xquery - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../lib"
# PROP BASE Intermediate_Dir "Release\xquery"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../lib"
# PROP Intermediate_Dir "Release\xquery"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XQENGINE_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../src" /I "../include" /I "../../pathan/include" /I "../../xerces-c-src/include" /I "../../xerces-c-src/src" /I "../../include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_USRDLL" /D "XQENGINE_EXPORTS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 /libpath:"../../lib" Pathan.lib xerces-c_2.lib /nologo /base:"0x69000000" /dll /machine:I386 /out:"../../bin/libxquery12.dll" /libpath:"../../pathan/lib" /libpath:"../../xerces-c-src/Build/Win32/VC6/Release" /OPT:REF
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "xquery - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../lib"
# PROP BASE Intermediate_Dir "Debug\xquery"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../lib"
# PROP Intermediate_Dir "Debug\xquery"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XQENGINE_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../src" /I "../include" /I "../../pathan/include" /I "../../xerces-c-src/include" /I "../../xerces-c-src/src" /I "../../include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_USRDLL" /D "XQENGINE_EXPORTS" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /libpath:"../../lib" PathanD.lib xerces-c_2D.lib /nologo /base:"0x69000000" /dll /debug /machine:I386 /out:"../../bin/debug/libxquery12d.dll" /pdbtype:sept /libpath:"../../pathan/lib" /pdb:../../bin/debug/xquery.pdb /libpath:"../../xerces-c-src/Build/Win32/VC6/Debug"

!ENDIF 

# Begin Target

# Name "xquery - Win32 Release"
# Name "xquery - Win32 Debug"
# Begin Source File

SOURCE=..\src\Grammar.cpp
# End Source File
# Begin Source File

SOURCE=..\src\PrintDataItemTree.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Scanner.cpp
# End Source File
# Begin Source File

SOURCE=..\src\XQContext.cpp
# End Source File
# Begin Source File

SOURCE=..\src\XQContextImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\XQDebugHook.cpp
# End Source File
# Begin Source File

SOURCE=..\src\XQDOMConstructor.cpp
# End Source File
# Begin Source File

SOURCE=..\src\XQDynamicContextImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\XQEngine.cpp
# End Source File
# Begin Source File

SOURCE=..\src\XQEvaluator.cpp
# End Source File
# Begin Source File

SOURCE=..\src\XQFactoryImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\XQFLWOR.cpp
# End Source File
# Begin Source File

SOURCE=..\src\XQFunctionCall.cpp
# End Source File
# Begin Source File

SOURCE=..\src\XQFunction.cpp
# End Source File
# Begin Source File

SOURCE=..\src\XQGlobalVariable.cpp
# End Source File
# Begin Source File

SOURCE=..\src\XQOrderingChange.cpp
# End Source File
# Begin Source File

SOURCE=..\src\XQQuantified.cpp
# End Source File
# Begin Source File

SOURCE=..\src\XQQuery.cpp
# End Source File
# Begin Source File

SOURCE=..\src\XQRemoteDebugger.cpp
# End Source File
# Begin Source File

SOURCE=..\src\XQScopedNamespace.cpp
# End Source File
# Begin Source File

SOURCE=..\src\XQTypeswitch.cpp
# End Source File
# Begin Source File

SOURCE=..\src\XQValidate.cpp
# End Source File
# Begin Source File

SOURCE=..\src\XQVariableBinding.cpp
# End Source File
# End Target
# End Project
