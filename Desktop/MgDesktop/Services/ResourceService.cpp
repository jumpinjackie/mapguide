#include "ResourceService.h"
#include "Foundation.h"
#include "Services/Feature/FeatureServiceCache.h"
#include "Services/Resource/ResourceDefs.h"
#include "Services/Resource/ResourcePackageLoader.h"
#include "Services/Resource/UnmanagedDataManager.h"
#include "ace/Dirent_Selector.h"
#include "Services/Feature/FdoConnectionPool.h"

static int MgDirEntrySelector(const ACE_DIRENT *d)
{
    return (0 != ACE_OS::strcmp(d->d_name, ACE_TEXT(".")) &&
            0 != ACE_OS::strcmp(d->d_name, ACE_TEXT("..")));
}

static int MgDirEntryComparator(const ACE_DIRENT **d1, const ACE_DIRENT **d2)
{
    return ACE_OS::strcmp((*d1)->d_name, (*d2)->d_name);
}

//INTERNAL_API:
MgdResourceService::MgdResourceService(CREFSTRING libraryContentRoot, CREFSTRING libraryDataRoot, CREFSTRING sessionContentRoot, CREFSTRING sessionDataRoot)
: MgResourceService()
{ 
	m_libraryContentPath = libraryContentRoot;
	m_libraryDataPath = libraryDataRoot;
	m_sessionContentPath = sessionContentRoot;
	m_sessionDataPath = sessionDataRoot;
}

MgdResourceService::~MgdResourceService() { }

MgByteReader* MgdResourceService::EnumerateRepositories(CREFSTRING repositoryType) 
{ 
	NOT_IMPLEMENTED(L"MgdResourceService::EnumerateRepositories");
}

void MgdResourceService::CreateRepository(MgResourceIdentifier* resource, MgByteReader* content, MgByteReader* header) 
{ 
	NOT_IMPLEMENTED(L"MgdResourceService::CreateRepository");
}

void MgdResourceService::DeleteRepository(MgResourceIdentifier* resource) 
{ 
	NOT_IMPLEMENTED(L"MgdResourceService::DeleteRepository");
}

//PUBLISHED_API:
void MgdResourceService::UpdateRepository(MgResourceIdentifier* resource, MgByteReader* content, MgByteReader* header) 
{ 
	NOT_IMPLEMENTED(L"MgdResourceService::UpdateRepository");
}

STRING MgdResourceService::GetContentDirectory(MgResourceIdentifier* resId)
{
    CHECKARGUMENTNULL(resId, L"MgdResourceService::ResolveContentPath");

	STRING type = resId->GetRepositoryType();
	STRING cntPath;
	if (type == L"Library")
	{
		// [ROOT]/Content/[path]/

		STRING path = resId->GetPath();
		STRING type = resId->GetResourceType();
		
		cntPath = m_libraryContentPath;
		MgFileUtil::AppendSlashToEndOfPath(cntPath);

		cntPath += path;
        MgFileUtil::AppendSlashToEndOfPath(cntPath);

        if (MgResourceType::Folder == resId->GetResourceType())
        {
            cntPath += resId->GetName();
            MgFileUtil::AppendSlashToEndOfPath(cntPath);
        }

		return cntPath;
	}
	else if (type == L"Session")
	{
		// [ROOT]/Content/[sessionID]/[path]/

		STRING path = resId->GetPath();
		STRING type = resId->GetResourceType();
		STRING sessionId = resId->GetRepositoryName();
		
		cntPath = m_sessionContentPath;
		MgFileUtil::AppendSlashToEndOfPath(cntPath);

		cntPath += sessionId;
		MgFileUtil::AppendSlashToEndOfPath(cntPath);

		cntPath += path;
        MgFileUtil::AppendSlashToEndOfPath(cntPath);

        if (MgResourceType::Folder == resId->GetResourceType())
        {
            cntPath += resId->GetName();
            MgFileUtil::AppendSlashToEndOfPath(cntPath);
        }

		return cntPath;
	}
	else 
	{
		throw new MgInvalidArgumentException(L"MgdResourceService::ResolveContentPath", __LINE__, __WFILE__, NULL, L"", NULL);
	}
}

STRING MgdResourceService::ResolveContentPath(MgResourceIdentifier* resId)
{
	STRING path = GetContentDirectory(resId);
    STRING type = resId->GetResourceType();
    if (MgResourceType::Folder != type)
    {
        STRING name = resId->GetName();
        
        path += name;
        path += L".";
        path += type;
    }
    return path;
}

STRING MgdResourceService::ResolveDataPath(MgResourceIdentifier* resId)
{
	CHECKARGUMENTNULL(resId, L"MgdResourceService::ResolveDataPath");

	STRING type = resId->GetRepositoryType();
	STRING cntPath;
	if (type == L"Library")
	{
		// [ROOT]/Data/[path]/[name].[resourceType]/

		STRING path = resId->GetPath();
		STRING type = resId->GetResourceType();
        STRING resName = resId->GetName();
		
		cntPath = m_libraryDataPath;
		MgFileUtil::AppendSlashToEndOfPath(cntPath);

		cntPath += path;
        MgFileUtil::AppendSlashToEndOfPath(cntPath);

        cntPath += resName;
        if (resId->GetResourceType() != MgResourceType::Folder)
        {
		    cntPath += L".";
		    cntPath += type;
        }
		MgFileUtil::AppendSlashToEndOfPath(cntPath);

		return cntPath;
	}
	else if (type == L"Session")
	{
		// [ROOT]/Data/[sessionID]/[path].[resourceType]/

		STRING path = resId->GetPath();
		STRING type = resId->GetResourceType();
        STRING resName = resId->GetName();
		STRING sessionId = resId->GetRepositoryName();
		
		cntPath = m_sessionDataPath;
		MgFileUtil::AppendSlashToEndOfPath(cntPath);

		cntPath += sessionId;
		MgFileUtil::AppendSlashToEndOfPath(cntPath);

		cntPath += path;
        MgFileUtil::AppendSlashToEndOfPath(cntPath);

        cntPath += resName;
        if (resId->GetResourceType() != MgResourceType::Folder)
        {
		    cntPath += L".";
		    cntPath += type;
        }
		MgFileUtil::AppendSlashToEndOfPath(cntPath);

		return cntPath;
	}
	else 
	{
		throw new MgInvalidArgumentException(L"MgdResourceService::ResolveDataPath", __LINE__, __WFILE__, NULL, L"", NULL);
	}
}

