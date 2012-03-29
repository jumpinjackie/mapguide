#ifndef DESKTOP_RESOURCE_PACKAGE_MANIFEST_PARSER_H
#define DESKTOP_RESOURCE_PACKAGE_MANIFEST_PARSER_H

#include "ResourcePackageManifestHandler.h"
#include "OperationInfo.h"

typedef std::vector<MgOperationInfo> MgOpInfoVector;

class MgResourcePackageManifestParser : public MgResourcePackageManifestHandler
{
/// Constructors/Destructor

public:

    MgResourcePackageManifestParser();
    virtual ~MgResourcePackageManifestParser();

private:

    // Unimplemented Methods

    MgResourcePackageManifestParser(const MgResourcePackageManifestParser&);
    MgResourcePackageManifestParser& operator=(
        const MgResourcePackageManifestParser&);

/// Methods

public:

    const MgOpInfoVector& GetOperations() const;

    void Parse(const string& manifest);

/// Data Members

private:

    MgOpInfoVector m_operations;
};

/// Inline Methods

inline const MgOpInfoVector& MgResourcePackageManifestParser::GetOperations() const
{
    return m_operations;
}

#endif