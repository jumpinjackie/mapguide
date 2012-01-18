#include "ResourceDefs.h"
#include "ResourcePackageManifestHandler.h"

const STRING MgResourcePackageManifestHandler::sm_manifestFileName = L"MgResourcePackageManifest.xml";

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgResourcePackageManifestHandler::MgResourcePackageManifestHandler()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgResourcePackageManifestHandler::~MgResourcePackageManifestHandler()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets the resource package description.
///
void MgResourcePackageManifestHandler::SetDescription(CREFSTRING description)
{
    m_description = description;
}
