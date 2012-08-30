#include "ResourceDefs.h"
#include "OperationInfo.h"

// Resource Service operations
// TODO: Share the following strings with the Web tier.

//const STRING MgdOperationName::CreateRepository=L"CREATEREPOSITORY";
//const STRING MgdOperationName::DeleteRepository=L"DELETEREPOSITORY";
const STRING MgdOperationName::UpdateRepository=L"UPDATEREPOSITORY";
//const STRING MgdOperationName::ApplyResourcePackage=L"APPLYRESOURCEPACKAGE";

const STRING MgdOperationName::SetResource=L"SETRESOURCE";
const STRING MgdOperationName::DeleteResource=L"DELETERESOURCE";
const STRING MgdOperationName::MoveResource=L"MOVERESOURCE";
const STRING MgdOperationName::CopyResource=L"COPYRESOURCE";
const STRING MgdOperationName::ChangeResourceOwner=L"CHANGERESOURCEOWNER";
const STRING MgdOperationName::InheritPermissionsFrom=L"INHERITPERMISSIONSFROM";

const STRING MgdOperationName::SetResourceData=L"SETRESOURCEDATA";
const STRING MgdOperationName::DeleteResourceData=L"DELETERESOURCEDATA";
const STRING MgdOperationName::RenameResourceData=L"RENAMERESOURCEDATA";

const STRING MgOperationInfo::sm_currentVersion = L"1.0.0";
const MgdOperationParameter MgOperationInfo::sm_blankParameter;


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
const MgdOperationParameter& MgOperationInfo::GetParameter(CREFSTRING name,
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

        throw new MgdParameterNotFoundException(
            L"MgOperationInfo.GetParameter", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    return sm_blankParameter;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Adds the specified parameter to the operation parameter map.
///
void MgOperationInfo::AddParameter(CREFSTRING name,
    const MgdOperationParameter& opParam)
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

        throw new MgdDuplicateParameterException(
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
