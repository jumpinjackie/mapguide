//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#include "GeometryCommon.h"
#include "CoordSysCommon.h"
#include "CriticalSection.h"
#include "CoordSysUtil.h"                   //for Convert_Wide_To_Ascii, CsDictionaryOpenMode

#include "CoordSysCategory.h"               //for CCategoryName
#include "CoordSysTransform.h"              //for CCoordinateSystemTransform
#include "CoordSysEnum.h"                   //for CCoordinateSystemEnum
#include "CoordSysDictionary.h"             //for CCoordinateSystemDictionary
#include "CoordSysEnumDatum.h"              //for CCoordinateSystemEnumDatum
#include "CoordSysEnumEllipsoid.h"          //for CCoordinateSystemEnumEllipsoid
#include "CoordSysDictionaryBase.h"                 //for CCoordinateSystemDictionaryBase
#include "CoordSysGeodeticPath.h"                   //for CCoordinateSystemGeodeticPath
#include "CoordSysGeodeticTransformDefParams.h"     //for CCoordinateSystemGeodeticTransformDefParams
#include "CoordSysGeodeticAnalyticalTransformDefParams.h"
#include "CoordSysGeodeticInterpolationTransformDefParams.h"
#include "CoordSysGeodeticMultipleRegressionTransformDefParams.h"
#include "CoordSysGeodeticTransformGridFile.h"
#include "CoordSysGeodeticTransformDef.h"           //for CCoordinateSystemGeodeticTransformDef
#include "CoordSysDatumDictionary.h"        //for CCoordinateSystemDatumDictionary
#include "CoordSysEllipsoidDictionary.h"    //for CCoordinateSystemEllipsoidDictionary
#include "CoordSysGeodeticPathDictionary.h"         //for CCoordinateSystemGeodeticPathDictionary
#include "CoordSysGeodeticTransformDefDictionary.h" //for CCoordinateSystemGeodeticTransformDefDictionary
#include "CoordSysCategoryDictionary.h"     //for CCoordinateSystemCategoryDictionary
#include "CoordSysMathComparator.h"         //for CCoordinateSystemMathComparator
#include "CoordSysFormatConverter.h"        //for CCoordinateSystemFormatConverter
#include "CoordSysProjectionInformation.h"  //for CCoordinateSystemProjectionInformation
#include "CoordSysUnitInformation.h"        //for CCoordinateSystemUnitInformation
#include "CoordSysTransform.h"              //for CCoordinateSystemTransform
#include "CoordSysDictionaryUtility.h"      //for CCoordinateSystemDictionaryUtility

#include "CoordSysCatalog.h"                //for CCoordinateSystemCatalog
#include "CoordSysGeodeticTransformation.h" //for CCoordinateSystemGeodeticTransformation

#include "cs_NameMapper.hpp"                 //for csReleaseNameMapper
#include "cs_map.h"

#ifdef _WIN32
#include <tchar.h>                          //for _tsplitpath
#include <shlobj.h>                         //for SHGetFolderPath
#endif

//Global variables needed from Mentor
extern "C"
{
    extern char cs_Unique;
    extern short cs_Protect;
}

