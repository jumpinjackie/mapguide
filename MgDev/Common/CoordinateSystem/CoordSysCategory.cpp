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

#include <vector>

#include "GeometryCommon.h"
#include "CoordSysCommon.h"

#include "CoordSysCategory.h"                   //for CCoordinateSystemCategory
#include "CoordSysEnumCoordinateSystemInCategory.h"   //for CCoordinateSystemEnumCoordinateSystemInCategory
#include "CoordSysUtil.h"                       //for Convert_Wide_To_UTF8
#include "MentorUtil.h"                            //for IsLegalMentorName
#include <algorithm>                            //for std::find

#include "cs_map.h"

using namespace std;
using namespace CSLibrary;

extern "C"
{
    extern int cs_Error;
}

void CCoordinateSystemCategory::CtorInit(CCoordinateSystemCategory* pToInitialize, MgCoordinateSystemCatalog *pCatalog,
    cs_Ctdef_* pSourceCtDef,
    bool copyDef)
{
    CHECKARGUMENTNULL(pCatalog, L"MgCoordinateSystemCategory.MgCoordinateSystemCategory");

    _ASSERT(NULL != pToInitialize && NULL == pToInitialize->mp_ctDef);

    cs_Ctdef_ *pCategoryCopy = NULL;
    if (NULL != pSourceCtDef)
    {
        if (copyDef)
            pCategoryCopy = CScpyCategory(pSourceCtDef);
        else
            pCategoryCopy = pSourceCtDef;
    }
    else
    {
        pCategoryCopy = CSnewCategory(NULL);
    }

    if (NULL == pCategoryCopy)
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategory.CtorInit", __LINE__, __WFILE__, NULL, L"", NULL);

    pToInitialize->m_pCatalog = SAFE_ADDREF(pCatalog);
    pToInitialize->mp_ctDef = pCategoryCopy;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemCategory::CCoordinateSystemCategory(MgCoordinateSystemCatalog *pCatalog)
    : mp_ctDef(NULL)
{
    CCoordinateSystemCategory::CtorInit(this, pCatalog);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemCategory::CCoordinateSystemCategory(MgCoordinateSystemCatalog *pCatalog, cs_Ctdef_* pCategory)
    : mp_ctDef(NULL)
{
    CCoordinateSystemCategory::CtorInit(this, pCatalog, pCategory, false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemCategory::CCoordinateSystemCategory(CCoordinateSystemCategory const& toCopyFrom)
    : mp_ctDef(NULL)
{
    CCoordinateSystemCategory::CtorInit(this, toCopyFrom.m_pCatalog, toCopyFrom.mp_ctDef);
}

CCoordinateSystemCategory& CCoordinateSystemCategory::operator=(const CCoordinateSystemCategory& other)
{
    this->CopyFrom(&other);
    return *this;
}

CCoordinateSystemCategory::~CCoordinateSystemCategory()
{
    CSrlsCategory(this->mp_ctDef);
    this->mp_ctDef = NULL;
}

//MgDisposable
void CCoordinateSystemCategory::Dispose()
{
    delete this;
}

//Make self into a copy of another category def.  Note that the other
//def need only support MgCoordinateSystemCategory-- it doesn't necessarily have
//to be a CCoordinateSystemCategory.
//
void CCoordinateSystemCategory::CopyFrom(CCoordinateSystemCategory const* pDef)
{
    _ASSERT(NULL != pDef);
    CHECKARGUMENTNULL(pDef, L"MgCoordinateSystemCategory.CopyFrom");

    if (!const_cast<CCoordinateSystemCategory*>(pDef)->IsValid())
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCategory.CopyFrom", __LINE__, __WFILE__, NULL, L"", NULL);

    CCoordinateSystemCategory const* pOtherCategory = dynamic_cast<CCoordinateSystemCategory const*>(pDef);
    if (NULL == pOtherCategory)
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCategory.CopyFrom", __LINE__, __WFILE__, NULL, L"", NULL);

    cs_Ctdef_* pCategoryCopy = CScpyCategory(pOtherCategory->mp_ctDef);
    if (NULL == pCategoryCopy)
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategory.CopyFrom", __LINE__, __WFILE__, NULL, L"", NULL);

    MG_TRY()

        this->Clear();
        this->mp_ctDef = pCategoryCopy;

    MG_CATCH_AND_THROW(L"MgCoordinateSystemCategory.CopyFrom")
}

//Gets the name of the def.
STRING CCoordinateSystemCategory::GetName()
{
    STRING categoryName;
    wchar_t* pszwName = NULL;

    MG_TRY()

        pszwName = Convert_UTF8_To_Wide(this->mp_ctDef->ctName);
        categoryName = pszwName;

    MG_CATCH(L"MgCoordinateSystemCategory.GetName");

    delete[] pszwName;
    pszwName = NULL;

    MG_THROW()

    return categoryName;
}

//Sets the name of the def.
////Throws an exception MgInvalidArgumentException
//if the specified name is not legal.
void CCoordinateSystemCategory::SetName(CREFSTRING sName)
{
    char* pszNewName = NULL;

    MG_TRY()

        if (!IsLegalName(sName))
        {
            throw new MgInvalidArgumentException(L"MgCoordinateSystemCategory.SetName", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        pszNewName = Convert_Wide_To_UTF8(sName.c_str());
        CS_stncp(this->mp_ctDef->ctName, pszNewName, cs_CATDEF_CATNMSZ);

    MG_CATCH(L"MgCoordinateSystemCategory.SetName")

        delete[] pszNewName;
        pszNewName = NULL;

    MG_THROW()
}

//Gets whether the def is "valid."  Validity, in this case, is
//defined in terms of whether a catalog will accept this def
//when attempting to add it.  An example of an invalid def
//would be a coordinate system definition whose numerical
//parameters are mathematically invalid for its projection.
//If this function returns false, then attempting to add the
//def to a catalog will result in failure.
//
//In the specific case of a category definition, its validity
//is determined simply by whether its name is legal (a freshly
//constructed object has an empty name and is therefore not
//valid until the name has been set).
//
bool CCoordinateSystemCategory::IsValid()
{
    return IsLegalName(this->mp_ctDef->ctName);
}

//Private member function which returns whether the specified string
//is a legal category name or not.
//
bool CCoordinateSystemCategory::IsLegalName(const char *kpName)
{
    //Null pointers are illegal
    CHECKARGUMENTNULL(kpName, L"MgCoordinateSystemCategory.IsLegalName");

    //String can't be empty or too long
    INT32 nLen = static_cast<UINT32>(strlen(kpName));
    return ((nLen > 0) && (nLen < cs_CATDEF_CATNMSZ));
}

//Tests the specified string to find out whether it's a legal
//name for this def.
//
bool CCoordinateSystemCategory::IsLegalName(CREFSTRING sName)
{
    return (sName.length() > 0) && (sName.length() < cs_CATDEF_CATNMSZ);
}

//Gets whether the def is usable in the context of the given catalog.
//The IsValid() function, above, simply checks the def for internal
//validity in its own right.  This function, on the other hand,
//checks for other conditions such as unresolved references and so
//forth which can only be evaluated in a larger context.  For
//example, a coordinate system definition may specify that it uses
//datum "FOO", which is a perfectly legitimate datum name, so
//IsValid() would return true; but unless there is in fact a datum
//"FOO" present in the catalog where we try to use the coordinate
//system, it can't be used and IsUsable() would return false.
//Note that this function will always return false if IsValid()
//returns false.
//
//In the specific case of a category definition, it has no dependencies
//on anything other than itself, so IsUsable() simply returns the
//result of IsValid().  (The catalog will not attempt to make sure
//that the coordinate systems listed in the category are in fact
//present in the catalog.)
//
bool CCoordinateSystemCategory::IsUsable(MgCoordinateSystemCatalog *pCatalog)
{
    return IsValid();
}

//Compares the def with another and returns true if they're the
//"same," false otherwise.  The comparison does not consider the name
//(i.e., two defs which have different key names but are otherwise
//identical will return true), but does consider everything else.  If
//the two defs are not of the same type, returns false.
//
bool CCoordinateSystemCategory::IsSameAs(MgGuardDisposable *pDef)
{
    MG_TRY()

    CHECKARGUMENTNULL(pDef, L"MgCoordinateSystemCategory.IsSameAs");

    //Make sure it's a category def
    CCoordinateSystemCategory* pCtDef=dynamic_cast<CCoordinateSystemCategory*>(pDef);
    if (!pCtDef)
    {
        return false;
    }

    //our [mp_ctDef]'s must always be set, we don't delete that; unless we ran into a situation where we deleted
    //the field and then couldn't create a new instance via CSnewCategory
    if (NULL == pCtDef->mp_ctDef)
    {
        _ASSERT(false);
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCategory.IsSameAs", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (NULL == this->mp_ctDef)
    {
        _ASSERT(false);
        throw new MgInvalidOperationException(L"MgCoordinateSystemCategory.IsSameAs", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (this->mp_ctDef->nameCnt != pCtDef->mp_ctDef->nameCnt)
        return false;

    //now check, whether all the CRS names are the same
    for(ulong32_t i = 0; i < this->mp_ctDef->nameCnt; ++i)
    {
        char const* pCrsNameThis = (this->mp_ctDef->csNames + i)->csName;
        char const* pCrsNameOther = (pCtDef->mp_ctDef->csNames + i)->csName;

        if (0 != CS_stricmp(pCrsNameThis, pCrsNameOther))
            return false;
    }

    return true;

    MG_CATCH_AND_THROW(L"MgCoordinateSystemCategory.IsSameAs")

    return false;
}

//Creates another def object copied from this one.  Caller is
//responsible for releasing the clone.
//
MgCoordinateSystemCategory* CCoordinateSystemCategory::CreateClone()
{
    MG_TRY()

        //Create a clone object
        return new CCoordinateSystemCategory(*this);

    MG_CATCH_AND_THROW(L"MgCoordinateSystemCategory.CreateClone")

    _ASSERT(false);
    return NULL;
}

//Gets the number of coordinate systems listed in the category.
//
UINT32 CCoordinateSystemCategory::GetSize()
{
    return static_cast<UINT32>(this->GetAllCsNames().size());
}

//Gets an enumerator for listing the names of all the coordinate
//systems in the category.  The caller is responsible for releasing
//the enumerator when done with it.
//
MgCoordinateSystemEnum* CCoordinateSystemCategory::GetEnum()
{
    Ptr<CCoordinateSystemEnumCoordinateSystemInCategory> pNameEnum;

    MG_TRY()

        pNameEnum=new CCoordinateSystemEnumCoordinateSystemInCategory(m_pCatalog);
        pNameEnum->Initialize(this->GetAllCsNames());

    MG_CATCH_AND_THROW(L"MgCoordinateSystemCategory.GetEnum")

    return pNameEnum.Detach();
}

//-------------------------------------------------------------------------------
std::vector<STRING>& CCoordinateSystemCategory::GetAllCsNames()
{
    MG_TRY()

        if (0 == this->m_listCoordinateSystemNames.size() && this->mp_ctDef->nameCnt > 0)
        {
            for(ulong32_t i = 0; i < this->mp_ctDef->nameCnt; ++i)
            {
                wchar_t *pszwCsName = Convert_UTF8_To_Wide(this->mp_ctDef->csNames[i].csName);
                this->m_listCoordinateSystemNames.push_back(pszwCsName);
                delete[] pszwCsName;
            }
        }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemCategory.GetAllCsNames")

    return this->m_listCoordinateSystemNames;
}

//-------------------------------------------------------------------------------
void CCoordinateSystemCategory::ClearAllCsNames()
{
    this->m_listCoordinateSystemNames.clear();
}

//-------------------------------------------------------------------------------
MgStringCollection* CCoordinateSystemCategory::GetCoordinateSystems()
{
    Ptr<MgStringCollection> pCsColl;

    MG_TRY()
    pCsColl = new MgStringCollection;

    std::vector<STRING> const& allCsNames = this->GetAllCsNames();
    for(size_t i = 0; i < allCsNames.size(); ++i)
    {
        pCsColl->Add(allCsNames[i]);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemCategory.GetCoordinateSystems")
    return pCsColl.Detach();
}

//Adds the specified coordinate system key name to the category.
//Throws an exception MgInvalidArgumentException (if the specified string is not a legal
//coordinate system key name) or MgCoordinateSystemCategoryDuplicateException (if the specified
//coordinate system name already exists in the category).
//
void CCoordinateSystemCategory::AddCoordinateSystem(CREFSTRING sName)
{
    char *pName = NULL;

    MG_TRY()

    std::vector<STRING>& allCsNames = this->GetAllCsNames();

    //Make sure it's a legal name
    pName = Convert_Wide_To_UTF8(sName.c_str());    //need to delete [] pName
    if (NULL == pName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategory.AddCoordinateSystem", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!IsLegalMentorName(pName))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCategory.AddCoordinateSystem", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //See if it's already present
    std::vector<STRING>::iterator const& itList = std::find(allCsNames.begin(), allCsNames.end(), sName);
    if (itList != allCsNames.end())
    {
        //duplicate!
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCategory.AddCoordinateSystem", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemCategoryDuplicateException", NULL);
    }

    if (CSaddItmNameEx(this->mp_ctDef, pName))
    {
        _ASSERT(false); //whatever the problem is here... the name we validated; OOM ? hopefully unlikely
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCategory.AddCoordinateSystem", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemCategoryDuplicateException", NULL);
    }

    //Try to insert it in our list
    allCsNames.push_back(sName);

    //Success!
    MG_CATCH(L"MgCoordinateSystemCategory.AddCoordinateSystem")

    delete [] pName;
    pName = NULL;

    if (NULL != mgException)
        this->ClearAllCsNames();

    MG_THROW();

}

//Removes the specified coordinate system key name from the category.
//Throws an exception MgInvalidArgumentException (if the specified string is not a legal
//coordinate system key name) or throws if there is no such
//coordinate system name in the category.
//
void CCoordinateSystemCategory::RemoveCoordinateSystem(CREFSTRING sName)
{
    char *pName = NULL;

    MG_TRY()

        std::vector<STRING>& allCsNames = this->GetAllCsNames();

        //Make sure it's a legal name
        pName = Convert_Wide_To_UTF8(sName.c_str());    //need to delete [] pName
        _ASSERT(NULL != pName); //would throw an exception otherwise

        if (!IsLegalMentorName(pName))
            throw new MgInvalidArgumentException(L"MgCoordinateSystemCategory.RemoveCoordinateSystem", __LINE__, __WFILE__, NULL, L"", NULL);

        if (CSrmvItmNameEx(this->mp_ctDef, pName))
        {
            if (cs_Error != cs_CT_CS_NOT_IN)
            {
                //the name should be valid... the only expected error here would be
                //that we couldn't find the CS in the category
                _ASSERT(false);
                allCsNames.clear();
            }

            throw new MgInvalidArgumentException(L"MgCoordinateSystemCategory.RemoveCoordinateSystem", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //See if it's already present
        std::vector<STRING>::iterator const& itList = std::find(allCsNames.begin(), allCsNames.end(), sName);
        if (itList != allCsNames.end())
            allCsNames.erase(itList);

    MG_CATCH(L"MgCoordinateSystemCategory.RemoveCoordinateSystem")

    delete [] pName;
    pName = NULL;

    if (NULL != mgException)
        this->ClearAllCsNames();

    MG_THROW()
}

//Tests whether the category contains the specified coordinate system
//key name.
//
bool CCoordinateSystemCategory::HasCoordinateSystem(CREFSTRING sName)
{
    bool bHas = false;
    char *pName = NULL;

    std::vector<STRING>& allCsNames = this->GetAllCsNames();

    MG_TRY()

        //Make sure it's a legal name
        pName = Convert_Wide_To_UTF8(sName.c_str());
        if (IsLegalMentorName(pName))
        {
            //Try to find it
            //See if it's already present
            std::vector<STRING>::iterator const& itList = std::find(allCsNames.begin(), allCsNames.end(), sName);
            bHas = (itList != allCsNames.end());
        }
        //else:
            //the name is not valid so the category doesn't have it. No assertion since the name might be ok for something else than this coordsys library

    MG_CATCH(L"MgCoordinateSystemCategory.HasCoordinateSystem")

    delete[] pName;
    pName = NULL;

    MG_THROW()

    return bHas;
}

cs_Ctdef_ const* CCoordinateSystemCategory::GetCategoryDef() const
{
    return this->mp_ctDef;
}

//Clears the object to a freshly constructed state.
//
void CCoordinateSystemCategory::Clear()
{
    this->ClearAllCsNames();

    //create the new empty cs_Ctdef_ instance first
    struct cs_Ctdef_* pNewCategory = CSnewCategory(NULL);
    if (NULL == pNewCategory)
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategory.MgCoordinateSystemCategory", __LINE__, __WFILE__, NULL, L"", NULL);

    CSrlsCategory(this->mp_ctDef);
    this->mp_ctDef = pNewCategory;
}

//*****************************************************************************
MgCoordinateSystemCatalog* CCoordinateSystemCategory::GetCatalog()
{
    return SAFE_ADDREF(m_pCatalog.p);
}
