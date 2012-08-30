#ifndef DESKTOP_RESOURCE_PACKAGE_LOADER_H
#define DESKTOP_RESOURCE_PACKAGE_LOADER_H

#include "ResourcePackageHandler.h"
#include "ZipFileReader.h"
#include "ResourcePackageManifestParser.h"

class MgdResourcePackageLoader : public MgdResourcePackageHandler
{
/// Constructors/Destructor

public:

    explicit MgdResourcePackageLoader(MgResourceService& repositoryManager);
    virtual ~MgdResourcePackageLoader();

private:

    // Unimplemented Methods

    MgdResourcePackageLoader();
    MgdResourcePackageLoader(const MgdResourcePackageLoader&);
    MgdResourcePackageLoader& operator=(const MgdResourcePackageLoader&);

/// Methods

public:

    void Start(CREFSTRING packagePathname, bool logActivities);
    virtual void End(MgException* except);

protected:

    void PerformOperation(const MgOperationInfo& opInfo);

    void UpdateRepository(const MgOperationInfo& opInfo);
    void SetResource(const MgOperationInfo& opInfo);
    void DeleteResource(const MgOperationInfo& opInfo);
    void MoveResource(const MgOperationInfo& opInfo);
    void CopyResource(const MgOperationInfo& opInfo);
    void ChangeResourceOwner(const MgOperationInfo& opInfo);
    void InheritPermissionsFrom(const MgOperationInfo& opInfo);
    void SetResourceData(const MgOperationInfo& opInfo);
    void DeleteResourceData(const MgOperationInfo& opInfo);
    void RenameResourceData(const MgOperationInfo& opInfo);

private:

    MgResourceIdentifier* CreateResourceIdentifier(
        const MgdOperationParameter& opParam) const;
    MgByteReader* CreateByteReader(
        const MgdOperationParameter& opParam, bool direct = true) const;

/// Data Members

private:

    auto_ptr<MgdZipFileReader> m_zipFileReader;
    MgdResourcePackageManifestParser m_manifestParser;
};

/// Inline Methods

#endif