using namespace CSLibrary;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemCatalog::CCoordinateSystemCatalog() :
    m_libraryStatus(lsInitializationFailed)
{
    MG_TRY()

    m_pCsDict = new CCoordinateSystemDictionary(this);
    m_pDtDict = new CCoordinateSystemDatumDictionary(this);
    m_pElDict = new CCoordinateSystemEllipsoidDictionary(this);
    m_pCtDict = new CCoordinateSystemCategoryDictionary(this);
    m_pGpDict = new CCoordinateSystemGeodeticPathDictionary(this);
    m_pGxDict = new CCoordinateSystemGeodeticTransformDefDictionary(this);

    if (!m_pCsDict || !m_pDtDict || !m_pElDict || !m_pCtDict || !m_pGpDict || !m_pGxDict)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCatalog.MgCoordinateSystemCatalog", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Set up Mentor global variables.  See CSDATA.C for explanation.
    cs_Unique = '\0';
    cs_Protect = 0;

    // Initialize the critical section class
    CriticalClass.Initialize();

    try
    {
        SetDefaultDictionaryDirAndFileNames();
    }
    catch (MgException* pEPath)
    {
        //We do not throw this exception because:
        //- we might just not care about the default values
        //of the directory and file names.
        //They can very well be setup later on by the client
        //- or the client might just not care about them because the dictionaries are
        //being compiled using an interface such as MgCoordinateSystemDictionaryUtility
        //- if we throw here, the constructor aborts and the factory constructor aborts as well
        //for something we can resolve later on (2 reasons described above)
        ACE_DEBUG((LM_DEBUG, L"(%t) %W caught in CCoordinateSystemCatalog constructor - %W\n", pEPath->GetClassName().c_str(), pEPath->GetDetails().c_str()));
        /// now we have shared the info.... now we can get rid of the exception
        SAFE_RELEASE(pEPath);

        m_sDir=L"";

        //internal API value needed for the server's initialization which relies
        //on the default values being setup automatically
        m_libraryStatus=lsInitializationFailed;
    }

    MG_CATCH(L"MgCoordinateSystemCatalog.MgCoordinateSystemCatalog")
    if (mgException != NULL)
    {
        m_pCsDict = NULL;
        m_pDtDict = NULL;
        m_pElDict = NULL;
        m_pCtDict = NULL;
        m_pGpDict = NULL;
        m_pGxDict = NULL;

        //NOTE: the following behavior happens only in DEBUG if we do not reset the countFlag
        //If an exception is thrown from within the constructor of this MgDisposable derived class
        //is caught and then forwarded by MG_THROW()
        //then MgDisposable::~MgDisposable is called right away.
        //In the process of trying to build CCoordinateSystemCatalog the refcount has been bumped up by the constructors of the dictionary classes
        //catching the exception in turns bumps them down when releasing the objects
        //MgDisposable::m_refCountFlag is now equal to "true" and m_refCount is equal to 1
        //That triggers an ASSERTS in MgDisposable::~MgDisposable if we do not rest the flag to false:
        //ACE_ASSERT(m_refCountFlag ? (m_refCount == 0) : (m_refCount == 1));
        ResetRefCountFlag();
    }
    MG_THROW()
}

//-----------------------------------------------------------------------------------
CCoordinateSystemCatalog::~CCoordinateSystemCatalog()
{
    //frees the Mentor mapper
    csReleaseNameMapper();

    // free CSMap resources
    CS_recvr();
}

//-----------------------------------------------------------------------------------
void CCoordinateSystemCatalog::PrepareForDispose()
{
    m_pCsDict = NULL;
    m_pDtDict = NULL;
    m_pElDict = NULL;
    m_pCtDict = NULL;
    m_pGpDict = NULL;
    m_pGxDict = NULL;
}

//-----------------------------------------------------------------------------------
MgCoordinateSystemDictionary* CCoordinateSystemCatalog::GetCoordinateSystemDictionary()
{
    return SAFE_ADDREF(m_pCsDict.p);
}

//-----------------------------------------------------------------------------------
MgCoordinateSystemDatumDictionary* CCoordinateSystemCatalog::GetDatumDictionary()
{
    return SAFE_ADDREF(m_pDtDict.p);
}

//-----------------------------------------------------------------------------------
MgCoordinateSystemEllipsoidDictionary* CCoordinateSystemCatalog::GetEllipsoidDictionary()
{
    return SAFE_ADDREF(m_pElDict.p);
}

//-----------------------------------------------------------------------------------
MgCoordinateSystemCategoryDictionary* CCoordinateSystemCatalog::GetCategoryDictionary()
{
    return SAFE_ADDREF(m_pCtDict.p);
}

//-----------------------------------------------------------------------------------
MgCoordinateSystemGeodeticPathDictionary* CCoordinateSystemCatalog::GetGeodeticPathDictionary()
{
    return SAFE_ADDREF(m_pGpDict.p);
}

//-----------------------------------------------------------------------------------
MgCoordinateSystemGeodeticTransformDefDictionary* CCoordinateSystemCatalog::GetGeodeticTransformDefDictionary()
{
    return SAFE_ADDREF(m_pGxDict.p);
}

