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

#include "CoordSysEllipsoid.h"              //for CCoordinateSystemEllipsoid
#include "CoordSysEnum.h"                   //for CCoordinateSystemEnum
#include "CoordSysEnumEllipsoid.h"          //for CCoordinateSystemEnumEllipsoid
#include "CoordSysEllipsoidDictionary.h"    //for CCoordinateSystemEllipsoidDictionary

#include "CoordSysUtil.h"                   //for Convert_Wide_To_UTF8
#include "MentorUtil.h"                     //for OpenDictionaryFile()
#include "MentorDictionary.h"
#include "cs_map.h"

using namespace CSLibrary;

//MENTOR_MAINTENANCE:  The following struct definition duplicates code
//from Mentor source file CS_rlsUpdt.c.
#define cs_Eldef07_ cs_Eldef_
#define cs_Eldef08_ cs_Eldef_
struct csElrup_
{
    short level;
    short crypt;
    union
    {
        struct cs_Eldef05_ eldef05;
        struct cs_Eldef07_ eldef07;
        struct cs_Eldef08_ eldef08;
        struct cs_Eldef_   eldef;
    } u;
};
#undef cs_Eldef07_
#undef cs_Eldef08_



//Externs from Mentor
extern "C"
{
    int CSelrupReadOld (csFILE *oldStrm,struct csElrup_ *elrup,int old_lvl);
}

//Function which returns whether the specified "magic number" is
//a valid one for a Mentor ellipsoid dictionary.  The returned value
//indicates the highest access level allowed:  CsDictionaryOpenMode::Write if current
//Mentor version, CsDictionaryOpenMode::Read for old versions, CsDictionaryOpenMode::Closed if invalid.
//
static CsDictionaryOpenMode
EllipsoidValidMagic(long lMagic)
{
    switch (lMagic)
    {
    case cs_ELDEF_MAGIC05:
    case cs_ELDEF_MAGIC07:
        return Read;
    case cs_ELDEF_MAGIC:
        return Write;
    default:
        return Closed;
    }
}


//Function which returns the Mentor level of a dictionary, given
//a specified magic number.  Returns zero if invalid.
static int
EllipsoidVersion(long lMagic)
{
    switch (lMagic)
    {
    case cs_ELDEF_MAGIC05:
        return 5;
    case cs_ELDEF_MAGIC07:
        return 7;
    case cs_ELDEF_MAGIC:
        return 8;
    default:
        return 0;
    }
}


//Function which sets up parameters of a Mentor struct as a function
//of its version number.
//
void
GetEllipsoidSizeInfo(long lMagic, UINT32& nSize, UINT32& nNameSize)
{
    //The following locals are pointless and unused, except that I have
    //to declare them to be able to use sizeof() as desired.
    cs_Eldef05_ v5;
    cs_Eldef_ v8;

    switch (lMagic)
    {
    case cs_ELDEF_MAGIC05:
        nSize = sizeof(v5);
        nNameSize = sizeof(v5.key_nm);
        return;
    case cs_ELDEF_MAGIC07:
    case cs_ELDEF_MAGIC:
        nSize = sizeof(v8);
        nNameSize = sizeof(v8.key_nm);
        return;
    default:
        assert(0);
    }

    //The following will never actually get
    //executed, but is needed to turn off the
    //compiler's "unreferenced local variable"
    //warning.
    v5.key_nm[0] = v8.key_nm[0] = '\0';
}

//Local utility functions for comparing various versions of Mentor structs.
//See mentorutil.h for details.
DECLARE_MENTOR_COMPARISON_FUNCTION(ElCompareV5, cs_Eldef05_, Mentor6Strnicmp)
DECLARE_MENTOR_COMPARISON_FUNCTION(ElCompareV8, cs_Eldef_, _strnicmp)//_tcsnicmp)

