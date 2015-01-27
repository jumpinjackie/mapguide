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

#include "CoordSysCategory.h"                   //for CCoordinateSystemCategory
#include "CoordSysUtil.h"                       //for Convert_Wide_To_UTF8, CsDictionaryOpenMode
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
    _ASSERT(CriticalClass.IsEntered());

    MG_TRY()
    if (m_bIndexDirty)
    {
        //if dirty it needs to be regenerated
        GenerateIndex();
    }
    MG_CATCH(L"MgCoordinateSystemCategoryDictionary.Index")
    MG_THROW()

    return m_index;
}

CCategoryNameList& CCoordinateSystemCategoryDictionary::List()
{
    //if dirty it needs to be regenerated
    SmartCriticalClass critical;

    MG_TRY()

        if (m_bIndexDirty)
        {
            GenerateIndex();
        }

    MG_CATCH(L"MgCoordinateSystemCategoryDictionary.Index")

        if (NULL != mgException)
            this->InvalidateIndex();

    MG_THROW()

    return m_list;
}

void CCoordinateSystemCategoryDictionary::InvalidateIndex()
{
    _ASSERT(CriticalClass.IsEntered());

    CSrlsCategories(); //our index got of synch; make sure, we force CS-Map to release its in-memory list, too
    m_bIndexDirty = true;

    m_index.clear();
    m_list.clear();
}

