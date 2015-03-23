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

#include "CoordSys.h"           //for CCoordinateSystem
#include "CoordSysEnum.h"       //for CCoordinateSystemEnum
#include "CoordSysUtil.h"       //for Convert_Wide_To_UTF8, CsDictionaryOpenMode
#include "CoordSysDictionary.h" //for CCoordinateSystemDictionary
#include "MentorUtil.h"            //for OpenDictionaryFile()
#include "MentorDictionary.h"   //for MentorDictionary
#include "cs_map.h"

using namespace CSLibrary;

//MENTOR_MAINTENANCE:  The following struct definition duplicates code
//from Mentor source file CS_rlsUpdt.c.
#define cs_Csdef07_ cs_Csdef_
#define cs_Csdef08_ cs_Csdef_
struct csCsrup_
{
    short level;
    short crypt;
    union
    {
        struct cs_Csdef05_ csdef05;
        struct cs_Csdef06_ csdef06;
        struct cs_Csdef07_ csdef07;
        struct cs_Csdef08_ csdef08;
    } u;
};
#undef cs_Csdef07_
#undef cs_Csdef08_


//Externs from Mentor
extern "C"
{
    int CScsrupReadOld (csFILE *oldStrm,struct csCsrup_ *csrup,int old_lvl);
    int CScsrupRead05 (csFILE *oldStrm,struct csCsrup_ *csrup);
    int CScsrupRead06 (csFILE *oldStrm,struct csCsrup_ *csrup);
}

//Function which works like CS_csrd(), except that it reads version
//5 coordsys structs.
//
static int
CS_csrd05(
    csFILE *oldStrm,
    struct cs_Csdef05_ *cs_def,
    int *crypt)
{
    csCsrup_ csrup;
    int nStatus = CScsrupRead05(oldStrm, &csrup);
    if (nStatus > 0)
    {
        //success!
        *crypt = csrup.crypt;
        memcpy(cs_def, &csrup.u.csdef05, sizeof(cs_Csdef05_));
    }
    return nStatus;
}

//Function which works like CS_csrd(), except that it reads version
//6 coordsys structs.
//
static int
CS_csrd06(
    csFILE *oldStrm,
    struct cs_Csdef06_ *cs_def,
    int *crypt)
{
    csCsrup_ csrup;
    int nStatus = CScsrupRead06(oldStrm, &csrup);
    if (nStatus > 0)
    {
        //success!
        *crypt = csrup.crypt;
        memcpy(cs_def, &csrup.u.csdef06, sizeof(cs_Csdef06_));
    }
    return nStatus;
}

//Function which returns whether the specified "magic number" is
//a valid one for a Mentor coordsys dictionary.  The returned value
//indicates the highest access level allowed:  CsDictionaryOpenMode::Write if current
//Mentor version, CsDictionaryOpenMode::Read for old versions, CsDictionaryOpenMode::Closed if invalid.
//
static CsDictionaryOpenMode
CoordinateSystemValidMagic(long lMagic)
{
    switch (lMagic)
    {
    case cs_CSDEF_MAGIC05:
    case cs_CSDEF_MAGIC06:
    case cs_CSDEF_MAGIC07:
        return Read;
    case cs_CSDEF_MAGIC:
        return Write;
    default:
        return Closed;
    }
}


//Function which returns the Mentor level of a dictionary, given
//a specified magic number.  Returns zero if invalid.
static int
CoordinateSystemVersion(long lMagic)
{
    switch (lMagic)
    {
    case cs_CSDEF_MAGIC05:
        return 5;
    case cs_CSDEF_MAGIC06:
        return 6;
    case cs_CSDEF_MAGIC07:
        return 7;
    case cs_CSDEF_MAGIC:
        return 8;
    default:
        return 0;
    }
}


