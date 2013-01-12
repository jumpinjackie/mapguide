#ifndef DESKTOP_RESOURCE_SERVICE_H
#define DESKTOP_RESOURCE_SERVICE_H

#include "MgDesktop.h"
/// \ingroup Desktop_Service_Module
/// \{
class MG_DESKTOP_API MgdResourceService : public MgResourceService
{
    friend class MgdFdoConnectionUtil;

    DECLARE_CLASSNAME(MgdResourceService)

INTERNAL_API:
    MgdResourceService(CREFSTRING libraryContentRoot, CREFSTRING libraryDataRoot, CREFSTRING sessionContentRoot, CREFSTRING sessionDataRoot, CREFSTRING schemaPath);

    virtual MgByteReader* EnumerateRepositories(CREFSTRING repositoryType);

    virtual void CreateRepository(MgResourceIdentifier* resource, MgByteReader* content, MgByteReader* header);

    virtual void DeleteRepository(MgResourceIdentifier* resource);

EXTERNAL_API:
    virtual ~MgdResourceService();

PUBLISHED_API:
    virtual void UpdateRepository(MgResourceIdentifier* resource, MgByteReader* content, MgByteReader* header);

    virtual void ApplyResourcePackage(MgByteReader* resourcePackage);

    virtual void SetResource(MgResourceIdentifier* resource, MgByteReader* content, MgByteReader* header);

    virtual void DeleteResource(MgResourceIdentifier* resource);

    virtual void CopyResource(MgResourceIdentifier* sourceResource, MgResourceIdentifier* destResource, bool overwrite);

    virtual void MoveResource(MgResourceIdentifier* sourceResource, MgResourceIdentifier* destResource, bool overwrite);

    virtual void MoveResource(MgResourceIdentifier* sourceResource, MgResourceIdentifier* destResource, bool overwrite, bool cascade);

    virtual MgByteReader* GetResourceHeader(MgResourceIdentifier* resource);

    virtual void ChangeResourceOwner(MgResourceIdentifier* resource, CREFSTRING owner, bool includeDescendants);

    virtual void InheritPermissionsFrom(MgResourceIdentifier* resource);

    virtual void SetResourceData(MgResourceIdentifier* resource, CREFSTRING dataName, CREFSTRING dataType, MgByteReader* data);

    virtual void DeleteResourceData(MgResourceIdentifier* resource, CREFSTRING dataName);

    virtual void RenameResourceData(MgResourceIdentifier* resource, CREFSTRING oldDataName, CREFSTRING newDataName, bool overwrite);

    virtual MgByteReader* GetResourceData(MgResourceIdentifier* resource, CREFSTRING dataName);

    virtual MgByteReader* EnumerateResourceData(MgResourceIdentifier* resource);

    virtual MgByteReader* GetRepositoryContent(MgResourceIdentifier* resource);

    virtual MgByteReader* GetRepositoryHeader(MgResourceIdentifier* resource);

    virtual MgByteReader* EnumerateReferences(MgResourceIdentifier* resource);

    virtual bool ResourceExists(MgResourceIdentifier* resource);

    virtual MgStringCollection* GetResourceContents(MgStringCollection* resources, MgStringCollection* preProcessTags);

    virtual MgByteReader* EnumerateUnmanagedData(CREFSTRING path, bool recursive, CREFSTRING type, CREFSTRING filter);

    void SetResourceCredentials(MgResourceIdentifier* resource, CREFSTRING userName, CREFSTRING password);

INTERNAL_API:
    virtual MgByteReader* EnumerateResources(MgResourceIdentifier* resource,
        INT32 depth, CREFSTRING type, INT32 properties,
        CREFSTRING fromDate, CREFSTRING toDate);

    virtual MgByteReader* EnumerateResources(MgResourceIdentifier* resource,
        INT32 depth, CREFSTRING type, INT32 properties,
        CREFSTRING fromDate, CREFSTRING toDate, bool computeChildren);

    void LoadResourcePackage(CREFSTRING packagePathname);

    virtual MgByteReader* GetResourceContent(MgResourceIdentifier* resource, CREFSTRING preProcessTags);

    virtual MgByteReader* GetResourceData(MgResourceIdentifier* resource, CREFSTRING dataName, CREFSTRING preProcessTags);

    virtual MgDateTime* GetResourceModifiedDate(MgResourceIdentifier* resource);

    virtual MgSerializableCollection* EnumerateParentMapDefinitions(MgSerializableCollection* resources);

    STRING GetContentDirectory(MgResourceIdentifier* resId);
	STRING ResolveContentPath(MgResourceIdentifier* resId);
	STRING ResolveDataPath(MgResourceIdentifier* resId);

    void DeleteSessionFiles();

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_Desktop_ResourceService_ResourceService;

protected:
    virtual INT32 GetClassId() { return m_cls_id; }

protected:
	virtual void Dispose() { delete this; }

private:
    MgByteReader* GetRawCredentials(MgResourceIdentifier* resource);
    void ReleasePotentialLocks(MgResourceIdentifier* resource);
    static bool ListDirectories(MgStringCollection* directoryNames, CREFSTRING path);
    void WriteResourceFolderEntry(REFSTRING xml, INT32 maxDepth, STRING type, MgResourceIdentifier* resId, CREFSTRING enumeratedFolderId = L"");
    void WriteResourceDocumentEntry(REFSTRING xml, MgResourceIdentifier* resId);

	STRING m_libraryContentPath;
	STRING m_libraryDataPath;
	STRING m_sessionContentPath;
	STRING m_sessionDataPath;
    STRING m_schemaPath;
};
/// \}
#endif