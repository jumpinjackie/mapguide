#include "ResourceService.h"
#include "Foundation.h"
#include "Services/Feature/FeatureServiceCache.h"
#include "Services/Resource/ResourceContentCache.h"
#include "Services/Resource/ResourceDefs.h"
#include "Services/Resource/ResourcePackageLoader.h"
#include "Services/Resource/UnmanagedDataManager.h"
#include "ace/Dirent_Selector.h"
#include "Services/Feature/FdoConnectionPool.h"
#include "CryptographyUtil.h"

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
MgdResourceService::MgdResourceService(CREFSTRING libraryContentRoot, CREFSTRING libraryDataRoot, CREFSTRING sessionContentRoot, CREFSTRING sessionDataRoot, CREFSTRING schemaPath)
: MgResourceService()
{ 
	m_libraryContentPath = libraryContentRoot;
	m_libraryDataPath = libraryDataRoot;
	m_sessionContentPath = sessionContentRoot;
	m_sessionDataPath = sessionDataRoot;
    m_schemaPath = schemaPath;
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
    STRING path;

    MG_RESOURCE_SERVICE_TRY()

    CHECKARGUMENTNULL(resId, L"MgdResourceService::ResolveContentPath");

	path = GetContentDirectory(resId);
    STRING type = resId->GetResourceType();
    if (MgResourceType::Folder != type)
    {
        STRING name = resId->GetName();
        
        path += name;
        path += L".";
        path += type;
    }

    MgdLogDetail logDetail(MgServiceType::ResourceService, MgdLogDetail::InternalTrace, L"MgdResourceService::ResolveContentPath", mgStackParams);
    logDetail.AddResourceIdentifier(L"resId", resId);
    logDetail.AddString(L"resolvedPath", path);
    logDetail.Create();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgdResourceService::ResolveContentPath")

    return path;
}

STRING MgdResourceService::ResolveDataPath(MgResourceIdentifier* resId)
{
    STRING cntPath;
    MG_RESOURCE_SERVICE_TRY()

	CHECKARGUMENTNULL(resId, L"MgdResourceService::ResolveDataPath");

	STRING type = resId->GetRepositoryType();
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
	}
	else 
	{
		throw new MgInvalidArgumentException(L"MgdResourceService::ResolveDataPath", __LINE__, __WFILE__, NULL, L"", NULL);
	}

    MgdLogDetail logDetail(MgServiceType::ResourceService, MgdLogDetail::InternalTrace, L"MgdResourceService::ResolveDataPath", mgStackParams);
    logDetail.AddResourceIdentifier(L"resId", resId);
    logDetail.AddString(L"repositoryType", type);
    logDetail.AddString(L"resolvedPath", cntPath);
    logDetail.Create();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgdResourceService::ResolveDataPath")

    return cntPath;
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
        MG_DESKTOP_LOG_EXCEPTION();
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
    auto_ptr<MgdResourcePackageLoader> packageLoader;

    MG_RESOURCE_SERVICE_TRY()

    packageLoader.reset(new MgdResourcePackageLoader(*this));
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
        //Keep a wide copy
        Ptr<MgByteSink> sink = new MgByteSink(content);
        std::string xml;
        sink->ToStringUtf8(xml);

        XercesDOMParser parser;  
        parser.setValidationScheme(XercesDOMParser::Val_Always);
        parser.cacheGrammarFromParse(true);
        DefaultHandler handler;
        parser.setErrorHandler(&handler);
        try 
        {
            MemBufInputSource source((const XMLByte*)xml.c_str(), xml.size(), "MgdResourceService.Class", false);
            parser.parse(source);

            DOMDocument* doc = parser.getDocument();
            CHECKNULL(doc, L"MgdResourceService::SetResource");
            STRING resType = resource->GetResourceType();
            DOMElement* docEl = doc->getDocumentElement();
            CHECKNULL(docEl, L"MgdResourceService::SetResource");
            STRING docElName = docEl->getNodeName();

            //Now make sure it's the right type of XML document

            //Everything except SymbolDefinitions will have the same root element name as the resource type
            if (resType != MgResourceType::SymbolDefinition)
            {
                if (resType != docElName)
                {
                    MgStringCollection args;
                    args.Add(docElName);
                    throw new MgInvalidResourceTypeException(L"MgdResourceService::SetResource", __LINE__, __WFILE__, &args, L"", NULL);
                }
            }
            else
            {
                if (docElName != L"SimpleSymbolDefinition" && docElName != L"CompoundSymbolDefinition")
                {
                    MgStringCollection args;
                    args.Add(docElName);
                    throw new MgInvalidResourceTypeException(L"MgdResourceService::SetResource", __LINE__, __WFILE__, &args, L"", NULL);
                }
            }
        }
        catch (const SAXParseException& e)
        {
            STRING msg = e.getMessage();
            XMLFileLoc lineNum = e.getLineNumber();
            XMLFileLoc colNum = e.getColumnNumber();

            const XMLCh* pubId = e.getPublicId();
            const XMLCh* sysId = e.getSystemId();

            STRING pubIdStr;
            if (NULL != pubId)
                pubIdStr = pubId;
            STRING sysIdStr;
            if (NULL != sysId)
                sysIdStr = sysId;

            STRING lineNumStr;
            STRING colNumStr;
            MgUtil::Int64ToString(lineNum, lineNumStr);
            MgUtil::Int64ToString(colNum, colNumStr);

            MgStringCollection args;
            args.Add(msg);
            args.Add(lineNumStr);
            args.Add(colNumStr);
            args.Add(pubIdStr);
            args.Add(sysIdStr);
            throw new MgXmlParserException(L"MgdResourceService::SetResource", __LINE__, __WFILE__, &args, L"", NULL);
        }

        //Can't rewind a byte sink. So make another one
        if (content->IsRewindable())
        {
            content->Rewind();
            Ptr<MgByteSink> sink2 = new MgByteSink(content);
            sink2->ToFile(path);
        }
        else
        {
            //Make a new byte source/reader/sink from the checked out xml string
            Ptr<MgByteSource> bs = new MgByteSource((BYTE_ARRAY_IN)xml.c_str(), (INT32)xml.length());
            Ptr<MgByteReader> rdr = bs->GetReader();
            Ptr<MgByteSink> sink2 = new MgByteSink(rdr);
            sink2->ToFile(path);
        }

        ReleasePotentialLocks(resource);
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_RESOURCE_SERVICE_CATCH(L"MgdResourceService::SetResource")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
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
        ReleasePotentialLocks(resource);

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
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_RESOURCE_SERVICE_THROW()
}

