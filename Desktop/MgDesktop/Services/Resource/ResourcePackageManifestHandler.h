#ifndef DESKTOP_RESOURCE_PACKAGE_MANIFEST_HANDLER_H
#define DESKTOP_RESOURCE_PACKAGE_MANIFEST_HANDLER_H

#include "System/XmlUtil.h"

class MgdResourcePackageManifestHandler
{
/// Constructors/Destructor

public:

    MgdResourcePackageManifestHandler();
    virtual ~MgdResourcePackageManifestHandler();

private:

    // Unimplemented Methods

    MgdResourcePackageManifestHandler(const MgdResourcePackageManifestHandler&);
    MgdResourcePackageManifestHandler& operator=(
        const MgdResourcePackageManifestHandler&);

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

inline STRING MgdResourcePackageManifestHandler::GetDescription() const
{
    return m_description;
}


#endif