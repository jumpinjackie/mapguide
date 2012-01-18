#ifndef DESKTOP_RESOURCE_PACKAGE_LOADER_H
#define DESKTOP_RESOURCE_PACKAGE_LOADER_H

#include "ResourcePackageHandler.h"
#include "ZipFileReader.h"
#include "ResourcePackageManifestParser.h"

class MgResourcePackageLoader : public MgResourcePackageHandler
{
/// Constructors/Destructor

public:

    explicit MgResourcePackageLoader(MgResourceService& repositoryManager);
    virtual ~MgResourcePackageLoader();

private:

    // Unimplemented Methods

    MgResourcePackageLoader();
    MgResourcePackageLoader(const MgResourcePackageLoader&);
    MgResourcePackageLoader& operator=(const MgResourcePackageLoader&);

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
        const MgOperationParameter& opParam) const;
    MgByteReader* CreateByteReader(
        const MgOperationParameter& opParam, bool direct = true) const;

/// Data Members

private:

    auto_ptr<MgZipFileReader> m_zipFileReader;
    MgResourcePackageManifestParser m_manifestParser;
};

/// Inline Methods

#endif