void MgdResourceService::ReleasePotentialLocks(MgResourceIdentifier* resource)
{
    //Empty cached version
    MgdResourceContentCache* cache = MgdResourceContentCache::GetInstance();
    cache->RemoveContentEntry(resource);

    CHECKARGUMENTNULL(resource, L"MgdResourceService::DeleteResource");
    if (resource->GetResourceType() == MgResourceType::FeatureSource)
    {
        //Invalidate any cached information
        MgdFeatureServiceCache* cache = MgdFeatureServiceCache::GetInstance();
        cache->RemoveEntry(resource);
        //Boot pooled connections to ensure no locks are held
        MgdFdoConnectionPool::PurgeCachedConnections(resource);
    }
    else if (resource->GetResourceType() == MgResourceType::Folder)
    {
        //TODO: We obviously need a fine-grained version instead of going nuclear
        MgdFeatureServiceCache* cache = MgdFeatureServiceCache::GetInstance();
        cache->Clear();
        //Boot pooled connections to ensure no locks are held
        MgdFdoConnectionPool::PurgeCachedConnectionsUnderFolder(resource);
    }
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
        ReleasePotentialLocks(destResource);
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
        MG_DESKTOP_LOG_EXCEPTION();
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
        MG_DESKTOP_LOG_EXCEPTION();
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
        ReleasePotentialLocks(destResource);
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
        MG_DESKTOP_LOG_EXCEPTION();
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
        MG_DESKTOP_LOG_EXCEPTION();
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

    ReleasePotentialLocks(resource);
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
        MG_DESKTOP_LOG_EXCEPTION();
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
        MG_DESKTOP_LOG_EXCEPTION();
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
        MG_DESKTOP_LOG_EXCEPTION();
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
        MG_DESKTOP_LOG_EXCEPTION();
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
        MG_DESKTOP_LOG_EXCEPTION();
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
        MG_DESKTOP_LOG_EXCEPTION();
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
        MG_DESKTOP_LOG_EXCEPTION();
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
        MG_DESKTOP_LOG_EXCEPTION();
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

    //Think of the hard disks. Check we have a cached copy and return that, otherwise
    //stash the content in the cache for future calls on the same resource
    MgdResourceContentCache* cache = MgdResourceContentCache::GetInstance();
    STRING resContent = cache->GetContentEntry(resource);
    if (resContent.empty())
    {
        STRING path = ResolveContentPath(resource);
        if (!MgFileUtil::IsFile(path))
        {
            MgStringCollection arguments;
            arguments.Add(resource->ToString());
            throw new MgResourceNotFoundException(L"MgdResourceService::GetResourceContent", __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        Ptr<MgByteSource> source = new MgByteSource(path);
        Ptr<MgByteReader> reader = source->GetReader();

        Ptr<MgByteSink> sink = new MgByteSink(reader);
        sink->ToString(resContent);

        std::string mbXml = MgUtil::WideCharToMultiByte(resContent);
        //Stash it for future calls
        cache->PutContentEntry(resource, resContent);

        Ptr<MgByteSource> source2 = new MgByteSource((BYTE_ARRAY_IN)mbXml.c_str(), mbXml.length());
        source2->SetMimeType(MgMimeType::Xml);
        content = source2->GetReader();
    }
    else
    {
        std::string mbXml = MgUtil::WideCharToMultiByte(resContent);
        Ptr<MgByteSource> source = new MgByteSource((BYTE_ARRAY_IN)mbXml.c_str(), mbXml.length());
        source->SetMimeType(MgMimeType::Xml);
        content = source->GetReader();
    }
    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_RESOURCE_SERVICE_CATCH(L"MgdResourceService::GetResourceContent")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_RESOURCE_SERVICE_THROW()

    return content.Detach();
}

MgByteReader* MgdResourceService::GetResourceData(MgResourceIdentifier* resource,
    CREFSTRING dataName, CREFSTRING preProcessTags) 
{
    Ptr<MgByteReader> reader;

    MG_RESOURCE_SERVICE_TRY()

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

	Ptr<MgByteSource> source;

    //As per MapGuide Server behaviour, if request is for MG_USER_CREDENTIALS return the decrypted username only
    if (dataName == MgResourceDataName::UserCredentials)
    {
        source = new MgByteSource(path);
        reader = source->GetReader();
        Ptr<MgByteSink> sink = new MgByteSink(reader);
        std::string data;
        sink->ToStringUtf8(data);

        std::string mbUsername;
        std::string mbPassword;
        MgCryptographyUtil crypto;
        crypto.DecryptCredentials(data, mbUsername, mbPassword);

        source = NULL;
        reader = NULL;
        source = new MgByteSource((BYTE_ARRAY_IN)mbUsername.c_str(), mbUsername.length());
        reader = source->GetReader();
    }
    else
    {
        source = new MgByteSource(path);
	    reader = source->GetReader();
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgdResourceService::GetResourceData")

	return reader.Detach();
}

MgByteReader* MgdResourceService::GetRawCredentials(MgResourceIdentifier* resource)
{
    Ptr<MgByteReader> blob;

    MG_RESOURCE_SERVICE_TRY()

    CHECKARGUMENTNULL(resource, L"MgdResourceService::GetResourceData");
    if (!ResourceExists(resource))
    {
        MgStringCollection arguments;
        arguments.Add(resource->ToString());
        throw new MgResourceNotFoundException(L"MgdResourceService::GetResourceData", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    STRING path = ResolveDataPath(resource);
    path += MgResourceDataName::UserCredentials;

	if (!MgFileUtil::IsFile(path))
		throw new MgResourceDataNotFoundException(L"MgdResourceService::GetResourceData", __LINE__, __WFILE__, NULL, L"", NULL);

    Ptr<MgByteSource> source = new MgByteSource(path);
    blob = source->GetReader();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgdResourceService::GetRawCredentials")

    return blob.Detach();
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

    byteReader = MgdUnmanagedDataManager::GetInstance()->EnumerateUnmanagedData(path, recursive, type, filter);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_RESOURCE_SERVICE_CATCH(L"MgdResourceService::EnumerateUnmanagedData")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_RESOURCE_SERVICE_THROW()

    return byteReader.Detach();
};

void MgdResourceService::SetResourceCredentials(MgResourceIdentifier* resource, CREFSTRING userName, CREFSTRING password)
{
    MG_LOG_OPERATION_MESSAGE(L"SetResourceCredentials");

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 3);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(userName.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"<Password>");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdResourceService::SetResourceCredentials()");

    std::string data;
    std::string mgUsername = MgUtil::WideCharToMultiByte(userName);
    std::string mgPassword = MgUtil::WideCharToMultiByte(password);
    MgCryptographyUtil crypto;
    crypto.EncryptCredentials(mgUsername, mgPassword, data);

    Ptr<MgByteSource> bs = new MgByteSource((BYTE_ARRAY_IN)data.c_str(), data.length());
    Ptr<MgByteReader> br = bs->GetReader();

    //Invalidate any cached connections as they may use old credentials
    if (resource->GetResourceType() == MgResourceType::FeatureSource)
    {
        MgdFdoConnectionPool::PurgeCachedConnections(resource);
    }

    SetResourceData(resource, MgResourceDataName::UserCredentials, MgResourceDataType::File, br);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_RESOURCE_SERVICE_CATCH(L"MgdResourceService::SetResourceCredentials")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_RESOURCE_SERVICE_THROW()
}