void MgdResourceService::ApplyResourcePackage(MgByteReader* packageStream) 
{ 
    CHECKARGUMENTNULL(packageStream, L"MgdResourceService::ApplyResourcePackage");

    STRING packagePathname;

    MG_LOG_OPERATION_MESSAGE(L"ApplyResourcePackage");

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1 ,0 ,0),1);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgByteReader");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdResourceService::ApplyResourcePackage()");

    packagePathname = MgFileUtil::GenerateTempFileName();
    MgByteSink byteSink(packageStream);

    byteSink.ToFile(packagePathname);
    LoadResourcePackage(packagePathname);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_RESOURCE_SERVICE_CATCH(L"MgdResourceService::ApplyResourcePackage")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    if (!packagePathname.empty())
    {
        try
        {
            MgFileUtil::DeleteFile(packagePathname);
        }
        catch (MgException* e)
        {
            SAFE_RELEASE(e);
        }
        catch (...)
        {
        }
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_RESOURCE_SERVICE_THROW()
}

void MgdResourceService::LoadResourcePackage(CREFSTRING packagePathname)
{
    ACE_ASSERT(!packagePathname.empty());
    auto_ptr<MgResourcePackageLoader> packageLoader;

    MG_RESOURCE_SERVICE_TRY()

    packageLoader.reset(new MgResourcePackageLoader(*this));
    packageLoader->Start(packagePathname, false);

    MG_RESOURCE_SERVICE_CATCH(L"MgLibraryRepositoryManager.LoadResourcePackage")

    if (NULL != packageLoader.get())
    {
        packageLoader->End(mgException);
    }

    MG_RESOURCE_SERVICE_THROW()
}

void MgdResourceService::SetResource(MgResourceIdentifier* resource, MgByteReader* content, MgByteReader* header) 
{
    MG_LOG_OPERATION_MESSAGE(L"SetResource");

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0),3);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgByteReader");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgByteReader");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdResourceService::SetResource()");

    CHECKARGUMENTNULL(resource, L"MgdResourceService::SetResource");
    //Only null check if not a folder, because otherwise this is a legit way of
    //creating a folder
    if (resource->GetResourceType() != MgResourceType::Folder)
    {
        CHECKARGUMENTNULL(content, L"MgdResourceService::SetResource");
    }

    STRING dir = GetContentDirectory(resource);
    if (!MgFileUtil::IsDirectory(dir))
        MgFileUtil::CreateDirectory(dir, false, true);

	STRING path = ResolveContentPath(resource);
   
    if (NULL != content)
    {
	    Ptr<MgByteSink> sink = new MgByteSink(content);
	    sink->ToFile(path);

        if (resource->GetResourceType() == MgResourceType::FeatureSource)
        {
            //Invalidate any cached information
            MgFeatureServiceCache* cache = MgFeatureServiceCache::GetInstance();
            cache->RemoveEntry(resource);
            //Boot pooled connections
            MgFdoConnectionPool::PurgeCachedConnections(resource);
        }
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_RESOURCE_SERVICE_CATCH(L"MgdResourceService::SetResource")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_RESOURCE_SERVICE_THROW()
}

void MgdResourceService::DeleteResource(MgResourceIdentifier* resource) 
{ 
    MG_LOG_OPERATION_MESSAGE(L"DeleteResource");

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 1);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdResourceService::DeleteResource()");

    CHECKARGUMENTNULL(resource, L"MgdResourceService::DeleteResource");
    if (ResourceExists(resource))
    {
        if (resource->GetResourceType() == MgResourceType::FeatureSource)
        {
            //Invalidate any cached information
            MgFeatureServiceCache* cache = MgFeatureServiceCache::GetInstance();
            cache->RemoveEntry(resource);
            //Boot pooled connections to ensure no locks are held (technically,
            //the pooled connections are closed, but just to play safe)
            MgFdoConnectionPool::PurgeCachedConnections(resource);
        }
        else if (resource->GetResourceType() == MgResourceType::Folder)
        {
            //TODO: We obviously need a fine-grained version instead of going nuclear
            MgFeatureServiceCache* cache = MgFeatureServiceCache::GetInstance();
            cache->Clear();
            //Boot pooled connections to ensure no locks are held (technically,
            //the pooled connections are closed, but just to play safe)
            MgFdoConnectionPool::PurgeCachedConnectionsUnderFolder(resource);
        }

	    STRING contentPath = ResolveContentPath(resource);
	    STRING dataPath = ResolveDataPath(resource);
	    if (MgFileUtil::IsFile(contentPath))
	    {
		    MgFileUtil::DeleteFile(contentPath);

		    if (MgFileUtil::IsDirectory(dataPath))
			    MgFileUtil::DeleteDirectory(dataPath);
	    }
        else if (MgFileUtil::IsDirectory(contentPath))
        {
            //For directory deletes, everything underneath
            //is expendable too
            MgFileUtil::DeleteDirectory(contentPath, true);

            if (MgFileUtil::IsDirectory(dataPath))
			    MgFileUtil::DeleteDirectory(dataPath, true);
        }
    }
    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_RESOURCE_SERVICE_CATCH(L"MgdResourceService::DeleteResource")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_RESOURCE_SERVICE_THROW()
}

