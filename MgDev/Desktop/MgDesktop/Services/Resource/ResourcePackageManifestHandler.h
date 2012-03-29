#ifndef DESKTOP_RESOURCE_PACKAGE_MANIFEST_HANDLER_H
#define DESKTOP_RESOURCE_PACKAGE_MANIFEST_HANDLER_H

#include "System/XmlUtil.h"

class MgResourcePackageManifestHandler
{
/// Constructors/Destructor

public:

    MgResourcePackageManifestHandler();
    virtual ~MgResourcePackageManifestHandler();

private:

    // Unimplemented Methods

    MgResourcePackageManifestHandler(const MgResourcePackageManifestHandler&);
    MgResourcePackageManifestHandler& operator=(
        const MgResourcePackageManifestHandler&);

/// Methods

public:

    STRING GetDescription() const;
    void SetDescription(CREFSTRING description);

/// Data Members

public:

    static const STRING sm_manifestFileName;

protected:

    auto_ptr<MgXmlUtil> m_xmlUtil;
    STRING m_description;
};

/// Inline Methods

inline STRING MgResourcePackageManifestHandler::GetDescription() const
{
    return m_description;
}


#endif