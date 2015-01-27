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

#include "CoordSysDatum.h"              //for CCoordinateSystemDatum
#include "CoordSysEnum.h"               //for CCoordinateSystemEnum
#include "CoordSysEnumDatum.h"          //for CCoordinateSystemEnumDatum
#include "CoordSysDatumDictionary.h"    //for CCoordinateSystemDatumDictionary

#include "CoordSysUtil.h"               //for Convert_Wide_To_UTF8
#include "MentorUtil.h"                 //for OpenDictionaryFile()
#include "MentorDictionary.h"
#include "cs_map.h"

using namespace CSLibrary;

//MENTOR_MAINTENANCE:  The following struct definition duplicates code
//from Mentor source file CS_rlsUpdt.c.
#define cs_Dtdef07_ cs_Dtdef_
#define cs_Dtdef08_ cs_Dtdef_
struct csDtrup_
{
    short level;
    short crypt;
    union
    {
        struct cs_Dtdef05_ dtdef05;
        struct cs_Dtdef06_ dtdef06;
        struct cs_Dtdef07_ dtdef07;
        struct cs_Dtdef08_ dtdef08;
    } u;
};
#undef cs_Dtdef07_
#undef cs_Dtdef08_

//Externs from Mentor
extern "C"
{
    int CSdtrupReadOld (csFILE *oldStrm,struct csDtrup_ *dtrup,int old_lvl);
    int CSdtrupRead05 (csFILE *oldStrm,struct csDtrup_ *dtrup);
    int CSdtrupRead06 (csFILE *oldStrm,struct csDtrup_ *dtrup);
    extern char cs_Dir[];
}


//Function which works like CS_dtrd(), except that it reads version
//5 datum structs.
//
static int
CS_dtrd05(
    csFILE *oldStrm,
    struct cs_Dtdef05_ *dt_def,
    int *crypt)
{
    csDtrup_ dtrup;
    int nStatus = CSdtrupRead05(oldStrm, &dtrup);
    if (nStatus > 0)
    {
        //success!
        *crypt = dtrup.crypt;
        memcpy(dt_def, &dtrup.u.dtdef05, sizeof(cs_Dtdef05_));
    }
    return nStatus;
}



//Function which works like CS_dtrd(), except that it reads version
//6 datum structs.
//
static int
CS_dtrd06(
    csFILE *oldStrm,
    struct cs_Dtdef06_ *dt_def,
    int *crypt)
{
    csDtrup_ dtrup;
    int nStatus = CSdtrupRead06(oldStrm, &dtrup);
    if (nStatus > 0)
    {
        //success!
        *crypt = dtrup.crypt;
        memcpy(dt_def, &dtrup.u.dtdef06, sizeof(cs_Dtdef06_));
    }
    return nStatus;
}


//Function which returns whether the specified "magic number" is
//a valid one for a Mentor datum dictionary.  The returned value
//indicates the highest access level allowed:  CsDictionaryOpenMode::Write if current
//Mentor version, CsDictionaryOpenMode::Read for old versions, CsDictionaryOpenMode::Closed if invalid.
//
static CsDictionaryOpenMode
DatumValidMagic(long lMagic)
{
    switch (lMagic)
    {
    case cs_DTDEF_MAGIC05:
    case cs_DTDEF_MAGIC06:
    case cs_DTDEF_MAGIC07:
        return Read;
    case cs_DTDEF_MAGIC:
        return Write;
    default:
        return Closed;
    }
}


//Function which returns the Mentor level of a dictionary, given
//a specified magic number.  Returns zero if invalid.
static int
DatumVersion(long lMagic)
{
    switch (lMagic)
    {
    case cs_DTDEF_MAGIC05:
        return 5;
    case cs_DTDEF_MAGIC06:
        return 6;
    case cs_DTDEF_MAGIC07:
        return 7;
    case cs_DTDEF_MAGIC:
        return 8;
    default:
        return 0;
    }
}