void MgdResourceService::CopyResource(MgResourceIdentifier* sourceResource, MgResourceIdentifier* destResource, bool overwrite) 
{ 
    MG_LOG_OPERATION_MESSAGE(L"CopyResource");

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 3);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == sourceResource) ? L"MgResourceIdentifier" : sourceResource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == destResource) ? L"MgResourceIdentifier" : destResource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_BOOL(overwrite);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdResourceService::CopyResource()");

    CHECKARGUMENTNULL(sourceResource, L"MgdResourceService::CopyResource");
    CHECKARGUMENTNULL(destResource, L"MgdResourceService::CopyResource");

    if (sourceResource->ToString() == destResource->ToString())
        throw new MgInvalidArgumentException(L"MgdResourceService::CopyResource", __LINE__, __WFILE__, NULL, L"", NULL);

	if (!ResourceExists(sourceResource))
    {
        MgStringCollection arguments;
        arguments.Add(sourceResource->ToString());
		throw new MgResourceNotFoundException(L"MgdResourceService::CopyResource", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    if (ResourceExists(destResource) && !overwrite)
    {
        MgStringCollection arguments;
        arguments.Add(destResource->ToString());
        throw new MgDuplicateResourceException(L"MgdResourceService::CopyResource", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

	STRING srcContentPath = ResolveContentPath(sourceResource);
	STRING srcDataPath = ResolveDataPath(sourceResource);
	STRING dstContentPath = ResolveContentPath(destResource);
    STRING dstContentDir = GetContentDirectory(destResource);
	STRING dstDataPath = ResolveDataPath(destResource);

	//Copy content
	if (!overwrite)
    {
        if (!MgFileUtil::IsFile(dstContentPath))
        {   
            if (!MgFileUtil::PathnameExists(dstContentDir))
                MgFileUtil::CreateDirectory(dstContentDir, false, true);
	        MgFileUtil::CopyFile(srcContentPath, dstContentPath, overwrite);
        }
    }
    else
    {
        if (!MgFileUtil::PathnameExists(dstContentDir))
            MgFileUtil::CreateDirectory(dstContentDir, false, true);
        MgFileUtil::CopyFile(srcContentPath, dstContentPath, overwrite);
    }
	
    //Copy data files
    if (MgFileUtil::IsDirectory(srcDataPath))
    {
	    Ptr<MgStringCollection> dataFiles = new MgStringCollection();
	    MgFileUtil::GetFilesInDirectory(dataFiles, srcDataPath, false, true);

        if (!MgFileUtil::IsDirectory(dstDataPath))
            MgFileUtil::CreateDirectory(dstDataPath, false, true);

	    for (int i = 0; i < dataFiles->GetCount(); i++)
	    {
		    STRING fileName = dataFiles->GetItem(i);

		    STRING src = srcDataPath + fileName;
		    STRING dst = dstDataPath + fileName;

            //Overwrite flag not set and destination file exists
            if (!overwrite && MgFileUtil::IsFile(dst))
                continue;

		    MgFileUtil::CopyFile(src, dst, overwrite);
	    }
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_RESOURCE_SERVICE_CATCH(L"MgdResourceService::CopyResource")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_RESOURCE_SERVICE_THROW()
}

void MgdResourceService::MoveResource(MgResourceIdentifier* sourceResource, MgResourceIdentifier* destResource, bool overwrite)
{
    MG_LOG_OPERATION_MESSAGE(L"MoveResource");

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 3);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == sourceResource) ? L"MgResourceIdentifier" : sourceResource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == destResource) ? L"MgResourceIdentifier" : destResource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_BOOL(overwrite);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdResourceService::MoveResource()");

    MoveResource(sourceResource, destResource, overwrite, false);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_RESOURCE_SERVICE_CATCH(L"MgdResourceService::MoveResource")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_RESOURCE_SERVICE_THROW()
}

void MgdResourceService::MoveResource(MgResourceIdentifier* sourceResource, MgResourceIdentifier* destResource, bool overwrite, bool cascade)
{
    MG_LOG_OPERATION_MESSAGE(L"MoveResource");

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 4);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == sourceResource) ? L"MgResourceIdentifier" : sourceResource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == destResource) ? L"MgResourceIdentifier" : destResource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_BOOL(overwrite);
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_BOOL(cascade);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdResourceService::MoveResource()");

    CHECKARGUMENTNULL(sourceResource, L"MgdResourceService::MoveResource");
    CHECKARGUMENTNULL(destResource, L"MgdResourceService::MoveResource");

    STRING srcResId = sourceResource->ToString();
    STRING dstResId = destResource->ToString();

    if (srcResId == dstResId)
        throw new MgInvalidArgumentException(L"MgdResourceService::MoveResource", __LINE__, __WFILE__, NULL, L"", NULL);

    if (!ResourceExists(sourceResource))
    {
        MgStringCollection arguments;
        arguments.Add(sourceResource->ToString());
		throw new MgResourceNotFoundException(L"MgdResourceService::MoveResource", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

	STRING srcContentPath = ResolveContentPath(sourceResource);
	STRING srcDataPath = ResolveDataPath(sourceResource);
	STRING dstContentPath = ResolveContentPath(destResource);
    STRING dstContentDir = GetContentDirectory(destResource);
	STRING dstDataPath = ResolveDataPath(destResource);

	//Copy content
    if (!overwrite)
    {
        if (!MgFileUtil::IsFile(dstContentPath))
        {
            if (!MgFileUtil::PathnameExists(dstContentDir))
                MgFileUtil::CreateDirectory(dstContentDir, false, true);
	        MgFileUtil::CopyFile(srcContentPath, dstContentPath, overwrite);
        }
    }
    else
    {
        if (!MgFileUtil::PathnameExists(dstContentDir))
            MgFileUtil::CreateDirectory(dstContentDir, false, true);
        MgFileUtil::CopyFile(srcContentPath, dstContentPath, overwrite);
    }
	
     //Copy data files
    if (MgFileUtil::IsDirectory(srcDataPath))
    {  
	    Ptr<MgStringCollection> dataFiles = new MgStringCollection();
	    MgFileUtil::GetFilesInDirectory(dataFiles, srcDataPath, false, true);

        if (!MgFileUtil::PathnameExists(dstDataPath))
            MgFileUtil::CreateDirectory(dstDataPath, false, true);

	    for (int i = 0; i < dataFiles->GetCount(); i++)
	    {
		    STRING fileName = dataFiles->GetItem(i);

		    STRING src = srcDataPath + fileName;
		    STRING dst = dstDataPath + fileName;

            //Overwrite flag not set and destination file exists
            if (!overwrite && MgFileUtil::IsFile(dst))
                continue;

		    MgFileUtil::CopyFile(src, dst, overwrite);
	    }
    }
    DeleteResource(sourceResource);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_RESOURCE_SERVICE_CATCH(L"MgdResourceService::MoveResource")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_RESOURCE_SERVICE_THROW()
}

MgByteReader* MgdResourceService::GetResourceHeader(MgResourceIdentifier* resource) 
{ 
	NOT_IMPLEMENTED(L"MgdResourceService::GetResourceHeader");
}

void MgdResourceService::ChangeResourceOwner(MgResourceIdentifier* resource,
    CREFSTRING owner, bool includeDescendants) 
{ 
	NOT_IMPLEMENTED(L"MgdResourceService::ChangeResourceOwner");
}

void MgdResourceService::InheritPermissionsFrom(MgResourceIdentifier* resource) 
{ 
	NOT_IMPLEMENTED(L"MgdResourceService::InheritPermissionsFrom");
}

void MgdResourceService::SetResourceData(MgResourceIdentifier* resource,
    CREFSTRING dataName, CREFSTRING dataType, MgByteReader* data) 
{ 
    MG_LOG_OPERATION_MESSAGE(L"SetResourceData");

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 4);
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(dataName.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(dataType.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgByteReader");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdResourceService::SetResourceData()");

    CHECKARGUMENTNULL(resource, L"MgdResourceService::SetResourceData");
    CHECKARGUMENTNULL(data, L"MgdResourceService::SetResourceData");

    if (dataName.empty())
        throw new MgNullArgumentException(L"MgdResourceService::SetResourceData", __LINE__, __WFILE__, NULL, L"", NULL);

	STRING path = ResolveDataPath(resource);
    if (!MgFileUtil::IsDirectory(path))
        MgFileUtil::CreateDirectory(path, false, true);

	path += dataName;

	Ptr<MgByteSink> sink = new MgByteSink(data);
	sink->ToFile(path);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_RESOURCE_SERVICE_CATCH(L"MgdResourceService::SetResourceData")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_RESOURCE_SERVICE_THROW()
}

void MgdResourceService::DeleteResourceData(MgResourceIdentifier* resource, CREFSTRING dataName) 
{ 
    MG_LOG_OPERATION_MESSAGE(L"DeleteResourceData");

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 2);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(dataName.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdResourceService::DeleteResourceData()");

    CHECKARGUMENTNULL(resource, L"MgdResourceService::DeleteResourceData");

    if (dataName.empty())
        throw new MgNullArgumentException(L"MgdResourceService::DeleteResourceData", __LINE__, __WFILE__, NULL, L"", NULL);

	STRING path = ResolveDataPath(resource);
	path += dataName;

	if (!MgFileUtil::IsFile(path))
        throw new MgResourceDataNotFoundException(L"MgdResourceService::DeleteResourceData", __LINE__, __WFILE__, NULL, L"", NULL);

    MgFileUtil::DeleteFile(path);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_RESOURCE_SERVICE_CATCH(L"MgdResourceService::DeleteResourceData")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_RESOURCE_SERVICE_THROW()
}

void MgdResourceService::RenameResourceData(MgResourceIdentifier* resource,
    CREFSTRING oldDataName, CREFSTRING newDataName, bool overwrite) 
{ 
    MG_LOG_OPERATION_MESSAGE(L"RenameResourceData");

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 4);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(oldDataName.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(newDataName.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_BOOL(overwrite);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdResourceService::RenameResourceData()");

    CHECKARGUMENTNULL(resource, L"MgdResourceService::RenameResourceData");
    if (!ResourceExists(resource))
    {
        MgStringCollection arguments;
        arguments.Add(resource->ToString());
        throw new MgResourceNotFoundException(L"MgdResourceService::RenameResourceData", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    if (oldDataName.empty())
        throw new MgNullArgumentException(L"MgdResourceService::RenameResourceData", __LINE__, __WFILE__, NULL, L"", NULL);

    if (newDataName.empty())
        throw new MgNullArgumentException(L"MgdResourceService::RenameResourceData", __LINE__, __WFILE__, NULL, L"", NULL);

	STRING path = ResolveDataPath(resource);

    STRING src = path + oldDataName;
    if (!MgFileUtil::IsFile(src))
        throw new MgResourceDataNotFoundException(L"MgdResourceService::RenameResourceData", __LINE__, __WFILE__, NULL, L"", NULL);

	MgFileUtil::RenameFile(path, oldDataName, newDataName, overwrite);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_RESOURCE_SERVICE_CATCH(L"MgdResourceService::RenameResourceData")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_RESOURCE_SERVICE_THROW()
}

MgByteReader* MgdResourceService::GetResourceData(MgResourceIdentifier* resource, CREFSTRING dataName) 
{ 
    Ptr<MgByteReader> ret;
    MG_LOG_OPERATION_MESSAGE(L"GetResourceData");

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 2);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(dataName.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdResourceService::GetResourceData()");

    STRING preProcessTags(L"");
    ret = GetResourceData(resource, dataName, preProcessTags);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_RESOURCE_SERVICE_CATCH(L"MgdResourceService::GetResourceData")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_RESOURCE_SERVICE_THROW()

    return ret.Detach();
}

bool MgdResourceService::ListDirectories(MgStringCollection* directoryNames, CREFSTRING path)
{
    ACE_Dirent_Selector sel;
    sel.open(MG_WCHAR_TO_TCHAR(path), MgDirEntrySelector, MgDirEntryComparator);

    for(INT32 i = 0; i < sel.length(); i++)
    {
        ACE_DIRENT* dir = sel[i];
        STRING dirName = MG_TCHAR_TO_WCHAR(dir->d_name);
        directoryNames->Add(dirName);
    }

    return true;
}

void MgdResourceService::WriteResourceFolderEntry(REFSTRING content, INT32 maxDepth, STRING type, MgResourceIdentifier* resource, CREFSTRING enumeratedFolderId)
{
    STRING path = ResolveContentPath(resource);

    STRING xml;
    Ptr<MgStringCollection> files = new MgStringCollection();
    MgFileUtil::GetFilesInDirectory(files, path, false, true);

    Ptr<MgStringCollection> dirs = new MgStringCollection();
    ListDirectories(dirs, path);

    INT32 resDepth = resource->GetDepth();
    bool writeFolder = true;
    if (type != L"" && type != MgResourceType::Folder)
        writeFolder = false;

    bool folderOnly = false;

    if (maxDepth == 0)
        folderOnly = true;

    //Only write folder information if type filter is empty or set to "Folder"
    if (writeFolder && (
       (resource->ToString() != enumeratedFolderId && (type == L"" || type == MgResourceType::Folder)) ||
       (maxDepth <= 0 && resource->ToString() == enumeratedFolderId)))
    {
        xml += L"\t<ResourceFolder>\n";
        xml += L"\t\t<ResourceId>" + resource->ToString() + L"</ResourceId>\n";
        STRING depthStr;
        MgUtil::Int32ToString(resDepth, depthStr);
        xml += L"\t\t<Depth>";
        xml += depthStr;
        xml += L"</Depth>\n";
        xml += L"\t\t<Owner>Administrator</Owner>\n";
        MgDateTime createDate = MgFileUtil::GetFileCreationTime(path);
        MgDateTime modifyDate = MgFileUtil::GetFileModificationTime(path);
        xml += L"\t\t<CreatedDate>" + createDate.ToXmlString() + L"</CreatedDate>\n";
        xml += L"\t\t<ModifiedDate>" + modifyDate.ToXmlString() + L"</ModifiedDate>\n";

        STRING numFolderStr;
        MgUtil::Int32ToString(dirs->GetCount(), numFolderStr);
        xml += L"\t\t<NumberOfFolders>";
        xml += numFolderStr;
        xml += L"</NumberOfFolders>\n";

        STRING numDocStr;
        MgUtil::Int32ToString(files->GetCount(), numDocStr);
        xml += L"\t\t<NumberOfDocuments>";
        xml += numDocStr;
        xml += L"</NumberOfDocuments>\n";

        //Even though we don't support it, the schema requires this, so flub a generic response.
        xml += L"\t\t<ResourceFolderHeader xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"ResourceFolderHeader-1.0.0.xsd\">\n";
        xml += L"\t\t\t<Security xsi:noNamespaceSchemaLocation=\"ResourceSecurity-1.0.0.xsd\">\n";
        xml += L"\t\t\t\t<Inherited>true</Inherited>\n";
        xml += L"\t\t\t\t<Groups><Group><Name>Everyone</Name><Permissions>r,w</Permissions></Group></Groups>\n";
        xml += L"\t\t\t</Security>";
        xml += L"\t\t</ResourceFolderHeader>";

        xml += L"\t</ResourceFolder>\n";

        content.append(xml);
    }

    if (folderOnly)
        return;

    //Write child directories
    for (INT32 i = 0; i < dirs->GetCount(); i++)
    {
        STRING item = dirs->GetItem(i);
        STRING tpath = path + item;
        MgFileUtil::AppendSlashToEndOfPath(tpath);
        if (MgFileUtil::IsDirectory(tpath))
        {
            STRING resIdStr = resource->ToString();
            resIdStr += item;
            MgFileUtil::AppendSlashToEndOfPath(resIdStr);
            Ptr<MgResourceIdentifier> folder = new MgResourceIdentifier(resIdStr);
            
            //Skip if this folder breaches the maximum depth of enumeration and only if
            //this maximum depth is >= 0. -1 depth meaning go all the way.
            if (maxDepth >= 0 && folder->GetDepth() > maxDepth)
                continue;

            WriteResourceFolderEntry(content, maxDepth, type, folder);
        }
    }

    //Only process child documents if depth is -1 or current depth is between [0, maxDepth]
    if (maxDepth == -1 || (resDepth >= 0 && resDepth < maxDepth))
    {
        //Write child documents
        for (INT32 i = 0; i < files->GetCount(); i++)
        {
            STRING item = files->GetItem(i);
            STRING tpath = path + item;
            if (MgFileUtil::IsFile(tpath))
            {
                STRING resIdStr = resource->ToString();
                resIdStr += item;
                Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(resIdStr);
                if (type.empty() || resId->GetResourceType() == type)
                    WriteResourceDocumentEntry(content, resId);
            }
        }
    }
}

void MgdResourceService::WriteResourceDocumentEntry(REFSTRING content, MgResourceIdentifier* resource)
{
    STRING path = ResolveContentPath(resource);
    STRING xml;

    xml += L"\t<ResourceDocument>\n";
    xml += L"\t\t<ResourceId>" + resource->ToString() + L"</ResourceId>\n";
    wchar_t depth[10];
    ACE_OS::itoa(resource->GetDepth(), depth, 10);
    xml += L"\t\t<Depth>";
    xml += depth;
    xml += L"</Depth>\n";
    xml += L"\t\t<Owner>Administrator</Owner>\n";
    MgDateTime createDate = MgFileUtil::GetFileCreationTime(path);
    MgDateTime modifyDate = MgFileUtil::GetFileModificationTime(path);
    xml += L"\t\t<CreatedDate>" + createDate.ToXmlString() + L"</CreatedDate>\n";
    xml += L"\t\t<ModifiedDate>" + modifyDate.ToXmlString() + L"</ModifiedDate>\n";

    //Even though we don't support it, the schema requires this, so flub a generic response.
    xml += L"\t\t<ResourceDocumentHeader xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"ResourceDocumentHeader-1.0.0.xsd\">\n";
    xml += L"\t\t\t<Security xsi:noNamespaceSchemaLocation=\"ResourceSecurity-1.0.0.xsd\">\n";
    xml += L"\t\t\t\t<Inherited>true</Inherited>\n";
    xml += L"\t\t\t\t<Groups><Group><Name>Everyone</Name><Permissions>r,w</Permissions></Group></Groups>\n";
    xml += L"\t\t\t</Security>";
    xml += L"\t\t</ResourceDocumentHeader>";

    xml += L"\t</ResourceDocument>\n";

    content.append(xml);
}

MgByteReader* MgdResourceService::EnumerateResources(MgResourceIdentifier* resource,
        INT32 depth, CREFSTRING type, INT32 properties,
        CREFSTRING fromDate, CREFSTRING toDate)
{
    Ptr<MgByteReader> ret;
    MG_LOG_OPERATION_MESSAGE(L"EnumerateResources");

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 6);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_INT32(depth);
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(type.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_INT32(properties);
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(fromDate.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(toDate.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdResourceService::EnumerateResources()");

    ret = EnumerateResources(resource, depth, type, properties, fromDate, toDate, true);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_RESOURCE_SERVICE_CATCH(L"MgdResourceService::EnumerateResources")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_RESOURCE_SERVICE_THROW()

    return ret.Detach();
}

MgByteReader* MgdResourceService::EnumerateResources(MgResourceIdentifier* resource,
        INT32 depth, CREFSTRING type, INT32 properties,
        CREFSTRING fromDate, CREFSTRING toDate, bool computeChildren)
{
    Ptr<MgByteReader> response;
    MG_LOG_OPERATION_MESSAGE(L"EnumerateResources");

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 7);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_INT32(depth);
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(type.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_INT32(properties);
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(fromDate.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(toDate.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_BOOL(computeChildren);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdResourceService::EnumerateResources()");

    CHECKARGUMENTNULL(resource, L"MgdResourceService::EnumerateResources");

    //Disregard: properties, fromDate, toDate. 

    /*
    Sample results using Sheboygan Dataset:
    
    - Library://, "", 1
        - Folder: Library://Samples/
    
    - Library://, "", 0
        - Folder: Library://
    
    - Library://, "", 2
        - Folder: Library://Samples/
        - Folder: Library://Samples/Sheboygan/

    - Library://Samples/,"", 0
        - Folder: Library://Samples/

    - Library://Samples/,"", 1
        - Folder: Library://Samples/Sheboygan/

    - Library://Samples/,"", 2
        - Folder: Library://Samples/Sheboygan/
        - Folder: Library://Samples/Sheboygan/Data/
        - Folder: Library://Samples/Sheboygan/Layers/
        - Folder: Library://Samples/Sheboygan/Maps/
        - Folder: Library://Samples/Sheboygan/MapsTiled/
        - Folder: Library://Samples/Sheboygan/Layouts/
        - Folder: Library://Samples/Sheboygan/Symbols/

    - Library://Samples/Sheboygan/,"FeatureSource",2
        - Document: Library://Samples/Sheboygan/Data/BuildingOutlines.FeatureSource
        - Document: Library://Samples/Sheboygan/Data/CityLimits.FeatureSource
        - Document: Library://Samples/Sheboygan/Data/HydrographicLines.FeatureSource
        - Document: Library://Samples/Sheboygan/Data/HydrographicPolygons.FeatureSource
        - Document: Library://Samples/Sheboygan/Data/Islands.FeatureSource
        - Document: Library://Samples/Sheboygan/Data/LandUse.FeatureSource
        - Document: Library://Samples/Sheboygan/Data/Parcels.FeatureSource
        - Document: Library://Samples/Sheboygan/Data/Rail.FeatureSource
        - Document: Library://Samples/Sheboygan/Data/RoadCenterLines.FeatureSource
        - Document: Library://Samples/Sheboygan/Data/Soils.FeatureSource
        - Document: Library://Samples/Sheboygan/Data/Trees.FeatureSource
        - Document: Library://Samples/Sheboygan/Data/VotingDistricts.FeatureSource

    */

    STRING rtype = resource->GetResourceType();
    STRING path = ResolveContentPath(resource);

    STRING xml = L"<ResourceList xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"ResourceList-1.0.0.xsd\">";

    if (MgResourceType::Folder == rtype)
    {
        INT32 maxDepth = depth;
        if (!MgFileUtil::IsDirectory(path))
        {
            MgStringCollection arguments;
            arguments.Add(resource->ToString());
            throw new MgResourceNotFoundException(L"MgdResourceService::EnumerateResources", __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        if (maxDepth > 0)
        {
            maxDepth += resource->GetDepth();
        }

        //This will recurse however many levels deep. -1 depth means go all the way
        //otherwise it will stop recursing when the folder to be recursed exceeds the
        //maximum depth
        WriteResourceFolderEntry(xml, maxDepth, type, resource, resource->ToString());
    }
    else
    {
        //Conversely this is way easier. Just check for zero depth as it is the only allowed
        //value, and then write the document entry.
        if (depth != 0)
            throw new MgInvalidArgumentException(L"MgdResourceService::EnumerateResources", __LINE__, __WFILE__, NULL, L"", NULL);

        if (!MgFileUtil::IsFile(path))
        {
            MgStringCollection arguments;
            arguments.Add(resource->ToString());
            throw new MgResourceNotFoundException(L"MgdResourceService::EnumerateResources", __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        WriteResourceDocumentEntry(xml, resource);
    }

    xml += L"</ResourceList>";
    std::string cxml = MgUtil::WideCharToMultiByte(xml);

    Ptr<MgByteSource> source = new MgByteSource((unsigned char*)cxml.c_str(), (INT32)cxml.length());
    source->SetMimeType(MgMimeType::Xml);
    response = source->GetReader();

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_RESOURCE_SERVICE_CATCH(L"MgdResourceService::EnumerateResources")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_RESOURCE_SERVICE_THROW()

    return response.Detach();
}

MgByteReader* MgdResourceService::EnumerateResourceData(MgResourceIdentifier* resource) 
{ 
    Ptr<MgByteReader> reader;
    MG_LOG_OPERATION_MESSAGE(L"EnumerateResourceData");

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 1);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdResourceService::EnumerateResourceData()");

    CHECKARGUMENTNULL(resource, L"MgdResourceService::EnumerateResourceData");

    if (!ResourceExists(resource))
    {
        MgStringCollection arguments;
        arguments.Add(resource->ToString());
        throw new MgResourceNotFoundException(L"MgdResourceService::EnumerateResourceData", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

	STRING path = ResolveDataPath(resource);
	STRING content = L"<ResourceDataList>\n";

    if (MgFileUtil::IsDirectory(path))
    {
        Ptr<MgStringCollection> dataFiles = new MgStringCollection();
	    MgFileUtil::GetFilesInDirectory(dataFiles, path, false, true);
	    for (INT32 i = 0; i < dataFiles->GetCount(); i++)
	    {
		    content += L"\t<ResourceData>\n";
		    content += L"\t\t<Name>";
		    content += dataFiles->GetItem(i);
		    content += L"</Name>\n";
		    content += L"\t\t<Type>File</Type>\n";
		    content += L"\t</ResourceData>\n";
	    }
    }
	content += L"</ResourceDataList>";

    std::string ccontent = MgUtil::WideCharToMultiByte(content);

	Ptr<MgByteSource> byteSource = new MgByteSource((unsigned char*)ccontent.c_str(), (INT32)ccontent.length());
    byteSource->SetMimeType(MgMimeType::Xml);
	reader = byteSource->GetReader();

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_RESOURCE_SERVICE_CATCH(L"MgdResourceService::EnumerateResourceData")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_RESOURCE_SERVICE_THROW()

	return reader.Detach();
}

MgByteReader* MgdResourceService::GetRepositoryContent(MgResourceIdentifier* resource) 
{ 
	NOT_IMPLEMENTED(L"MgdResourceService::GetRepositoryContent");
}

MgByteReader* MgdResourceService::GetRepositoryHeader(MgResourceIdentifier* resource) 
{
	NOT_IMPLEMENTED(L"MgdResourceService::GetRepositoryHeader");
}

MgByteReader* MgdResourceService::EnumerateReferences(MgResourceIdentifier* resource) 
{
	NOT_IMPLEMENTED(L"MgdResourceService::EnumerateReferences");
}

bool MgdResourceService::ResourceExists(MgResourceIdentifier* resource) 
{ 
    bool ret = false;
    MG_LOG_OPERATION_MESSAGE(L"ResourceExists");

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 1);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdResourceService::ResourceExists()");

    CHECKARGUMENTNULL(resource, L"MgdResourceService::ResourceExists");

	STRING path = ResolveContentPath(resource);
    STRING type = resource->GetResourceType();

    if (MgResourceType::Folder == type)
        ret = MgFileUtil::IsDirectory(path);
    else
	    ret = MgFileUtil::IsFile(path);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_RESOURCE_SERVICE_CATCH(L"MgdResourceService::ResourceExists")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_RESOURCE_SERVICE_THROW()

    return ret;
}

MgStringCollection* MgdResourceService::GetResourceContents(MgStringCollection* resources, MgStringCollection* preProcessTags)
{
    Ptr<MgStringCollection> contents;
    MG_LOG_OPERATION_MESSAGE(L"GetResourceContents");

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 2);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgStringCollection");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgStringCollection");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdResourceService::GetResourceContents()");

    CHECKARGUMENTNULL(resources, L"MgdResourceService::GetResourceContents");

    contents = new MgStringCollection();
    for (INT32 i = 0; i < resources->GetCount(); i++)
    {
        Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(resources->GetItem(i));
        Ptr<MgByteReader> content = GetResourceContent(resId, L"");

        STRING xml = content->ToString();
        contents->Add(xml);
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_RESOURCE_SERVICE_CATCH(L"MgdResourceService::GetResourceContents")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_RESOURCE_SERVICE_THROW()

    return contents.Detach();
}

//INTERNAL_API:

MgByteReader* MgdResourceService::GetResourceContent(MgResourceIdentifier* resource,
    CREFSTRING preProcessTags) 
{ 
    Ptr<MgByteReader> content;
    MG_LOG_OPERATION_MESSAGE(L"GetResourceContent");

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 2);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(preProcessTags);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdResourceService::GetResourceContent()");

	CHECKARGUMENTNULL(resource, L"MgdResourceService::GetResourceContent");
    
    STRING resType = resource->GetResourceType();
    if (MgResourceType::Folder == resType ||
        MgResourceType::Group == resType ||
        MgResourceType::Role == resType ||
        MgResourceType::User == resType)
    {
        throw new MgInvalidResourceTypeException(L"MgdResourceService::GetResourceContent", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    STRING path = ResolveContentPath(resource);
    if (!MgFileUtil::IsFile(path))
    {
        MgStringCollection arguments;
        arguments.Add(resource->ToString());
        throw new MgResourceNotFoundException(L"MgdResourceService::GetResourceContent", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    Ptr<MgByteSource> source = new MgByteSource(path);
    source->SetMimeType(MgMimeType::Xml);
    content = source->GetReader();

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_RESOURCE_SERVICE_CATCH(L"MgdResourceService::GetResourceContent")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_RESOURCE_SERVICE_THROW()

    return content.Detach();
}

MgByteReader* MgdResourceService::GetResourceData(MgResourceIdentifier* resource,
    CREFSTRING dataName, CREFSTRING preProcessTags) 
{ 
	CHECKARGUMENTNULL(resource, L"MgdResourceService::GetResourceData");
    if (!ResourceExists(resource))
    {
        MgStringCollection arguments;
        arguments.Add(resource->ToString());
        throw new MgResourceNotFoundException(L"MgdResourceService::GetResourceData", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    if (dataName.empty())
        throw new MgNullArgumentException(L"MgdResourceService::GetResourceData", __LINE__, __WFILE__, NULL, L"", NULL);

	STRING path = ResolveDataPath(resource);
	path += dataName;

	if (!MgFileUtil::IsFile(path))
		throw new MgResourceDataNotFoundException(L"MgdResourceService::GetResourceData", __LINE__, __WFILE__, NULL, L"", NULL);

	Ptr<MgByteReader> reader;
	Ptr<MgByteSource> source = new MgByteSource(path);
	reader = source->GetReader();
	return reader.Detach();
}

MgDateTime* MgdResourceService::GetResourceModifiedDate(MgResourceIdentifier* resource) 
{ 
	CHECKARGUMENTNULL(resource, L"MgdResourceService::GetResourceModifiedDate");

    STRING path = ResolveContentPath(resource);
    if (!MgFileUtil::IsFile(path))
    {
        MgStringCollection arguments;
        arguments.Add(resource->ToString());
        throw new MgResourceNotFoundException(L"MgdResourceService::GetResourceModifiedDate", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    MgDateTime mdt = MgFileUtil::GetFileModificationTime(path);
    return new MgDateTime(mdt);
}

MgSerializableCollection* MgdResourceService::EnumerateParentMapDefinitions(
    MgSerializableCollection* resources) 
{ 
	NOT_IMPLEMENTED(L"MgdResourceService::EnumerateParentMapDefinitions");
}

void MgdResourceService::DeleteSessionFiles()
{
    Ptr<MgStringCollection> dirs = new MgStringCollection();
    ListDirectories(dirs, m_sessionContentPath);
    for (INT32 i = 0; i < dirs->GetCount(); i++)
    {
        STRING dir = dirs->GetItem(i);
        STRING path = m_sessionContentPath + dir;
        if (MgFileUtil::IsDirectory(path))
            MgFileUtil::DeleteDirectory(path);
    }
    dirs->Clear();
    ListDirectories(dirs, m_sessionDataPath);
    for (INT32 i = 0; i < dirs->GetCount(); i++)
    {
        STRING dir = dirs->GetItem(i);
        STRING path = m_sessionDataPath + dir;
        if (MgFileUtil::IsDirectory(path))
            MgFileUtil::DeleteDirectory(path);
    }
}

MgByteReader* MgdResourceService::EnumerateUnmanagedData(CREFSTRING path, bool recursive, CREFSTRING type, CREFSTRING filter)
{
    Ptr<MgByteReader> byteReader;

    MG_LOG_OPERATION_MESSAGE(L"EnumerateUnmanagedData");

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 4);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(path.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_BOOL(recursive);
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(type.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(filter.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdResourceService::EnumerateUnmanagedData()");

    byteReader = MgUnmanagedDataManager::GetInstance()->EnumerateUnmanagedData(path, recursive, type, filter);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_RESOURCE_SERVICE_CATCH(L"MgdResourceService::EnumerateUnmanagedData")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_RESOURCE_SERVICE_THROW()

    return byteReader.Detach();
};