//Function which gets size of a Mentor struct as a function
//of its version number.
//
void
GetCoordinateSystemSizeInfo(long lMagic, UINT32& nSize, UINT32& nNameSize)
{
    //Locals which do nothing but are needed to use sizeof()
    //as desired.
    cs_Csdef05_ v5;
    cs_Csdef06_ v6;
    cs_Csdef_ v8;

    switch (lMagic)
    {
    case cs_CSDEF_MAGIC05:
        nSize = sizeof(v5);
        nNameSize = sizeof(v5.key_nm);
        return;
    case cs_CSDEF_MAGIC06:
        nSize = sizeof(v6);
        nNameSize = sizeof(v6.key_nm);
        return;
    case cs_CSDEF_MAGIC07:
    case cs_CSDEF_MAGIC:
        nSize = sizeof(v8);
        nNameSize = sizeof(v8.key_nm);
        return;
    default:
        throw new MgCoordinateSystemInitializationFailedException(
            L"GetCoordinateSystemSizeInfo", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //The following will never actually get
    //executed, but is needed to turn off the
    //compiler's "unreferenced local variable"
    //warning.
    v5.key_nm[0] = v6.key_nm[0] = v8.key_nm[0] = '\0';
}



//Local utility functions for comparing various versions of Mentor structs.
//See mentorutil.h for details.
DECLARE_MENTOR_COMPARISON_FUNCTION(CsCompareV5, cs_Csdef05_, Mentor6Strnicmp)
DECLARE_MENTOR_COMPARISON_FUNCTION(CsCompareV6, cs_Csdef06_, Mentor6Strnicmp)
DECLARE_MENTOR_COMPARISON_FUNCTION(CsCompareV8, cs_Csdef_, _strnicmp)//_tcsnicmp)

//Function which gets a comparison function pointer according
//to version number.
static TCmpFunc
CsComparisonFunction(long lMagic)
{
    switch (lMagic)
    {
    case cs_CSDEF_MAGIC05:
        return CsCompareV5;
    case cs_CSDEF_MAGIC06:
        return CsCompareV6;
    case cs_CSDEF_MAGIC07:
    case cs_CSDEF_MAGIC:
        return CsCompareV8;
    default:
        assert(0);
        return NULL;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemDictionary::CCoordinateSystemDictionary(MgCoordinateSystemCatalog *pCatalog)
                            : m_pmapSystemNameDescription(NULL),
                            m_lMagic(0)
{
    m_pCatalog = SAFE_ADDREF(pCatalog);
    m_sFileName = GetDefaultFileName();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemDictionary::~CCoordinateSystemDictionary()
{
    if (NULL != m_pmapSystemNameDescription)
    {
        m_pmapSystemNameDescription->clear();
        delete m_pmapSystemNameDescription;
        m_pmapSystemNameDescription = NULL;
    }
    m_lMagic = 0;
}

//Private member function which tries to get a def with the specified
//name from the stream, which is assumed to be already open.  If found,
//returns an allocated def which the caller must free via CS_free().
//If not found, returns NULL.
//
cs_Csdef_ * CCoordinateSystemDictionary::csdef(const char *kpName)
{
    //If the dictionary version is current, we can just
    //use the regular function.
    if (cs_CSDEF_MAGIC == m_lMagic)
    {
        SmartCriticalClass critical(true);
        cs_Csdef_* pDef=CS_csdef(kpName);
        if (pDef)
        {
            this->DoCsDefPostReadProcessing(pDef);
        }
        return pDef;
    }

    //It's an old version.  We need to do a special search
    //in the file, and then, if found, update it to a current struct.
    UINT32 nStructSize, nNameSize;
    GetCoordinateSystemSizeInfo(m_lMagic, nStructSize, nNameSize);
    if (strlen(kpName) > nNameSize-1) return NULL;

    //Make a buffer to be our "record pointer"
    char *pBuf = new char[nStructSize];
    if (NULL == pBuf) return NULL;
    memset(pBuf, 0, nStructSize);
    strcpy(pBuf, kpName);

    //Search for the record in the file
    char szMode[10];
    GetFileModes(Read, szMode);
    SmartCriticalClass critical(true);
    csFILE *pFile=CS_fopen(kpName, szMode);
    if (!pFile)
    {
        delete [] pBuf;
        return NULL;
    }
    int nResult = CS_bins(
        pFile,
        sizeof(cs_magic_t),
        0L,
        nStructSize,
        pBuf,
        CsComparisonFunction(m_lMagic));
    delete [] pBuf;
    if (nResult != 1)
    {
        //Not found in file.
        return NULL;
    }

    //Okay, we're positioned properly.  Now we're ready to read it in.
    int nLevel = CoordinateSystemVersion(m_lMagic);

    csCsrup_ csCsrup;

    //Try to read a struct out of the file
    cs_Csdef_ *pDef = NULL;
    if (1 == CScsrupReadOld(pFile, &csCsrup, nLevel))
    {
        //Success!
        pDef = new cs_Csdef_;
        if (NULL == pDef) return NULL;
        memcpy(pDef, &csCsrup.u.csdef08, sizeof(cs_Csdef_));
    }
    CS_fclose(pFile);
    if (pDef)
    {
        this->DoCsDefPostReadProcessing(pDef);
    }

    return pDef;
}

//------------------------------------------------------------------------
//does some post processing after a [cs_Csdef_] struct has been read from the
//CsMap dictionary; [pDef] might get changed!
void CCoordinateSystemDictionary::DoCsDefPostReadProcessing(cs_Csdef_* pDef)
{
    INT32 unit = UnitFromString(pDef->unit);
    INT32 unitType;
    double dfUnitScale=0.;
    if (GetUnitInfo(unit, &unitType, &dfUnitScale))
    {
        pDef->unit_scl=dfUnitScale;
    }
}

//------------------------------------------------------------------------
MgCoordinateSystem* CCoordinateSystemDictionary::NewCoordinateSystem()
{
    Ptr<CCoordinateSystem> pNewDef;

    MG_TRY()

    pNewDef = new CCoordinateSystem(m_pCatalog);

    if (NULL == pNewDef.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDictionary.NewCoordinateSystem", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemDictionary.NewCoordinateSystem")

    return pNewDef.Detach();
}

//------------------------------------------------------------------------
STRING CCoordinateSystemDictionary::GetDefaultFileName()
{
    return L"Coordsys.CSD";  // NOXLATE
}

//-----------------------------------------------------------------------------
//Gets the file name of the dictionary file.
STRING CCoordinateSystemDictionary::GetFileName()
{
    return m_sFileName;
}

//------------------------------------------------------------------------
void CCoordinateSystemDictionary::SetFileName(CREFSTRING sFileName)
{
    char* szCs = NULL;

    MG_TRY()

    //Make local variables to hold converted strings
    bool bResult = IsValidDictionaryName(sFileName);
    if (!bResult)
    {
        MgStringCollection arguments;
        arguments.Add(sFileName);
        throw new MgFileIoException(L"MgCoordinateSystemDictionary.SetFileName", __LINE__, __WFILE__, &arguments, L"MgInvalidArgumentException", NULL);
    }

    MentorDictionary::SetFileName(
        m_lMagic,
        CoordinateSystemValidMagic,
        m_pCatalog->GetDictionaryDir(),
        sFileName,
        m_sFileName,
        L"MgCoordinateSystemDictionary.SetFileName");

    SmartCriticalClass csMapLock;

    //Okay, everybody opened all right, so update Mentor's global
    //variables appropriately.
    szCs = Convert_Wide_To_Ascii(sFileName.c_str());
    CS_csfnm(szCs);

    delete m_pmapSystemNameDescription;
    m_pmapSystemNameDescription = NULL;

    MG_CATCH(L"MgCoordinateSystemDictionary.SetFileName")

        delete[] szCs;
        szCs = NULL;

    MG_THROW()
}

//------------------------------------------------------------------------
//Gets the path to the dictionary file, if currently open.
STRING CCoordinateSystemDictionary::GetPath()
{
    STRING sPath;
    MG_TRY()
    sPath = m_pCatalog->GetDictionaryDir() + m_sFileName;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemDictionary.GetPath")
    return sPath;
}

//------------------------------------------------------------------------
UINT32 CCoordinateSystemDictionary::GetSize()
{
    UINT32 nSize=0;

    MG_TRY()
    //If we have a summary available, we can just return its size.
    if (NULL != m_pmapSystemNameDescription)
    {
        return static_cast<UINT32>(m_pmapSystemNameDescription->size());
    }

    SmartCriticalClass critical(true);
    STRING strPath=GetPath();
    csFILE *pFile=MentorDictionary::Open(m_lMagic, CoordinateSystemValidMagic, strPath.c_str(), Read);

    //We're open, but no summary is available.  We need to examine
    //the file itself to find out how big it is.
    INT32 pos =  CS_ftell(pFile);
    CS_fseek(pFile, 0, SEEK_END);
    UINT32 ulFileBytes = 1 + CS_ftell(pFile);
    CS_fseek(pFile, pos, SEEK_SET);
    UINT32 nRecSize, nDummy;
    GetCoordinateSystemSizeInfo(m_lMagic, nRecSize, nDummy);
    nSize=(ulFileBytes - sizeof(m_lMagic)) / nRecSize;

    if (0!=CS_fclose(pFile))
    {
        throw new MgFileIoException(L"MgCoordinateSystemDictionary.GetSize", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDictionaryCloseFailedException", NULL);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemDictionary.GetSize")

    return nSize;
}

//Adds the specified def.
//Throws an exception MgCoordinateSystemMismatchException if the
//def is not the right kind.
//The supplied MgGuardDisposable object will be copied, and can therefore be safely disposed
//of once the function returns.
//Throws an exception Duplicate (if there's already a definition in the set with that name),
//a ReadOnly (if the set isn't allowing write access-- for example, if this
//is an interface to a catalog that doesn't have the coordsys dictionary
//open for write).
void CCoordinateSystemDictionary::Add(MgGuardDisposable *pDefinition)
{
    MG_TRY()

    assert(NULL != pDefinition);
    CHECKARGUMENTNULL(pDefinition, L"MgCoordinateSystemDictionary.Add");

    MgCoordinateSystem* pCoordinateSystem=dynamic_cast<MgCoordinateSystem*>(pDefinition);
    if (!pCoordinateSystem)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemDictionary.Add", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }

    MentorDictionary::UpdateDef<cs_Csdef_, MgCoordinateSystem>(
        m_pmapSystemNameDescription,
        CsKey,
        CsDesc,
        &MgCoordinateSystem::IsValid,
        CS_csdef,
        CS_csupd,
        BuildCsDefFromInterface,
        pCoordinateSystem,
        false);

    MG_CATCH_AND_THROW(L"MgCoordinateSystemDictionary.Add")
}

//Removes the coordsys definition with the specified name from the
//set.
//Throws an exception if there is
//no definition with that name in the catalog, a ReadOnly if the set
//isn't allowing write access).
//
void CCoordinateSystemDictionary::Remove(CREFSTRING sName)
{
    MG_TRY()
    MentorDictionary::RemoveDef<cs_Csdef_>(
        m_pmapSystemNameDescription,
        CS_csdef,
        CS_csdel,
        sName.c_str());
    MG_CATCH_AND_THROW(L"MgCoordinateSystemDictionary.Remove")
}

//Modifies the specified def in the set.
//Throws an exception MgCoordinateSystemMismatchException if the
//def is not the right kind.
void CCoordinateSystemDictionary::Modify(MgGuardDisposable *pDefinition)
{
    MG_TRY()

    assert(NULL != pDefinition);
    CHECKARGUMENTNULL(pDefinition, L"MgCoordinateSystemDictionary.Modify");

    MgCoordinateSystem* pCoordinateSystem=dynamic_cast<MgCoordinateSystem*>(pDefinition);
    if (!pCoordinateSystem)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemDictionary.Modify", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }

    MentorDictionary::UpdateDef<cs_Csdef_, MgCoordinateSystem>(
        m_pmapSystemNameDescription,
        CsKey,
        CsDesc,
        &MgCoordinateSystem::IsValid,
        CS_csdef,
        CS_csupd,
        BuildCsDefFromInterface,
        pCoordinateSystem,
        true);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemDictionary.Modify")
}

//This function looks for a coordsys definition in the set with
//the specified name and, if found, creates an MgCoordinateSystem and
//returns it (user is responsible for freeing the def via Release()
//function).
//Throws an exception if no such definition exists in the catalog.
MgGuardDisposable* CCoordinateSystemDictionary::Get(CREFSTRING sName)
{
    return GetCoordinateSystem(sName);
}

//creates an MgCoordinateSystem by querying the underlying dictionary files - incl.
//these for the ellipsoids and the datums.
//If no such CS exists, an exception will be thrown, i.e. this method does
//never return NULL
MgCoordinateSystem* CCoordinateSystemDictionary::GetCoordinateSystem(CREFSTRING sName)
{
    Ptr<MgCoordinateSystem> pDefinition;
    cs_Csdef_ *pDef = NULL;
    char *pName = NULL;

    MG_TRY()

    //Get the name to search for
    pName = Convert_Wide_To_UTF8(sName.c_str()); //need to delete [] pName

    if (NULL == pName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDictionary.GetCoordinateSystem", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Look in the dictionary
    pDef = csdef(pName);

    if (NULL == pDef)
    {
        //wasn't there!
        MgStringCollection whatArguments;
        whatArguments.Add(sName);
        throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemDictionary.GetCoordinateSystem", __LINE__, __WFILE__, &whatArguments, L"", NULL);
    }

    //Okay, at this point we have a Mentor coordsys definition
    //struct.  Build an object out of it.
    pDefinition = this->GetCoordinateSystem(pDef, NULL);
    assert(NULL != pDefinition);

    MG_CATCH(L"MgCoordinateSystemDictionary.GetCoordinateSystem")
    
    if (NULL != pDef)
    {
        CS_free(pDef);
        pDef = NULL;
    }

    delete [] pName;
    pName = NULL;

    MG_THROW()

    return pDefinition.Detach();
}

//creates an MgCoordinateSystem; basically, it forward the
//call to [InitFromCatalog] which will deal with the information
//as it might have been passed by [datumEllipsoidInfos]
MgCoordinateSystem* CCoordinateSystemDictionary::GetCoordinateSystem(const cs_Csdef_* csDef,
                                                                     const std::vector<std::map<STRING,Ptr<MgDisposable> >*>* const datumEllipsoidInfos)
{
    Ptr<MgCoordinateSystem> pDefinition;

    MG_TRY()

    pDefinition = NewCoordinateSystem();

    CCoordinateSystem* pImp = static_cast<CCoordinateSystem*>(pDefinition.p);

    assert(NULL == datumEllipsoidInfos || 2 == datumEllipsoidInfos->size());
    if (NULL != datumEllipsoidInfos && 2 == datumEllipsoidInfos->size())
    {
        pImp->InitFromCatalog(*csDef, datumEllipsoidInfos->at(0) /* ellipsoids */, datumEllipsoidInfos->at(1) /* datums */);
    }
    else
    {
        pImp->InitFromCatalog(*csDef);
    }

    MG_CATCH(L"MgCoordinateSystemDictionary.GetCoordinateSystem")

    MG_THROW()

    return pDefinition.Detach();
}

//Checks whether the named coordsys exists in the set.
bool CCoordinateSystemDictionary::Has(CREFSTRING sName)
{
    bool bHas=false;

    MG_TRY()
    //Get the name to search for
    char *pName = Convert_Wide_To_UTF8(sName.c_str()); //need to delete [] pName
    if (NULL == pName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDictionary.Has", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (NULL != m_pmapSystemNameDescription)
    {
        CSystemNameDescriptionMap::const_iterator it = m_pmapSystemNameDescription->find(CSystemName(pName));
        bHas = (it != m_pmapSystemNameDescription->end());
    }
    else
    {

        //Look in the dictionary
        cs_Csdef_ *pDef = csdef(pName);
        if (NULL != pDef)
        {
            bHas = true;
            CS_free(pDef);
        }
    }
    delete [] pName;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemDictionary.Has")

    return bHas;
}

//------------------------------------------------------------------------
//Reads *all* coordinate systems from the dictionary in one run
//Does also read the necessary datum and ellipsoid information from the respective dictionaries
MgDisposableCollection* CCoordinateSystemDictionary::ReadAllCoordinateSystems(/*IN, required*/ MgCoordinateSystemDictionaryBase* targetDictionary,
                                                                              /*IN, optional*/ const std::vector<MgCoordinateSystemFilter*>* const filters)
{
    CHECKARGUMENTNULL(targetDictionary, L"MgCoordinateSystemDictionary.ReadAllCoordinateSystems");

    CCoordinateSystemDictionary* csDictionary = dynamic_cast<CCoordinateSystemDictionary*>(targetDictionary);
    if (NULL == csDictionary)
        throw new MgInvalidArgumentException(L"MgCoordinateSystemDictionary.ReadAllCoordinateSystems", __LINE__, __WFILE__, NULL, L"", NULL);

    //in order to create the MgCoordinateSystem objects, we have to have also the information on the datums and ellipsoids
    map<STRING, Ptr<MgDisposable> > ellipsoidMap;
    map<STRING, Ptr<MgDisposable> > datumMap;

    //place a lock here - we don't want any interference; what we need is the *current* status of all dictionary files
    SmartCriticalClass dictionaryLock(true);

    Ptr<MgCoordinateSystemCatalog> catalog = targetDictionary->GetCatalog();
    Ptr<MgCoordinateSystemEllipsoidDictionary> ellipsoidDictionary = catalog->GetEllipsoidDictionary();
    Ptr<MgCoordinateSystemDatumDictionary> datumDictionary = catalog->GetDatumDictionary();

    //read all ellipsoids and all datums
    MentorDictionary::ReadAllDefinitions<MgCoordinateSystemEllipsoid>(ellipsoidDictionary, &MgCoordinateSystemEllipsoid::GetElCode, ellipsoidMap);
    MentorDictionary::ReadAllDefinitions<MgCoordinateSystemDatum>(datumDictionary, &MgCoordinateSystemDatum::GetDtCode, datumMap);

    //place the std::maps in a vector which later be passed to the [GetCoordinateSystem] method - the oder is important!
    std::vector<std::map<STRING,Ptr<MgDisposable> >*> datumEllipsoidInfos;
    datumEllipsoidInfos.push_back(&ellipsoidMap);
    datumEllipsoidInfos.push_back(&datumMap);

    //finally, read all "root" coordinate system definitions from the dictionary
    return MentorDictionary::ReadAllDefinitions<MgCoordinateSystem, cs_Csdef_, CCoordinateSystemDictionary>(
        csDictionary,
        CS_csdefAll, 
        &CCoordinateSystemDictionary::DoCsDefPostReadProcessing,
        &CCoordinateSystemDictionary::GetCoordinateSystem,
        &datumEllipsoidInfos,
        filters);
}

//--------------------------------------------------------------
//Gets an enumerator for all the defs
MgCoordinateSystemEnum* CCoordinateSystemDictionary::GetEnum()
{
    MgCoordinateSystemEnum* pEnum=NULL;

    MG_TRY()

    CCoordinateSystemEnum* pNew=GetEnumImp();
    assert(pNew);
    pEnum=static_cast<MgCoordinateSystemEnum*>(pNew);
    assert(pEnum);

    MG_CATCH_AND_THROW(L"MgCoordinateSystemDictionary.GetEnum")

    return pEnum;
}

//------------------------------------------------------------------
CCoordinateSystemEnum* CCoordinateSystemDictionary::GetEnumImp()
{
    Ptr<CCoordinateSystemEnum> pNew;
    csFILE *pFile=NULL;
    SmartCriticalClass critical(true);

    bool setQuickReadCallback = false;

    MG_TRY()
    
    STRING strPath=GetPath();
    pFile = MentorDictionary::Open(m_lMagic, CoordinateSystemValidMagic, strPath.c_str(), Read);
    const int nVersion = CoordinateSystemVersion(m_lMagic);
    assert(nVersion > 0);

    setQuickReadCallback = (7 == nVersion || 8 == nVersion);

    //Generate a summary list, if we don't already have one
    if (NULL == m_pmapSystemNameDescription)
    {
        switch (nVersion)
        {
        case 5:
            //Generate summary for version 5 coordsys file.
            m_pmapSystemNameDescription = MentorDictionary::GenerateSystemNameDescriptionMap<cs_Csdef05_>(
                pFile,
                CsKey05,
                CsDesc05,
                CS_csrd05);
            break;
        case 6:
            //Generate summary for version 6 coordsys file.
            m_pmapSystemNameDescription = MentorDictionary::GenerateSystemNameDescriptionMap<cs_Csdef06_>(
                pFile,
                CsKey06,
                CsDesc06,
                CS_csrd06);
            break;
        case 7:
        case 8:
            //Generate summary for version 7 or 8 coordsys file.
            //close the file before calling into the [read all CS-Map defs] method
            if (0 != CS_fclose(pFile))
                throw new MgFileIoException(L"MgCoordinateSystemDictionary.GetEnum", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDictionaryCloseFailedException", NULL);

            pFile = NULL;

            m_pmapSystemNameDescription = MentorDictionary::GenerateSystemNameDescriptionMap<cs_Csdef_>(
                CsKey,
                CsDesc,
                CS_csdefAll);
            break;
        default:
            assert(0);
        }
        if (NULL == m_pmapSystemNameDescription)
        {
            throw new MgInvalidArgumentException(L"MgCoordinateSystemDictionary.GetEnum", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }

    //Make an enumerator object
    pNew = new CCoordinateSystemEnum;

    if (NULL == pNew.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDictionary.GetEnum", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (pFile && 0 != CS_fclose(pFile))
    {
        throw new MgFileIoException(L"MgCoordinateSystemDictionary.GetEnum", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDictionaryCloseFailedException", NULL);
    }
    pFile = NULL;

    MG_CATCH(L"MgCoordinateSystemDictionary.GetEnum")

    if (pFile)
    {
        CS_fclose(pFile);
    }

    MG_THROW()

    //Set it up to use our list
    pNew->Initialize(this, m_pmapSystemNameDescription);
    if (setQuickReadCallback)
        pNew->SetReadAllDefinitionCallback(CCoordinateSystemDictionary::ReadAllCoordinateSystems);

    return pNew.Detach();
}

//MgDisposable
void CCoordinateSystemDictionary::Dispose()
{
    delete this;
}

//*****************************************************************************
MgCoordinateSystemCatalog* CCoordinateSystemDictionary::GetCatalog()
{
    return SAFE_ADDREF(m_pCatalog.p);
}
