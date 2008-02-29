//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
#include "CoordSysDictionary.h"             //for CCoordinateSystemDictionary
#include "CoordSysDatumDictionary.h"        //for CCoordinateSystemDatumDictionary
#include "CoordSysEllipsoidDictionary.h"    //for CCoordinateSystemEllipsoidDictionary
#include "CoordSysCategoryDictionary.h"     //for CCoordinateSystemCategoryDictionary
#include "CoordSysCatalog.h"                //for CCoordinateSystemCatalog

#include "CoordSysCategoryCollection.h"
#include "CoordSysCategory.h"
#include "CoordSysInformation.h"

using namespace CSLibrary;

//See CsCategoryDictionary.h for discussion of the purpose of this class and
//the format of the category dictionary file.

typedef std::pair<CCategoryName, long> CCategoryNameIndexPair;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemCategoryDictionary::CCoordinateSystemCategoryDictionary(MgCoordinateSystemCatalog *pCatalog)
{
    m_pCatalog = SAFE_ADDREF(pCatalog);
}

//Destructor.  Closes the dictionary, if open.
CCoordinateSystemCategoryDictionary::~CCoordinateSystemCategoryDictionary()
{
    m_sPath = L"";
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

//-----------------------------------------------------------------------------
void CCoordinateSystemCategoryDictionary::Rename(CREFSTRING sOldName, CREFSTRING sNewName)
{
    throw new MgNotImplementedException(L"CCoordinateSystemCategoryDictionary.Rename", __LINE__, __WFILE__, NULL, L"", NULL);
}

//-----------------------------------------------------------------------------
STRING CCoordinateSystemCategoryDictionary::GetDefaultFileName()
{
    throw new MgNotImplementedException(L"CCoordinateSystemCategoryDictionary.GetDefaultFileName", __LINE__, __WFILE__, NULL, L"", NULL);
}

STRING CCoordinateSystemCategoryDictionary::GetFileName()
{
    throw new MgNotImplementedException(L"CCoordinateSystemCategoryDictionary.GetFileName", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystemCategoryDictionary::SetFileName(CREFSTRING sFileName)
{
    throw new MgNotImplementedException(L"CCoordinateSystemCategoryDictionary.SetFileName", __LINE__, __WFILE__, NULL, L"", NULL);
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

    CCoordinateSystemCatalog* pCatalog = dynamic_cast<CCoordinateSystemCatalog*>(m_pCatalog.p);
    if(NULL != pCatalog)
    {
        CCoordinateSystemCategoryCollection* catCollection = pCatalog->GetCoordinateSystemCategories();
        if(NULL != catCollection)
        {
            nSize = static_cast<UINT32>(catCollection->GetCount());
        }
    }

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
    return GetCategory(sName); 
}

//-----------------------------------------------------------------------------
//Gets the def with the specified name from the set.
MgCoordinateSystemCategory* CCoordinateSystemCategoryDictionary::GetCategory(CREFSTRING sName)
{
    Ptr<CCoordinateSystemCategory> pNew;

    MG_TRY()

    CCoordinateSystemCatalog* pCatalog = dynamic_cast<CCoordinateSystemCatalog*>(m_pCatalog.p);

    if (NULL != pCatalog)
    {
        CCoordinateSystemCategory* category = pCatalog->GetCoordinateSystemCategory(sName);

        if(NULL != category)
        {
            pNew = (CCoordinateSystemCategory*)category->CreateClone();
        }
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemCategoryDictionary.GetCategory")

    return pNew.Detach();
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
    return SAFE_ADDREF(m_pCatalog.p);
}

void CCoordinateSystemCategoryDictionary::Initialize()
{
    CCoordinateSystemCatalog* pCatalog = dynamic_cast<CCoordinateSystemCatalog*>(m_pCatalog.p);
    if(NULL != pCatalog)
    {
        CCoordinateSystemCategoryCollection* categories = pCatalog->GetCoordinateSystemCategories();
        if(NULL != categories)
        {
            size_t size = categories->GetCount();
            for(size_t i=0;i<size;i++)
            {
                CCoordinateSystemCategory* pCategory = categories->GetItem(i);
                if(NULL != pCategory)
                {
                    char* categoryTemp = Convert_Wide_To_Ascii(pCategory->GetName().c_str());
                    CCategoryName category(categoryTemp);
                    delete [] categoryTemp;
                    m_index.insert(CCategoryNameIndexPair(category, 0));
                    m_list.push_back(category);
                }
            }
        }
    }
}