//-----------------------------------------------------------------------------------
STRING CCoordinateSystemCatalog::GetDefaultDictionaryDir()
{
    STRING sDir;

    MG_TRY()

    bool bResult=false;
#ifdef _WIN32

    STRING sDirDefault;

    // Check to see if the environment variable is set
    const TCHAR* szPathVar = _tgetenv(_T(MENTOR_DICTIONARY_PATH));

    //if not set then try the default location
    if(!szPathVar)
    {
        #ifdef _DEBUG
        // This debug statement appears in the Apache log even under release builds. Wrapping this in an #ifdef will ensure it doesn't appear in RELEASE builds
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("\n(%t) CCoordinateSystemCatalog::GetDefaultDictionaryDir() - environment variable MENTOR_DICTIONARY_PATH not set.\n")));
        #endif

        TCHAR szPath[MAX_PATH];
        if(SHGetSpecialFolderPath(NULL, szPath, CSIDL_COMMON_APPDATA, FALSE))
        {
            // Failed to get the default path for the coordinate system dictionaries
            sDirDefault = szPath;
            sDirDefault += _T("\\Autodesk\\Geospatial Coordinate Systems"); //NOXLATE
            szPathVar = sDirDefault.c_str();
        }
    }

    if(szPathVar)
    {
        bResult = true;
        sDir = szPathVar;
    }
#else
    // Linux initialization

    char* szPath = getenv(MENTOR_DICTIONARY_PATH);
    if (szPath)
    {
        bResult = true;
        MgUtil::MultiByteToWideChar(string(szPath), sDir);
    }

#endif
    if (sDir.empty())
    {
        ACE_DEBUG((LM_DEBUG, "(%t) DefaultDir for the Coordinate System library has been set but is empty.\n"));
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCatalog.GetDefaultDictionaryDir", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!MgFileUtil::IsDirectory(sDir.c_str()))
    {
        bResult = false;
        ACE_DEBUG((LM_DEBUG, "(%t) DefaultDir: \"%W\" is not a directory! Install the Coordinate System library into this directory or set MENTOR_DICTIONARY_PATH to where they are currently installed.\n", sDir.c_str()));
    }

    if (!bResult)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemCatalog.GetDefaultDictionaryDir", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //And return success.
    MG_CATCH_AND_THROW(L"MgCoordinateSystemCatalog.GetDefaultDictionaryDir")

    return sDir;
}

//Returns the complete path to the directory where the dictionaries are
//located.
STRING CCoordinateSystemCatalog::GetDictionaryDir()
{
    return m_sDir;
}

//Sets the complete path to the directory where the dictionaries are
//located.  This method can be invoked only when not
//currently connected to a dictionary database.  This method just verifies that
//the dictionary path exists, and, if appropriate, whether a file named Coordsys.CSD could be found-
//Full validation is performed later on by the OpenDictionaries() method.
//Throws [MgFileIoException] if the targeted directory is invalid and/or cannot be accessed in the
//in the mode indicated by [dirWriteAccess]

