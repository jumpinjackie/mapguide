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

#include "CoordSysCategory.h"                   //for CCoordinateSystemCategory
#include "CoordSysEnumCoordinateSystemInCategory.h"   //for CCoordinateSystemEnumCoordinateSystemInCategory
#include "CoordSysUtil.h"                       //for Convert_Wide_To_Ascii
#include "MentorUtil.h"                            //for IsLegalMentorName
#include <algorithm>                            //for std::find

using namespace CSLibrary;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemCategory::CCoordinateSystemCategory(MgCoordinateSystemCatalog *pCatalog)
{
    m_pCatalog = SAFE_ADDREF(pCatalog);
}

CCoordinateSystemCategory::~CCoordinateSystemCategory()
{
}

//MgDisposable
void CCoordinateSystemCategory::Dispose()
{
    delete this;
}

//Gets the name of the category definition.
//
char * CCoordinateSystemCategory::Name()
{
    return const_cast<char*>(m_categoryName.Name());
}

//Saves the object to a file.  Purpose of the ulMinSize parameter:
//A category definition is, of necessity, a variable-length record
//in a file, since it can contain an arbitrary number of coordinate
//systems.  Therefore, when written, it stores the number of coordinate
//systems which it contains.  However, suppose you have a file which
//consists of a bunch of category definitions, one after the other,
//and you modify one of them by deleting a single coordinate system
//from it, thereby making it a bit smaller.  In that case, it seems
//inefficient to have to re-write the entire file, compacting it; better
//to simply be slightly disk-space-inefficient, write the individual
//record in situ, and mark it so that the reader knows to skip a bit
//of space.  Thus, a category definition in a disk file actually stores
//two numbers:  how many coordinate systems it actually contains,
//and how much space (in terms of number of coordinate systems) it
//occupies in the disk file.  The second number will be greater than
//or equal to the first.  The first number will always be supplied by
//the category definition itself, but the caller needs to supply the
//second number, which the ulMinSize parameter supplies.
//
//This function returns true for success, false for failure.  If the
//ulMinSize parameter is less than the number of coordinate systems
//in the category, it will be ignored.
//
void CCoordinateSystemCategory::SaveToFstream(csFILE *pFile, UINT32 ulMinSize)
{
    long previousPosition = -1;
    MG_TRY()

    UINT32 ulSize = static_cast<UINT32>(m_listCoordinateSystemNames.size());
    if (ulMinSize < ulSize)
    {
        ulMinSize = ulSize;
    }

    //make sure the file is okay
    if (!pFile || -1==CS_ftell(pFile))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCategory.SaveToFstream", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    if (ferror(pFile))
    {
        throw new MgFileIoException(L"MgCoordinateSystemCategory.SaveToFstream", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    previousPosition = CS_ftell(pFile);    //in case of error

    //We write in this order:
    // 1. Name
    // 2. Size (actual number of coordinate systems)
    // 3. File size (ulMinSize; see description above)
    // 4. The coordinate system names
    // 5. Blank space (if ulMinSize > size)

    //Name.
    CS_fwrite(m_categoryName.Name(), sizeof(char), knMaxCategoryNameLen, pFile);

    //Size.
    CS_fwrite(reinterpret_cast<char *>(&ulSize), sizeof(ulSize), 1, pFile);

    //Size in file.
    CS_fwrite(reinterpret_cast<char *>(&ulMinSize), sizeof(ulMinSize), 1, pFile);

    //Coordinate system names.
    CSystemNameList::const_iterator iter;
    for (iter=m_listCoordinateSystemNames.begin(); iter!=m_listCoordinateSystemNames.end(); iter++)
    {
        CS_fwrite((*iter).Name(), sizeof(char), cs_KEYNM_DEF, pFile);
    }

    //Blank space, if needed
    assert(ulMinSize >= ulSize);
    UINT32 ulDiff = ulMinSize - ulSize;
    if (ulDiff > 0)
    {
        //I could just pad the blank space with nulls,
        //but I anticipate that it may be handy in the
        //future for diagnostic purposes to be able go into
        //the category dictionary with a binary editor and
        //spot where the blanks are.  So I'll pick a distinctive
        //string that's not likely to crop up elsewhere.
        CSystemName dummy(/*NOXLATE*/"fnord");
        for (UINT32 i=0; i<ulDiff; i++)
        {
            CS_fwrite(dummy.Name(), sizeof(char), cs_KEYNM_DEF, pFile);
        }
    }

    //We're done!  Return success.
    MG_CATCH(L"MgCoordinateSystemCategory.SaveToFstream")
    //If we encountered an error, reset to where we started
    if (0!=ferror(pFile) && -1!=previousPosition)
    {
        CS_fseek(pFile, previousPosition, SEEK_SET);
        throw new MgFileIoException(L"MgCoordinateSystemCategory.SaveToFstream", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemInternalException", NULL);
    }
    MG_THROW()
}

//Load the object from a file, returning true for success or false for
//failure.  In case of failure, the object will be cleared to a freshly
//constructed state, and the stream pointer will be positioned where it was
//to start with.  In case of success, the stream pointer will be positioned
//just past the end of the object.
//
//See description under CCoordinateSystemCategory::SaveToFstream() for details.
//
void CCoordinateSystemCategory::LoadFromFstream(csFILE *pFile)
{
    long previousPosition = -1;

    MG_TRY()

    UINT32 ulDiff;
    CSystemName member;

    Clear();

    //make sure the stream is okay
    assert(pFile && CS_ftell(pFile)>=0);
    if (!pFile || -1==CS_ftell(pFile))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCategory.LoadFromFstream", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    if (ferror(pFile))
    {
        throw new MgFileIoException(L"MgCoordinateSystemCategory.LoadFromFstream", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    previousPosition = CS_ftell(pFile);    //in case of error

    //We read in this order:
    // 1. Name
    // 2. Size (actual number of coordinate systems)
    // 3. File size (ulMinSize; see description under SaveToFstream())
    // 4. The coordinate system names
    // 5. Blank space (if ulMinSize > size)

    //Name.
    char tempCharBuffer[knMaxCategoryNameLen] = { '\0' };
    const size_t expectedReadCount = sizeof(tempCharBuffer) / sizeof(char);
    size_t nRead=CS_fread(tempCharBuffer, sizeof(char), expectedReadCount, pFile);
    if (expectedReadCount != nRead)
    {
        _ASSERT(0 == nRead); //otherwise something else is going on here...

        //we reached the end of the file
        throw new MgFileIoException(L"MgCoordinateSystemCategory.LoadFromFstream", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //copy the category name into our [m_categoryName] TNameStruct
    m_categoryName = tempCharBuffer;

    //Size.
    UINT32 ulSize;
    nRead=CS_fread(reinterpret_cast<char *>(&ulSize), sizeof(ulSize), 1, pFile);
    if (1!=nRead)
    {
        throw new MgFileIoException(L"MgCoordinateSystemCategory.LoadFromFstream", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Size in file.
    UINT32 ulMinSize;
    nRead=CS_fread(reinterpret_cast<char *>(&ulMinSize), sizeof(ulMinSize), 1, pFile);
    if (1!=nRead)
    {
        throw new MgFileIoException(L"MgCoordinateSystemCategory.LoadFromFstream", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    assert(ulMinSize >= ulSize);
    if (ulMinSize < ulSize)
    {
        throw new MgFileIoException(L"MgCoordinateSystemCategory.LoadFromFstream", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    char keyNameBuffer[cs_KEYNM_DEF]  = { '\0' };
    const size_t expectedBufferCountRead = sizeof(keyNameBuffer) / sizeof(char);
    //Coordinate system names.
    for (UINT32 i=0; i<ulSize; i++)
    {
        keyNameBuffer[0] = '\0';
        nRead=CS_fread(keyNameBuffer, sizeof(char), expectedBufferCountRead, pFile);
        if (expectedBufferCountRead != nRead)
        {
            throw new MgFileIoException(L"MgCoordinateSystemCategory.LoadFromFstream", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        member = keyNameBuffer;

        // TODO - WORKAROUND TO SKIP BAD COORDINATE SYSTEMS IN CURRENT DICTIONARIES
        if((strcmp(member.Name(), "IGN63/Hiva") != 0) &&
           (strcmp(member.Name(), "Phoenix") != 0))
        {
            m_listCoordinateSystemNames.push_back(member);
        }
    }

    //Blank space, if needed
    assert(ulMinSize >= ulSize);
    ulDiff = ulMinSize - ulSize;
    if (ulDiff > 0)
    {
        CS_fseek(pFile, CS_ftell(pFile) + ulDiff * (sizeof(keyNameBuffer) / sizeof(char)), SEEK_SET);
    }
    if (ferror(pFile))
    {
        throw new MgFileIoException(L"MgCoordinateSystemCategory.LoadFromFstream", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //We're done!  Return success.
    MG_CATCH(L"MgCoordinateSystemCategory.LoadFromFstream")
    //If we encountered an error, reset to where we started
    if (ferror(pFile) && -1!=previousPosition)
    {
        CS_fseek(pFile, previousPosition, SEEK_SET);
        Clear();
        throw new MgFileIoException(L"MgCoordinateSystemCategory.LoadFromFstream", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemInternalException", NULL);
    }
    MG_THROW()
}

//Make self into a copy of another category def.  Note that the other
//def need only support MgCoordinateSystemCategory-- it doesn't necessarily have
//to be a CCoordinateSystemCategory.
//
void CCoordinateSystemCategory::CopyFrom(MgCoordinateSystemCategory *pDef)
{
    MG_TRY()

    assert(NULL != pDef);
    if (!pDef)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemCategory.CopyFrom", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Clear();

    //Copy the name
    STRING str = pDef->GetName();
    SetName(str);

    //Get an enumerator
    Ptr<MgCoordinateSystemEnum> pEnum=pDef->GetEnum();
    if (!pEnum)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategory.CopyFrom", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Add the coordinate systems
    for ( ; ; )
    {
        Ptr<MgStringCollection> pCsNameColl=pEnum->NextName(1);
        if (1 != pCsNameColl->GetCount()) break;
        AddCoordinateSystem(pCsNameColl->GetItem(0));
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemCategory.CopyFrom")
}

//Gets the name of the def.
STRING CCoordinateSystemCategory::GetName()
{
    STRING sName;

    MG_TRY()
    wchar_t *pName = Convert_Ascii_To_Wide(m_categoryName.Name());
    if (!pName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategory.GetName", __LINE__, __WFILE__, NULL, L"", NULL);
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

    char *pName = Convert_Wide_To_Ascii(sName.c_str());    //need to delete [] pName
    if (!pName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategory.SetName", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    
    //assign the name to our internal [TNameStruct]
    m_categoryName = pName;

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
    return IsLegalName(m_categoryName.Name());
}

//Private member function which returns whether the specified string
//is a legal category name or not.
//
bool CCoordinateSystemCategory::IsLegalName(const char *kpName)
{
    //Null pointers are illegal
    if (NULL == kpName)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemCategory.IsLegalName", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //String can't be empty or too long
    INT32 nLen = static_cast<UINT32>(strlen(kpName));
    return ((nLen > 0) && (nLen < knMaxCategoryNameLen));
}

//Tests the specified string to find out whether it's a legal
//name for this def.
//
bool CCoordinateSystemCategory::IsLegalName(CREFSTRING sName)
{
    return (sName.length() > 0) && (sName.length() < knMaxCategoryNameLen);
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
        throw new MgNullArgumentException(L"MgCoordinateSystemCategory.IsSameAs", __LINE__, __WFILE__, NULL, L"", NULL);
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
    }    //for each coordsys name in the category

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

    //Copy the list of coordinate systems
    pNew->m_listCoordinateSystemNames = m_listCoordinateSystemNames;

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

//-------------------------------------------------------------------------------
MgStringCollection* CCoordinateSystemCategory::GetCoordinateSystems()
{
    Ptr<MgStringCollection> pCsColl;
    MG_TRY()

    pCsColl=new MgStringCollection;
    if (NULL == pCsColl)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategory.GetCoordinateSystems", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    CSystemNameList::const_iterator iter;
    for (iter=m_listCoordinateSystemNames.begin(); iter!=m_listCoordinateSystemNames.end(); iter++)
    {
        wchar_t *pName = Convert_Ascii_To_Wide((*iter).Name());    //need to delete [] pName
        if (NULL == pName)
        {
            throw new MgOutOfMemoryException(L"MgCoordinateSystemCategory.GetCoordinateSystems", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        pCsColl->Add(pName);
        delete[] pName;
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
    MG_TRY()
    //Make sure it's a legal name
    char *pName = Convert_Wide_To_Ascii(sName.c_str());    //need to delete [] pName
    if (NULL == pName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategory.AddCoordinateSystem", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    if (!IsLegalMentorName(pName))
    {
        delete [] pName;
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCategory.AddCoordinateSystem", __LINE__, __WFILE__, NULL, L"", NULL);
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
    char *pName = Convert_Wide_To_Ascii(sName.c_str());    //need to delete [] pName
    if (NULL == pName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategory.RemoveCoordinateSystem", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    if (!IsLegalMentorName(pName))
    {
        delete [] pName;
        throw new MgInvalidArgumentException(L"MgCoordinateSystemCategory.RemoveCoordinateSystem", __LINE__, __WFILE__, NULL, L"", NULL);
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
    char *pName = Convert_Wide_To_Ascii(sName.c_str());    //need to delete [] pName
    if (NULL == pName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCategory.HasCoordinateSystem", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    if (!IsLegalMentorName(pName))
    {
        delete [] pName;
        return false; //the name is not valid so the category doesn't have it. No assertion since the name might be ok for something else than this coordsys library
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

//Clears the object to a freshly constructed state.
//
void CCoordinateSystemCategory::Clear()
{
    m_categoryName = "\0";
    m_listCoordinateSystemNames.clear();
}

//*****************************************************************************
MgCoordinateSystemCatalog* CCoordinateSystemCategory::GetCatalog()
{
    return SAFE_ADDREF(m_pCatalog.p);
}

//End of file.
