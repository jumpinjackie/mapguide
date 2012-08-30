#ifndef DESKTOP_RESOURCE_PACKAGE_MANIFEST_PARSER_H
#define DESKTOP_RESOURCE_PACKAGE_MANIFEST_PARSER_H

#include "ResourcePackageManifestHandler.h"
#include "OperationInfo.h"

typedef std::vector<MgOperationInfo> MgOpInfoVector;

class MgdResourcePackageManifestParser : public MgdResourcePackageManifestHandler
{
/// Constructors/Destructor

public:

    MgdResourcePackageManifestParser();
    virtual ~MgdResourcePackageManifestParser();

private:

    // Unimplemented Methods

    MgdResourcePackageManifestParser(const MgdResourcePackageManifestParser&);
    MgdResourcePackageManifestParser& operator=(
        const MgdResourcePackageManifestParser&);

/// Methods

public:

    const MgOpInfoVector& GetOperations() const;

    void Parse(const string& manifest);

/// Data Members

private:

    MgOpInfoVector m_operations;
};

/// Inline Methods

inline const MgOpInfoVector& MgdResourcePackageManifestParser::GetOperations() const
{
    return m_operations;
}

#endif