STRING CCoordinateSystemCatalog::SetDictionaryDir(CREFSTRING sDirPath, bool dirWriteAccess, int (*CsMapDirFunc)(const char *pszDirectoryPath))
{
    CHECKARGUMENTNULL(CsMapDirFunc, L"MgCoordinateSystemCatalog.SetDictionaryDir");

    if (sDirPath.empty())
    {
        CsMapDirFunc("");
        return L"";
    }

#ifdef _WIN32
    //Make sure that the dir path we've been given is
    //properly terminated in a backslash.  If it's not,
    //set it up correctly.
    wchar_t szPath[_MAX_PATH] = {0};
    wchar_t szDrive[_MAX_DRIVE] = {0};
    wchar_t szDir[_MAX_DIR] = {0};
    wchar_t szFname[_MAX_FNAME] = {0};
    wchar_t szExt[_MAX_EXT] = {0};

    //make sure, we don't have any '/' in the string that CS-Map could stumble over
    STRING sPathCopy = sDirPath;
    std::replace(sPathCopy.begin(), sPathCopy.end(), L'/', L'\\');

    _tsplitpath(sPathCopy.c_str(), szDrive, szDir, szFname, szExt);
    if ((_tcslen(szFname) > 0) || (_tcslen(szExt) > 0))
    {
        //Nope, not properly terminated, need to fix it.
        assert(_tcslen(szDir) + _tcslen(szFname) + _tcslen(szExt) < _MAX_DIR);
        _tcscat(szDir, szFname);
        _tcscat(szDir, szExt);
        _tmakepath(szPath, szDrive, szDir, NULL, NULL);
    }
    else
    {
        _tmakepath(szPath, szDrive, szDir, NULL, NULL);
    }
#else
    //Make sure that the dir path we've been given is
    //properly terminated in a backslash.  If it's not,
    //set it up correctly.
    STRING sUpdatedDirPath = sDirPath;
    if(sUpdatedDirPath.at(sUpdatedDirPath.length()-1) != L'/')
    {
        sUpdatedDirPath.append(L"/");
    }
    const wchar_t *szPath = sUpdatedDirPath.c_str();
#endif
    EFileValidity reason;
    //Make sure the directory's okay
    if (!ValidateFile(
        szPath,    //path to directory
        true,    //must exist
        true,    //must be a directory
        dirWriteAccess,    //must be writable?
        &reason))
    {
        ThrowFileError(L"MgCoordinateSystemCatalog.SetDictionaryDir", szPath, reason);
    }

    //initializes CS-Map's target directory function with the new path
    SmartCriticalClass csmapLock;
    char* pszPath = Convert_Wide_To_Ascii(szPath);
    int csmapResult = CsMapDirFunc(pszPath);
    delete[] pszPath;

    //CS-Map return -1 if for cs_altdr() the Coordsys.CSD file couldn't be found; 0 otherwise

    if (csmapResult)
        throw new MgFileIoException(L"MgCoordinateSystemCatalog.SetDictionaryDir", __LINE__, __WFILE__, NULL, L"", NULL);

    return szPath;
}

void CCoordinateSystemCatalog::SetDictionaryDir(CREFSTRING sDirPath)
{
    MG_TRY()

    if (sDirPath.empty())
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCatalog.SetDictionaryDir", __LINE__, __WFILE__, NULL, L"", NULL);

    m_libraryStatus = lsInitializationFailed;
    m_sDir = this->SetDictionaryDir(sDirPath, false, CS_altdr);

    //now need to validate the dictionary files
    //an reinitialize their internal data
    STRING sCs=m_pCsDict->GetFileName();
    STRING sDt=m_pDtDict->GetFileName();
    STRING sEl=m_pElDict->GetFileName();
    STRING sCt=m_pCtDict->GetFileName();
    STRING sGp=m_pGpDict->GetFileName();
    STRING sGx=m_pGxDict->GetFileName();

    //Set the dictionary file names
    //this will perform a validation of the existence of the files inside the directory
    m_pCsDict->SetFileName(sCs);
    m_pDtDict->SetFileName(sDt);
    m_pElDict->SetFileName(sEl);
    m_pCtDict->SetFileName(sCt);
    m_pGpDict->SetFileName(sGp);
    m_pGxDict->SetFileName(sGx);

    m_libraryStatus = lsInitialized; 

    MG_CATCH_AND_THROW(L"MgCoordinateSystemCatalog.SetDictionaryDir")
}

