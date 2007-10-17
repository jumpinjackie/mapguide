//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "CoordSysCategory.h"                   //for CCoordinateSystemCategory
#include "CoordSysUtil.h"                       //for Convert_Wide_To_Ascii, CsDictionaryOpenMode
#include "ArbitraryCoordsys.h"                  //for CCsArbitraryCoordinateSystemUtil
#include "CoordSysCategoryDictionary.h"         //for CCoordinateSystemCategoryDictionary
#include "CoordSysEnumCategory.h"               //for CCoordinateSystemEnumCategory

using namespace CSLibrary;

//See CsCategoryDictionary.h for discussion of the purpose of this class and
//the format of the category dictionary file.

typedef std::pair<CCategoryName, long> CCategoryNameIndexPair;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemCategoryDictionary::CCoordinateSystemCategoryDictionary(MgCoordinateSystemCatalog *pCatalog)
    : m_pCatalog(pCatalog)
{
    SAFE_ADDREF(m_pCatalog);
}

//Destructor.  Closes the dictionary, if open.
CCoordinateSystemCategoryDictionary::~CCoordinateSystemCategoryDictionary()
{
    m_sPath = L"";
    m_index.clear();
    m_list.clear();
    SAFE_RELEASE(m_pCatalog);
}

MgCoordinateSystemCategory* CCoordinateSystemCategoryDictionary::NewCategory()
{
    CCoordinateSystemCategory* pNewDef=NULL;
    MgCoordinateSystemCategory* pDefinition=NULL;
    MG_TRY()
    pNewDef= new CCoordinateSystemCategory(m_pCatalog);
    pDefinition=dynamic_cast<MgCoordinateSystemCategory*>(pNewDef);
    if (!pDefinition)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategoryDictionary.NewCategory", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH(L"MgCoordinateSystemCategoryDictionary.NewCategory")
    if (mgException != NULL)
    {
        delete pNewDef;
    }
    MG_THROW()

    return pDefinition;
}

//-----------------------------------------------------------------------------
void CCoordinateSystemCategoryDictionary::Rename(CREFSTRING sOldName, CREFSTRING sNewName)
{
    throw new MgNotImplementedException(L"CCoordinateSystemCategoryDictionary.Rename", __LINE__, __WFILE__, NULL, L"", NULL);
}

//-----------------------------------------------------------------------------
void CCoordinateSystemCategoryDictionary::SetPath(CREFSTRING sPath)
{
    throw new MgNotImplementedException(L"CCoordinateSystemCategoryDictionary.SetPath", __LINE__, __WFILE__, NULL, L"", NULL);
}

//-----------------------------------------------------------------------------
//Gets the path to the dictionary file, if currently open.
STRING CCoordinateSystemCategoryDictionary::GetPath()
{
    throw new MgNotImplementedException(L"CCoordinateSystemCategoryDictionary.GetPath", __LINE__, __WFILE__, NULL, L"", NULL);
}

//-----------------------------------------------------------------------------
//Gets the size of the set (number of defs it currently contains).
UINT32 CCoordinateSystemCategoryDictionary::GetSize()
{
    UINT32 nSize=0;

    MG_TRY()
    //Return the size.
    nSize=static_cast<UINT32>(m_index.size());
    MG_CATCH_AND_THROW(L"MgCoordinateSystemCategoryDictionary.GetSize")

    return nSize;
}

//-----------------------------------------------------------------------------
//Adds the specified def to the set.
//Throws an exception MgCoordinateSystemMismatchException if the
//def is not the right kind.  Otherwise, works like AddCategory().
void CCoordinateSystemCategoryDictionary::Add(MgGuardDisposable *pDefinition)
{
    throw new MgNotImplementedException(L"CCoordinateSystemCategoryDictionary.Add", __LINE__, __WFILE__, NULL, L"", NULL);
}

//-----------------------------------------------------------------------------
//Removes the specified def from the set.
void CCoordinateSystemCategoryDictionary::Remove(CREFSTRING sName)
{
    throw new MgNotImplementedException(L"CCoordinateSystemCategoryDictionary.Remove", __LINE__, __WFILE__, NULL, L"", NULL);
}

//-----------------------------------------------------------------------------
//Modifies the specified def in the set.
////Throws an exception MgCoordinateSystemMismatchException if the
//def is not the right kind.
void CCoordinateSystemCategoryDictionary::Modify(MgGuardDisposable *pDefinition)
{
    throw new MgNotImplementedException(L"CCoordinateSystemCategoryDictionary.Modify", __LINE__, __WFILE__, NULL, L"", NULL);
}

//-----------------------------------------------------------------------------
//Gets the def with the specified name from the set.
MgGuardDisposable* CCoordinateSystemCategoryDictionary::Get(CREFSTRING sName)
{
    CCoordinateSystemCategory *pNew = NULL;
    MG_TRY()

    //Look for it in our index
    char *pName = Convert_Wide_To_Ascii(sName.c_str()); //need to delete [] pName
    if (NULL == pName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategoryDictionary.Get", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    CCategoryNameIndexMap::const_iterator iter = m_index.find(CCategoryName(pName));
    delete [] pName;

    //Did we find it?
    if (iter == m_index.end())
    {
        //Nope.
        throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemCategoryDictionary.Get", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Make a new object
    pNew = new CCoordinateSystemCategory(m_pCatalog);
    if (NULL == pNew)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategoryDictionary.Get", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // TODO

    MG_CATCH(L"MgCoordinateSystemCategoryDictionary.Get")
    if (mgException != NULL)
    {
        delete pNew;
        pNew=NULL;
    }
    MG_THROW()

    return pNew;
}

//Returns whether the set contains a def with the specified name.
bool CCoordinateSystemCategoryDictionary::Has(CREFSTRING sName)
{
    throw new MgNotImplementedException(L"CCoordinateSystemCategoryDictionary.Has", __LINE__, __WFILE__, NULL, L"", NULL);
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
    pNewEnum->Initialize(pSet, &m_list);

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

MgCoordinateSystemCatalog* CCoordinateSystemCategoryDictionary::GetCatalog()
{
    return SAFE_ADDREF(m_pCatalog);
}

void CCoordinateSystemCategoryDictionary::Initialize()
{
    // TODO
}