//Function which gets a comparison function pointer according
//to version number.
static TCmpFunc
ElComparisonFunction(long lMagic)
{
    switch (lMagic)
    {
    case cs_ELDEF_MAGIC05:
        return ElCompareV5;
    case cs_ELDEF_MAGIC07:
    case cs_ELDEF_MAGIC:
        return ElCompareV8;
    default:
        assert(0);
        return NULL;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemEllipsoidDictionary::CCoordinateSystemEllipsoidDictionary(MgCoordinateSystemCatalog *pCatalog)
    : m_pmapSystemNameDescription(NULL), m_lMagic(0)
{
    m_pCatalog = SAFE_ADDREF(pCatalog);
    m_sFileName = GetDefaultFileName();
}

//Destructor.  Closes the dictionary, if open.
CCoordinateSystemEllipsoidDictionary::~CCoordinateSystemEllipsoidDictionary()
{
    if (NULL != m_pmapSystemNameDescription)
    {
        m_pmapSystemNameDescription->clear();
        delete m_pmapSystemNameDescription; m_pmapSystemNameDescription = NULL;
    }
    m_lMagic = 0;
}

//Private member function which tries to get a def with the specified
//name from the file, which is assumed to be already open.  If found,
//returns an allocated def which the caller must free via CS_free().
//If not found, returns NULL.
//
cs_Eldef_ * CCoordinateSystemEllipsoidDictionary::eldef(const char *kpName) const
{
    //If the dictionary version is current, we can just
    //use the regular function.
    if (cs_ELDEF_MAGIC == m_lMagic)
    {
        SmartCriticalClass critical(true);
        return CS_eldef(kpName);
    }

    //It's an old version.  We need to do a special search
    //in the file, and then, if found, update it to a current struct.    
    UINT32 nStructSize, nNameSize;
    GetEllipsoidSizeInfo(m_lMagic, nStructSize, nNameSize);
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
        ElComparisonFunction(m_lMagic));
    delete [] pBuf;
    if (nResult != 1)
    {
        //Not found in file.
        return NULL;
    }

    //Okay, we're positioned properly.  Now we're ready to read it in.
    int nLevel = EllipsoidVersion(m_lMagic);

    csElrup_ csElrup;

    //Try to read a struct out of the file
    cs_Eldef_ *pDef = NULL;
    if (1 == CSelrupReadOld(pFile, &csElrup, nLevel))
    {
        //Success!
        pDef = new cs_Eldef_;
        if (NULL == pDef) return NULL;
        memcpy(pDef, &csElrup.u.eldef, sizeof(cs_Eldef_));
    }
    CS_fclose(pFile);
    return pDef;
}

//-----------------------------------------------------------------------------
MgCoordinateSystemEllipsoid* CCoordinateSystemEllipsoidDictionary::NewEllipsoid()
{
    Ptr<CCoordinateSystemEllipsoid> pNewDef;

    MG_TRY()

    pNewDef = new CCoordinateSystemEllipsoid(m_pCatalog);

    if (NULL == pNewDef.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEllipsoidDictionary.NewEllipsoid", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoidDictionary.NewEllipsoid")

    return pNewDef.Detach();
}

//-----------------------------------------------------------------------------
STRING CCoordinateSystemEllipsoidDictionary::GetDefaultFileName()
{
    return L"Elipsoid.CSD";  // NOXLATE
}

//-----------------------------------------------------------------------------
//Gets the file name of the dictionary file.
STRING CCoordinateSystemEllipsoidDictionary::GetFileName()
{
    return m_sFileName;
}

//-----------------------------------------------------------------------------
void CCoordinateSystemEllipsoidDictionary::SetFileName(CREFSTRING sFileName)
{
    MG_TRY()

    //Make local variables to hold converted strings
    bool bResult = IsValidDictionaryName(sFileName);
    if (!bResult)
    {
        MgStringCollection arguments;
        arguments.Add(sFileName);
        throw new MgFileIoException(L"MgCoordinateSystemEllipsoidDictionary.SetFileName", __LINE__, __WFILE__, &arguments, L"MgInvalidArgumentException", NULL);
    }

    MentorDictionary::SetFileName(
        m_lMagic,
        EllipsoidValidMagic,
        m_pCatalog->GetDictionaryDir(),
        sFileName,
        m_sFileName,
        L"MgCoordinateSystemEllipsoidDictionary.SetFileName");

    //Okay, everybody opened all right, so update Mentor's global
    //variables appropriately.
    char* szEl=Convert_Wide_To_Ascii(sFileName.c_str());
    CriticalClass.Enter();
    CS_elfnm(szEl);
    CriticalClass.Leave();
    delete[] szEl;

    if (m_pmapSystemNameDescription)
    {
        m_pmapSystemNameDescription->clear();
        delete m_pmapSystemNameDescription;
        m_pmapSystemNameDescription=NULL;
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoidDictionary.SetFileName")
}

//------------------------------------------------------------------------
//Gets the path to the dictionary file, if currently open.
STRING CCoordinateSystemEllipsoidDictionary::GetPath()
{
    STRING sPath;
    MG_TRY()
    sPath = m_pCatalog->GetDictionaryDir() + m_sFileName;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoidDictionary.GetPath")
    return sPath;
}

//------------------------------------------------------------------------
//Gets the size of the set (number of defs it currently contains).
UINT32 CCoordinateSystemEllipsoidDictionary::GetSize()
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
    csFILE *pFile=MentorDictionary::Open(m_lMagic, EllipsoidValidMagic, strPath.c_str(), Read);

    //We're open, but no summary is available.  We need to examine
    //the file itself to find out how big it is.
    INT32 pos =  CS_ftell(pFile);
    CS_fseek(pFile, 0, SEEK_END);
    UINT32 ulFileBytes = 1 + CS_ftell(pFile);
    CS_fseek(pFile, pos, SEEK_SET);
    nSize=(ulFileBytes - sizeof(m_lMagic)) / sizeof(cs_Eldef_);
    if (0!=CS_fclose(pFile))
    {
        throw new MgFileIoException(L"MgCoordinateSystemEllipsoidDictionary.GetSize", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDictionaryCloseFailedException", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoidDictionary.GetSize")

    return nSize;
}

//Adds the specified def to the set.
//Throws an exception MgCoordinateSystemMismatchException if the
//def is not the right kind.
//The supplied MgCoordinateSystemEllipsoid
//object will be copied into the set, and can therefore be safely disposed
//of once the function returns.
//Throws an exception Duplicate (if there's already a definition in the set with that name),
//ReadOnly (if the set isn't allowing write access-- for example, if this
//is an interface to a catalog that doesn't have the ellipsoid dictionary
//open for write).
void CCoordinateSystemEllipsoidDictionary::Add(MgGuardDisposable *pDefinition)
{
    MG_TRY()

    assert(NULL != pDefinition);
    CHECKARGUMENTNULL(pDefinition, L"MgCoordinateSystemEllipsoidDictionary.Add");

    MgCoordinateSystemEllipsoid* pEllipsoidDef=dynamic_cast<MgCoordinateSystemEllipsoid*>(pDefinition);
    if (!pEllipsoidDef)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemEllipsoidDictionary.Add", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }

    MentorDictionary::UpdateDef<cs_Eldef_, MgCoordinateSystemEllipsoid>(
        m_pmapSystemNameDescription,
        ElKey,
        ElDesc,
        &MgCoordinateSystemEllipsoid::IsValid,
        CS_eldef,
        CS_elupd,
        BuildElDefFromInterface,
        pEllipsoidDef,
        false);

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoidDictionary.Add")
}

//Removes the ellipsoid definition with the specified name from the
//set.
//Throws an exception if there is
//no definition with that name in the catalog, ReadOnly if the set
//isn't allowing write access.
void CCoordinateSystemEllipsoidDictionary::Remove(CREFSTRING sName)
{
    MG_TRY()
    MentorDictionary::RemoveDef<cs_Eldef_>(
        m_pmapSystemNameDescription,
        CS_eldef,
        CS_eldel,
        sName.c_str());
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoidDictionary.Remove")
}

//Modifies the specified def in the set.
//Throws an exception MgCoordinateSystemMismatchException if the
//def is not the right kind.  Otherwise, works like ModifyEllipsoid().
//Modifies an ellipsoid definition in the set.  The existing definition
//of the same name will be replaced with a copy of the def pointed to by
//kpDef.
//Throws an exception if there is no definition with that name
//in the catalog, ReadOnly if the set isn't allowing write access.
void CCoordinateSystemEllipsoidDictionary::Modify(MgGuardDisposable *pDefinition)
{
    MG_TRY()
    assert(NULL != pDefinition);
    CHECKARGUMENTNULL(pDefinition, L"MgCoordinateSystemEllipsoidDictionary.Modify");

    MgCoordinateSystemEllipsoid* pEllipsoidDef=dynamic_cast<MgCoordinateSystemEllipsoid*>(pDefinition);
    if (!pEllipsoidDef)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemEllipsoidDictionary.Modify", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }

    MentorDictionary::UpdateDef<cs_Eldef_, MgCoordinateSystemEllipsoid>(
        m_pmapSystemNameDescription,
        ElKey,
        ElDesc,
        &MgCoordinateSystemEllipsoid::IsValid,
        CS_eldef,
        CS_elupd,
        BuildElDefFromInterface,
        pEllipsoidDef,
        true);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoidDictionary.Modify")
}

//This function looks for an ellipsoid definition in the set with
//the specified name and, if found, creates an MgCoordinateSystemEllipsoid and
//returns it (user is responsible for freeing the def via Release()
//function).
//Throws an exception if no such definition exists in the catalog.
MgGuardDisposable* CCoordinateSystemEllipsoidDictionary::Get(CREFSTRING sName)
{
    return GetEllipsoid(sName);
}

//--------------------------------------------------------------
MgCoordinateSystemEllipsoid* CCoordinateSystemEllipsoidDictionary::GetEllipsoid(CREFSTRING sName)
{
    Ptr<MgCoordinateSystemEllipsoid> pEllipsoidDef;
    char *pName = NULL;
    cs_Eldef_ *pDef = NULL;

    MG_TRY()

    //Get the name to search for
    pName = Convert_Wide_To_UTF8(sName.c_str()); //need to delete [] pName

    //Look in the dictionary
    pDef = eldef(pName);

    if (NULL == pDef)
    {
        //wasn't there!
        MgStringCollection whatArguments;
        whatArguments.Add(sName);
        throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemEllipsoidDictionary.GetEllipsoid", __LINE__, __WFILE__, &whatArguments, L"", NULL);
    }

    //Okay, at this point we have a Mentor ellipsoid definition
    //struct.  Build an object out of it.
    //Try to get the def from the set
    pEllipsoidDef = this->GetEllipsoid(pDef);
    assert(NULL != pEllipsoidDef);

    MG_CATCH(L"MgCoordinateSystemEllipsoidDictionary.GetEllipsoid")

    if (NULL != pDef)
    {
        CS_free(pDef);
        pDef = NULL;
    }

    delete [] pName;
    pName = NULL;

    MG_THROW()

    return pEllipsoidDef.Detach();
}

//--------------------------------------------------------------
//needed to work with the [ReadAllEllipsoids] method
MgCoordinateSystemEllipsoid* CCoordinateSystemEllipsoidDictionary::GetEllipsoid(const cs_Eldef_* elDef, const std::vector<std::map<STRING,Ptr<MgDisposable> >*>* const /* unused */)
{
    Ptr<MgCoordinateSystemEllipsoid> pEllipsoidDef = BuildInterfaceFromElDef(*elDef, m_pCatalog);

    if (NULL == pEllipsoidDef.p)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemEllipsoidDictionary.GetEllipsoid", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return pEllipsoidDef.Detach();
}

//--------------------------------------------------------------
//Returns whether the set contains a def with the specified name.
bool CCoordinateSystemEllipsoidDictionary::Has(CREFSTRING sName)
{
    bool bHas=false;

    MG_TRY()

    //Get the name to search for
    char *pName = Convert_Wide_To_UTF8(sName.c_str()); //need to delete [] pName
    if (NULL == pName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEllipsoidDictionary.Has", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (NULL != m_pmapSystemNameDescription)
    {
        CSystemNameDescriptionMap::const_iterator it = m_pmapSystemNameDescription->find(CSystemName(pName));
        bHas = (it != m_pmapSystemNameDescription->end());
    }
    else
    {

        //Look in the dictionary
        cs_Eldef_ *pDef = eldef(pName);
        if (NULL != pDef)
        {
            bHas = true;
            CS_free(pDef);
        }
    }
    delete [] pName;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoidDictionary.Has")
    return bHas;
}

//--------------------------------------------------------------
//Gets an enumerator for all the defs in the set.
MgCoordinateSystemEnum* CCoordinateSystemEllipsoidDictionary::GetEnum()
{
    MgCoordinateSystemEnum* pEnum=NULL;

    MG_TRY()
    CCoordinateSystemEnumEllipsoid* pNew=GetEnumImp();
    assert(pNew);
    pEnum=static_cast<MgCoordinateSystemEnum*>(pNew);
    assert(pEnum);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoidDictionary.GetEnum")
    return pEnum;
}

//----------------------------------------------------------------------
CCoordinateSystemEnumEllipsoid* CCoordinateSystemEllipsoidDictionary::GetEnumImp()
{
    Ptr<CCoordinateSystemEnumEllipsoid> pNew;
    csFILE *pFile=NULL;
    SmartCriticalClass critical(true);

    MG_TRY()
    STRING strPath=GetPath();
    pFile = MentorDictionary::Open(m_lMagic, EllipsoidValidMagic, strPath.c_str(), Read);
    _ASSERT(pFile);

    //close the file before calling into the [read all CS-Map defs] method
    if (0 != CS_fclose(pFile))
        throw new MgFileIoException(L"MgCoordinateSystemDictionary.GetEnum", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDictionaryCloseFailedException", NULL);

    pFile = NULL;

    //Generate a summary list, if we don't already have one
    if (NULL == m_pmapSystemNameDescription)
    {
        m_pmapSystemNameDescription = MentorDictionary::GenerateSystemNameDescriptionMap<cs_Eldef_>(
            ElKey,
            ElDesc,
            CS_eldefAll);
        if (NULL == m_pmapSystemNameDescription)
        {
            throw new MgInvalidArgumentException(L"MgCoordinateSystemEllipsoidDictionary.GetEnum", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }

    //Make an enumerator object
    pNew = new CCoordinateSystemEnumEllipsoid;

    if (pFile && 0 != CS_fclose(pFile))
    {
        throw new MgFileIoException(L"MgCoordinateSystemEllipsoidDictionary.GetEnum", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDictionaryCloseFailedException", NULL);
    }

    pFile = NULL;

    MG_CATCH(L"MgCoordinateSystemEllipsoidDictionary.GetEnum")

    if (pFile)
    {
        CS_fclose(pFile);
    }

    MG_THROW()

    //Set it up to use our list
    pNew->Initialize(this, m_pmapSystemNameDescription);
    pNew->SetReadAllDefinitionCallback(CCoordinateSystemEllipsoidDictionary::ReadAllEllipsoids);

    return pNew.Detach();
}

//----------------------------------------------------------------------
MgDisposableCollection* CCoordinateSystemEllipsoidDictionary::ReadAllEllipsoids(/*IN, required*/MgCoordinateSystemDictionaryBase* targetDictionary,
                                                                                /*IN, optional*/const std::vector<MgCoordinateSystemFilter*>* const filters)
{
    CHECKARGUMENTNULL(targetDictionary, L"CCoordinateSystemEllipsoidDictionary.ReadAllEllipsoids");

    CCoordinateSystemEllipsoidDictionary* ellipsoidDictionary = dynamic_cast<CCoordinateSystemEllipsoidDictionary*>(targetDictionary);
    if (NULL == ellipsoidDictionary)
        throw new MgInvalidArgumentException(L"CCoordinateSystemEllipsoidDictionary.ReadAllEllipsoids", __LINE__, __WFILE__, NULL, L"", NULL);

    return MentorDictionary::ReadAllDefinitions<MgCoordinateSystemEllipsoid, cs_Eldef_, CCoordinateSystemEllipsoidDictionary>(
        ellipsoidDictionary,
        CS_eldefAll,
        NULL, //no post processing needed
        &CCoordinateSystemEllipsoidDictionary::GetEllipsoid,
        NULL, //no additional info needed/used to create an MgCoordinateSystemEllipsoid object
        filters);
}

//MgDisposable
void CCoordinateSystemEllipsoidDictionary::Dispose()
{
    delete this;
}

//*****************************************************************************
MgCoordinateSystemCatalog* CCoordinateSystemEllipsoidDictionary::GetCatalog()
{
    return SAFE_ADDREF(m_pCatalog.p);
}