STRING CCoordinateSystemCatalog::GetDefaultUserDictionaryDir()
{
    STRING defaultUserDir;

#ifdef _WIN32

    //let the [MENTOR_USER_DICTIONARY_PATH] environment variable override the
    //the default path - which on Windows systems is the "(user-)Local app data path\Autodesk\User Geospatial Coordinate Systems"

    // Check to see if the environment variable is set
    const TCHAR* szPathVar = _tgetenv(_T(MENTOR_USER_DICTIONARY_PATH));
    bool variableDefined = (szPathVar && L'\0' != *szPathVar);

    if(!variableDefined)
    {
        TCHAR szPath[MAX_PATH] = { L'\0' };
        if(TRUE == SHGetSpecialFolderPath(NULL, szPath, CSIDL_LOCAL_APPDATA, FALSE))
        {
            defaultUserDir = szPath;
            defaultUserDir += _T("\\Autodesk\\User Geospatial Coordinate Systems"); //NOXLATE
        }
    }
    else
        defaultUserDir = szPathVar;

    if (!defaultUserDir.empty() && L'\\' != defaultUserDir[defaultUserDir.length() - 1])
        defaultUserDir.append(L"\\");

#else

    char* szPath = getenv(MENTOR_USER_DICTIONARY_PATH);
    bool variableDefined = (szPath && L'\0' != *szPath);

    if (variableDefined)
    {
        MgUtil::MultiByteToWideChar(string(szPath), defaultUserDir);
        if(L'/' != defaultUserDir[defaultUserDir.length() - 1])
            defaultUserDir.append(L"/");
    }

#endif

    if (defaultUserDir.empty())
        return L"";


    return defaultUserDir;
}

void CCoordinateSystemCatalog::SetUserDictionaryDir(CREFSTRING sDirPath)
{
    MG_TRY()

        m_sUserDir = this->SetDictionaryDir(sDirPath /* can be empty */, true, CS_usrdr);

    MG_CATCH_AND_THROW(L"MgCoordinateSystemCatalog.SetUserDictionaryDir")
}

STRING CCoordinateSystemCatalog::GetUserDictionaryDir()
{
    return m_sUserDir;
}

