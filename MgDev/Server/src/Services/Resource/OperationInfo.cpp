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

#include "ResourceServiceDefs.h"
#include "OperationInfo.h"

// Resource Service operations
// TODO: Share the following strings with the Web tier.

const STRING MgOperationName::EnumerateRepositories=L"ENUMERATEREPOSITORIES";
const STRING MgOperationName::CreateRepository=L"CREATEREPOSITORY";
const STRING MgOperationName::DeleteRepository=L"DELETEREPOSITORY";
const STRING MgOperationName::UpdateRepository=L"UPDATEREPOSITORY";
const STRING MgOperationName::GetRepositoryContent=L"GETREPOSITORYCONTENT";
const STRING MgOperationName::GetRepositoryHeader=L"GETREPOSITORYHEADER";
const STRING MgOperationName::ApplyResourcePackage=L"APPLYRESOURCEPACKAGE";

const STRING MgOperationName::EnumerateResources=L"ENUMERATERESOURCES";
const STRING MgOperationName::SetResource=L"SETRESOURCE";
const STRING MgOperationName::DeleteResource=L"DELETERESOURCE";
const STRING MgOperationName::MoveResource=L"MOVERESOURCE";
const STRING MgOperationName::CopyResource=L"COPYRESOURCE";
const STRING MgOperationName::GetResourceContent=L"GETRESOURCECONTENT";
const STRING MgOperationName::GetResourceHeader=L"GETRESOURCEHEADER";
const STRING MgOperationName::EnumerateResourceReferences=L"ENUMERATERESOURCEREFERENCES";
const STRING MgOperationName::ChangeResourceOwner=L"CHANGERESOURCEOWNER";
const STRING MgOperationName::InheritPermissionsFrom=L"INHERITPERMISSIONSFROM";

const STRING MgOperationName::EnumerateResourceData=L"ENUMERATERESOURCEDATA";
const STRING MgOperationName::SetResourceData=L"SETRESOURCEDATA";
const STRING MgOperationName::DeleteResourceData=L"DELETERESOURCEDATA";
const STRING MgOperationName::RenameResourceData=L"RENAMERESOURCEDATA";
const STRING MgOperationName::GetResourceData=L"GETRESOURCEDATA";

const STRING MgOperationName::EnumerateUnmanagedData=L"ENUMERATEUNMANAGEDDATA";

const STRING MgOperationInfo::sm_currentVersion = L"1.0.0";
const MgOperationParameter MgOperationInfo::sm_blankParameter;


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgOperationInfo::MgOperationInfo()
{
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object with the specified operation name.
/// The operation version will be intialized accoordingly.
///
MgOperationInfo::MgOperationInfo(CREFSTRING name) :
    m_name(name),
    // TODO: Revisit this code if the version is changed in future releases.
    m_version(sm_currentVersion)
{
    if (m_name.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(
            L"MgOperationInfo.MgOperationInfo",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Copy constructor.
///
MgOperationInfo::MgOperationInfo(const MgOperationInfo& opInfo)
{
    *this = opInfo;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgOperationInfo::~MgOperationInfo()
{
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Assignment operator.
///
MgOperationInfo& MgOperationInfo::operator=(const MgOperationInfo& opInfo)
{
    if (&opInfo != this)
    {
        m_name = opInfo.m_name;
        m_version = opInfo.m_version;

        m_parameters.clear();
        m_parameters = opInfo.m_parameters;
    }

    return *this;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets the name of the operation.
///
void MgOperationInfo::SetName(CREFSTRING name)
{
    m_name = name;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets the version of the operation.
///
void MgOperationInfo::SetVersion(CREFSTRING version)
{
    m_version = version;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the specified parameter from the operation parameter map.
///
const MgOperationParameter& MgOperationInfo::GetParameter(CREFSTRING name,
    bool required) const
{
    MgOpParamMap::const_iterator i = m_parameters.find(name);

    if (m_parameters.end() != i)
    {
        return (*i).second;
    }
    else if (required)
    {
        MgStringCollection arguments;
        arguments.Add(name);

        throw new MgParameterNotFoundException(
            L"MgOperationInfo.GetParameter", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    return sm_blankParameter;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Adds the specified parameter to the operation parameter map.
///
void MgOperationInfo::AddParameter(CREFSTRING name,
    const MgOperationParameter& opParam)
{
    MgOpParamMap::const_iterator i = m_parameters.find(name);

    if (m_parameters.end() == i)
    {
        m_parameters.insert(MgOpParamMap::value_type(name, opParam));
    }
    else
    {
        MgStringCollection arguments;
        arguments.Add(name);

        throw new MgDuplicateParameterException(
            L"MgOperationInfo.SetParameter", __LINE__, __WFILE__, &arguments, L"", NULL);
    }
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Removes the specified parameter from the operation parameter map.
///
void MgOperationInfo::RemoveParameter(CREFSTRING name)
{
    m_parameters.erase(name);
}
