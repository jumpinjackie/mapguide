# Microsoft Developer Studio Project File - Name="xquery_example_eval" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=xquery_example_eval - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xquery_example_eval.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xquery_example_eval.mak" CFG="xquery_example_eval - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xquery_example_eval - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "xquery_example_eval - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xquery_example_eval - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../bin"
# PROP BASE Intermediate_Dir "Release\xquery_example_eval"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../bin"
# PROP Intermediate_Dir "Release\xquery_example_eval"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /I "../include" /I "@XERCES_WINHOME"/include /I "../../xerces-c-src/src" /I "../../pathan/include" /I "../../include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32  /libpath:../../lib xerces-c_2.lib Pathan.lib libxquery12.lib /libpath:Release /libpath:../../xerces-c-src/Build/Win32/VC6/Release /libpath:../../pathan/lib /nologo /out:../../bin/xquery_example_eval.exe /subsystem:console /machine:I386 /nodefaultlib:"libcmt"

!ELSEIF  "$(CFG)" == "xquery_example_eval - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../bin/debug"
# PROP BASE Intermediate_Dir "Debug\xquery_example_eval"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../bin/debug"
# PROP Intermediate_Dir "Debug\xquery_example_eval"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "." /I "../include" /I "../../xerces-c-src/include" /I "../../xerces-c-src/src" /I "../../pathan/include" /I "../../include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32  /libpath:../../lib xerces-c_2d.lib Pathand.lib libxquery12d.lib /libpath:Debug /libpath:../../xerces-c-src/Build/Win32/VC6/Debug /libpath:../../pathan/lib /nologo /out:../../bin/debug/xquery_example_eval.exe /pdb:../../bin/debug/xquery_example_eval.pdb /subsystem:console /machine:I386 /nodefaultlib:"libcmt"

!ENDIF 

# Begin Target

# Name "xquery_example_eval - Win32 Release"
# Name "xquery_example_eval - Win32 Debug"
# Begin Source File

SOURCE=..\examples\eval\eval.cpp
# End Source File
# End Target
# End Project