//check if the files are writable
//The read mode was tracked down when the file names were set up
//if the files were not valid an assertion was thorwn at that time
//this additional method can check if the files can be open in write mode
//if the file names and path have not yet been set up, an assertion is thrown
bool CCoordinateSystemCatalog::AreDictionaryFilesWritable()
{
    MG_TRY()

    //the default dictionary path must contain the relevant CSD files,
    //whereas the directory into which we might store any user-defined
    //definitions does not necessarily contain any of the CSD files
    if (m_sDir.empty()
        || !m_pCsDict || m_pCsDict->GetFileName().empty()
        || !m_pDtDict || m_pDtDict->GetFileName().empty()
        || !m_pElDict || m_pElDict->GetFileName().empty()
        || !m_pCtDict || m_pCtDict->GetFileName().empty()
        || !m_pGpDict || m_pGpDict->GetFileName().empty()
        || !m_pGxDict || m_pGxDict->GetFileName().empty())
    {
        //Directory hasn't been specified yet.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemCatalog.AreDictionaryFilesWritable", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNotReadyException", NULL);
    }

    //the [m_sUserDir] will only contain something, if [SetUserDictionaryDir] had been called
    //successfully before - either by the automatic initialization or explicitly
    //
    if (!m_sUserDir.empty()) 
    {
        //note, that we're assuming here that if we can write to the directory
        //we'll also be able to write-open any CSD file contained in it.
        //As this here is just a pre-check for the current point in time,
        //the actual attempt to write to the CSD file might then still fail what is
        //unlikely and should actually be considered a problem in the user-specific setup of the CSD
        //files & folder
        return ValidateFile(
            m_sUserDir.c_str(),    //path to directory 
            true,    //must exist 
            true,    //must be a directory 
            true,    //must be writable? 
            NULL);
    }

    EFileValidity reason;
    STRING sPath=m_pElDict->GetPath();
    if (!ValidateFile(
        sPath.c_str(),          //file name
        true,                   //must exist
        false,                  //mustn't be directory
        true,                   //neeed write access?
        &reason))
    {
        return false;
    }

    sPath=m_pDtDict->GetPath();
    if (!ValidateFile(
        sPath.c_str(),          //file name
        true,                   //must exist
        false,                  //mustn't be directory
        true,                   //neeed write access?
        &reason))
    {
        return false;
    }

    sPath=m_pCsDict->GetPath();
    if (!ValidateFile(
        sPath.c_str(),          //file name
        true,                   //must exist
        false,                  //mustn't be directory
        true,                   //neeed write access?
        &reason))
    {
        return false;
    }

    sPath=m_pCtDict->GetPath();
    if (!ValidateFile(
        sPath.c_str(),          //file name
        true,                   //must exist
        false,                  //mustn't be directory
        true,                   //neeed write access?
        &reason))
    {
        return false;
    }

    sPath=m_pGpDict->GetPath();
    if (!ValidateFile(
        sPath.c_str(),          //file name
        true,                   //must exist
        false,                  //mustn't be directory
        true,                   //neeed write access?
        &reason))
    {
        return false;
    }

    sPath=m_pGxDict->GetPath();
    if (!ValidateFile(
        sPath.c_str(),          //file name
        true,                   //must exist
        false,                  //mustn't be directory
        true,                   //neeed write access?
        &reason))
    {
        return false;
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemCatalog.AreDictionaryFilesWritable")
    return true;
}

//Attempts to set the dictionary directory and dictionary file names
//to the system default values, if present.
//Throws an exception if it did not complete successfully.
//Note that the effect of this function could be
//duplicated by first calling GetSysDefaultDictionaryInfo(), then
//calling SetDictionaryDir() and on each dictionary SetFileName().
void CCoordinateSystemCatalog::SetDefaultDictionaryDirAndFileNames()
{
    MG_TRY()

    //Get the system default path information
    STRING sDir=GetDefaultDictionaryDir();
    STRING sCs=m_pCsDict->GetDefaultFileName();
    STRING sDt=m_pDtDict->GetDefaultFileName();
    STRING sEl=m_pElDict->GetDefaultFileName();
    STRING sCt=m_pCtDict->GetDefaultFileName();
    STRING sGp=m_pGpDict->GetDefaultFileName();
    STRING sGx=m_pGxDict->GetDefaultFileName();

    //sets the path to the dictionaries
    //this will perform a validation of the existence of the directory
    SetDictionaryDir(sDir);

    //Set the dictionary file names
    //this will perform a validation of the existence of the files inside the directory
    m_pCsDict->SetFileName(sCs);
    m_pDtDict->SetFileName(sDt);
    m_pElDict->SetFileName(sEl);
    m_pCtDict->SetFileName(sCt);
    m_pGpDict->SetFileName(sGp);
    m_pGxDict->SetFileName(sGx);

    MG_CATCH_AND_THROW(L"MgCoordinateSystemCatalog.SetDefaultDictionaryDirAndFileNames")

    STRING sDefUserDictionaryDir;
    {
        MG_TRY()

            //Get the default path where to store the custom CS information
            //note, that we don't require that to be available for (the server
            sDefUserDictionaryDir = this->GetDefaultUserDictionaryDir();
            if (!sDefUserDictionaryDir.empty())
            {
                this->SetUserDictionaryDir(sDefUserDictionaryDir);
            }

        MG_CATCH_AND_RELEASE()

        #ifdef _DEBUG
            if (NULL != mgException)
            {
                _ASSERT(!sDefUserDictionaryDir.empty());
                //log the fact, that we couldn't set the right user dictionary dir
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("\n(%t) CCoordinateSystemCatalog::SetDefaultDictionaryDirAndFileNames() - The directory to store user defined CS information could not be set using this directory\n%W.\n"), 
                    sDefUserDictionaryDir.c_str()));
            }
            else if (!sDefUserDictionaryDir.empty())
            {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("\n(%t) CCoordinateSystemCatalog::SetDefaultDictionaryDirAndFileNames() - The directory to store user defined CS information in has been set to \n%W.\n"), 
                        sDefUserDictionaryDir.c_str()));
            }
        #endif
    }

    m_libraryStatus = lsInitialized;
}


