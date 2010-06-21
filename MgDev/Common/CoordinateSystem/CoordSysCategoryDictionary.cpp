//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#include "CoordSysCategory.h"                   //for CCoordinateSystemCategory
#include "CoordSysUtil.h"                       //for Convert_Wide_To_Ascii, CsDictionaryOpenMode
#include "CoordSysCategoryDictionary.h"         //for CCoordinateSystemCategoryDictionary
#include "CoordSysEnumCategory.h"               //for CCoordinateSystemEnumCategory

#include "MentorUtil.h"                            //for OpenDictionaryFile()
#include "MentorDictionary.h"
#include "cs_map.h"

#ifdef _WIN32
#include <tchar.h>                          //for _tsplitpath
#endif

using namespace CSLibrary;

//See CsCategoryDictionary.h for discussion of the purpose of this class and
//the format of the category dictionary file.

typedef std::pair<CCategoryName, long> CCategoryNameIndexPair;

//Utility function which returns whether a category dictionary file is valid,
//as a function of its magic number.
//
static CsDictionaryOpenMode ValidCategoryMagic(long lMagic)
{
    if (klCategoryMagic == lMagic)
    {
        return Write;
    }
    else
    {
        return Closed;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemCategoryDictionary::CCoordinateSystemCategoryDictionary(MgCoordinateSystemCatalog *pCatalog)
    : m_bIndexDirty(true)
{
    m_pCatalog = SAFE_ADDREF(pCatalog);
    m_sFileName = GetDefaultFileName();
}

//Destructor.  Closes the dictionary, if open.
CCoordinateSystemCategoryDictionary::~CCoordinateSystemCategoryDictionary()
{
    m_index.clear();
    m_list.clear();
}

MgCoordinateSystemCategory* CCoordinateSystemCategoryDictionary::NewCategory()
{
    Ptr<CCoordinateSystemCategory> pNewDef;

    MG_TRY()

    pNewDef = new CCoordinateSystemCategory(m_pCatalog);

    if (NULL == pNewDef.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategoryDictionary.NewCategory", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemCategoryDictionary.NewCategory")

    return pNewDef.Detach();
}

CCategoryNameIndexMap& CCoordinateSystemCategoryDictionary::Index()
{
    csFILE* pFile=NULL;

    MG_TRY()
    if (m_bIndexDirty)
    {
        //if dirty it needs to be regenerated
        SmartCriticalClass critical(true);
        pFile=Open(Read);
        GenerateIndex(pFile);
    }
    MG_CATCH(L"MgCoordinateSystemCategoryDictionary.Index")
    if (pFile)
    {
        CS_fclose(pFile);
    }
    MG_THROW()

    return m_index;
}

CCategoryNameList& CCoordinateSystemCategoryDictionary::List()
{
    csFILE* pFile=NULL;

    MG_TRY()
    if (m_bIndexDirty)
    {
        //if dirty it needs to be regenerated
        SmartCriticalClass critical(true);
        pFile=Open(Read);
        GenerateIndex(pFile);
    }
    MG_CATCH(L"MgCoordinateSystemCategoryDictionary.Index")
    if (pFile)
    {
        CS_fclose(pFile);
    }
    MG_THROW()

    return m_list;
}

//Private member function which generates an index from the specified
//file, which is assumed to point to an open category dictionary file.
//Returns true for success, false for failure.
//
void CCoordinateSystemCategoryDictionary::GenerateIndex(csFILE *pFile)
{
    long previousPos = -1;
    MG_TRY()

    assert(pFile && CS_ftell(pFile)>=0);
    previousPos = CS_ftell(pFile);
    m_index.clear();
    m_list.clear();

    //Position the stream to the start of the first record
    CS_fseek(pFile, sizeof(klCategoryMagic), SEEK_SET);
    if (ferror(pFile))
    {
        throw new MgFileIoException(L"MgCoordinateSystemCategoryDictionary.GenerateIndex", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Start reading records.
    CCoordinateSystemCategory def(m_pCatalog);
    long curPos=0;
    while (!feof(pFile))
    {
        curPos = CS_ftell(pFile);
        try
        {
            def.LoadFromFstream(pFile);
        }
        catch (MgException *pELoad)
        {
            SAFE_RELEASE(pELoad);
            if (feof(pFile)) break;
        }
        CCategoryName category(def.Name());
        m_index.insert(CCategoryNameIndexPair(category, curPos));
        m_list.push_back(category);
    }    //for each def in the file

    //Success!
    m_bIndexDirty=false;
    MG_CATCH(L"MgCoordinateSystemCategoryDictionary.GenerateIndex")
    if (mgException != NULL)
    {
        m_index.clear();
    }
    if (-1!=previousPos)
    {
        CS_fseek(pFile, previousPos, SEEK_SET);
    }
    MG_THROW()
}


//Private member function.  Rewrites the file.  If pDef is NULL,
//the def specified by kpDefName will simply be omitted; if not NULL,
//it will be replaced by the def provided.
//
//This function is provided to avoid duplicating code between
//ModifyCategory() and RemoveCategory().
//
void CCoordinateSystemCategoryDictionary::RewriteFile(const char *kpDefName, CCoordinateSystemCategory *pDef)
{
    csFILE *pFile=NULL;
    CriticalClass.Enter();
    MG_TRY()
    assert(NULL != kpDefName);
    bool bReplace = (NULL != pDef);
    STRING tempName;
    STRING strPath=GetPath();

#ifdef _WIN32
    //Get a temporary file name
    wchar_t szDrive[_MAX_DRIVE],
         szDir[_MAX_DIR],
         szFname[_MAX_FNAME],
         szExt[_MAX_EXT],
         szPath[_MAX_PATH],
         szTempPath[_MAX_PATH];
    _tsplitpath(strPath.c_str(), szDrive, szDir, szFname, szExt);
    _tmakepath(szPath, szDrive, szDir, NULL, NULL);

    if (!GetTempFileName(szPath, /*NOXLATE*/L"cat", 0, szTempPath))
    {
        //couldn't open file
        throw new MgFileIoException(L"MgCoordinateSystemCategoryDictionary.RewriteFile", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoCategoryDictionaryException", NULL);
    }
    tempName=szTempPath;
#else
    tempName = MgFileUtil::GenerateTempFileName();
#endif

    //Open the file
    csFILE *pTemp;
    char szMode[10];
    GetFileModes(Write, szMode);
    pTemp = fopen( MgUtil::WideCharToMultiByte(tempName).c_str(), szMode );
    if (!pTemp || ferror(pTemp))
    {
        //couldn't open file
        throw new MgFileIoException(L"MgCoordinateSystemCategoryDictionary.RewriteFile", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoCategoryDictionaryException", NULL);
    }

    //Copy the magic number
    INT32 lMagic;
    GetFileModes(Read, szMode);
    pFile=OpenDictionaryFile(strPath.c_str(), szMode, lMagic, ValidCategoryMagic);
    CS_fwrite(reinterpret_cast<char *>(&lMagic), sizeof(lMagic), 1, pTemp);

    //Copy the category definitions across
    CCoordinateSystemCategory curDef(m_pCatalog);
    CCoordinateSystemCategory *pDefToWrite;
    bool bFound = false;
    bool bEndOfFileReached=false;
    while (!feof(pFile))
    {
        //Get def from original file
        try
        {
            curDef.LoadFromFstream(pFile);
        }
        catch (MgException *pELoad)
        {
            SAFE_RELEASE(pELoad);
            if (feof(pFile)) break;
            CS_fclose(pTemp);
            CS_fclose(pFile);
            pFile=NULL;
            throw new MgFileIoException(L"MgCoordinateSystemCategoryDictionary.RewriteFile", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //Is it the one we're replacing?
        pDefToWrite = &curDef;
        if ((!bFound) && (0 == CS_stricmp(kpDefName, curDef.Name())))
        {
            bFound = true;
            pDefToWrite = pDef;
        }

        if (NULL != pDefToWrite)
        {
            //Write appropriate def to output file
            bool bOk=true;
            try
            {
                pDefToWrite->SaveToFstream(pTemp);
            }
            catch (MgException *pESave)
            {
                SAFE_RELEASE(pESave);
                bOk=false;
            }
            if (!bOk || ferror(pTemp))
            {
                CS_fclose(pFile);
                pFile=NULL;
                CS_fclose(pTemp);
                throw new MgFileIoException(L"MgCoordinateSystemCategoryDictionary.RewriteFile", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
    }    //until we reach the end of the original file

    //Okay, we're done.  Switch 'em around.
    CS_fclose(pFile);
    pFile=NULL;
    CS_fclose(pTemp);
    char* sz_msPath=Convert_Wide_To_Ascii(strPath.c_str());
    if (!sz_msPath)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategoryDictionary.RewriteFile", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MgFileUtil::DeleteFile( MgUtil::MultiByteToWideChar(sz_msPath) );
    MgFileUtil::RenameFile( tempName, MgUtil::MultiByteToWideChar(sz_msPath) );

    GetFileModes(Read, szMode);
    pFile=fopen(sz_msPath, szMode);
    delete[] sz_msPath;

    //Since the contents of the file presumably changed,
    //we need to re-generate the index.
    GenerateIndex(pFile);

    //Return success.
    MG_CATCH(L"MgCoordinateSystemCategoryDictionary.RewriteFile")
    if (pFile)
    {
        CS_fclose(pFile);
        pFile=NULL;
    }
    CriticalClass.Leave();
    MG_THROW();
}

//-----------------------------------------------------------------------------
csFILE* CCoordinateSystemCategoryDictionary::Open(CsDictionaryOpenMode nOpenMode)
{
    csFILE* pFile=NULL;
    MG_TRY()

    //Make sure they've specified a valid mode
    bool bWriting;
    switch (nOpenMode)
    {
    case Read:
        bWriting = false;
        break;
    case Write:
        bWriting = true;
        break;
    default:
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCategoryDictionary.Open", __LINE__, __WFILE__, NULL, L"", NULL);
        break;
    }

    //Validate the existence of the file
    EFileValidity reason;
    STRING strPath=GetPath();
    if (!ValidateFile(
        strPath.c_str(),    //the file to validate
        true,        //must already exist
        false,        //mustn't be a directory,
        bWriting,    //whether we need write access
        &reason))    //reason for failure
    {
        switch (reason)
        {
        case kFileInvalidEmptyString:
            throw new MgInvalidArgumentException(L"MgCoordinateSystemCategoryDictionary.Open", __LINE__, __WFILE__, NULL, L"", NULL);
            break;
        case kFileInvalidPath:
        case kFileInvalidDoesNotExist:
        case kFileInvalidIsADir:
            {
            MgStringCollection arguments;
            arguments.Add(strPath);
            throw new MgFileNotFoundException(L"MgCoordinateSystemCategoryDictionary.Open", __LINE__, __WFILE__, &arguments, L"", NULL);
            }
            break;
        case kFileInvalidCantWrite:
            MgStringCollection arguments;
            arguments.Add(strPath);
            throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemCategoryDictionary.Open", __LINE__, __WFILE__, &arguments, L"MgCoordinateSystemDictionaryReadOnlyException", NULL);
            break;
        }
    }

    //Try to open the dictionary file
    char szMode[10];
    GetFileModes(nOpenMode, szMode);
    INT32 lDummy;
    if (NULL==(pFile=OpenDictionaryFile(strPath.c_str(), szMode, lDummy, ValidCategoryMagic)))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCategoryDictionary.Open", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //And we're done!  Return success.
    MG_CATCH_AND_THROW(L"MgCoordinateSystemCategoryDictionary.Open")
    return pFile;
}

//-----------------------------------------------------------------------------
STRING CCoordinateSystemCategoryDictionary::GetDefaultFileName()
{
    return L"Category.CSD";  // NOXLATE
}

//-----------------------------------------------------------------------------
//Gets the file name of the dictionary file.
STRING CCoordinateSystemCategoryDictionary::GetFileName()
{
    return m_sFileName;
}

//-----------------------------------------------------------------------------
void CCoordinateSystemCategoryDictionary::SetFileName(CREFSTRING sFileName)
{
    MG_TRY()

    //Make local variables to hold converted strings
    bool bResult = IsValidDictionaryName(sFileName);
    if (!bResult)
    {
        MgStringCollection arguments;
        arguments.Add(sFileName);
        throw new MgFileIoException(L"MgCoordinateSystemDictionary.SetFileName", __LINE__, __WFILE__, &arguments, L"MgInvalidArgumentException", NULL);
    }

    if (!sFileName.empty())
    {
        STRING sPath = m_pCatalog->GetDictionaryDir() + sFileName;

        //Validate the existence of the file
        EFileValidity reason;
        if (!ValidateFile(
            sPath.c_str(),  //the file to validate
            true,           //must already exist
            false,          //mustn't be a directory,
            false,          //whether we need write access
            &reason))       //reason for failure
        {
            ThrowFileError(L"MgCoordinateSystemDictionary.SetFileName", sPath, reason);
        }
    }//if dictionary is specified

    //set the index flag to dirty so that it will be generated
    //automatically the next time we need it
    m_bIndexDirty=true;
    m_sFileName=sFileName;

    MG_CATCH_AND_THROW(L"MgCoordinateSystemDictionary.SetFileName")
}

//-----------------------------------------------------------------------------
//Gets the path to the dictionary file, if currently open.
STRING CCoordinateSystemCategoryDictionary::GetPath()
{
    STRING sPath;
    MG_TRY()
    sPath = m_pCatalog->GetDictionaryDir() + m_sFileName;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemDictionary.GetPath")
    return sPath;
}

//-----------------------------------------------------------------------------
//Gets the size of the set (number of defs it currently contains).
UINT32 CCoordinateSystemCategoryDictionary::GetSize()
{
    UINT32 nSize=0;

    MG_TRY()
    //Return the size.
    nSize=static_cast<UINT32>(Index().size());
    MG_CATCH_AND_THROW(L"MgCoordinateSystemCategoryDictionary.GetSize")

    return nSize;
}

//-----------------------------------------------------------------------------
//Adds the specified def to the set.
//Throws an exception MgCoordinateSystemMismatchException if the
//def is not the right kind.  Otherwise, works like AddCategory().
void CCoordinateSystemCategoryDictionary::Add(MgGuardDisposable *pDefinition)
{
    csFILE* pFile=NULL;
    SmartCriticalClass critical(false);
    MG_TRY()
    assert(NULL != pDefinition);
    if (NULL == pDefinition)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemCategoryDictionary.Add", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgCoordinateSystemCategory* pCategoryDef=dynamic_cast<MgCoordinateSystemCategory*>(pDefinition);
    if (!pCategoryDef)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCategoryDictionary.Add", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }

    //Look for it in our index
    STRING str = pCategoryDef->GetName();
    if (str.empty())
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCategoryDictionary.Add", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    char *pName = Convert_Wide_To_Ascii(str.c_str());    //need to delete [] pName
    if (NULL == pName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategoryDictionary.Add", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    CCategoryNameIndexMap::const_iterator iter = Index().find(CCategoryName(pName));
    delete [] pName;

    //Did we find it?
    if (iter != Index().end())
    {
        //Yup.
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCategoryDictionary.Add", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDuplicateException", NULL);
    }

    //Is it a valid def?
    bool bValid = pCategoryDef->IsValid();
    if (!bValid)
    {
        //Invalid def!
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCategoryDictionary.Add", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Build a CCoordinateSystemCategory out of it.
    CCoordinateSystemCategory def(m_pCatalog);
    def.CopyFrom(pCategoryDef);

    //Try to add it to the end of the list.
    critical.Enter();
    pFile=Open(Write);
    CS_fseek(pFile, 0, SEEK_END);
    long pos = CS_ftell(pFile);
    def.SaveToFstream(pFile);

    //Add it to our index
    CCategoryName category(def.Name());
    Index().insert(CCategoryNameIndexPair(category, pos));
    List().push_back(category);

    //Success!
    MG_CATCH(L"MgCoordinateSystemCategoryDictionary.Add")
    if (pFile)
    {
        CS_fclose(pFile);
    }
    MG_THROW()
}

//-----------------------------------------------------------------------------
//Removes the specified def from the set.
void CCoordinateSystemCategoryDictionary::Remove(CREFSTRING sName)
{
    char *pName = NULL;
    MG_TRY()

    //Look for it in our index
    pName = Convert_Wide_To_Ascii(sName.c_str());    //need to delete [] pName
    if (NULL == pName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategoryDictionary.Remove", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    CCategoryNameIndexMap::const_iterator iter = Index().find(CCategoryName(pName));

    //Did we find it?
    if (iter == Index().end())
    {
        MgStringCollection arguments;
        arguments.Add(sName);
        throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemCategoryDictionary.Remove", __LINE__, __WFILE__, &arguments, L"MgCoordinateSystemNotFoundException", NULL);
    }

    //Rewrite the file, skipping the specified category.
    //Doing so will re-generate the index as needed,
    //which will invalidate iter.
    RewriteFile(pName);

    MG_CATCH(L"MgCoordinateSystemCategoryDictionary.Remove")
    delete [] pName;
    MG_THROW()
}

//-----------------------------------------------------------------------------
//Modifies the specified def in the set.
////Throws an exception MgCoordinateSystemMismatchException if the
//def is not the right kind.
void CCoordinateSystemCategoryDictionary::Modify(MgGuardDisposable *pDefinition)
{
    CCoordinateSystemCategory def(m_pCatalog);
    csFILE* pFile=NULL;
    SmartCriticalClass critical(false);

    MG_TRY()
    assert(NULL != pDefinition);
    if (NULL == pDefinition)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemCategoryDictionary.Modify", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgCoordinateSystemCategory* pCategoryDef=dynamic_cast<MgCoordinateSystemCategory*>(pDefinition);
    if (!pCategoryDef)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCategoryDictionary.Modify", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }

    //Look for it in our index
    STRING str = pCategoryDef->GetName();
    if (str.empty())
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCategoryDictionary.Modify", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    char *pName = Convert_Wide_To_Ascii(str.c_str());    //need to delete [] pName
    if (NULL == pName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategoryDictionary.Modify", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    CCategoryNameIndexMap::const_iterator iter = Index().find(CCategoryName(pName));
    delete [] pName;

    //Did we find it?
    if (iter == Index().end())
    {
        MgStringCollection arguments;
        arguments.Add(str);
        throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemCategoryDictionary.Modify", __LINE__, __WFILE__, &arguments, L"MgCoordinateSystemNotFoundException", NULL);
    }

    //Make a CCoordinateSystemCategory object out of what we're given
    def.CopyFrom(pCategoryDef);

    //If the new definition is smaller than or equal to the existing
    //definition's space in the file, we can do a simple in-place
    //replacement in the file.  Otherwise, we'll need to re-write the
    //whole file to make room.
    //
    //Find out how big the existing def is in the file.
    critical.Enter();
    pFile=Open(Write);
    long pos = (*iter).second;
    UINT32 ulFileSize;
    CS_fseek(pFile, pos + knMaxCategoryNameLen + sizeof(UINT32), SEEK_SET);
    size_t nRead=CS_fread(reinterpret_cast<char *>(&ulFileSize), sizeof(ulFileSize), 1, pFile);
    if (1!=nRead)
    {
        throw new MgFileIoException(L"MgCoordinateSystemCategoryDictionary.Modify", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Can we do in-place replacement?
    UINT32 ulSize = def.GetSize();
    if (ulSize <= ulFileSize)
    {
        //Yep.
        CS_fseek(pFile, pos, SEEK_SET);
        def.SaveToFstream(pFile, ulFileSize);
        //Success!  We're done.
        CS_fclose(pFile);
        return;
    }

    MG_CATCH(L"MgCoordinateSystemCategoryDictionary.Modify")
    if (pFile)
    {
        CS_fclose(pFile);
    }
    //No, we can't do in-place replacement.  We have to re-write the file.
    RewriteFile(def.Name(), &def);
    MG_THROW()
}

//-----------------------------------------------------------------------------
//Gets the def with the specified name from the set.
MgGuardDisposable* CCoordinateSystemCategoryDictionary::Get(CREFSTRING sName)
{
    return GetCategory(sName);
}

MgCoordinateSystemCategory* CCoordinateSystemCategoryDictionary::GetCategory(CREFSTRING sName)
{
    Ptr<CCoordinateSystemCategory> pNew;
    csFILE* pFile=NULL;
    char *pName = NULL;
    SmartCriticalClass critical(false);
    MG_TRY()

    //Look for it in our index
    pName = Convert_Wide_To_Ascii(sName.c_str());    //need to delete [] pName

    if (NULL == pName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategoryDictionary.GetCategory", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    CCategoryNameIndexMap::const_iterator iter = Index().find(CCategoryName(pName));

    //Did we find it?
    if (iter == Index().end())
    {
        //Nope.
        MgStringCollection arguments;
        arguments.Add(sName);
        throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemCategoryDictionary.GetCategory", __LINE__, __WFILE__, &arguments, L"MgCoordinateSystemNotFoundException", NULL);
    }

    //Make a new object
    pNew = new CCoordinateSystemCategory(m_pCatalog);

    if (NULL == pNew.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategoryDictionary.GetCategory", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Get the definition out of the file.
    critical.Enter();
    pFile=Open(Read);
    long pos = (*iter).second;
    CS_fseek(pFile, pos, SEEK_SET);
    pNew->LoadFromFstream(pFile);

    MG_CATCH(L"MgCoordinateSystemCategoryDictionary.GetCategory")

    if (pFile)
    {
        CS_fclose(pFile);
    }

    delete [] pName;

    MG_THROW()

    return pNew.Detach();
}

//Returns whether the set contains a def with the specified name.
bool CCoordinateSystemCategoryDictionary::Has(CREFSTRING sName)
{
    bool bHas=false;

    MG_TRY()

    //Look in our index
    char *pName = Convert_Wide_To_Ascii(sName.c_str());    //need to delete [] pName
    if (NULL == pName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategoryDictionary.Has", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    CCategoryNameIndexMap::const_iterator iter = Index().find(CCategoryName(pName));
    delete [] pName;
    bHas=(iter != Index().end());
    MG_CATCH_AND_THROW(L"MgCoordinateSystemCategoryDictionary.Has")

    return bHas;
}

//-----------------------------------------------------------------------------
//Gets an enumerator for all the defs in the set.
MgCoordinateSystemEnum* CCoordinateSystemCategoryDictionary::GetEnum()
{
    MgCoordinateSystemEnum* pEnum=NULL;

    MG_TRY()

    //Make an enumerator object
    CCoordinateSystemEnumCategory *pNewEnum=new CCoordinateSystemEnumCategory;
    //Set it up to use our list
    //const_cast because "this" will not be modified in the process
    MgCoordinateSystemCategoryDictionary* pSet=dynamic_cast<MgCoordinateSystemCategoryDictionary*>(this);
    assert(pSet);
    pNewEnum->Initialize(pSet, &List());

    //Now that we have the new object set up, QI it to get the
    //proper interface pointer.  This also takes care of reference
    //counting so that Release() will properly free the new object.
    pEnum=static_cast<MgCoordinateSystemEnum*>(pNewEnum);
    assert(pEnum);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemCategoryDictionary.GetEnum")
    return pEnum;
}

//-----------------------------------------------------------------------------
//MgDisposable
void CCoordinateSystemCategoryDictionary::Dispose()
{
    delete this;
}

typedef std::pair<CCategoryName, long> CCategoryNameIndexPair;

//-----------------------------------------------------------------------------
void CCoordinateSystemCategoryDictionary::Rename(CREFSTRING sOldName, CREFSTRING sNewName)
{
    csFILE* pFile=NULL;

    MG_TRY()

    //Make sure new name is a valid one
    CCoordinateSystemCategory def(this->m_pCatalog);
    bool bLegal = def.IsLegalName(sNewName);
    if (!bLegal)
    {
        MgStringCollection whatArguments;
        whatArguments.Add(sNewName);
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCategoryDictionary.Rename", __LINE__, __WFILE__, &whatArguments, L"", NULL);
    }

    //Make sure def with old name is present
    char *pName = Convert_Wide_To_Ascii(sOldName.c_str());
    CCategoryName oldCategory(pName);
    delete [] pName;
    CCategoryNameIndexMap::iterator iter = Index().find(oldCategory);
    if (iter == Index().end())
    {
        //nope, not there
        MgStringCollection arguments;
        arguments.Add(sOldName);
        throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemCategoryDictionary.Rename", __LINE__, __WFILE__, &arguments, L"MgCoordinateSystemNotFoundException", NULL);
    }

    //Make sure def with new name is *not* present
    bool bHas = Has(sNewName);
    if (bHas)
    {
        //there's already a def with that name
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCategoryDictionary.Rename", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDuplicateException", NULL);
    }

    INT32 lMagic;
    char szMode[10];
    GetFileModes(Write, szMode);
    STRING strPath=GetPath();
    pFile=OpenDictionaryFile(strPath.c_str(), szMode, lMagic, ValidCategoryMagic);

    //load our previous def, with the old name
    long pos = (*iter).second;
    CS_fseek(pFile, pos, SEEK_SET);
    def.LoadFromFstream(pFile);

    //Change its name
    def.SetName(sNewName);

    //Find out how big the existing def is in the file.
    UINT32 ulFileSize;
    CS_fseek(pFile, pos + knMaxCategoryNameLen + sizeof(UINT32), SEEK_SET);
    size_t nRead=CS_fread(reinterpret_cast<char *>(&ulFileSize), sizeof(ulFileSize), 1, pFile);
    if (1!=nRead)
    {
        throw new MgFileIoException(L"MgCoordinateSystemCategoryDictionary.Rename", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Overwrite the def
    CS_fseek(pFile, pos, SEEK_SET);
    clearerr(pFile);
    def.SaveToFstream(pFile, ulFileSize);

    //Update our index as appropriate.
    pName = Convert_Wide_To_Ascii(sNewName.c_str());
    CCategoryName newCategory(pName);
    delete [] pName;
    try
    {
        Index().erase(iter);
        Index().insert(CCategoryNameIndexPair(newCategory, pos));
    }
    catch (std::bad_alloc)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategoryDictionary.Rename", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    CCategoryNameList::iterator iterList;
    for (iterList = List().begin(); iterList != List().end(); iterList++)
    {
        if (oldCategory == *iterList)
        {
            *iterList = newCategory;
            break;
        }
    }

    //Success!
    MG_CATCH(L"MgCoordinateSystemCategoryDictionary.rename")
    if (pFile)
    {
        CS_fclose(pFile);
        pFile=NULL;
    }
    MG_THROW()
}

//*****************************************************************************
MgCoordinateSystemCatalog* CCoordinateSystemCategoryDictionary::GetCatalog()
{
    return SAFE_ADDREF(m_pCatalog.p);
}
