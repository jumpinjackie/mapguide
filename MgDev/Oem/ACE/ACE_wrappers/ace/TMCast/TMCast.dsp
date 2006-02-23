# Microsoft Developer Studio Project File - Name="TMCast" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=TMCast - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE run the tool that generated this project file and specify the
!MESSAGE nmake output type.  You can then use the following command:
!MESSAGE
!MESSAGE NMAKE /f "TMCast.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "TMCast.mak" CFG="TMCast - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "TMCast - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TMCast - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TMCast - Win32 Release"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib"
# PROP Intermediate_Dir "Release\TMCast"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /O2 /W3 /GX /MD /GR /I "..\.." /D NDEBUG /D WIN32 /D _WINDOWS /D TMCAST_BUILD_DLL /FD /c
# SUBTRACT CPP /YX
# ADD RSC /l 0x409 /d NDEBUG /i "..\.."
BSC32=bscmake.exe
# ADD BSC32 /nologo /o"..\..\lib\TMCast.bsc"
LINK32=link.exe
# ADD LINK32 advapi32.lib user32.lib /INCREMENTAL:NO ACE.lib /libpath:"..\..\lib" /nologo /version:5.4 /subsystem:windows /pdb:"..\..\lib\TMCast.pdb" /dll  /machine:I386 /out:"..\..\lib\TMCast.dll"

!ELSEIF  "$(CFG)" == "TMCast - Win32 Debug"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib"
# PROP Intermediate_Dir "Debug\TMCast"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /Ob0 /W3 /Gm /GX /Zi /MDd /GR /Gy /I "..\.." /D _DEBUG /D WIN32 /D _WINDOWS /D TMCAST_BUILD_DLL /FD /c
# SUBTRACT CPP /Fr /YX
# ADD RSC /l 0x409 /d _DEBUG /i "..\.."
BSC32=bscmake.exe
# ADD BSC32 /nologo /o"..\..\lib\TMCast.bsc"
LINK32=link.exe
# ADD LINK32 advapi32.lib user32.lib /INCREMENTAL:NO ACEd.lib /libpath:"..\..\lib" /nologo /version:5.4 /subsystem:windows /pdb:"..\..\lib\TMCastd.pdb" /dll /debug /machine:I386 /out:"..\..\lib\TMCastd.dll"

!ENDIF

# Begin Target

# Name "TMCast - Win32 Release"
# Name "TMCast - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;cxx;c"
# Begin Source File

SOURCE="Group.cpp"
# End Source File
# Begin Source File

SOURCE="Protocol.cpp"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hh"
# Begin Source File

SOURCE="Export.hpp"
# End Source File
# Begin Source File

SOURCE="FaultDetector.hpp"
# End Source File
# Begin Source File

SOURCE="Group.hpp"
# End Source File
# Begin Source File

SOURCE="GroupFwd.hpp"
# End Source File
# Begin Source File

SOURCE="LinkListener.hpp"
# End Source File
# Begin Source File

SOURCE="Messaging.hpp"
# End Source File
# Begin Source File

SOURCE="MTQueue.hpp"
# End Source File
# Begin Source File

SOURCE="Protocol.hpp"
# End Source File
# Begin Source File

SOURCE="TransactionController.hpp"
# End Source File
# End Group
# Begin Group "Documentation"

# PROP Default_Filter ""
# Begin Source File

SOURCE="README"
# End Source File
# End Group
# End Target
# End Project
