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

#include "PlatformBase.h"
#include "ResourceService.h"

MG_IMPL_DYNCREATE(MgResourceIdentifier);

///////////////////////////////////////////////////////////////////////////////
///<summary>
///Initialize resource identifier from a string
///</summary>
///<param name="resource">
///Resource string similar to "Library://Telus/Calgary/Maps/Downtown.map"
///</param>
///EXCEPTIONS
///  MgResourceLocationException
///  MgOutOfMemoryException
///  MgInvalidRepositoryTypeException
MgResourceIdentifier::MgResourceIdentifier(CREFSTRING resource)
{
    SetResource(resource);
}

///////////////////////////////////////////////////////////////////////////////
///<summary>
///Construct an empty identifier
///</summary>
MgResourceIdentifier::MgResourceIdentifier()
{
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs an MgResourceIdentifier object based on the specified
/// repository type/name and resource path/name/type.
/// </summary>
///
/// <exceptions>
/// MgInvalidRepositoryTypeException, MgInvalidRepositoryNameException,
/// MgInvalidResourcePathException, MgInvalidResourceNameException,
/// MgInvalidResourceTypeException, MgOutOfMemoryException
/// </exceptions>
MgResourceIdentifier::MgResourceIdentifier(
    CREFSTRING repositoryType, CREFSTRING repositoryName,
    CREFSTRING resourcePath, CREFSTRING resourceName, CREFSTRING resourceType)
{
    SetRepositoryType(repositoryType);
    SetRepositoryName(repositoryName);
    SetPath(resourcePath);
    SetName(resourceName);
    SetResourceType(resourceType);

    Validate();
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs a new MgResourceIdentifier object and initializes the member
/// variables to values given by a referenced MgResourceIdentifier object.
/// </summary>
/// <param name="resourceIdentifier">
/// Reference to an MgResourceIdentifier object.
/// </param>
/// <exceptions>
/// MgOutOfMemoryException
/// </exceptions>
MgResourceIdentifier::MgResourceIdentifier(
    const MgResourceIdentifier& resourceIdentifier)
{
    *this = resourceIdentifier;
}

///////////////////////////////////////////////////////////////////////////////
///<summary>
///Destruct a MgResourceIdentifier object
///</summary>
MgResourceIdentifier::~MgResourceIdentifier()
{
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Overloaded assignment operator for MgResourceIdentifier.
/// </summary>
/// <param name="resourceIdentifier">
/// Reference to the structure that is to be copied.
/// </param>
/// <return>
/// Reference to the structure (the one assigned to).
/// </return>
/// <exceptions>
/// MgOutOfMemoryException
/// </exceptions>
MgResourceIdentifier& MgResourceIdentifier::operator=(
    const MgResourceIdentifier& resourceIdentifier)
{
    if (&resourceIdentifier != this)
    {
        m_repositoryType = resourceIdentifier.m_repositoryType;
        m_repositoryName = resourceIdentifier.m_repositoryName;
        m_path = resourceIdentifier.m_path;
        m_name = resourceIdentifier.m_name;
        m_type = resourceIdentifier.m_type;
    }

    return *this;
}

///////////////////////////////////////////////////////////////////////////////
///<summary>Dispose this object.</summary>
///<returns>Returns nothing</returns>
void MgResourceIdentifier::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
///<summary>
///Set the resource identifier from a string
///</summary>
///<param name="resource">
///Resource string similar to "Library://Telus/Calgary/Maps/Downtown.map"
///</param>
///EXCEPTIONS
///  MgResourceLocationException
///  MgOutOfMemoryException
///  MgInvalidRepositoryTypeException
void MgResourceIdentifier::SetResource(CREFSTRING resource)
{
    ParseIdentifier(resource);
}

///////////////////////////////////////////////////////////////////////////////
///<summary>
///Return the repository type for the identifier
///</summary>
///<returns>
///Repository type string (Library, Session)
///</returns>
STRING MgResourceIdentifier::GetRepositoryType()
{
    return m_repositoryType;
}

///////////////////////////////////////////////////////////////////////////////
///<summary>
///Set the repository type for the identifier
///</summary>
///<param name="type">
///Repository type string
///</param>
void MgResourceIdentifier::SetRepositoryType(CREFSTRING type)
{
    m_repositoryType = type;
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the repository name from the identifier.  For session
/// repositories, the name is the client id.  For library repositories, an
/// empty string is returned.
/// </summary>
/// <returns>
/// The Repository name.  See summary.
/// </returns>
STRING MgResourceIdentifier::GetRepositoryName()
{
    return m_repositoryName;
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the repository name for the identifier.  Only valid for session
/// resources.
/// </summary>
/// <param name="name">
/// Repository name.
/// </param>
/// <returns>
/// Nothing.
/// <returns>
void MgResourceIdentifier::SetRepositoryName(CREFSTRING name)
{
    m_repositoryName = name;
}

///////////////////////////////////////////////////////////////////////////////
///<summary>
///Convert resource identifier into a fully qualified resource specifier string similar
///to "Library://Telus/Calgary/Maps/Downtown.map"
///</summary>
///<returns>
///Fully qualified resource specifier string
///</returns>
STRING MgResourceIdentifier::ToString()
{
    STRING resource = GetFullPath(true);

    if (!m_name.empty())
    {
        resource += m_name;

        if (MgResourceType::Folder == m_type)
        {
            resource += L"/";
        }
        else if (!m_type.empty())
        {
            resource += L".";
            resource += m_type;
        }
    }

    return resource;
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Determines if the resource is of root type.
/// </summary>
/// <returns>
/// true if the resource is of root type, and false otherwise.
/// </returns>
bool MgResourceIdentifier::IsRoot() const
{
    return (m_name.empty() && m_path.empty() && IsFolder());
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Determines if the resource is of folder type.
/// </summary>
/// <returns>
/// true if the resource is of folder type, and false otherwise.
/// </returns>
bool MgResourceIdentifier::IsFolder() const
{
    return IsResourceTypeOf(MgResourceType::Folder);
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Determines if the resource is of the specified repository type.
/// </summary>
/// <returns>
/// true if the resource is of the specified repository type, false otherwise.
/// </returns>
bool MgResourceIdentifier::IsRepositoryTypeOf(CREFSTRING type) const
{
    return (type == m_repositoryType);
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Determines if the resource is of the specified resource type.
/// </summary>
/// <returns>
/// true if the resource is of the specified resource type, false otherwise.
/// </returns>
bool MgResourceIdentifier::IsResourceTypeOf(CREFSTRING type) const
{
    return (type == m_type);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Determines if the resource is a runtime resource.
///
/// \return
/// true if the resource is a runtime resource, false otherwise.
///
bool MgResourceIdentifier::IsRuntimeResource() const
{
    return (MgResourceType::Map       == m_type
         || MgResourceType::Selection == m_type);
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the root path of the resource.
/// </summary>
/// <returns>
/// Root path of the resource.
/// </returns>
STRING MgResourceIdentifier::GetRootPath() const
{
    assert(!m_repositoryType.empty());
    STRING rootPath;

    rootPath += m_repositoryType;
    rootPath += L":";
    rootPath += m_repositoryName;
    rootPath += L"//";

    return rootPath;
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
///  Returns the full path of the resource.
/// </summary>
/// <param name="appendSlash">
/// Flag specifying whether or not if the full path should be appended with
/// a forward slash.
/// </param>
/// <returns>
///  Full path of the resource.
/// </returns>
STRING MgResourceIdentifier::GetFullPath(bool appendSlash) const
{
    STRING fullPath = GetRootPath();

    if (!m_path.empty())
    {
        fullPath += m_path;

        if (appendSlash)
        {
            fullPath += L"/";
        }
    }

    return fullPath;
}

///////////////////////////////////////////////////////////////////////////////
///<summary>
/// Returns the resource depth.
///</summary>
///<returns>
/// Resource depth.
///</returns>
INT32 MgResourceIdentifier::GetDepth() const
{
    INT32 depth;

    if (IsRoot())
    {
        depth = 0;
    }
    else if (m_path.empty())
    {
        depth = 1;
    }
    else
    {
        depth = 2;

        size_t index = 0;

        while ((index = m_path.find(L'/', index)) != STRING::npos)
        {
            ++depth;
            ++index;
        }
    }

    return depth;
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Parses the resource identifier from a string.
/// </summary>
///
/// <param name="resource">
/// Resource string similar to "Library://Telus/Calgary/Maps/Downtown.map"
/// </param>
///
/// EXCEPTIONS:
/// MgInvalidRepositoryTypeException, MgInvalidRepositoryNameException,
/// MgInvalidResourcePathException, MgInvalidResourceNameException,
/// MgInvalidResourceTypeException
///
void MgResourceIdentifier::ParseIdentifier(CREFSTRING resource)
{
    // Reset the repository type and name.
    m_repositoryType = m_repositoryName = L"";

    // Get the repository type and name.
    unsigned int firstPos = (unsigned int)resource.find(L':');

    if (STRING::npos != firstPos)
    {
        m_repositoryType = resource.substr(0, firstPos);
        ++firstPos;
        unsigned int secondPos = (unsigned int)resource.find(L"//", firstPos);

        if (STRING::npos != secondPos)
        {
            m_repositoryName = resource.substr(firstPos, secondPos - firstPos);

            // Parse the resource path, name, and type.
            ParsePathname(resource.substr(secondPos + 2));
        }
        else
        {
            // Reset the resource path, name, and type.
            m_path = m_name = m_type =  L"";
        }
    }
    else
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgResourceIdentifier.ParseIdentifier", __LINE__,  __WFILE__, NULL, L"", NULL);
    }

    Validate();
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Parses the resource path, name, and type.
/// </summary>
///
/// <param name="resource">
/// Resource string similar to "Telus/Calgary/Maps/Downtown.map"
/// </param>
///
/// EXCEPTIONS:
/// MgInvalidRepositoryTypeException, MgInvalidRepositoryNameException,
/// MgInvalidResourcePathException, MgInvalidResourceNameException,
/// MgInvalidResourceTypeException
///
void MgResourceIdentifier::ParsePathname(CREFSTRING resource)
{
    // Reset the resource path, name, and type.
    m_path = m_name = m_type =  L"";

    // If length is zero, then the resource is of folder type.
    unsigned int len = (unsigned int)resource.length();

    if (len > 0)
    {
        // If last character is '/', means it is folder.
        unsigned int firstPos = (unsigned int)resource.rfind(L'/');

        // Get the resource path, name, and type (foldername can also be a
        // resource name).
        if ((len - 1) == firstPos)
        {
            unsigned int secondPos = (unsigned int)resource.rfind(L'/', firstPos - 1);

            if (STRING::npos != secondPos)
            {
                m_path = resource.substr(0, secondPos++);
                m_name = resource.substr(secondPos, firstPos - secondPos);
            }
            else
            {
                m_name = resource.substr(0, firstPos);
            }

            m_type = MgResourceType::Folder;
        }
        else
        {
            if (STRING::npos != firstPos)
            {
                m_path = resource.substr(0, firstPos);
            }

            ++firstPos;
            unsigned int secondPos = (unsigned int)resource.rfind(L'.');

            if (STRING::npos != secondPos)
            {
                m_name = resource.substr(firstPos, secondPos - firstPos);
                m_type = resource.substr(secondPos + 1);
            }
            else
            {
                throw new MgInvalidRepositoryTypeException(
                    L"MgResourceIdentifier.ParsePathname", __LINE__,  __WFILE__, NULL, L"", NULL);
            }
        }
    }
    else
    {
        m_type = MgResourceType::Folder;
    }
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Validates the resource identifier.
/// </summary>
///
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgInvalidRepositoryTypeException, MgInvalidRepositoryNameException,
/// MgInvalidResourcePathException, MgInvalidResourceNameException,
/// MgInvalidResourceTypeException
///
void MgResourceIdentifier::Validate()
{
    CheckType(m_repositoryType, m_type);
    CheckRepositoryName();
    CheckPath();
    CheckName();
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Checks that the repository type is valid.
/// </summary>
///
/// EXCEPTIONS
/// MgInvalidRepositoryTypeException
///
void MgResourceIdentifier::CheckRepositoryType()
{
    if (m_repositoryType != MgRepositoryType::Library
     && m_repositoryType != MgRepositoryType::Session
     && m_repositoryType != MgRepositoryType::Site)
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgResourceIdentifier.CheckRepositoryType", __LINE__,  __WFILE__, NULL, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Checks that the repository name is valid.
/// </summary>
///
/// EXCEPTIONS
/// MgInvalidRepositoryNameException
///
void MgResourceIdentifier::CheckRepositoryName()
{
    if (MgRepositoryType::Session == m_repositoryType && m_repositoryName.empty())
    {
        throw new MgNullArgumentException(
            L"MgResourceIdentifier.CheckRepositoryName", __LINE__,  __WFILE__, NULL, L"", NULL);
    }
    else if (MgRepositoryType::Session != m_repositoryType && !m_repositoryName.empty())
    {
        MgStringCollection arguments;
        arguments.Add(m_repositoryName);
        throw new MgInvalidRepositoryNameException(
            L"MgResourceIdentifier.CheckRepositoryName", __LINE__,  __WFILE__, &arguments, L"", NULL);
    }

    try
    {
        MgUtil::CheckSpacesAtBeginEnd(m_repositoryName);
        MgUtil::CheckReservedCharacters(m_repositoryName, MgReservedCharacterSet::Name);
    }
    catch (MgException* e)
    {
        SAFE_RELEASE(e);

        MgStringCollection arguments;
        arguments.Add(m_repositoryName);

        throw new MgInvalidRepositoryNameException(L"MgResourceIdentifier.CheckRepositoryName",
            __LINE__,  __WFILE__, &arguments, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Checks that the resource path is valid.
/// </summary>
///
/// EXCEPTIONS
/// MgInvalidResourcePathException
///
void MgResourceIdentifier::CheckPath()
{
    try
    {
        MgUtil::CheckSpacesAtBeginEnd(m_path);
        MgUtil::CheckReservedCharacters(m_path, MgReservedCharacterSet::Path);
        MgUtil::CheckReservedCharacters(m_path, L"//", false);
        MgUtil::CheckReservedCharacters(m_path, L" /", false);
        MgUtil::CheckReservedCharacters(m_path, L"/ ", false);
        MgUtil::CheckSlashAtBeginEnd(m_path);
    }
    catch (MgException* e)
    {
        SAFE_RELEASE(e);

        MgStringCollection arguments;
        arguments.Add(m_path);

        throw new MgInvalidResourcePathException(L"MgResourceIdentifier.CheckPath",
            __LINE__,  __WFILE__, &arguments, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Checks that the resource name is valid.
/// </summary>
///
/// EXCEPTIONS
/// MgInvalidResourceNameException
///
void MgResourceIdentifier::CheckName()
{
    if (m_name.empty() && MgResourceType::Folder != m_type)
    {
        throw new MgNullArgumentException(
            L"MgResourceIdentifier.CheckName", __LINE__,  __WFILE__, NULL, L"", NULL);
    }
    else if (MgResourceType::Role == m_type
         && MgRole::Administrator != m_name
         && MgRole::Author        != m_name
         && MgRole::Viewer        != m_name)
    {
        MgStringCollection arguments;
        arguments.Add(m_name);

        throw new MgInvalidResourceNameException(L"MgResourceIdentifier.CheckName",
            __LINE__,  __WFILE__, &arguments, L"", NULL);
    }

    try
    {
        MgUtil::CheckSpacesAtBeginEnd(m_name);
        MgUtil::CheckReservedCharacters(m_name, MgReservedCharacterSet::Name);
    }
    catch (MgException* e)
    {
        SAFE_RELEASE(e);

        MgStringCollection arguments;
        arguments.Add(m_name);

        throw new MgInvalidResourceNameException(L"MgResourceIdentifier.CheckName",
            __LINE__,  __WFILE__, &arguments, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Checks that the specified repository type an resource type are valid.
/// </summary>
///
/// EXCEPTIONS
/// MgInvalidRepositoryTypeException
/// MgInvalidResourceTypeException
///
void MgResourceIdentifier::CheckType(CREFSTRING repositoryType,
    CREFSTRING resourceType)
{
    try
    {
        MgUtil::CheckSpacesAtBeginEnd(resourceType);
        MgUtil::CheckReservedCharacters(resourceType, MgReservedCharacterSet::Name);
    }
    catch (MgException* e)
    {
        SAFE_RELEASE(e);

        throw new MgInvalidResourceTypeException(
                L"MgResourceIdentifier.CheckType", __LINE__,  __WFILE__, NULL, L"", NULL);
    }

    if (MgRepositoryType::Session == repositoryType)
    {
        if (MgResourceType::User  == resourceType
         || MgResourceType::Group == resourceType
         || MgResourceType::Role  == resourceType)
        {
            throw new MgInvalidResourceTypeException(
                L"MgResourceIdentifier.CheckType", __LINE__,  __WFILE__, NULL, L"", NULL);
        }
    }
    else if (MgRepositoryType::Library == repositoryType)
    {
        if (MgResourceType::Map       == resourceType
         || MgResourceType::Selection == resourceType
         || MgResourceType::User      == resourceType
         || MgResourceType::Group     == resourceType
         || MgResourceType::Role      == resourceType)
        {
            throw new MgInvalidResourceTypeException(
                L"MgResourceIdentifier.CheckType", __LINE__,  __WFILE__, NULL, L"", NULL);
        }
    }
    else if (MgRepositoryType::Site == repositoryType)
    {
        if (MgResourceType::User   != resourceType
         && MgResourceType::Group  != resourceType
         && MgResourceType::Role   != resourceType
         && MgResourceType::Folder != resourceType)
        {
            throw new MgInvalidResourceTypeException(
                L"MgResourceIdentifier.CheckType", __LINE__,  __WFILE__, NULL, L"", NULL);
        }
    }
    else
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgResourceIdentifier.CheckType", __LINE__,  __WFILE__, NULL, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////////
///<summary>
/// Serialize data to TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>
void MgResourceIdentifier::Serialize(MgStream* stream)
{
    Ptr<MgStreamHelper> helper = stream->GetStreamHelper();
    STRING resId = ToString();
    helper->WriteString(resId);
}

///////////////////////////////////////////////////////////////////////////////
///<summary>
/// Deserialize data from TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>
void MgResourceIdentifier::Deserialize(MgStream* stream)
{
    Ptr<MgStreamHelper> helper = stream->GetStreamHelper();
    STRING resId;
    helper->GetString(resId);
    SetResource(resId);
}

///////////////////////////////////////////////////////////////////////////////
///<summary>
///Returns the path within the repository to the resource
///</summary>
///<returns>
///Path within the repository
///</returns>
STRING MgResourceIdentifier::GetPath()
{
    return m_path;
}

///////////////////////////////////////////////////////////////////////////////
///<summary>
///Set the resource path within the repository.
///</summary>
///<param name="path">
///Resource path within the repository
///</param>
void MgResourceIdentifier::SetPath(CREFSTRING path)
{
    m_path = path;
}

///////////////////////////////////////////////////////////////////////////////
///<summary>
///Return the resource name for the desired resource
///</summary>
///<returns>
///Resource name
///</returns>
STRING MgResourceIdentifier::GetName()
{
    return m_name;
}

///////////////////////////////////////////////////////////////////////////////
///<summary>
///Set the resource name
///</summary>
///<param name="name">
///Resource name
///</param>
void MgResourceIdentifier::SetName(CREFSTRING name)
{
    m_name = name;
}

///////////////////////////////////////////////////////////////////////////////
///<summary>
///Return the resource type string for the desired resource
///</summary>
///<returns>
///Resource type string.  Valid strings are defined in ResourceType.
///</returns>
STRING MgResourceIdentifier::GetResourceType()
{
    return m_type;
}

///////////////////////////////////////////////////////////////////////////////
///<summary>
///Set the resource type.  Valid strings are defined in ResourceType.
///</summary>
///<param name="type">
///Resource type string
///</param>
void MgResourceIdentifier::SetResourceType(CREFSTRING type)
{
    m_type = type;
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the class ID.
/// </summary>
INT32 MgResourceIdentifier::GetClassId()
{
    return m_cls_id;
}