//Function which sets up parameters of a Mentor struct as a function
//of its version number.
//
void
GetDatumSizeInfo(
    long lMagic,
    UINT32& nSize,
    UINT32& nNameSize)
{
    //The following locals are pointless and unused, except that I have
    //to declare them to be able to use sizeof() as desired.
    cs_Dtdef05_ v5;
    cs_Dtdef06_ v6;
    cs_Dtdef_ v8;

    switch (lMagic)
    {
    case cs_DTDEF_MAGIC05:
        nSize = sizeof(v5);
        nNameSize = sizeof(v5.key_nm);
        return;
    case cs_DTDEF_MAGIC06:
        nSize = sizeof(v6);
        nNameSize = sizeof(v6.key_nm);
        return;
    case cs_DTDEF_MAGIC07:
    case cs_DTDEF_MAGIC:
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
    v5.key_nm[0] = v6.key_nm[0] = v8.key_nm[0] = '\0';
}

//Local utility functions for comparing various versions of Mentor structs.
//See mentorutil.h for details.
DECLARE_MENTOR_COMPARISON_FUNCTION(DtCompareV5, cs_Dtdef05_, Mentor6Strnicmp)
DECLARE_MENTOR_COMPARISON_FUNCTION(DtCompareV6, cs_Dtdef06_, Mentor6Strnicmp)
DECLARE_MENTOR_COMPARISON_FUNCTION(DtCompareV8, cs_Dtdef_, _strnicmp)//_tcsnicmp)

//Function which gets a comparison function pointer according
//to version number.
static TCmpFunc
DtComparisonFunction(long lMagic)
{
    switch (lMagic)
    {
    case cs_DTDEF_MAGIC05:
        return DtCompareV5;
    case cs_DTDEF_MAGIC06:
        return DtCompareV6;
    case cs_DTDEF_MAGIC07:
    case cs_DTDEF_MAGIC:
        return DtCompareV8;
    default:
        assert(0);
        return NULL;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemDatumDictionary::CCoordinateSystemDatumDictionary(MgCoordinateSystemCatalog *pCatalog)
    : m_pmapSystemNameDescription(NULL),
    m_lMagic(0)
{
    m_pCatalog = SAFE_ADDREF(pCatalog);
    m_sFileName = GetDefaultFileName();
}

CCoordinateSystemDatumDictionary::~CCoordinateSystemDatumDictionary()
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
cs_Dtdef_ * CCoordinateSystemDatumDictionary::dtdef(const char *kpName) const
{
    //If the dictionary version is current, we can just
    //use the regular function.
    if (cs_DTDEF_MAGIC == m_lMagic)
    {
        SmartCriticalClass critical(true);
        return CS_dtdef(kpName);
    }

    //It's an old version.  We need to do a special search
    //in the file, and then, if found, update it to a current struct.
    
    UINT32 nStructSize, nNameSize;
    GetDatumSizeInfo(m_lMagic, nStructSize, nNameSize);
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
        DtComparisonFunction(m_lMagic));
    delete [] pBuf;
    if (nResult != 1)
    {
        //Not found in file.
        return NULL;
    }

    //Okay, we're positioned properly.  Now we're ready to read it in.
    int nLevel = DatumVersion(m_lMagic);

    csDtrup_ csDtrup;

    //Try to read a struct out of the file.
    cs_Dtdef_ *pDef = NULL;
    if (1 == CSdtrupReadOld(pFile, &csDtrup, nLevel))
    {
        //Success!
        pDef = new cs_Dtdef_;
        if (NULL == pDef) return NULL;
        memcpy(pDef, &csDtrup.u.dtdef08, sizeof(cs_Dtdef_));
    }
    CS_fclose(pFile);
    return pDef;
}

//-------------------------------------------------------------------------------
MgCoordinateSystemDatum* CCoordinateSystemDatumDictionary::NewDatum()
{
    Ptr<CCoordinateSystemDatum> pNewDef;

    MG_TRY()

    pNewDef = new CCoordinateSystemDatum(m_pCatalog);

    if (NULL == pNewDef.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDatumDictionary.NewDatum", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatumDictionary.NewDatum")

    return pNewDef.Detach();
}

//-----------------------------------------------------------------------------
STRING CCoordinateSystemDatumDictionary::GetDefaultFileName()
{
    return L"Datums.CSD";  // NOXLATE
}

//-----------------------------------------------------------------------------
//Gets the file name of the dictionary file.
STRING CCoordinateSystemDatumDictionary::GetFileName()
{
    return m_sFileName;
}

//-----------------------------------------------------------------------------
void CCoordinateSystemDatumDictionary::SetFileName(CREFSTRING sFileName)
{
    MG_TRY()

    //Make local variables to hold converted strings
    bool bResult = IsValidDictionaryName(sFileName);
    if (!bResult)
    {
        MgStringCollection arguments;
        arguments.Add(sFileName);
        throw new MgFileIoException(L"MgCoordinateSystemDatumDictionary.SetFileName", __LINE__, __WFILE__, &arguments, L"MgInvalidArgumentException", NULL);
    }

    MentorDictionary::SetFileName(
        m_lMagic,
        DatumValidMagic,
        m_pCatalog->GetDictionaryDir(),
        sFileName,
        m_sFileName,
        L"MgCoordinateSystemDatumDictionary.SetFileName");

    //Okay, everybody opened all right, so update Mentor's global
    //variables appropriately.
    char* szDt=Convert_Wide_To_Ascii(sFileName.c_str());
    CriticalClass.Enter();
    CS_dtfnm(szDt);
    CriticalClass.Leave();
    delete[] szDt;

    if (m_pmapSystemNameDescription)
    {
        m_pmapSystemNameDescription->clear();
        delete m_pmapSystemNameDescription;
        m_pmapSystemNameDescription=NULL;
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemDictionary.SetFileName")
}

//------------------------------------------------------------------------
//Gets the path to the dictionary file, if currently open.
STRING CCoordinateSystemDatumDictionary::GetPath()
{
    STRING sPath;
    MG_TRY()
    sPath = m_pCatalog->GetDictionaryDir() + m_sFileName;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatumDictionary.GetPath")
    return sPath;
}

//------------------------------------------------------------------------
//Gets the size of the set (number of defs it currently contains).
UINT32 CCoordinateSystemDatumDictionary::GetSize()
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
    csFILE *pFile=MentorDictionary::Open(m_lMagic, DatumValidMagic, strPath.c_str(), Read);

    //We're open, but no summary is available.  We need to examine
    //the file itself to find out how big it is.
    INT32 pos =  CS_ftell(pFile);
    CS_fseek(pFile, 0, SEEK_END);
    UINT32 ulFileBytes = 1 + CS_ftell(pFile);
    CS_fseek(pFile, pos, SEEK_SET);
    UINT32 nRecSize, nDummy;
    GetDatumSizeInfo(m_lMagic, nRecSize, nDummy);
    nSize=(ulFileBytes - sizeof(m_lMagic)) / nRecSize;
    if (0!=CS_fclose(pFile))
    {
        throw new MgFileIoException(L"MgCoordinateSystemDatumDictionary.GetSize", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDictionaryCloseFailedException", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatumDictionary.GetSize")

    return nSize;
}

//Adds the specified def to the set.
//Throws an exception MgCoordinateSystemMismatchException if the
//def is not the right kind.
//Adds a new datum definition to the set.  The supplied MgCoordinateSystemDatum
//object will be copied into the set, and can therefore be safely disposed
//of once the function returns.
//Throws an exception Duplicate (if there's already a definition in the set with that name),
//a ReadOnly (if the set isn't allowing write access-- for example, if this
//is an interface to a catalog that doesn't have the datum dictionary
//open for write).
void CCoordinateSystemDatumDictionary::Add(MgGuardDisposable *pDefinition)
{
    MG_TRY()
    assert(NULL != pDefinition);
    CHECKARGUMENTNULL(pDefinition, L"MgCoordinateSystemDatumDictionary.Add");

    MgCoordinateSystemDatum* pDatumDef=dynamic_cast<MgCoordinateSystemDatum*>(pDefinition);
    if (!pDatumDef)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemDatumDictionary.Add", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }

    MentorDictionary::UpdateDef<cs_Dtdef_, MgCoordinateSystemDatum>(
        m_pmapSystemNameDescription,
        DtKey,
        DtDesc,
        &MgCoordinateSystemDatum::IsValid,
        CS_dtdef,
        CS_dtupd,
        BuildDtDefFromInterface,
        pDatumDef,
        false);

    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatumDictionary.Add")
}

//Removes the datum definition with the specified name from the
//set.
//Throws an exception (if there is
//no definition with that name in the catalog), ReadOnly (if the set
//isn't allowing write access).
void CCoordinateSystemDatumDictionary::Remove(CREFSTRING sName)
{
    MG_TRY()
    MentorDictionary::RemoveDef<cs_Dtdef_>(
        m_pmapSystemNameDescription,
        CS_dtdef,
        CS_dtdel,
        sName.c_str());
    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatumDictionary.Remove")
}

//Modifies a datum definition in the set.  The existing definition
//of the same name will be replaced with a copy of the def pointed to by
//kpDef.
//Throws an exception (if there is no definition with that name
//in the catalog), a ReadOnly (if the set isn't allowing write access).
void CCoordinateSystemDatumDictionary::Modify(MgGuardDisposable *pDefinition)
{
    MG_TRY()
    assert(NULL != pDefinition);
    CHECKARGUMENTNULL(pDefinition, L"MgCoordinateSystemDatumDictionary.Modify");

    MgCoordinateSystemDatum* pDatumDef=dynamic_cast<MgCoordinateSystemDatum*>(pDefinition);
    if (!pDatumDef)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemDatumDictionary.Modify", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }

    MentorDictionary::UpdateDef<cs_Dtdef_, MgCoordinateSystemDatum>(
        m_pmapSystemNameDescription,
        DtKey,
        DtDesc,
        &MgCoordinateSystemDatum::IsValid,
        CS_dtdef,
        CS_dtupd,
        BuildDtDefFromInterface,
        pDatumDef,
        true);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatumDictionary.Modify")
}

//This function looks for a datum definition in the set with
//the specified name and, if found, creates an MgCoordinateSystemDatum and
//returns it (user is responsible for freeing the def via Release()
//function).
//Throws an exception (if no such definition exists in the catalog).
//Delegates the call to the ::GetDatum(CREFSTRING) method
MgGuardDisposable* CCoordinateSystemDatumDictionary::Get(CREFSTRING sName)
{
    return GetDatum(sName);
}

//--------------------------------------------------------------
//Reads a CsMap definition from the dictionaries and then creates an MgCoordinateSystemDatum
//out of it - this method is the more expensive one as it interacts with CsMap and
//the dictionary files; if you've already read all ellipsoid information
//you may want to consider using the overloaded [GetDatum(std::*)] method instead
MgCoordinateSystemDatum* CCoordinateSystemDatumDictionary::GetDatum(CREFSTRING sName)
{
    cs_Dtdef_ *pDef = NULL;
    char *pName = NULL;

    Ptr<MgCoordinateSystemDatum> pDatum;

    MG_TRY()

    //Get the name to search for
    pName = Convert_Wide_To_UTF8(sName.c_str()); //need to delete [] pName

    //Look in the dictionary
    pDef = dtdef(pName);

    if (NULL == pDef)
    {
        //wasn't there!
        MgStringCollection whatArguments;
        whatArguments.Add(sName);
        throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemDatumDictionary.GetDatum", __LINE__, __WFILE__, &whatArguments, L"", NULL);
    }

    //Okay, at this point we have a Mentor coordsys definition
    //struct.  Build an object out of it.
    pDatum = this->GetDatum(pDef, NULL /* we don't have a std::map available that contains all ellipsoids*/);

    MG_CATCH(L"MgCoordinateSystemDatumDictionary.GetDatum")

    delete [] pName;
    if (NULL != pDef)
    {
        CS_free(pDef);
        pDef = NULL;
    }

    MG_THROW()

    return pDatum.Detach();
}

//--------------------------------------------------------------
//Method that creates an [MgCoordinateSystemDatum] from a CsMap [cs_Dtdef_] struct
//and (optionally) a std::map of ellipsoids the caller knows of. Passing this
//information in, will prevent the [InitFromCatalog] method from making too many calls
//into the File-IO methods of CsMap. [ellipsoidInfos] can be NULL - the necessary information is then
//read from via CsMap
MgCoordinateSystemDatum* CCoordinateSystemDatumDictionary::GetDatum(const cs_Dtdef_* pDef,
                                                                    const std::vector<std::map<STRING,Ptr<MgDisposable> >*>* const ellipsoidInfos)
{
    Ptr<CCoordinateSystemDatum> pDatum = static_cast<CCoordinateSystemDatum*>(NewDatum());
    
    assert(NULL == ellipsoidInfos || 1 == ellipsoidInfos->size());
    if (NULL != ellipsoidInfos && 1 == ellipsoidInfos->size())
    {
        pDatum->InitFromCatalog(*pDef, ellipsoidInfos->at(0));
    }
    else
    {
        pDatum->InitFromCatalog(*pDef);
    }

    return pDatum.Detach();
}

//--------------------------------------------------------------
//Returns whether the set contains a def with the specified name.
bool CCoordinateSystemDatumDictionary::Has(CREFSTRING sName)
{
    bool bHas=false;

    MG_TRY()

    //Get the name to search for
    char *pName = Convert_Wide_To_UTF8(sName.c_str()); //need to delete [] pName
    if (NULL == pName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDatumDictionary.Has", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (NULL != m_pmapSystemNameDescription)
    {
        CSystemNameDescriptionMap::const_iterator it = m_pmapSystemNameDescription->find(CSystemName(pName));
        bHas = (it != m_pmapSystemNameDescription->end());
    }
    else
    {

        //Look in the dictionary
        cs_Dtdef_ *pDef = dtdef(pName);
        if (NULL != pDef)
        {
            bHas = true;
            CS_free(pDef);
        }
    }
    delete [] pName;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatumDictionary.Has")
    return bHas;
}

//--------------------------------------------------------------
//Gets an enumerator for all the defs in the set.
MgCoordinateSystemEnum* CCoordinateSystemDatumDictionary::GetEnum()
{
    MgCoordinateSystemEnum* pEnum=NULL;

    MG_TRY()

    //Make an enumerator object
    CCoordinateSystemEnumDatum *pNew = GetEnumImp();
    assert(pNew);
    pEnum=static_cast<MgCoordinateSystemEnum*>(pNew);
    assert(pEnum);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatumDictionary.GetEnum")
    return pEnum;
}

//--------------------------------------------------------------
CCoordinateSystemEnumDatum* CCoordinateSystemDatumDictionary::GetEnumImp()
{
    Ptr<CCoordinateSystemEnumDatum> pNew;
    csFILE *pFile=NULL;
    SmartCriticalClass critical(true);

    //use to create an optimized enum that returns *all* available definitions in a block
    //this does only work for the latest versions of the dictionaries
    bool setReadAllDatumsCallback = false;

    MG_TRY()

    STRING strPath=GetPath();
    pFile=MentorDictionary::Open(m_lMagic, DatumValidMagic, strPath.c_str(), Read);

    const int nVersion = DatumVersion(m_lMagic);
    assert(nVersion > 0);
    setReadAllDatumsCallback = (7 == nVersion || 8 == nVersion);
    
    //Generate a summary list, if we don't already have one
    if (NULL == m_pmapSystemNameDescription)
    {
        switch (nVersion)
        {
        case 5:
            //Generate summary for version 5 datum file.
            m_pmapSystemNameDescription = MentorDictionary::GenerateSystemNameDescriptionMap<cs_Dtdef05_>(
                pFile,
                DtKey05,
                DtDesc05,
                CS_dtrd05);
            break;
        case 6:
            //Generate summary for version 6 datum file.
            m_pmapSystemNameDescription = MentorDictionary::GenerateSystemNameDescriptionMap<cs_Dtdef06_>(
                pFile,
                DtKey06,
                DtDesc06,
                CS_dtrd06);
            break;
        case 7:
        case 8:
            //Generate summary for version 7 or 8 datum file.

            //close the file before calling into the [read all CS-Map defs] method
            if (0 != CS_fclose(pFile))
                throw new MgFileIoException(L"MgCoordinateSystemDictionary.GetEnum", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDictionaryCloseFailedException", NULL);

            pFile = NULL;

            m_pmapSystemNameDescription = MentorDictionary::GenerateSystemNameDescriptionMap<cs_Dtdef_>(
                DtKey,
                DtDesc,
                CS_dtdefAll);
            break;
        default:
            assert(0);
        }
        if (NULL == m_pmapSystemNameDescription)
        {
            throw new MgInvalidArgumentException(L"MgCoordinateSystemDatumDictionary.GetEnum", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }

    //Make an enumerator object
    pNew = new CCoordinateSystemEnumDatum;

    if (NULL == pNew.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDatumDictionary.GetEnum", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH(L"MgCoordinateSystemDatumDictionary.GetEnum")

    if (pFile)
    {
        const bool fileClosed = (0 == CS_fclose(pFile));
        if (!fileClosed && NULL == mgException) //throw this exception in case no other exception has already been caught
            mgException = new MgFileIoException(L"MgCoordinateSystemDatumDictionary.GetEnum", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDictionaryCloseFailedException", NULL);

        pFile = NULL;
    }

    MG_THROW()

    //Set it up to use our list
    pNew->Initialize(this, m_pmapSystemNameDescription);
    
    if (setReadAllDatumsCallback) //if we're working on the latest dictionaries, allow read-as-block, if so required by the caller in the ::Next(ULONG) method
        pNew->SetReadAllDefinitionCallback(CCoordinateSystemDatumDictionary::ReadAllDatums);

    return pNew.Detach();
}

//--------------------------------------------------------------
//Reads all datums in one run - opens the dictionary file and then scans the whole file
//all definitions found are stored in the [definitionsFound] collection that the caller will
//have to release - incl. its content (what is done automatically, when disposing the collection itself)
MgDisposableCollection* CCoordinateSystemDatumDictionary::ReadAllDatums(/*IN, required*/MgCoordinateSystemDictionaryBase* targetDictionary,
                                                                        /*IN, optional*/const std::vector<MgCoordinateSystemFilter*>* const filters)
{
    CHECKARGUMENTNULL(targetDictionary, L"CCoordinateSystemDatumDictionary.ReadAllDatums");

    CCoordinateSystemDatumDictionary* datumDictionary = dynamic_cast<CCoordinateSystemDatumDictionary*>(targetDictionary);
    if (NULL == datumDictionary) //everything else is an implementation defect
        throw new MgInvalidArgumentException(L"CCoordinateSystemDatumDictionary.ReadAllDatums", __LINE__, __WFILE__, NULL, L"", NULL);

    //place a lock here - we don't want any interference; what we need is the *current* status of all dictionary files
    SmartCriticalClass dictionaryLock;

    Ptr<MgCoordinateSystemCatalog> catalog = targetDictionary->GetCatalog();
    Ptr<MgCoordinateSystemEllipsoidDictionary> ellipsoidDictionary = catalog->GetEllipsoidDictionary();

    //read all ellipsoids
    map<STRING, Ptr<MgDisposable> > ellipsoidMap;
    MentorDictionary::ReadAllDefinitions<MgCoordinateSystemEllipsoid>(ellipsoidDictionary, &MgCoordinateSystemEllipsoid::GetElCode, ellipsoidMap);

    //place the std::maps in a vector which later be passed to the [GetDatum] method
    std::vector<std::map<STRING,Ptr<MgDisposable> >*> ellipsoidInfos;
    ellipsoidInfos.push_back(&ellipsoidMap);

    //finally, read all "root" coordinate system definitions from the dictionary
    return MentorDictionary::ReadAllDefinitions<MgCoordinateSystemDatum, cs_Dtdef_, CCoordinateSystemDatumDictionary>(
        datumDictionary,
        CS_dtdefAll, 
        NULL, //no additional processing
        &CCoordinateSystemDatumDictionary::GetDatum,
        &ellipsoidInfos,
        filters);
}

//MgDisposable
void CCoordinateSystemDatumDictionary::Dispose()
{
    delete this;
}

//*****************************************************************************
MgCoordinateSystemCatalog* CCoordinateSystemDatumDictionary::GetCatalog()
{
    return SAFE_ADDREF(m_pCatalog.p);
}
