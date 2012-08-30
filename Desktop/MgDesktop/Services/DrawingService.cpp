#include "DrawingService.h"
#include "ServiceFactory.h"
#include "Services/Drawing/DrawingDefs.h"
#include "Services/Drawing/DrawingServiceUtil.h"

#include "dwfcore/MIME.h"
#include "dwf/package/Manifest.h"
#include "dwf/package/GlobalSection.h"
#include "dwf/package/EPlotSection.h"
#include "dwf/package/writer/extensions/6.01/PackageVersionExtension.h"

//////////////////////////////////////////////////////////////////
/// <summary>
/// GetDrawing() returns the DWF stream for a drawing specified by resource identifier.
/// </summary>
/// <param name="resource">Input
/// MgResourceIdentifier object identifying the DWF resource
/// </param>
/// <returns>
/// Returns DWF stream.
/// </returns>
/// EXCEPTIONS:
/// - MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing in the resource repository.
/// - MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
/// - See MgResourceService for additional exceptions.
MgByteReader* MgdDrawingService::GetDrawing(MgResourceIdentifier* resource)
{
    Ptr<MgByteReader> byteReader;

    MG_LOG_OPERATION_MESSAGE(L"CopyResource");

    MG_SERVER_DRAWING_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 1);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdDrawingService::GetDrawing()");

    if (0 == resource)
    {
        throw new MgNullArgumentException(L"MgdDrawingService.GetDrawing", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Get the name of the dwf file from the resource content and remove the path from the filename
    STRING dwfFileName = L"";
    STRING dwfCoordinateSpace = L"";
    Ptr<MgByteReader> reader = m_resourceService->GetResourceContent(resource, L"");
    MgdDrawingServiceUtil::ParseDrawingResourceContent(reader, dwfFileName, dwfCoordinateSpace);
    dwfFileName = dwfFileName.substr( dwfFileName.rfind(L"%") + 1 );

    // Return the drawing via a MgByteReader
    byteReader = m_resourceService->GetResourceData(resource, dwfFileName, L"");

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_SERVER_DRAWING_SERVICE_CATCH(L"MgdDrawingService::GetDrawing")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_SERVER_DRAWING_SERVICE_THROW()

    return byteReader.Detach();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// DescribeDrawing() returns manifest.xml document located in the DWF.
/// Refer to the DWF Format Specification at http://viewers/web/Olema/pdf/DWF%206%20Corporate%20Publishing%20Format.pdf for more information on the manifest.
/// </summary>
/// <param name="resource">Input
/// MgResourceIdentifier object identifying the DWF resource
/// </param>
/// <returns>
/// Returns the byte stream containing XML (manifest.xml).
/// </returns>
/// EXCEPTIONS:
/// - MgInvalidCastException if there are problems reading the DWF into memory.
/// - MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing in the resource repository.
/// - MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
/// - MgdInvalidDwfPackageException if the DWF specified by the resource identifier is not a DWF of version 6.0 or greater.
/// - MgdDwfException if the DWF component encounters errors.
/// - See MgResourceService for additional exceptions.
MgByteReader* MgdDrawingService::DescribeDrawing(MgResourceIdentifier* resource)
{
    Ptr<MgByteReader> byteReader;

    MG_LOG_OPERATION_MESSAGE(L"DescribeDrawing");

    MG_SERVER_DRAWING_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 1);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdDrawingService::DescribeDrawing()");

    if (0 == resource)
    {
        throw new MgNullArgumentException(L"MgdDrawingService.DescribeDrawing", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    else
    {
        auto_ptr<DWFPackageReader> reader(MgdDrawingServiceUtil::OpenDrawingResource(m_resourceService, resource, m_bOpenTempDwfFile, m_tempDwfFileName));

        // Obtain the manifest from the DWF.
        DWFInputStream* pStream = reader->extract(MANIFEST_XML.c_str(), false);
        size_t nBytes = pStream->available();
        char* pBuffer = DWFCORE_ALLOC_MEMORY( char, nBytes );
        pStream->read(pBuffer, nBytes);

        // Filter the character buffer for any extraneous characters at the end of the manifest.xml
        char* pDest = strrchr(pBuffer, '>');     // NOXLATE
        if (0 != pDest)
        {
            size_t nFilteredBytes = pDest - pBuffer + 1;
            if (nFilteredBytes < nBytes)
            {
                nBytes = nFilteredBytes;
            }
        }

        char* stream = reinterpret_cast<char*>(pBuffer);
        if (0 == stream)
        {
            if (0 != pStream)
                DWFCORE_FREE_OBJECT(pStream);
            if (0 != pBuffer)
                DWFCORE_FREE_MEMORY(pBuffer);
            throw new MgInvalidCastException(L"MgdDrawingService.DescribeDrawing", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Return the manifest via a MgByteReader
        Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)stream, (INT32)nBytes);
        byteSource->SetMimeType(MgMimeType::Xml);
        byteReader = byteSource->GetReader();

        if (0 != pStream)
            DWFCORE_FREE_OBJECT(pStream);
        if (0 != pBuffer)
            DWFCORE_FREE_MEMORY(pBuffer);
    }

    MgdDrawingServiceUtil::CloseDrawingResource(m_bOpenTempDwfFile, m_tempDwfFileName);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_SERVER_DRAWING_SERVICE_CATCH(L"MgdDrawingService::DescribeDrawing")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_SERVER_DRAWING_SERVICE_THROW()

    return byteReader.Detach();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// GetSection() returns a DWF containing only the requested section.  The section is specified by resource identifier (to get the DWF from the repository),
/// and the section name.  The section names can be retrieved via call to MgdDrawingService::EnumerateSections() or from manifest.xml via call to MgdDrawingService::DescribeDrawing().
/// </summary>
/// <param name="resource">Input
/// MgResourceIdentifier object identifying the DWF resource
/// </param>
/// <param name="sectionName">Input
/// sectionName specifies the unique name of the section in the DWF resource.  Section names can be retrieved via call to MgdDrawingService::EnumerateSections() or from manifest.xml via call to MgdDrawingService::DescribeDrawing().
/// </param>
/// <returns>
/// Returns DWF stream containing the specified section.
/// </returns>
/// EXCEPTIONS:
/// - MgInvalidArgumentException if the section name is blank
/// - MgdDwfSectionNotFoundException if the requested section does not exist in the DWF package.
/// - MgdInvalidDwfSectionException if the 2d graphics resource for the DWF section cannot be found.
/// - MgNullReferenceException no data could be read from the DWF resource.
/// - MgOutOfMemoryException if there is insufficient memory to complete the operation.
/// - MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing in the resource repository.
/// - MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
/// - MgdInvalidDwfPackageException if the DWF specified by the resource identifier is not a DWF of version 6.0 or greater.
/// - MgdDwfException if the DWF component encounters errors.
/// - MgTemporaryFileNotAvailableException if a temporary file need to complete the operation cannot be generated or accessed.
/// - See MgResourceService for additional exceptions.
MgByteReader* MgdDrawingService::GetSection(MgResourceIdentifier* resource, CREFSTRING sectionName)
{
    Ptr<MgByteReader> byteReader;

    MG_LOG_OPERATION_MESSAGE(L"GetSection");

    MG_SERVER_DRAWING_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 2);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(sectionName.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdDrawingService::GetSection()");

    if (0 == resource)
    {
        throw new MgNullArgumentException(L"MgdDrawingService.GetSection", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    else if (sectionName.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgdDrawingService.GetSection",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }
    else
    {
        auto_ptr<DWFPackageReader> reader(MgdDrawingServiceUtil::OpenDrawingResource(m_resourceService, resource, m_bOpenTempDwfFile, m_tempDwfFileName));

        // Check if the section exists in the manifest (and get the mime type from the manifest)
        DWFManifest& manifest = reader->getManifest();
        DWFSection* pSection = manifest.findSectionByName(sectionName.c_str());
        if (0 == pSection)
        {
            MgStringCollection arguments;
            arguments.Add(sectionName);
            throw new MgdDwfSectionNotFoundException(L"MgdDrawingService.GetSection", __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        // Create a DWFPackageWriter for writing the section to a temporary DWF file
        STRING tempDwfPathname = MgFileUtil::GenerateTempFileName(false, /*NOXLATE*/L"MGDS");
        DWFFile oDWF(tempDwfPathname.c_str());

        DWFPackageVersionExtension* pVersionExtension =
            DWFCORE_ALLOC_OBJECT(DWFPackageVersionTypeInfoExtension(DWFInterface::kzEPlot_ID));

        DWFPackageWriter oWriter( oDWF, L"", pVersionExtension );

        // Add section to the writer
        oWriter.addSection(pSection);

        // Write the package
        oWriter.write(
            DWF_SOURCE_PRODUCT_VENDOR.c_str(),
            DWF_SOURCE_PRODUCT_NAME.c_str(),
            DWF_SOURCE_PRODUCT_VERSION.c_str(),
            DWF_PRODUCT_VENDOR.c_str(),
            _DWFTK_VERSION_STRING );

        // Return the section via a MgByteReader
        Ptr<MgByteSource> byteSource = new MgByteSource(tempDwfPathname, true);
        byteSource->SetMimeType(MgMimeType::Dwf);
        byteReader = byteSource->GetReader();
    }

    MgdDrawingServiceUtil::CloseDrawingResource(m_bOpenTempDwfFile, m_tempDwfFileName);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_SERVER_DRAWING_SERVICE_CATCH(L"MgdDrawingService::GetSection")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_SERVER_DRAWING_SERVICE_THROW()

    return byteReader.Detach();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// GetSectionResource() extracts a specific resource from the DWF package.
/// It is specified by the resource identifier (to get the DWF from the repository)
/// and the resource name (to get the DWF resource from the DWF package).
/// A list of resource names can be retrieved via call to MgdDrawingServices::EnumerateSectionResources(),
/// or from the manifest.xml via call to MgdDrawingServices::DescribeDrawing().
/// Refer to the DWF Format Specification at http://viewers/web/Olema/pdf/DWF%206%20Corporate%20Publishing%20Format.pdf for more information resource files associated with a particular section.
///
/// </summary>
/// <param name="resource">Input
/// MgResourceIdentifier object identifying the DWF resource
/// </param>
/// <param name="resourceName">Input
/// Unique resource name of an item in the DWF.
/// The item's name can be retrieved from call to MgdDrawingServices::EnumerateDrawingServices(),
/// or from the manifest.xml via call to MgdDrawingServices::DescribeDrawing().
/// </param>
/// <returns>
/// Returns byte stream for the item.
/// Content type will be same as what is specified in manifest.xml for
/// the specified item e.g. image/png or text/xml etc.
/// </returns>
/// EXCEPTIONS:
/// - MgInvalidArgumentException if the requested resourceName does not specify a section name.
/// - MgInvalidArgumentException if the section name is blank
/// - MgdDwfSectionNotFoundException if the requested section does not exist in the DWF package.
/// - MgdDwfSectionResourceNotFoundException if the requested section resource does not exist in the DWF package.
/// - MgInvalidCastException if there are problems reading the DWF into memory.
/// - MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing in the resource repository.
/// - MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
/// - MgdInvalidDwfPackageException if the DWF specified by the resource identifier is not a DWF of version 6.0 or greater.
/// - MgdDwfException if the DWF component encounters errors.
/// - MgTemporaryFileNotAvailableException if a temporary file need to complete the operation cannot be generated or accessed.
/// - See MgResourceService for additional exceptions.
MgByteReader* MgdDrawingService::GetSectionResource(MgResourceIdentifier* resource, CREFSTRING resourceName)
{
    Ptr<MgByteReader> byteReader;

    MG_LOG_OPERATION_MESSAGE(L"GetSectionResource");

    MG_SERVER_DRAWING_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 2);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(resourceName.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdDrawingService::GetSectionResource()");

    if (0 == resource)
    {
        throw new MgNullArgumentException(L"MgdDrawingService.GetSectionResource", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    else if (resourceName.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgdDrawingService.GetSectionResource",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }
    else
    {
        auto_ptr<DWFPackageReader> reader(MgdDrawingServiceUtil::OpenDrawingResource(m_resourceService, resource, m_bOpenTempDwfFile, m_tempDwfFileName));

        // Parse the section name from the resourceName
        STRING::size_type index = resourceName.rfind(RESOURCENAME_SEPARATOR);
        STRING sectionName = L"";
        if (STRING::npos == index)
        {
            MgStringCollection arguments;
            arguments.Add(L"2");
            arguments.Add(resourceName);

            throw new MgInvalidArgumentException(L"MgdDrawingService.GetSectionResource",
                __LINE__, __WFILE__, &arguments, L"MgResourceNameSeparatorNotFound", NULL);
        }
        sectionName = resourceName.substr(0, index);

        if (sectionName.empty())
        {
            MgStringCollection arguments;
            arguments.Add(L"2");
            arguments.Add(resourceName);

            throw new MgInvalidArgumentException(L"MgdDrawingService.GetSectionResource",
                __LINE__, __WFILE__, &arguments, L"MgResourceNameDoesNotContainSectionName", NULL);
        }

        // Check if the section exists in the manifest
        DWFManifest& manifest = reader->getManifest();
        DWFSection* pSection = manifest.findSectionByName(sectionName.c_str());
        if (0 == pSection)
        {
            MgStringCollection arguments;
            arguments.Add(sectionName);
            throw new MgdDwfSectionNotFoundException(L"MgdDrawingService.GetSectionResource", __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        // ...check if the resource exists in the section
        DWFResource* pResource = pSection->findResourceByHREF(resourceName.c_str());
        if (0 == pResource)
        {
            MgStringCollection arguments;
            arguments.Add(resourceName);
            throw new MgdDwfSectionResourceNotFoundException(L"MgdDrawingService.GetSectionResource", __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        // ...get the mime type for the resource
        STRING wsMimeType = (STRING)pResource->mime();

        // Retrieve the resource from the DWF.
        DWFInputStream* pStream = reader->extract(resourceName.c_str(), false);
        size_t nBytes = pStream->available();
        char* pBuffer = DWFCORE_ALLOC_MEMORY( char, nBytes );
        pStream->read(pBuffer, nBytes);
        char* stream = reinterpret_cast<char*>(pBuffer);
        if (0 == stream)
        {
            if (0 != pStream)
                DWFCORE_FREE_OBJECT(pStream);
            if (0 != pBuffer)
                DWFCORE_FREE_MEMORY(pBuffer);
            throw new MgInvalidCastException(L"MgdDrawingService.GetSectionResource", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Return the resource via a MgByteReader
        Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)stream, (INT32)nBytes);
        byteSource->SetMimeType(wsMimeType);
        byteReader = byteSource->GetReader();

        if (0 != pStream)
            DWFCORE_FREE_OBJECT(pStream);
        if (0 != pBuffer)
            DWFCORE_FREE_MEMORY(pBuffer);
    }

    MgdDrawingServiceUtil::CloseDrawingResource(m_bOpenTempDwfFile, m_tempDwfFileName);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_SERVER_DRAWING_SERVICE_CATCH(L"MgdDrawingService::GetSectionResource")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_SERVER_DRAWING_SERVICE_THROW()

    return byteReader.Detach();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// EnumerateLayers() returns the layer names in a DWF ePlot section.  An ePlot section is also known as a "sheet" in DWF viewers.
/// The ePlot section is specified by resource identifier (to get the DWF from the repository), and section name.
/// A list of all sections in a DWF can be retrieved from call to MgdDrawingServices::EnumerateSections(),
/// or from the manifest.xml via call to MgdDrawingServices::DescribeDrawing().
//  Refer to the DWF Format Specification at http://viewers/web/Olema/pdf/DWF%206%20Corporate%20Publishing%20Format.pdf for more information on the manifest and sections.
/// </summary>
/// <param name="resource">Input
/// MgResourceIdentifier object identifying the DWF resource
/// </param>
/// <param name="sectionName">Input
/// sectionName specifies the unique name of the section in the DWF resource.  Section names can be retrieved via call to MgdDrawingService::EnumerateSections() or from manifest.xml via call to MgdDrawingService::DescribeDrawing().
/// </param>
/// <returns>
/// Returns the pointer to a StringCollection of layer names.
/// </returns>
/// EXCEPTIONS:
/// - MgInvalidArgumentException if the section name is blank
/// - MgdDwfSectionNotFoundException if the requested section does not exist in the DWF package.
/// - MgdInvalidDwfSectionException if the 2d graphics resource for the DWF section cannot be found.
/// - MgNullReferenceException no data could be read from the DWF resource.
/// - MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing in the resource repository.
/// - MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
/// - MgdInvalidDwfPackageException if the DWF specified by the resource identifier is not a DWF of version 6.0 or greater.
/// - MgdDwfException if the DWF component encounters errors.
/// - MgTemporaryFileNotAvailableException if a temporary file need to complete the operation cannot be generated or accessed.
/// - See MgResourceService for additional exceptions.
MgStringCollection* MgdDrawingService::EnumerateLayers(MgResourceIdentifier* resource, CREFSTRING sectionName)
{
    Ptr<MgStringCollection> layers;

    MG_LOG_OPERATION_MESSAGE(L"EnumerateLayers");

    MG_SERVER_DRAWING_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 2);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(sectionName.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdDrawingService::EnumerateLayers()");

    if (0 == resource)
    {
        throw new MgNullArgumentException(L"MgdDrawingService.EnumerateLayers", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    else if (sectionName.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgdDrawingService.EnumerateLayers",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }
    else
    {
        auto_ptr<DWFPackageReader> reader(MgdDrawingServiceUtil::OpenDrawingResource(m_resourceService, resource, m_bOpenTempDwfFile, m_tempDwfFileName));

        // Check if the section exists in the manifest
        DWFManifest& manifest = reader->getManifest();
        DWFSection* pSection = manifest.findSectionByName(sectionName.c_str());
        if (0 == pSection)
        {
            MgStringCollection arguments;
            arguments.Add(sectionName);
            throw new MgdDwfSectionNotFoundException(L"MgdDrawingService.EnumerateLayers", __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        // Get the resources for the section
        DWFIterator<DWFResource*>* piResources = pSection->findResourcesByRole(DWFXML::kzRole_Graphics2d);
        int count = 0;

        DWFResource* pResource = NULL;

        if (piResources)
        {
            for (; piResources->valid(); piResources->next())
            {
                count++;

                // An ePlot section should have only one 2d streaming graphics resouce (the primary W2D file)
                if (count != 1)
                {
                    MgStringCollection arguments;
                    arguments.Add(sectionName);
                    throw new MgdInvalidDwfSectionException(L"MgdDrawingService.GetSection", __LINE__, __WFILE__, &arguments, L"", NULL);
                }

                pResource = piResources->get();
            }

            DWFCORE_FREE_OBJECT( piResources );
            piResources = NULL;
        }

        if (0 == pResource)
        {
            throw new MgNullReferenceException(L"MgdDrawingService.EnumerateLayers", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        DWFInputStream* pStream = pResource->getInputStream();
        if (0 == pStream)
        {
            throw new MgNullReferenceException(L"MgdDrawingService.EnumerateLayers", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        size_t nBytes = pStream->available();
        char* pBuffer = DWFCORE_ALLOC_MEMORY( char, nBytes );
        pStream->read(pBuffer, nBytes);
        DWFCORE_FREE_OBJECT(pStream);
        if (0 == pBuffer)
        {
            throw new MgNullReferenceException(L"MgdDrawingService.EnumerateLayers", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Write the memory buffer to a temporary file
        m_tempW2dFileName = MgFileUtil::GenerateTempFileName(false, /*NOXLATE*/ L"MGDS");
        m_bOpenTempW2dFile = true;
        string tempW2dFileName = MgUtil::WideCharToMultiByte(m_tempW2dFileName);
        FILE* fp = fopen( tempW2dFileName.c_str(), "wb+" );  // NOXLATE
        if (0 == fp)
        {
            throw new MgTemporaryFileNotAvailableException(L"MgdDrawingService.EnumerateLayers", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        fwrite(pBuffer, sizeof(char), nBytes, fp);
        rewind(fp);
        DWFCORE_FREE_MEMORY(pBuffer);

        // Now that we have the w2d in the form a temporary file,
        // use the WhipToolkit to read the W2D data and extract the layer names

        WT_File file;
        WT_Result result;

        // Set overrides for IO handling of the W2D file
        file.set_stream_open_action (MgdDrawingServiceUtil::MgWt_open);
        file.set_stream_close_action(MgdDrawingServiceUtil::MgWt_close);
        file.set_stream_read_action (MgdDrawingServiceUtil::MgWt_read);

        // Open the W2D for reading
        file.set_file_mode(WT_File::File_Read);
        file.open();
        file.set_stream_user_data(fp);

        // Process the W2D
        layers = new MgStringCollection();
        file.heuristics().set_user_data(layers);
        file.set_layer_action(MgdDrawingServiceUtil::MgWt_process_layer);
        do
        {
            result = file.process_next_object();
        }
        while (WT_Result::Success == result);

        // Finished processing the W2D
        file.close();
        ::remove( tempW2dFileName.c_str() );
        m_bOpenTempW2dFile = false;
    }

    MgdDrawingServiceUtil::CloseDrawingResource(m_bOpenTempDwfFile, m_tempDwfFileName);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_SERVER_DRAWING_SERVICE_CATCH(L"MgdDrawingService::EnumerateLayers")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_SERVER_DRAWING_SERVICE_THROW()

    return layers.Detach();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// GetLayer() extracts a layer from a particular ePlot section of a DWF.
/// </summary>
/// <param name="resource">Input
/// MgResourceIdentifier object identifying the DWF resource
/// </param>
/// <param name="sectionName">Input
/// sectionName specifies the unique name of the section in the DWF resource.  Section names can be retrieved via call to MgdDrawingService::EnumerateSections() or from manifest.xml via call to MgdDrawingService::DescribeDrawing().
/// </param>
/// <param name="layerName">Input
/// layerName specifies the name of the layer to retrieve from a particular section.  A list of layer names can can be retrieved via call to MgdDrawingService::EnumerateLayers().
/// </param>
/// <returns>
/// Returns DWF stream containing the specified layer (in a section)
/// </returns>
/// EXCEPTIONS:
/// - MgInvalidArgumentException if the section name is blank
/// - MgdDwfSectionNotFoundException if the requested section does not exist in the DWF package.
/// - MgdInvalidDwfSectionException if the 2d graphics resource for the DWF section cannot be found.
/// - MgNullReferenceException if data could not be read from the DWF resource.
/// - MgOutOfMemoryException if there is insufficient memory to complete the operation.
/// - MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing in the resource repository.
/// - MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
/// - MgdInvalidDwfPackageException if the DWF specified by the resource identifier is not a DWF of version 6.0 or greater.
/// - MgLayerNotFoundException if the requested layer does not exist in the requested section of the DWF package.
/// - MgdDwfException if the DWF component encounters errors.
/// - See MgResourceService for additional exceptions.
MgByteReader* MgdDrawingService::GetLayer( MgResourceIdentifier* resource, CREFSTRING sectionName, CREFSTRING layerName )
{
    Ptr<MgByteReader> byteReader;
    Ptr<MgStringCollection> layers;

    MG_LOG_OPERATION_MESSAGE(L"GetLayer");

    MG_SERVER_DRAWING_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 3);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(sectionName.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(layerName.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdDrawingService::GetLayer()");

    if (0 == resource)
    {
        throw new MgNullArgumentException(L"MgdDrawingService.GetLayer", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    else if (sectionName.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgdDrawingService.GetLayer",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }
    else
    {
        auto_ptr<DWFPackageReader> reader(MgdDrawingServiceUtil::OpenDrawingResource(m_resourceService, resource, m_bOpenTempDwfFile, m_tempDwfFileName));

        // Check if the section exists in the manifest
        DWFManifest& manifest = reader->getManifest();
        DWFSection* pSection = manifest.findSectionByName(sectionName.c_str());
        if (0 == pSection)
        {
            MgStringCollection arguments;
            arguments.Add(sectionName);
            throw new MgdDwfSectionNotFoundException(L"MgdDrawingService.GetLayer", __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        pSection->readDescriptor();

        // Get the resources for the section
        DWFIterator<DWFResource*>* piResources = pSection->findResourcesByRole(DWFXML::kzRole_Graphics2d);
        int count = 0;

        DWFResource* pResource = NULL;

        if (piResources)
        {
            for (; piResources->valid(); piResources->next())
            {
                count++;

                // An ePlot section should have only one 2d streaming graphics resouce (the primary W2D file)
                if (count != 1)
                {
                    MgStringCollection arguments;
                    arguments.Add(sectionName);
                    throw new MgdInvalidDwfSectionException(L"MgdDrawingService.GetSection", __LINE__, __WFILE__, &arguments, L"", NULL);
                }

                pResource = piResources->get();
            }

            DWFCORE_FREE_OBJECT( piResources );
            piResources = NULL;
        }

        if (0 == pResource)
        {
            throw new MgNullReferenceException(L"MgdDrawingService.EnumerateLayers", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        DWFInputStream* pStream = pResource->getInputStream();
        if (0 == pStream)
        {
            throw new MgNullReferenceException(L"MgdDrawingService.GetLayer", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        size_t nBytes = pStream->available();
        char* pBuffer = DWFCORE_ALLOC_MEMORY( char, nBytes );
        pStream->read(pBuffer, nBytes);
        DWFCORE_FREE_OBJECT(pStream);
        if (0 == pBuffer)
        {
            throw new MgNullReferenceException(L"MgdDrawingService.GetLayer", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Write the memory buffer to a temporary file
        m_tempW2dFileName = MgFileUtil::GenerateTempFileName(false, /*NOXLATE*/ L"MGDS");
        m_bOpenTempW2dFile = true;
        string tempW2dFileName = MgUtil::WideCharToMultiByte(m_tempW2dFileName);
        FILE* fp = fopen( tempW2dFileName.c_str(), "wb+" );  // NOXLATE
        if (0 == fp)
        {
            throw new MgTemporaryFileNotAvailableException(L"MgdDrawingService.GetLayer", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        fwrite(pBuffer, sizeof(char), nBytes, fp);
        rewind(fp);
        DWFCORE_FREE_MEMORY(pBuffer);

        // Now that the section's w2d has been extracted to a temporary file,
        // use the WhipToolkit to read the W2D data

        WT_File infile;
        WT_Result result;

        // Set overrides for IO handling of the W2D file
        infile.set_stream_open_action (MgdDrawingServiceUtil::MgWt_open);
        infile.set_stream_close_action(MgdDrawingServiceUtil::MgWt_close);
        infile.set_stream_read_action (MgdDrawingServiceUtil::MgWt_read);

        // Open the W2D for reading
        infile.set_file_mode(WT_File::File_Read);
        infile.open();
        infile.set_stream_user_data(fp);

        // Open a temporary file for W2D output and set some properties
        m_tempOutputW2dFileName = MgFileUtil::GenerateTempFileName(false, /*NOXLATE*/ L"MGDS");
        m_bOpenTempOutputW2dFile = true;
        string tempOutputW2dFileName = MgUtil::WideCharToMultiByte(m_tempOutputW2dFileName);
        WT_File outfile;
        outfile.set_filename( tempOutputW2dFileName.c_str() );
        outfile.set_file_mode(WT_File::File_Write);
        outfile.open();
        outfile.heuristics().set_allow_binary_data(true);
        outfile.heuristics().set_allow_data_compression(true);

        // Set overrides for extracting appropriate layer geometries to the temporary output W2D file.
        TargetLayer targetLayer;
        targetLayer.number = -1;
        targetLayer.name = layerName;
        targetLayer.pFile = &outfile;
        infile.heuristics().set_user_data(&targetLayer);
        infile.set_color_action(MgdDrawingServiceUtil::MgWt_process_color);
        infile.set_contour_set_action(MgdDrawingServiceUtil::MgWt_process_contour_set);
        infile.set_filled_ellipse_action(MgdDrawingServiceUtil::MgWt_process_filled_ellipse);
        infile.set_gouraud_polyline_action(MgdDrawingServiceUtil::MgWt_process_gouraud_polyline);
        infile.set_gouraud_polytriangle_action(MgdDrawingServiceUtil::MgWt_process_gouraud_polytriangle);
        infile.set_image_action(MgdDrawingServiceUtil::MgWt_process_image);
        infile.set_png_group4_image_action(MgdDrawingServiceUtil::MgWt_process_png_group4_image);
        infile.set_outline_ellipse_action(MgdDrawingServiceUtil::MgWt_process_outline_ellipse);
        infile.set_polygon_action(MgdDrawingServiceUtil::MgWt_process_polygon);
        infile.set_polyline_action(MgdDrawingServiceUtil::MgWt_process_polyline);
        infile.set_polymarker_action(MgdDrawingServiceUtil::MgWt_process_polymarker);
        infile.set_polytriangle_action(MgdDrawingServiceUtil::MgWt_process_polytriangle);
        infile.set_text_action(MgdDrawingServiceUtil::MgWt_process_text);

        //Process the input W2D file.
        do
        {
            result = infile.process_next_object();
        }
        while (WT_Result::Success == result);

        // Finished processing the W2D file
        outfile.close();
        infile.close();
        ::remove( tempW2dFileName.c_str() );
        m_bOpenTempW2dFile = false;

        if (-1 == targetLayer.number)
        {
            // Cannot find specified layer in the Dwf section
            MgStringCollection arguments;
            arguments.Add(targetLayer.name);
            throw new MgLayerNotFoundException(L"MgdDrawingService.GetLayer", __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        // Use EPlotSection to get section color, paper
        const DWFPaper* pPaper = ((DWFEPlotSection*)pSection)->paper();
        UINT32 color = ((DWFEPlotSection*)pSection)->color();

        // Use GraphicResource to get description, transform, clip, etc.
        DWFGraphicResource* dwfGraphicResource = (DWFGraphicResource*)pResource;
        const double* pTransform =((DWFGraphicResource*)pResource)->transform();
        const double* pClip = ((DWFGraphicResource*)pResource)->clip();
        const double* pExtents = ((DWFGraphicResource*)pResource)->extents();

        // Create a DWFEPlotSection
        DWFEPlotSection* pPage = DWFCORE_ALLOC_OBJECT( DWFEPlotSection(
            pSection->title(),
            pSection->objectID(),
            pSection->order(),
            pSection->source(),
            color,
            pPaper) );
        if (0 == pPage)
        {
            throw new MgOutOfMemoryException(L"MgdDrawingService.GetLayer", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        // Set properties for the section
        pPage->provideProperty(
            DWFCORE_ALLOC_OBJECT( DWFProperty( PROPERTY_CREATOR.c_str(), PROPERTY_CREATOR_VALUE.c_str(), L"", L"", L"") ) );

        // Define the 2d graphics resource
        DWFGraphicResource* p2Dgfx = DWFCORE_ALLOC_OBJECT( DWFGraphicResource(
            pResource->title(),
            DWFXML::kzRole_Graphics2d,
            DWFMIME::kzMIMEType_W2D,
            DWF_AUTHOR.c_str(),
            dwfGraphicResource->description(),
            L"",
            L"") );
        if (0 == p2Dgfx)
        {
            throw new MgOutOfMemoryException(L"MgdDrawingService.GetLayer", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        p2Dgfx->configureGraphic(pTransform, pExtents, pClip);

        // Add the resultant W2D file to the temporary DWF
        DWFFile oW2DFilename(tempOutputW2dFileName.c_str());
        DWFStreamFileDescriptor* pW2DFile = DWFCORE_ALLOC_OBJECT( DWFStreamFileDescriptor(oW2DFilename, L"rb") );  // NOXLATE
        if (pW2DFile == NULL)
        {
            DWFCORE_FREE_OBJECT( p2Dgfx );
            throw new MgOutOfMemoryException(L"MgdDrawingService.GetLayer", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        DWFFileInputStream* pW2DFilestream = DWFCORE_ALLOC_OBJECT( DWFFileInputStream );
        if (pW2DFilestream == NULL)
        {
            DWFCORE_FREE_OBJECT( p2Dgfx );
            DWFCORE_FREE_OBJECT( pW2DFile );
            throw new MgOutOfMemoryException(L"MgdDrawingService.GetLayer", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        //... open the file and bind it to the stream
        pW2DFile->open();
        pW2DFilestream->attach( pW2DFile, true );

        // ... hand the stream off to the resource
        // NOTE: since we don't already know the filesize (in the application space - of course we can look on disk...)
        // leave the second parameter (nBytes) default as zero, this will tell the package writer
        // to use the number of bytes it processed through the stream as the size attribute in the descriptor.
        //
        p2Dgfx->setInputStream( pW2DFilestream );

        //... drop resource into the section
        pPage->addResource( p2Dgfx, true );

        // ... create a DWFPackageWriter
        STRING tempDwfPathname = MgFileUtil::GenerateTempFileName(false, /*NOXLATE*/L"MGDS");
        DWFFile oDWF(MgUtil::WideCharToMultiByte(tempDwfPathname).c_str());

        DWFPackageVersionExtension* pVersionExtension =
            DWFCORE_ALLOC_OBJECT(DWFPackageVersionTypeInfoExtension(DWFInterface::kzEPlot_ID));

        DWFPackageWriter oWriter( oDWF, L"", pVersionExtension );

        // ... add the section to the writer
        oWriter.addSection(pPage);

        // ... write the package
        oWriter.write(
            DWF_SOURCE_PRODUCT_VENDOR.c_str(),
            DWF_SOURCE_PRODUCT_NAME.c_str(),
            DWF_SOURCE_PRODUCT_VERSION.c_str(),
            DWF_PRODUCT_VENDOR.c_str(),
            _DWFTK_VERSION_STRING );

        // Return the layer via a MgByteReader
        Ptr<MgByteSource> byteSource = new MgByteSource(tempDwfPathname, true);
        byteSource->SetMimeType(MgMimeType::Dwf);
        byteReader = byteSource->GetReader();

        // Delete the the temporary W2D file
        ::remove(tempOutputW2dFileName.c_str());
        m_bOpenTempOutputW2dFile = false;
    }

    MgdDrawingServiceUtil::CloseDrawingResource(m_bOpenTempDwfFile, m_tempDwfFileName);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_SERVER_DRAWING_SERVICE_CATCH(L"MgdDrawingService::GetLayer")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_SERVER_DRAWING_SERVICE_THROW()

    return byteReader.Detach();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// EnumerateSections() enumerates only the ePlot sections in a DWF.  The returned sections are in ascending order based on its plotOrder attribute.
/// The DWF is identified by it's resource identifier.
/// </summary>
/// <param name="resource">Input
/// MgResourceIdentifier object identifying the DWF resource
/// </param>
/// <returns>
/// Returns MgByteReader object representing the ePlot sections in a DWF sorted in ascending order based on plotOrder attribute.
///
/// </returns>
/// EXCEPTIONS:
/// - MgNullReferenceException no data could be read from the DWF resource.
/// - MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing in the resource repository.
/// - MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
/// - MgdInvalidDwfPackageException if the DWF specified by the resource identifier is not a DWF of version 6.0 or greater.
/// - MgdDwfException if the DWF component encounters errors.
/// - See MgResourceService for additional exceptions.
MgByteReader* MgdDrawingService::EnumerateSections(MgResourceIdentifier* resource)
{
    Ptr<MgByteReader> byteReader;

    MG_LOG_OPERATION_MESSAGE(L"EnumerateSections");

    MG_SERVER_DRAWING_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 1);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdDrawingService::EnumerateSections()");

    if (0 == resource)
    {
        throw new MgNullArgumentException(L"MgdDrawingService.EnumerateSections", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    else
    {
        auto_ptr<DWFPackageReader> reader(MgdDrawingServiceUtil::OpenDrawingResource(m_resourceService, resource, m_bOpenTempDwfFile, m_tempDwfFileName));

        // Get the ePlot sections in the DWF
        DWFManifest& manifest = reader->getManifest();
        DWFManifest::SectionIterator* iSection = manifest.findSectionsByType(L"com.autodesk.dwf.ePlot");  // NOXLATE

        // Iterate through the sections to get the names and write to xml document
        // This XML follows the DrawingSectionList-1.0.0.xsd schema
        string sections = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";  // NOXLATE
        sections += "<DrawingSectionList xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"DrawingSectionList-1.0.0.xsd\">\n";  // NOXLATE
        DWFSection* pSection;

        std::multimap<double, string> section_list;
        typedef pair<double, string> double_string_pair;

        if (iSection)
        {
            for (; iSection->valid(); iSection->next())
            {
                string strSection = "";

                pSection = iSection->get();

                strSection += "\t<Section>\n";  // NOXLATE

                strSection += "\t\t<Name>";  // NOXLATE
                if (pSection->name())
                {
                    STRING temp = (const wchar_t*)pSection->name();
                    strSection += MgUtil::WideCharToMultiByte( temp );
                }
                strSection += "</Name>\n";  // NOXLATE

                strSection += "\t\t<Type>";  // NOXLATE
                if (pSection->type())
                {
                    STRING temp = (const wchar_t*)pSection->type();
                    strSection += MgUtil::WideCharToMultiByte( temp );
                }
                strSection += "</Type>\n";  // NOXLATE

                strSection += "\t\t<Title>";  // NOXLATE
                if (pSection->title())
                {
                    STRING temp = (const wchar_t*)pSection->title();
                    strSection += MgUtil::WideCharToMultiByte( temp );
                }
                strSection += "</Title>\n";  // NOXLATE

                strSection += "\t</Section>\n";  // NOXLATE

                pSection->readDescriptor();

                section_list.insert( double_string_pair(pSection->order(), strSection) );
            }

            DWFCORE_FREE_OBJECT( iSection );
            iSection = NULL;

            std::multimap<double, string>::iterator iter;
            for (iter = section_list.begin(); iter != section_list.end(); iter++)
            {
                sections += iter->second;
            }
        }

        sections += "</DrawingSectionList>";   // NOXLATE

        // Create a byte reader.
        Ptr<MgByteSource> byteSource = new MgByteSource( (unsigned char*)sections.c_str(), (INT32)sections.length());
        byteSource->SetMimeType(MgMimeType::Xml);
        byteReader = byteSource->GetReader();
    }

    MgdDrawingServiceUtil::CloseDrawingResource(m_bOpenTempDwfFile, m_tempDwfFileName);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_SERVER_DRAWING_SERVICE_CATCH(L"MgdDrawingService::EnumerateSections")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_SERVER_DRAWING_SERVICE_THROW()

    return byteReader.Detach();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// EnumerateSectionResources() enumerates the resources of a DWF section.  The DWF is identified by it's
/// resource identifier and the section is identified by name.  The section name
/// will be retrieved from manifest.xml or from MgdDrawingServices::EnumerateSections() API.
/// </summary>
/// <param name="resource">Input
/// MgResourceIdentifier object identifying the DWF resource
/// </param>
/// <param name="sectionName">Input
/// sectionName specifies the unique name of the section in the DWF resource.  Section names can be retrieved via call to MgdDrawingService::EnumerateSections() or from manifest.xml via call to MgdDrawingService::DescribeDrawing().
/// </param>
/// <returns>
/// Returns MgByteReader object representing resources in a DWF section.
///
/// </returns>
/// EXCEPTIONS:
/// - MgInvalidArgumentException if the section name is blank
/// - MgdDwfSectionNotFoundException if the requested section does not exist in the DWF package.
/// - MgdInvalidDwfSectionException if the 2d graphics resource for the DWF section cannot be found.
/// - MgNullReferenceException no data could be read from the DWF resource.
/// - MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing in the resource repository.
/// - MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
/// - MgdInvalidDwfPackageException if the DWF specified by the resource identifier is not a DWF of version 6.0 or greater.
/// - MgdDwfException if the DWF component encounters errors.
/// - See MgResourceService for additional exceptions.
MgByteReader* MgdDrawingService::EnumerateSectionResources(MgResourceIdentifier* resource, CREFSTRING sectionName)
{
    Ptr<MgByteReader> byteReader;

    MG_LOG_OPERATION_MESSAGE(L"EnumerateSectionResources");

    MG_SERVER_DRAWING_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 2);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(sectionName.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdDrawingService::EnumerateSectionResources()");

    if (0 == resource)
    {
        throw new MgNullArgumentException(L"MgdDrawingService.EnumerateSectionResources", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    else if (sectionName.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgdDrawingService.EnumerateSectionResources",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }
    else
    {
        auto_ptr<DWFPackageReader> reader(MgdDrawingServiceUtil::OpenDrawingResource(m_resourceService, resource, m_bOpenTempDwfFile, m_tempDwfFileName));

        // Check if the section exists in the manifest
        DWFManifest& manifest = reader->getManifest();
        DWFSection* pSection = manifest.findSectionByName(sectionName.c_str());
        if (0 == pSection)
        {
            MgStringCollection arguments;
            arguments.Add(sectionName);
            throw new MgdDwfSectionNotFoundException(L"MgdDrawingService.EnumerateSectionResources", __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        // Get all the resources in the section
        DWFResourceContainer::ResourceKVIterator* iResources = pSection->getResourcesByHREF();

        if (0 == iResources)
        {
            MgStringCollection arguments;
            arguments.Add(sectionName);
            throw new MgdInvalidDwfSectionException(L"MgdDrawingService.EnumerateSectionResources", __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        // Iterate through the resources and write to xml document
        // This XML follows the DrawingSectionResourceList-1.0.0.xsd schema
        string sectionResources = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        sectionResources += "<DrawingSectionResourceList xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"DrawingSectionResourceList-1.0.0.xsd\">\n";
        DWFResource* pResource;

        for (; iResources->valid(); iResources->next())
        {
            pResource = iResources->value();

            sectionResources += "\t<SectionResource>\n";

            sectionResources += "\t\t<Href>";
            if (pResource->href())
            {
                STRING temp = (const wchar_t*)pResource->href();
                sectionResources += MgUtil::WideCharToMultiByte( temp );
            }
            sectionResources += "</Href>";

            sectionResources += "\t\t<Role>";
            if (pResource->role())
            {
                STRING temp = (const wchar_t*)pResource->role();
                sectionResources += MgUtil::WideCharToMultiByte( temp );
            }
            sectionResources += "</Role>";

            sectionResources += "\t\t<Mime>";
            if (pResource->mime())
            {
                STRING temp = (const wchar_t*)pResource->mime();
                sectionResources += MgUtil::WideCharToMultiByte( temp );
            }
            sectionResources += "</Mime>";

            sectionResources += "\t\t<Title>";
            if (pResource->title())
            {
                STRING temp = (const wchar_t*)pResource->title();
                sectionResources += MgUtil::WideCharToMultiByte( temp );
            }
            sectionResources += "</Title>\n";

            sectionResources += "\t</SectionResource>\n";
        }

        DWFCORE_FREE_OBJECT( iResources );
        iResources = NULL;

        sectionResources += "</DrawingSectionResourceList>";

        // Create a byte reader.
        Ptr<MgByteSource> byteSource = new MgByteSource( (unsigned char*)sectionResources.c_str(), (INT32)sectionResources.length());
        byteSource->SetMimeType(MgMimeType::Xml);
        byteReader = byteSource->GetReader();
    }

    MgdDrawingServiceUtil::CloseDrawingResource(m_bOpenTempDwfFile, m_tempDwfFileName);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_SERVER_DRAWING_SERVICE_CATCH(L"MgdDrawingService::EnumerateSections")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_SERVER_DRAWING_SERVICE_THROW()

    return byteReader.Detach();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// GetCoordinateSpace() returns the coordinate system assigned to the DWF drawing.
/// The coordinate system will be retrieved from the drawing source content document.
/// If no coordinate system is specified, the default will be LL84.
/// </summary>
/// <param name="resource">Input
/// MgResourceIdentifier object identifying the DWF resource
/// </param>
/// <returns>
/// Returns the STRING representing the coordinate system in AWKT format.
/// </returns>
/// EXCEPTIONS:
/// - MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing resource in the repository.
/// - MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
/// - See MgResourceService for additional exceptions.
STRING MgdDrawingService::GetCoordinateSpace(MgResourceIdentifier* resource)
{
    STRING dwfCoordinateSpace = L"";

    MG_LOG_OPERATION_MESSAGE(L"GetCoordinateSpace");

    MG_SERVER_DRAWING_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 1);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdDrawingService::GetCoordinateSpace()");

    if (0 == resource)
    {
        throw new MgNullArgumentException(L"MgdDrawingService.GetCoordinateSpace", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Get the coordinate space from the resource content.
    STRING dwfFileName = L"";
    Ptr<MgByteReader> reader = m_resourceService->GetResourceContent(resource, L"");
    MgdDrawingServiceUtil::ParseDrawingResourceContent(reader, dwfFileName, dwfCoordinateSpace);

    // Assume coordinate space is LL84 if none is specified in the resource content.
    if (dwfCoordinateSpace.empty())
    {
        dwfCoordinateSpace = DEFAULT_COORDINATE_SPACE;
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_SERVER_DRAWING_SERVICE_CATCH(L"MgdDrawingService::GetCoordinateSpace")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_SERVER_DRAWING_SERVICE_THROW()

    return dwfCoordinateSpace;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Construct an MgdDrawingService object
/// </summary>
MgdDrawingService::MgdDrawingService() :
    MgService(),
    m_bOpenTempDwfFile(false),
    m_bOpenTempW2dFile(false),
    m_bOpenTempOutputW2dFile(false)
{
    MG_SERVER_DRAWING_SERVICE_TRY()

        /*
    MgServiceManager* serviceMan = MgServiceManager::GetInstance();
    assert (NULL != serviceMan);

    m_resourceService = dynamic_cast<MgResourceService*>(
        serviceMan->RequestService(MgServiceType::ResourceService));
        */
    Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
    m_resourceService = static_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
    assert(m_resourceService != NULL);

    MG_SERVER_DRAWING_SERVICE_CATCH_AND_THROW(L"MgdDrawingService::MgdDrawingService")
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor
/// </summary>
MgdDrawingService::~MgdDrawingService()
{
    MG_SERVER_DRAWING_SERVICE_TRY()

    CleanUpTempFiles();

    MG_SERVER_DRAWING_SERVICE_CATCH(L"MgdDrawingService::~MgdDrawingService")
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Deletes the temporary files that have been created.
/// </summary>
/// <returns>
/// Nothing
/// </returns>
void MgdDrawingService::CleanUpTempFiles()
{
    MgdDrawingServiceUtil::CloseDrawingResource(m_bOpenTempDwfFile, m_tempDwfFileName);

    if (m_bOpenTempW2dFile)
    {
        if (MgFileUtil::PathnameExists(m_tempW2dFileName))
        {
            MgFileUtil::DeleteFile(m_tempW2dFileName);
            m_bOpenTempW2dFile = false;
        }
    }

    if (m_bOpenTempOutputW2dFile)
    {
        if (MgFileUtil::PathnameExists(m_tempOutputW2dFileName))
        {
            MgFileUtil::DeleteFile(m_tempOutputW2dFileName);
            m_bOpenTempOutputW2dFile = false;
        }
    }
}