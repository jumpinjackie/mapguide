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
#include "CoordSysUtil.h"

#include "CoordSysCategory.h"                //for CCoordinateSystemCategory
#include "CoordSysEnumCoordinateSystemInCategory.h"   //for CCoordinateSystemEnumCoordinateSystemInCategory

#include "CoordSysInformation.h"

using namespace CSLibrary;

CCoordinateSystemCategory::CCoordinateSystemCategory(MgCoordinateSystemCatalog *pCatalog)
{
    m_pCatalog = SAFE_ADDREF(pCatalog);

    m_coordinateSystems = new CoordinateSystemInformationVector();
    if (m_coordinateSystems == NULL)
    {
        STRING message = L"Could not allocate CoordinateSystemInformationVector.";
        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgOutOfMemoryException(L"CCoordinateSystemCategory.CCoordinateSystemCategory", __LINE__, __WFILE__, &arguments, L"", NULL);
    }
}

CCoordinateSystemCategory::~CCoordinateSystemCategory()
{
    if(m_coordinateSystems)
    {
        Clear();
        delete m_coordinateSystems;
        m_coordinateSystems = NULL;
    }
}

//MgDisposable
void CCoordinateSystemCategory::Dispose()
{
    delete this;
}

void CCoordinateSystemCategory::Add(CCoordinateSystemInformation* coordSysInfo)
{
    if (NULL == coordSysInfo)
    {
        throw new MgNullArgumentException(
            L"CCoordinateSystemCategory.Add",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    m_coordinateSystems->push_back(coordSysInfo);
    AddCoordinateSystem(coordSysInfo->m_code);
}

size_t CCoordinateSystemCategory::ContainsCode(CREFSTRING coordSysCode)
{
    size_t index = -1;
    bool bFound = false;

    for(index=0;index<m_coordinateSystems->size();index++)
    {
        CCoordinateSystemInformation* coordSysInfo = m_coordinateSystems->at(index);
        if(_wcsicmp(coordSysInfo->m_code.c_str(), coordSysCode.c_str()) == 0)
        {
            bFound = true;
            break;
        }
    }

    return bFound?index:-1;
}

size_t CCoordinateSystemCategory::ContainsProj4(CREFSTRING proj4Defn)
{
    size_t index = -1;
    bool bFound = false;

    for(index=0;index<m_coordinateSystems->size();index++)
    {
        CCoordinateSystemInformation* coordSysInfo = m_coordinateSystems->at(index);
        if(coordSysInfo->m_proj4Definition == proj4Defn)
        {
            bFound = true;
            break;
        }
    }

    return bFound?index:-1;
}

CoordinateSystemInformationVector* CCoordinateSystemCategory::GetCoordinateSystemsInfo()
{
    return m_coordinateSystems;
}


STRING CCoordinateSystemCategory::GetName()
{
    STRING sName;

    MG_TRY()
    wchar_t *pName = Convert_Ascii_To_Wide(m_categoryName.name);
    if (!pName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategory.GetName", __LINE__, __WFILE__, NULL, L"MgOutOfMemoryException", NULL);
    }

    sName=pName;
    delete[] pName;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemCategory.GetName")

    return sName;
}

//Sets the name of the def.
////Throws an exception MgInvalidArgumentException
//if the specified name is not legal.
void CCoordinateSystemCategory::SetName(CREFSTRING sName)
{
    MG_TRY()
    if (!IsLegalName(sName))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCategory.SetName", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    char *pName = Convert_Wide_To_Ascii(sName.c_str()); //need to delete [] pName
    if (!pName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategory.SetName", __LINE__, __WFILE__, NULL, L"MgOutOfMemoryException", NULL);
    }
    strcpy(m_categoryName.name, pName);
    delete [] pName;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemCategory.SetName")
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
    throw new MgNotImplementedException(L"CCoordinateSystemCategory.IsValid", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Tests the specified string to find out whether it's a legal
//name for this def.
//
bool CCoordinateSystemCategory::IsLegalName(CREFSTRING sName)
{
    return true;
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

    if (NULL == pDef)
    {
        throw new MgNullArgumentException(
            L"MgCoordinateSystemCategory.IsSameAs",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Make sure it's a category def
    MgCoordinateSystemCategory* pCtDef=dynamic_cast<MgCoordinateSystemCategory*>(pDef);
    if (!pCtDef)
    {
        return false;
    }

    //Get enumerators for doing comparison
    Ptr<MgCoordinateSystemEnum> pThis=GetEnum();
    assert(pThis);
    if (!pThis)
    {
        return false;
    }
    pThis->Reset();

    Ptr<MgCoordinateSystemEnum> pThat=pCtDef->GetEnum();
    assert(pThat);
    if (!pThat)
    {
        return false;
    }
    pThat->Reset();

    //Start comparing names.  Even though the enumerators may
    //allow us to grab many names at one go, we'll do them one
    //at a time.  This is to avoid the complication of "what do
    //we do when we ask A for a hundred, and B for a hundred,
    //and they give us different size chunks?"  Processing speed
    //isn't much of an issue here, so we'll keep the code simple.
    for ( ; ; )
    {
        Ptr<MgStringCollection> pCsNameCollThis=pThis->NextName(1);
        Ptr<MgStringCollection> pCsNameCollThat=pThat->NextName(1);
        if ((0 == pCsNameCollThis->GetCount()) && (0 == pCsNameCollThat->GetCount()))
        {
            //We've reached the end of the list without
            //finding any differences.  They're the same.
            return true;
        }
        else if (1==pCsNameCollThis->GetCount() && 1==pCsNameCollThat->GetCount())
        {
            //We got a name from each.  Compare them.
            bool bSameName = (0 == _wcsicmp(pCsNameCollThis->GetItem(0).c_str(), pCsNameCollThat->GetItem(0).c_str()));
            if (!bSameName)
            {
                //Different names!
                return false;
            }
        }
        else
        {
            //The lists are of different length!  Not the same.
            assert(pCsNameCollThis->GetCount() != pCsNameCollThat->GetCount());
            return false;
        }
    }   //for each coordsys name in the category

    MG_CATCH_AND_THROW(L"MgCoordinateSystemCategory.IsSameAs")

    return false;
}

//Creates another def object copied from this one.  Caller is
//responsible for releasing the clone.
//
MgCoordinateSystemCategory* CCoordinateSystemCategory::CreateClone()
{
    Ptr<CCoordinateSystemCategory> pNew;

    MG_TRY()

    //Create a clone object
    pNew = new CCoordinateSystemCategory(m_pCatalog);

    if (NULL == pNew.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategory.CreateClone", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Copy the category name
    pNew->m_categoryName = m_categoryName;

    // Copy the internal structure
    CoordinateSystemInformationVector* coordSystems = GetCoordinateSystemsInfo();
    if (NULL != coordSystems)
    {
        for(size_t i=0;i<coordSystems->size();i++)
        {
            CCoordinateSystemInformation* coordSys = coordSystems->at(i);
            if(NULL != coordSys)
            {
                CCoordinateSystemInformation* newCoordSys = coordSys->Clone();
                if(NULL != newCoordSys)
                {
                    pNew->Add(newCoordSys);
                }
            }
        }
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemCategory.CreateClone")

    //And we're done!  Return success.
    return pNew.Detach();
}

//Gets the number of coordinate systems listed in the category.
//
UINT32 CCoordinateSystemCategory::GetSize()
{
    return static_cast<UINT32>(m_listCoordinateSystemNames.size());
}

//Gets an enumerator for listing the names of all the coordinate
//systems in the category.  The caller is responsible for releasing
//the enumerator when done with it.
//
MgCoordinateSystemEnum* CCoordinateSystemCategory::GetEnum()
{
    CCoordinateSystemEnumCoordinateSystemInCategory* pNameEnum=NULL;
    MG_TRY()
    pNameEnum=new CCoordinateSystemEnumCoordinateSystemInCategory(m_pCatalog);
    if (!pNameEnum)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategory.GetEnum", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    pNameEnum->Initialize(&m_listCoordinateSystemNames);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemCategory.GetEnum")
    return pNameEnum;
}

//Adds the specified coordinate system key name to the category.
////Throws an exception MgInvalidArgumentException (if the specified string is not a legal
//coordinate system key name) or MgCoordinateSystemCategoryDuplicateException (if the specified
//coordinate system name already exists in the category).
//
void CCoordinateSystemCategory::AddCoordinateSystem(CREFSTRING sName)
{
    MG_TRY()
    //Make sure it's a legal name
    char *pName = Convert_Wide_To_Ascii(sName.c_str()); //need to delete [] pName
    if (NULL == pName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategory.AddCoordinateSystem", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Make a new member out of it
    CSystemName newMember(pName);
    delete [] pName;

    //See if it's already present
    CSystemNameList::iterator itList=std::find(m_listCoordinateSystemNames.begin(), m_listCoordinateSystemNames.end(), newMember);
    if (itList != m_listCoordinateSystemNames.end())
    {
        //duplicate!
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCategory.AddCoordinateSystem", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemCategoryDuplicateException", NULL);
    }

    //Try to insert it in our list
    m_listCoordinateSystemNames.push_back(newMember);

    //Success!
    MG_CATCH_AND_THROW(L"MgCoordinateSystemCategory.AddCoordinateSystem")
}

//Removes the specified coordinate system key name from the category.
//Throws an exception MgInvalidArgumentException (if the specified string is not a legal
//coordinate system key name) or throws if there is no such
//coordinate system name in the category.
//
void CCoordinateSystemCategory::RemoveCoordinateSystem(CREFSTRING sName)
{
    MG_TRY()

    //Make sure it's a legal name
    char *pName = Convert_Wide_To_Ascii(sName.c_str()); //need to delete [] pName
    if (NULL == pName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategory.RemoveCoordinateSystem", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Make a member out of it
    CSystemName member(pName);
    delete [] pName;

    CSystemNameList::iterator itList=std::find(m_listCoordinateSystemNames.begin(), m_listCoordinateSystemNames.end(), member);
    if (itList != m_listCoordinateSystemNames.end())
    {
        //Found it!
        m_listCoordinateSystemNames.erase(itList);
    }
    assert(itList != m_listCoordinateSystemNames.end());

    //Return success.
    MG_CATCH_AND_THROW(L"MgCoordinateSystemCategory.RemoveCoordinateSystem")
}

//Tests whether the category contains the specified coordinate system
//key name.
//
bool CCoordinateSystemCategory::HasCoordinateSystem(CREFSTRING sName)
{
    bool bHas=false;

    MG_TRY()
    //Make sure it's a legal name
    char *pName = Convert_Wide_To_Ascii(sName.c_str()); //need to delete [] pName
    if (NULL == pName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategory.HasCoordinateSystem", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Make a member out of it
    CSystemName member(pName);
    delete [] pName;

    //Try to find it
    CSystemNameList::iterator itList=std::find(m_listCoordinateSystemNames.begin(), m_listCoordinateSystemNames.end(), member);
    bHas=(itList != m_listCoordinateSystemNames.end());

    MG_CATCH_AND_THROW(L"MgCoordinateSystemCategory.HasCoordinateSystem")
    return bHas;
}

void CCoordinateSystemCategory::Clear()
{
    for(size_t index=0;index<m_coordinateSystems->size();index++)
    {
        CCoordinateSystemInformation* coordSysInfo = m_coordinateSystems->at(index);
        delete coordSysInfo;
        coordSysInfo = NULL;
    }

    m_coordinateSystems->clear();

    memset(m_categoryName.name, 0, knMaxCategoryNameLen);
    m_listCoordinateSystemNames.clear();
}

MgStringCollection* CCoordinateSystemCategory::GetCoordinateSystems()
{
    // TODO
    return NULL;
}

MgCoordinateSystemCatalog* CCoordinateSystemCategory::GetCatalog()
{
    return SAFE_ADDREF(m_pCatalog.p);
}