//Private member function which generates an index from the specified
//file, which is assumed to point to an open category dictionary file.
//Returns true for success, false for failure.
//
void CCoordinateSystemCategoryDictionary::GenerateIndex()
{
    _ASSERT(CriticalClass.IsEntered());

    struct cs_Ctdef_ **pAllCategories = NULL;
    int categoryCount = 0;

    MG_TRY()

        this->InvalidateIndex();

        categoryCount = CSgetCtDefAll(&pAllCategories);
        if (categoryCount < 0)
        {
            _ASSERT(NULL == pAllCategories);
            throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemCategoryDictionary.GenerateIndex", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        for(int i = 0; i < categoryCount; ++i)
        {
            CCategoryName category(pAllCategories[i]->ctName);
            m_index.insert(CCategoryNameIndexPair(category, i));
            m_list.push_back(category);
        }

        //Success!
        m_bIndexDirty = false;

    MG_CATCH(L"MgCoordinateSystemCategoryDictionary.GenerateIndex")

    if (NULL != mgException)
    {
        this->InvalidateIndex();
    }

    //clean up the array we've got from CS-Map, i.e. ...
    for(int i = 0; i < categoryCount; ++i)
    {
        //...delete each individual entry...
        CSrlsCategory(pAllCategories[i]);
        pAllCategories[i] = NULL;
    }

    //...and finally delete the entire array
    CS_free(pAllCategories);
    pAllCategories = NULL;

    MG_THROW()

    return;
}

//-----------------------------------------------------------------------------
STRING CCoordinateSystemCategoryDictionary::GetDefaultFileName()
{
    return WIDEN(cs_CT_NAME);
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
    this->InvalidateIndex();
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

        SmartCriticalClass csmapLock;
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
    SmartCriticalClass lock;
    MG_TRY()

    _ASSERT(NULL != pDefinition);
    CHECKARGUMENTNULL(pDefinition, L"MgCoordinateSystemCategoryDictionary.Add");

    CCoordinateSystemCategory* pCategoryDef=dynamic_cast<CCoordinateSystemCategory*>(pDefinition);
    if (!pCategoryDef)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCategoryDictionary.Add", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }

    cs_Ctdef_ const* pCsMapDef = pCategoryDef->GetCategoryDef();
    CCategoryNameIndexMap::const_iterator iter = Index().find(CCategoryName(pCsMapDef->ctName));

    //Did we find it?
    if (iter != Index().end())
    {
        //Yup.
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCategoryDictionary.Add", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDuplicateException", NULL);
    }

    int updateResult = CS_ctupd(pCsMapDef);
    if (updateResult < 0)
    {
        //the category could not be added to the dictionary
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCategoryDictionary.Add", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    _ASSERT(0 == updateResult); //we must have added it; +1 means "updated"
    if (0 == updateResult) //otherwise an existing one was updated, i.e. our list should be still in synch, theoretically...
    {
        //Add it to our index
        CCategoryName category(pCsMapDef->ctName);
        Index().insert(CCategoryNameIndexPair(category, List().size()));
        List().push_back(category);
    }
    else
    {
        this->InvalidateIndex();
    }

    MG_CATCH(L"MgCoordinateSystemCategoryDictionary.Add")

        if (NULL != mgException)
            this->InvalidateIndex();

    MG_THROW()
}

//-----------------------------------------------------------------------------
//Removes the specified def from the set.
void CCoordinateSystemCategoryDictionary::Remove(CREFSTRING sName)
{
    char *pName = NULL;
    cs_Ctdef_* pCategory = NULL;

    bool bInvalidIndexOnFailure = false;

    SmartCriticalClass critical;
    MG_TRY()

    //Look for it in our index
    pName = Convert_Wide_To_UTF8(sName.c_str());

    CCategoryNameIndexMap::const_iterator iter = Index().find(CCategoryName(pName));

    //Did we find it?
    if (iter == Index().end())
    {
        MgStringCollection arguments;
        arguments.Add(sName);
        throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemCategoryDictionary.Remove", __LINE__, __WFILE__, &arguments, L"MgCoordinateSystemNotFoundException", NULL);
    }

    bInvalidIndexOnFailure = true;

    pCategory = CS_ctdef(pName);
    if (NULL == pCategory)
    {
        MgStringCollection arguments;
        arguments.Add(sName);
        throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemCategoryDictionary.Remove", __LINE__, __WFILE__, &arguments, L"MgCoordinateSystemNotFoundException", NULL);
    }

    int deletionResult = CS_ctdel(pCategory);
    if (deletionResult)
    {
        MgStringCollection arguments;
        arguments.Add(sName);
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemCategoryDictionary.Remove", __LINE__, __WFILE__, &arguments, L"MgCoordinateSystemInternalException", NULL);
    }

    long position = iter->second;
    CCategoryName const& categoryName = List().at(position);
    if (0 != CS_stricmp(categoryName.Name(), pName)) //is the category name at the index what we think it is?
    {
        _ASSERT(false);
        this->InvalidateIndex();
    }
    else
    {
        Index().erase(iter->first);
        List().erase(List().begin() + position);
    }

    MG_CATCH(L"MgCoordinateSystemCategoryDictionary.Remove")

        delete [] pName;
        pName = NULL;

        CSrlsCategory(pCategory);
        pCategory = NULL;

        if (bInvalidIndexOnFailure && NULL != mgException)
            InvalidateIndex();

    MG_THROW()
}

//-----------------------------------------------------------------------------
//Modifies the specified def in the set.
////Throws an exception MgCoordinateSystemMismatchException if the
//def is not the right kind.
void CCoordinateSystemCategoryDictionary::Modify(MgGuardDisposable *pDefinition)
{
    char *pName = NULL;
    cs_Ctdef_* pCategory = NULL;

    bool invalidIndexOnFailure = false;

    SmartCriticalClass csmapLock;
    MG_TRY()

        _ASSERT(NULL != pDefinition);
        CHECKARGUMENTNULL(pDefinition, L"MgCoordinateSystemCategoryDictionary.Modify");

        CCoordinateSystemCategory* pCategoryDef=dynamic_cast<CCoordinateSystemCategory*>(pDefinition);
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
        pName = Convert_Wide_To_UTF8(str.c_str());

        CCategoryNameIndexMap::const_iterator iter = Index().find(CCategoryName(pName));

        //Did we find it?
        if (iter == Index().end())
        {
            MgStringCollection arguments;
            arguments.Add(str);
            throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemCategoryDictionary.Modify", __LINE__, __WFILE__, &arguments, L"MgCoordinateSystemNotFoundException", NULL);
        }

        invalidIndexOnFailure = true;

        cs_Ctdef_ const* pDef = pCategoryDef->GetCategoryDef();
        int updateResult = CS_ctupd(pDef);
        if (updateResult < 0)
        {
            MgStringCollection arguments;
            arguments.Add(str);
            throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemCategoryDictionary.Modify", __LINE__, __WFILE__, &arguments, L"MgCoordinateSystemInternalException", NULL);
        }

        _ASSERT(1 == updateResult);
        if (0 == updateResult)
        {
            //an item was added... something went wrong here; invalidate the index so we re-read it next time
            this->InvalidateIndex();
        }

    MG_CATCH(L"MgCoordinateSystemCategoryDictionary.Modify")

        delete[] pName;
        pName = NULL;

        CSrlsCategory(pCategory);
        pCategory = NULL;

        if(invalidIndexOnFailure && NULL != mgException)
            this->InvalidateIndex();

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
    char *pName = NULL;
    cs_Ctdef_* pCategory = NULL;

    bool invalidIndexOnFailure = false;

    SmartCriticalClass critical;
    MG_TRY()

        //Look for it in our index
        pName = Convert_Wide_To_UTF8(sName.c_str());    //need to delete [] pName
        CCategoryNameIndexMap::const_iterator iter = Index().find(CCategoryName(pName));

        //Did we find it?
        if (iter == Index().end())
        {
            //Nope.
            MgStringCollection arguments;
            arguments.Add(sName);
            throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemCategoryDictionary.GetCategory", __LINE__, __WFILE__, &arguments, L"MgCoordinateSystemNotFoundException", NULL);
        }

        invalidIndexOnFailure = true;

        cs_Ctdef_* pCategory = CS_ctdef(pName);
        if (NULL == pCategory)
        {
            //Nope.
            MgStringCollection arguments;
            arguments.Add(sName);
            throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemCategoryDictionary.GetCategory", __LINE__, __WFILE__, &arguments, L"MgCoordinateSystemNotFoundException", NULL);
        }

        //Make a new object with the [cs_Ctdef_] instance we got
        pNew = new CCoordinateSystemCategory(m_pCatalog, pCategory);
        pCategory = NULL; //is now owned by [pNew]

    MG_CATCH(L"MgCoordinateSystemCategoryDictionary.GetCategory")

        delete [] pName;
        pName = NULL;

        CSrlsCategory(pCategory);
        pCategory = NULL;

        if (NULL != mgException && invalidIndexOnFailure)
            this->InvalidateIndex();

    MG_THROW()

    return pNew.Detach();
}

//Returns whether the set contains a def with the specified name.
bool CCoordinateSystemCategoryDictionary::Has(CREFSTRING sName)
{
    char *pName = NULL;
    bool bHas = false;

    MG_TRY()

        //Look in our index
        pName = Convert_Wide_To_UTF8(sName.c_str());    //need to delete [] pName

        SmartCriticalClass csmapLock;

        CCategoryNameIndexMap::const_iterator iter = Index().find(CCategoryName(pName));
        bHas = (iter != Index().end());

    MG_CATCH(L"MgCoordinateSystemCategoryDictionary.Has")

        delete pName;
        pName = NULL;

    MG_THROW()

    return bHas;
}

//-----------------------------------------------------------------------------
//Gets an enumerator for all the defs in the set.
MgCoordinateSystemEnum* CCoordinateSystemCategoryDictionary::GetEnum()
{
    Ptr<CCoordinateSystemEnumCategory> pNewEnum;

    MG_TRY()

    //Make an enumerator object
    pNewEnum = new CCoordinateSystemEnumCategory;

    //Set it up to use our list
    pNewEnum->Initialize(this, &List());

    MG_CATCH_AND_THROW(L"MgCoordinateSystemCategoryDictionary.GetEnum")

    return pNewEnum.Detach();
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
    char *pOldName = NULL;
    char *pNewName = NULL;
    cs_Ctdef_* pCategory = NULL;

    bool invalidIndexOnFailure = false;
    SmartCriticalClass csmapLock;

    MG_TRY()

        //Make sure def with old name is present
        pOldName = Convert_Wide_To_UTF8(sOldName.c_str());
        CCategoryName oldCategory(pOldName);

        CCategoryNameIndexMap::iterator iter = Index().find(oldCategory);
        if (iter == Index().end())
        {
            //nope, not there
            MgStringCollection arguments;
            arguments.Add(sOldName);
            throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemCategoryDictionary.Rename", __LINE__, __WFILE__, &arguments, L"MgCoordinateSystemNotFoundException", NULL);
        }

        invalidIndexOnFailure = true;
        pCategory = CS_ctdef(pOldName);
        if (NULL == pCategory)
        {
            MgStringCollection arguments;
            arguments.Add(sOldName);
            throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemCategoryDictionary.Rename", __LINE__, __WFILE__, &arguments, L"MgCoordinateSystemNotFoundException", NULL);
        }

        pNewName = Convert_Wide_To_UTF8(sNewName.c_str());
        if (CSrplCatNameEx(pOldName, pNewName))
            throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemCategoryDictionary.Rename", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNotFoundException", NULL);

        if (CSupdCategories(NULL))
            throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemCategoryDictionary.Rename", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNotFoundException", NULL);

        long index = iter->second;
        Index().erase(iter);
        Index().insert(std::pair<CCategoryName,long>(CCategoryName(pNewName), index));

    MG_CATCH(L"MgCoordinateSystemCategoryDictionary.rename")

        delete[] pOldName;
        delete[] pNewName;
        pOldName = NULL;
        pNewName = NULL;

        if (NULL != mgException && invalidIndexOnFailure)
            this->InvalidateIndex();

    MG_THROW()
}

//*****************************************************************************
MgCoordinateSystemCatalog* CCoordinateSystemCategoryDictionary::GetCatalog()
{
    return SAFE_ADDREF(m_pCatalog.p);
}
