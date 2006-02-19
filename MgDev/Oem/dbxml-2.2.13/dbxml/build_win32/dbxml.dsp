# Microsoft Developer Studio Project File - Name="dbxml" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=dbxml - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dbxml.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dbxml.mak" CFG="dbxml - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dbxml - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dbxml - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dbxml - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../lib"
# PROP BASE Intermediate_Dir "Release\dbxml"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../lib"
# PROP Intermediate_Dir "Release\dbxml"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DBXML_DLL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /I "../include" /I "../src/dbxml"  /I "../../db-4.3.29/include" /I "../../db-4.3.29/build_win32" /I "../../db-4.3.29/dbinc" /I "../../pathan/include" /I "../../xquery-1.2.0/include" /I "../../xerces-c-src/include" /I "../../xerces-c-src/src" /I "../../include" /D "NDEBUG" /D "_USRDLL" /D "DBXML_CREATE_DLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /YX"DbXmlInternal.hpp" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 /libpath:../../lib xerces-c_2.lib libxquery12.lib libdb43.lib Pathan.lib /libpath:../../xquery-1.2.0/build_win32/Release /libpath:../../db-4.3.29/build_win32/Release /libpath:../../pathan/lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /machine:I386 /out:"../../bin/libdbxml22.dll" /libpath:../../xerces-c-src/lib /libpath:../../xerces-c-src/Build/Win32/VC6/release
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copy header files
PostBuild_Cmds=copyHeaders.bat
# End Special Build Tool
!ELSEIF  "$(CFG)" == "dbxml - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../lib"
# PROP BASE Intermediate_Dir "Debug\dbxml"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../lib"
# PROP Intermediate_Dir "Debug\dbxml"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DBXML_DLL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "." /I "../include" /I "../src/dbxml"  /I "../../db-4.3.29/include" /I "../../db-4.3.29/build_win32" /I "../../db-4.3.29/dbinc" /I "../../pathan/include" /I "../../xquery-1.2.0/include" /I "../../xerces-c-src/include"  /I "../../xerces-c-src/src" /I "../../include" /D "_DEBUG" /D "_USRDLL" /D "DBXML_CREATE_DLL" /D "_DLL" /D "DBG_ENABLED" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /YX"DbXmlInternal.hpp" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /libpath:../../lib libdb43d.lib PathanD.lib xerces-c_2d.lib libxquery12d.lib /libpath:../../xquery-1.2.0/build_win32/Debug /libpath:../../pathan/lib /libpath:../../db-4.3.29/build_win32/Debug kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:../../bin/debug/dbxml.pdb /debug /machine:I386 /out:"../../bin/debug/libdbxml22d.dll" /libpath:../../xerces-c-src/lib /libpath:../../xerces-c-src/Build/Win32/VC6/Debug
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copy header files
PostBuild_Cmds=copyHeaders.bat
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "dbxml - Win32 Release"
# Name "dbxml - Win32 Debug"
# Begin Source File

SOURCE=..\src\dbxml\Buffer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\BaseInputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\ConfigurationDatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Container.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Cursor.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Manager.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\DbWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\DbXmlInternal.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\DictionaryDatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Document.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\DocumentDatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Globals.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\HighResTimer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\ID.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\IDS.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\ImpliedSchemaNode.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\IndexSpecification.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\IndexEntry.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\IndexLookup.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Indexer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Key.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\KeyStash.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\KeyStatistics.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Log.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\LocalFileInputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\MemBufInputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\MetaDatum.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Modify.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Name.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\PrintDataItemTree.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\QueryContext.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\QueryExpression.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\QueryPlan.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\QueryExecutionContext.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\ReferenceCounted.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\ReferenceMinder.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Results.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Statistics.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\StdInInputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Syntax.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\SyntaxDatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\SyntaxManager.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Transaction.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\TransactedContainer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\URLInputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\UTF8.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\UpdateContext.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\Value.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\VariableBindings.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlContainer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlData.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlManager.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlDocument.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlException.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlIndexLookup.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlIndexSpecification.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlInputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlMetaDataIterator.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlModify.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlNamespace.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlQueryContext.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlQueryExpression.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlResolver.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlResults.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlStatistics.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlTransaction.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlUpdateContext.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\XmlValue.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\db_rdbt.c
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlCompare.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlContains.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlContext.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlContextImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlDocAvailable.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlDynamicContextImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlFactoryImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlFilter.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlNav.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlNodeImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlNodeTest.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlUri.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\DbXmlURIResolver.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\MetaDataFunction.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\QueryPlanFunction.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\dataItem\QueryPlanHolder.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\optimizer\CollectionAndDocOptimizer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\optimizer\NodeVisitingOptimizer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\optimizer\Optimizer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\optimizer\QueryPlanGenerator.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsDocument.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsDocumentDatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsDom.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsDomEventSource.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsEventGenerator.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsHandlerBase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsSAX2Reader.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsTransientDomBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsWriter.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsXercesDom.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsXercesTranscoder.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\NsXercesIndexer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\dbxml\nodeStore\PullEventInputStream.cpp
# End Source File
# End Target
# End Project

