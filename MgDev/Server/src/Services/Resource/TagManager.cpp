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

#include "ResourceServiceDefs.h"
#include "TagManager.h"
#include "CryptographyUtil.h"
#include "UnmanagedDataManager.h"

static const STRING MgTagReservedCharacterSet = L"\t\n";
static const int MgTagMaxLength = 1024;

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgTagManager::MgTagManager()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object based on the specified tags.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgTagManager::MgTagManager(CREFSTRING tags)
{
    ParseTags(tags);
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgTagManager::~MgTagManager()
{
    m_tagMap.clear();
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks whether or not the specified data name is a reserved tag.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

bool MgTagManager::IsReservedTag(CREFSTRING name)
{
    return (STRING::npos != name.find_first_of(L'%'));
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks whether or not the specified tag string is valid.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgTagManager::CheckTagString(CREFSTRING str)
{
    if (str.length() > MgTagMaxLength)
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(str);

        throw new MgInvalidArgumentException(L"MgTagManager.CheckTagString",
            __LINE__, __WFILE__, &arguments, L"MgStringTooLong", NULL);
    }

    if (STRING::npos != str.find_first_of(MgTagReservedCharacterSet))
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(str);

        MgStringCollection whyArguments;
        whyArguments.Add(MgTagReservedCharacterSet);

        throw new MgInvalidArgumentException(L"MgTagManager.CheckTagString",
            __LINE__, __WFILE__, &arguments, L"MgStringContainsReservedCharacters", &whyArguments);
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks whether or not the specified document contains the specified tag.
/// </summary>
///----------------------------------------------------------------------------

void MgTagManager::CheckTagString(const string& doc, CREFSTRING tag)
{
    string str;

    MgUtil::WideCharToMultiByte(tag, str);

    if (string::npos != doc.find(str))
    {
        MgStringCollection arguments;
        arguments.Add(tag);

        throw new MgResourceTagNotFoundException(L"MgTagManager.CheckTagString",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks whether or not the specified document contains any resource tag.
/// </summary>
///----------------------------------------------------------------------------

void MgTagManager::CheckTagStrings(const string& doc)
{
    if (string::npos != doc.find("%MG_"))
    {
        CheckTagString(doc, MgResourceTag::DataFilePath);
        CheckTagString(doc, MgResourceTag::Username);
        CheckTagString(doc, MgResourceTag::Password);
        CheckTagString(doc, MgResourceTag::LoginUsername);
        CheckTagString(doc, MgResourceTag::LoginPassword);
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Validates the specified tag name and type.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgTagManager::ValidateTag(CREFSTRING name, CREFSTRING type)
{
    // TODO: Add MgUtil::CheckWhiteSpace
    MgUtil::CheckBeginEnd(name, MgReservedCharacterSet::WhiteSpace);

    if (wstring::npos != name.find_first_of(MgReservedCharacterSet::Name))
    {
        MgStringCollection arguments;
        arguments.Add(name);

        throw new MgInvalidResourceDataNameException(
            L"MgTagManager.ValidateTag", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    if ((MgResourceDataType::String != type
            && MgResourceDataName::UserCredentials == name)
        || (MgResourceDataType::String == type
                && MgResourceDataName::UserCredentials != name))
    {
        throw new MgInvalidResourceDataTypeException(
            L"MgTagManager.ValidateTag", __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Parses the specified tags.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgTagManager::ParseTags(CREFSTRING tags)
{
    m_tagMap.clear();

    STRING tagName;
    MgTagInfo tagInfo;
    int fieldCount;
    size_t currPos, nextPos;

    fieldCount = -1;
    currPos = nextPos = 0;

    while (STRING::npos != (nextPos = tags.find_first_of(L"\t\n", currPos)))
    {
        if (L'\t' == tags[nextPos])
        {
            if (fieldCount < 0)
            {
                tagName = tags.substr(currPos, nextPos - currPos);
            }
            else if (fieldCount < MAX_RESOURCE_TAG_ATTRIBUTE_NUMBER)
            {
                tagInfo.SetAttribute(
                    static_cast<MgTagInfo::Attribute>(fieldCount),
                    tags.substr(currPos, nextPos - currPos));
            }

            ++fieldCount;
        }
        else // if (L'\n' == tags[nextPos])
        {
            if (fieldCount < 0)
            {
                MgStringCollection arguments;
                arguments.Add(L"1");
                arguments.Add(tags);

                throw new MgInvalidArgumentException(L"MgTagManager.ParseTags",
                    __LINE__, __WFILE__, &arguments, L"MgTagFieldNotFound", NULL);
            }
            else if (fieldCount < MAX_RESOURCE_TAG_ATTRIBUTE_NUMBER)
            {
                tagInfo.SetAttribute(
                    static_cast<MgTagInfo::Attribute>(fieldCount),
                    tags.substr(currPos, nextPos - currPos));
            }

            m_tagMap.insert(MgTagMap::value_type(tagName, tagInfo));
            fieldCount = -1;
        }

        currPos = nextPos + 1;
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the tags as a string.
/// </summary>
///----------------------------------------------------------------------------

STRING MgTagManager::GetTags() const
{
    STRING tags;

    MgTagMap::const_iterator i;

    for (i = m_tagMap.begin(); i != m_tagMap.end(); ++i)
    {
        const MgTagInfo& tagInfo = (*i).second;

        tags += (*i).first;

        for (int i = 0; i < MAX_RESOURCE_TAG_ATTRIBUTE_NUMBER; ++i)
        {
            tags += L'\t';
            tags += tagInfo.GetAttribute(static_cast<MgTagInfo::Attribute>(i));
        }

        tags += L'\n';
    }

    return tags;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Substitutes all the tags.
///
int MgTagManager::SubstituteTags(const MgDataBindingInfo& dataBindingInfo,
    string& doc)
{
    int count = 0;
    MgTagInfo tagInfo;

    if (GetTag(MgResourceTag::DataFilePath, tagInfo, false))
    {
        STRING path = dataBindingInfo.GetResourceDataFilePath();
        path += tagInfo.GetAttribute(MgTagInfo::TokenValue);
        path += L"/";

        count += MgUtil::ReplaceString(
            MgUtil::WideCharToMultiByte(MgResourceTag::DataFilePath),
            MgUtil::WideCharToMultiByte(path),
            doc, -1);
    }

    if (GetTag(MgResourceDataName::UserCredentials, tagInfo, false))
    {
        MG_CRYPTOGRAPHY_TRY()

        MgCryptographyUtil cryptoUtil;
        string username, password;

        cryptoUtil.DecryptCredentials(MgUtil::WideCharToMultiByte(
            tagInfo.GetAttribute(MgTagInfo::TokenValue)), username, password);

        count += MgUtil::ReplaceString(
            MgUtil::WideCharToMultiByte(MgResourceTag::Username),
            username,
            doc, -1);
        count += MgUtil::ReplaceString(
            MgUtil::WideCharToMultiByte(MgResourceTag::Password),
            password,
            doc, -1);

        MG_CRYPTOGRAPHY_CATCH_AND_THROW(L"MgTagManager.SubstituteTags")
    }

    count += MgUtil::ReplaceString(
        MgUtil::WideCharToMultiByte(MgResourceTag::LoginUsername),
        MgUtil::WideCharToMultiByte(dataBindingInfo.GetLoginUsername()),
        doc, -1);
    count += MgUtil::ReplaceString(
        MgUtil::WideCharToMultiByte(MgResourceTag::LoginPassword),
        MgUtil::WideCharToMultiByte(dataBindingInfo.GetLoginPassword()),
        doc, -1);

    if (dataBindingInfo.GetSubstituteUnmanagedDataMappings())
    {
        count += MgUnmanagedDataManager::SubstituteDataPathAliases(doc);
    }

    CheckTagStrings(doc);

    return count;
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the specified tag.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

bool MgTagManager::GetTag(CREFSTRING name, MgTagInfo& tagInfo,
    bool strict) const
{
    MgTagMap::const_iterator i = m_tagMap.find(name);

    if (m_tagMap.end() != i)
    {
        tagInfo = (*i).second;

        return true;
    }
    else if (strict)
    {
        MgStringCollection arguments;
        arguments.Add(name);

        throw new MgResourceDataNotFoundException(L"MgTagManager.GetTag",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    return false;
}

///----------------------------------------------------------------------------
/// <summary>
/// Adds the specified tag if it does not exists, or updates it otherwise.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgTagManager::SetTag(CREFSTRING name, CREFSTRING type, CREFSTRING value,
    CREFSTRING mimeType)
{
    CheckTagString(name);
    CheckTagString(type);
    CheckTagString(value);
    CheckTagString(mimeType);

    MgTagInfo tagInfo;

    if (GetTag(name, tagInfo, false))
    {
        if (tagInfo.GetAttribute(MgTagInfo::StorageType) != type)
        {
            MgStringCollection arguments;
            arguments.Add(name);

            throw new MgDuplicateResourceDataException(L"MgTagManager.SetTag",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        m_tagMap.erase(name);
    }
    else
    {
        tagInfo.SetAttribute(MgTagInfo::StorageType, type);
    }

    tagInfo.SetAttribute(MgTagInfo::TokenValue, value);
    tagInfo.SetAttribute(MgTagInfo::MimeType, mimeType);

    m_tagMap.insert(MgTagMap::value_type(name, tagInfo));
}

///----------------------------------------------------------------------------
/// <summary>
/// Adds the specified tag.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgTagManager::AddTag(CREFSTRING name, const MgTagInfo& tagInfo)
{
    MgTagMap::const_iterator i = m_tagMap.find(name);

    if (m_tagMap.end() != i)
    {
        MgStringCollection arguments;
        arguments.Add(name);

        throw new MgDuplicateResourceDataException(L"MgTagManager.AddTag",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    CheckTagString(name);
    m_tagMap.insert(MgTagMap::value_type(name, tagInfo));
}

///----------------------------------------------------------------------------
/// <summary>
/// Deletes the specified tag.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgTagManager::DeleteTag(CREFSTRING name, MgTagInfo& tagInfo)
{
    GetTag(name, tagInfo);
    m_tagMap.erase(name);
}

///----------------------------------------------------------------------------
/// <summary>
/// Renames the specified tag.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgTagManager::RenameTag(CREFSTRING oldName, CREFSTRING newName)
{
    MgTagInfo tagInfo;

    DeleteTag(oldName, tagInfo);
    AddTag(newName, tagInfo);
}
