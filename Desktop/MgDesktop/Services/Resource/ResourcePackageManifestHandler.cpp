#include "ResourceDefs.h"
#include "ResourcePackageManifestHandler.h"

const STRING MgdResourcePackageManifestHandler::sm_manifestFileName = L"MgResourcePackageManifest.xml";

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgdResourcePackageManifestHandler::MgdResourcePackageManifestHandler()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgdResourcePackageManifestHandler::~MgdResourcePackageManifestHandler()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets the resource package description.
///
void MgdResourcePackageManifestHandler::SetDescription(CREFSTRING description)
{
    m_description = description;
}