//Gets the list of available geodetic transformations MgCoordinateSystemGeodeticTransformation
//that can transform from the pSource to the target datum pTarget.
//Currently, only one geodetic transformation is available.
//In a near future, for a given couple suorce/target datums, there might be multiple possible ways for
//converting to the target datum.
//If pSource and/or pTarget is NULL, WGS84 is assumed
//Caller is responsible for freeing the list of geodetic transformations.
//
MgDisposableCollection* CCoordinateSystemCatalog::GetGeodeticTransformations(MgCoordinateSystemDatum* pSource, MgCoordinateSystemDatum *pTarget)
{
    Ptr<MgDisposableCollection> pColl;

    MG_TRY()

    //We can get a datum shift even if the catalog is closed, but
    //the directory must have been set, since the converter will
    //need to get at data files to do certain datum shifts.
    if (m_sDir.empty())
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemCatalog.GetGeodeticTransformations", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNotReadyException", NULL);
    }

    Ptr<CCoordinateSystemGeodeticTransformation> pNew = new CCoordinateSystemGeodeticTransformation(this, pSource, pTarget);

    if (NULL == pNew.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCatalog.GetGeodeticTransformations", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    pColl = new MgDisposableCollection;

    if (NULL == pColl.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCatalog.GetGeodeticTransformations", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    pColl->Add(pNew);

    //And we're done!  Return success.
    MG_CATCH_AND_THROW(L"MgCoordinateSystemCatalog.GetGeodeticTransformations")

    return pColl.Detach();
}

//--------------------------------------------------------------------------------------------
MgCoordinateSystemMathComparator* CCoordinateSystemCatalog::GetMathComparator()
{
    CCoordinateSystemMathComparator *pNew=NULL;
    MG_TRY()
    pNew=new CCoordinateSystemMathComparator;
    if (NULL == pNew)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCatalog.GetMathComparator", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemCatalog.GetMathComparator")
    //And we're done!  Return success.
    return pNew;
}

//--------------------------------------------------------------------------------------------
MgCoordinateSystemFormatConverter* CCoordinateSystemCatalog::GetFormatConverter()
{
    CCoordinateSystemFormatConverter *pNew=NULL;
    MG_TRY()
    pNew=new CCoordinateSystemFormatConverter(this);
    if (NULL == pNew)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCatalog.GetFormatConverter", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemCatalog.GetFormatConverter")

    //And we're done!  Return success.
    return pNew;
}

//--------------------------------------------------------------------------------------------
MgCoordinateSystemProjectionInformation* CCoordinateSystemCatalog::GetProjectionInformation()
{
    CCoordinateSystemProjectionInformation *pNew=NULL;
    MG_TRY()
    pNew=new CCoordinateSystemProjectionInformation();
    if (NULL == pNew)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCatalog.GetProjectionInformation", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemCatalog.GetProjectionInformation")

    //And we're done!  Return success.
    return pNew;
}

//--------------------------------------------------------------------------------------------
MgCoordinateSystemUnitInformation* CCoordinateSystemCatalog::GetUnitInformation()
{
    CCoordinateSystemUnitInformation *pNew=NULL;
    MG_TRY()
    pNew=new CCoordinateSystemUnitInformation();
    if (NULL == pNew)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCatalog.GetUnitInformation", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemCatalog.GetUnitInformation")

    //And we're done!  Return success.
    return pNew;
}

//--------------------------------------------------------------------------------------------
MgCoordinateSystemDictionaryUtility* CCoordinateSystemCatalog::GetDictionaryUtility()
{
    CCoordinateSystemDictionaryUtility *pNew=NULL;
    MG_TRY()
    pNew=new CCoordinateSystemDictionaryUtility(this);
    if (NULL == pNew)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCatalog.GetDictionaryUtility", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemCatalog.GetDictionaryUtility")

    //And we're done!  Return success.
    return pNew;
}

//--------------------------------------------------------------------------------------------
//Sets the "protection mode" for the catalog.  If sMode is zero, then
//items marked as read-only will be protected, but all user-defined
//items will be unprotected.  If sMode < 0, then all protection is
//turned off.  If sMode > 0, then in addition to items marked read-only,
//all user-defined items older than sMode days will also be protected.
//By default, the protection mode is initially set to zero.
void CCoordinateSystemCatalog::SetProtectionMode(INT16 nMode)
{
    cs_Protect = nMode;
}

//--------------------------------------------------------------------------------------------
//Gets the protection mode (see SetProtectionMode description for details).
INT16 CCoordinateSystemCatalog::GetProtectionMode()
{
    return cs_Protect;
}

//--------------------------------------------------------------------------------------------
//MgDisposable
void CCoordinateSystemCatalog::Dispose()
{
    delete this;
}

//--------------------------------------------------------------------------------------------
LibraryStatus CCoordinateSystemCatalog::GetLibraryStatus()
{
    return m_libraryStatus;
}
