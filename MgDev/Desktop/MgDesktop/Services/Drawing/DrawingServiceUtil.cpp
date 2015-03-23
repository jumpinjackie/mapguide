#include "MgDesktop.h"
#include "DrawingServiceUtil.h"
#include "DrawingDefs.h"
#include "System/XmlUtil.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgdDrawingServiceUtil::MgdDrawingServiceUtil()
{
}


///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgdDrawingServiceUtil::~MgdDrawingServiceUtil()
{
}


WT_Result MgdDrawingServiceUtil::MgWt_read(WT_File &file, int desired_bytes, int &bytes_read, void * buffer)
{
    FILE * fp = (FILE *) file.stream_user_data();
    if (feof(fp))
        return WT_Result::End_Of_File_Error;

    bytes_read = (int) fread(buffer, sizeof(WT_Byte), desired_bytes, fp);
    if (!bytes_read)
        return WT_Result::Unknown_File_Read_Error;

    return WT_Result::Success;
}


WT_Result MgdDrawingServiceUtil::MgWt_open(WT_File &)
{
    return WT_Result::Success;
}


WT_Result MgdDrawingServiceUtil::MgWt_close(WT_File &file)
{
    FILE * fp = (FILE *) file.stream_user_data();
    fclose (fp);
    file.set_stream_user_data(WD_Null);
    return WT_Result::Success;
}


WT_Result MgdDrawingServiceUtil::MgWt_process_layer(WT_Layer &layer, WT_File &file)
{
    MgStringCollection* layerList = (MgStringCollection*) file.heuristics().user_data();
    WT_String layerName = layer.layer_name();

    if (layerName)
    {
        wchar_t* name = WT_String::to_wchar(layerName.length(), layerName.unicode());
        layerList->Add(name);
        delete [] name;
    }

    return WT_Result::Success;
}


WT_Result MgdDrawingServiceUtil::MgWt_process_color(WT_Color &color, WT_File &file)
{
    TargetLayer* targetLayer = (TargetLayer*)file.heuristics().user_data();
    if (0 == targetLayer)
    {
        return WT_Result::Decompression_Terminated;
    }
    WD_CHECK(color.serialize(*(targetLayer->pFile)));

    return WT_Result::Success;
}


WT_Result MgdDrawingServiceUtil::MgWt_process_contour_set(WT_Contour_Set &contour_set, WT_File &file)
{
    if ( DetectTargetLayer(file) )
    {
        // write the polyline to the output W2D/DWF.
        TargetLayer* targetLayer = (TargetLayer*)file.heuristics().user_data();
        WD_CHECK(contour_set.serialize(*(targetLayer->pFile)));
    }

    return WT_Result::Success;
}


WT_Result MgdDrawingServiceUtil::MgWt_process_filled_ellipse(WT_Filled_Ellipse &filled_ellipse, WT_File &file)
{
    if ( DetectTargetLayer(file) )
    {
        // write the outline_ellipse to the output W2D/DWF
        TargetLayer* targetLayer = (TargetLayer*)file.heuristics().user_data();
        WD_CHECK(filled_ellipse.serialize(*(targetLayer->pFile)));
    }

    return WT_Result::Success;
}


WT_Result MgdDrawingServiceUtil::MgWt_process_gouraud_polyline(WT_Gouraud_Polyline &gouraud_polyline, WT_File &file)
{
    if ( DetectTargetLayer(file) )
    {
        // write the outline_ellipse to the output W2D/DWF
        TargetLayer* targetLayer = (TargetLayer*)file.heuristics().user_data();
        WD_CHECK(gouraud_polyline.serialize(*(targetLayer->pFile)));
    }

    return WT_Result::Success;
}


WT_Result MgdDrawingServiceUtil::MgWt_process_gouraud_polytriangle(WT_Gouraud_Polytriangle &gouraud_polytriangle, WT_File &file)
{
    if ( DetectTargetLayer(file) )
    {
        // write the outline_ellipse to the output W2D/DWF
        TargetLayer* targetLayer = (TargetLayer*)file.heuristics().user_data();
        WD_CHECK(gouraud_polytriangle.serialize(*(targetLayer->pFile)));
    }

    return WT_Result::Success;
}


WT_Result MgdDrawingServiceUtil::MgWt_process_image(WT_Image &image, WT_File &file)
{
    if ( DetectTargetLayer(file) )
    {
        // write the outline_ellipse to the output W2D/DWF
        TargetLayer* targetLayer = (TargetLayer*)file.heuristics().user_data();
        WD_CHECK(image.serialize(*(targetLayer->pFile)));
    }

    return WT_Result::Success;
}


WT_Result MgdDrawingServiceUtil::MgWt_process_png_group4_image(WT_PNG_Group4_Image &image, WT_File &file)
{
    if ( DetectTargetLayer(file) )
    {
        // write the outline_ellipse to the output W2D/DWF
        TargetLayer* targetLayer = (TargetLayer*)file.heuristics().user_data();
        WD_CHECK(image.serialize(*(targetLayer->pFile)));
    }

    return WT_Result::Success;
}


WT_Result MgdDrawingServiceUtil::MgWt_process_outline_ellipse(WT_Outline_Ellipse &outline_ellipse, WT_File &file)
{
    if ( DetectTargetLayer(file) )
    {
        // write the outline_ellipse to the output W2D/DWF
        TargetLayer* targetLayer = (TargetLayer*)file.heuristics().user_data();
        WD_CHECK(outline_ellipse.serialize(*(targetLayer->pFile)));
    }

    return WT_Result::Success;
}


WT_Result MgdDrawingServiceUtil::MgWt_process_polyline(WT_Polyline &polyline, WT_File &file)
{
    if ( DetectTargetLayer(file) )
    {
        // write the polyline to the output W2D/DWF.
        TargetLayer* targetLayer = (TargetLayer*)file.heuristics().user_data();
        WD_CHECK(polyline.serialize(*(targetLayer->pFile)));
    }

    return WT_Result::Success;
}


WT_Result MgdDrawingServiceUtil::MgWt_process_polygon(WT_Polygon &polygon, WT_File &file)
{
    if ( DetectTargetLayer(file) )
    {
        // write the polygon to the output W2D/DWF.
        TargetLayer* targetLayer = (TargetLayer*)file.heuristics().user_data();
        WD_CHECK(polygon.serialize(*(targetLayer->pFile)));
    }

    return WT_Result::Success;
}


WT_Result MgdDrawingServiceUtil::MgWt_process_polymarker(WT_Polymarker &polymarker, WT_File &file)
{
    if ( DetectTargetLayer(file) )
    {
        TargetLayer* targetLayer = (TargetLayer*)file.heuristics().user_data();
        if (file.heuristics().target_version() >= REVISION_WHEN_MACRO_IS_SUPPORTED)
        {
            WT_Macro_Draw macro_draw(polymarker.count(), polymarker.points(), true);
            WD_CHECK(macro_draw.serialize(*(targetLayer->pFile)));
        }
        else
        {
            WD_CHECK(polymarker.serialize(*(targetLayer->pFile)));
        }
    }

    return WT_Result::Success;
}


WT_Result MgdDrawingServiceUtil::MgWt_process_polytriangle(WT_Polytriangle &polytriangle, WT_File &file)
{
    if ( DetectTargetLayer(file) )
    {
        // write the polytriangle to the output W2D/DWF.
        TargetLayer* targetLayer = (TargetLayer*)file.heuristics().user_data();
        WD_CHECK(polytriangle.serialize(*(targetLayer->pFile)));
    }

    return WT_Result::Success;
}


WT_Result MgdDrawingServiceUtil::MgWt_process_text(WT_Text &text, WT_File &file)
{
    if ( DetectTargetLayer(file) )
    {
        // write the text to the output W2D/DWF.
        TargetLayer* targetLayer = (TargetLayer*)file.heuristics().user_data();
        WD_CHECK(text.serialize(*(targetLayer->pFile)));
    }

    return WT_Result::Success;
}


bool MgdDrawingServiceUtil::DetectTargetLayer(WT_File &file)
{
    bool bIsTargetLayer = false;
    TargetLayer* targetLayer = (TargetLayer*)file.heuristics().user_data();

    if (0 != targetLayer)
    {
        // The layer_name is referenced in the rendition only once.  Subsequent references to the layer are
        // identified by layer_num.  Thus, we need to extract the layer_num for the layer_name that we are interested in.
        WT_Layer currentLayer = file.rendition().layer();
        if ( currentLayer.layer_name().ascii() )
        {
            STRING layerName = MgUtil::MultiByteToWideChar( string(currentLayer.layer_name().ascii()) );
            if (targetLayer->name == layerName)
            {
                // Set the layer_num for identifying the desired layer
                targetLayer->number = currentLayer.layer_num();
            }
        }

        // The layer name has already been processed, and we must now refer to that layer by its layer_num.
        // So, if the layer_num matches the targetLayerNum, we know that we are interested in this layer.
        if (currentLayer.layer_num() == targetLayer->number)
        {
            bIsTargetLayer = true;
        }
    }

    return bIsTargetLayer;
}


DWFPackageReader* MgdDrawingServiceUtil::OpenDrawingResource(
    MgResourceService* resourceService, MgResourceIdentifier* resource,
    bool& bOpenTempFile, REFSTRING tempFileName)
{
    Ptr<MgByteReader> byteReader;
    auto_ptr<DWFPackageReader> reader;

    MG_SERVER_DRAWING_SERVICE_TRY()

    if (resourceService == 0)
    {
        throw new MgServiceNotAvailableException(L"MgdDrawingServiceUtil.OpenDrawingResource", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    byteReader = resourceService->GetResourceContent(resource, L"Substitution");

    // Parse the content to obtain the path, filename and coordinate space
    STRING dwfFileName = L"";
    STRING dwfCoordinateSpace = L"";
    ParseDrawingResourceContent(byteReader, dwfFileName, dwfCoordinateSpace);

    bool bLocalDataFile = MgFileUtil::PathnameExists(dwfFileName);

    // If the DWF file does not exist on the local disk, must get it from the server
    if (!bLocalDataFile)
    {
        // Remove path from the filename and retrieve the data file from resource service
        size_t index = dwfFileName.rfind(L"/");
        STRING dataName = dwfFileName.substr(index + 1);
        byteReader = resourceService->GetResourceData(resource, dataName, L"");

        // Write the DWF to a temporary file
        bOpenTempFile = true;
        tempFileName = MgFileUtil::GenerateTempFileName(true, L"", L".DWF");  // NOXLATE
        Ptr<MgByteSink> byteSink = new MgByteSink(byteReader);
        byteSink->ToFile(tempFileName);
        dwfFileName = tempFileName;
    }

    // Create the DWF reader
    DWFFile dwfFile( (MgUtil::WideCharToMultiByte(dwfFileName)).c_str() );
    DWFString password = "";
    reader.reset( new DWFPackageReader( dwfFile, password ) );

    // Open the DWF package and validate it
    DWFPackageReader::tPackageInfo tInfo;
    reader->getPackageInfo( tInfo );
    if (tInfo.eType != DWFPackageReader::eDWFPackage)
    {
        MgStringCollection arguments;
        arguments.Add(dwfFileName);
        throw new MgdInvalidDwfPackageException(L"MgdDrawingServiceUtil.OpenDrawingResource", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    MG_SERVER_DRAWING_SERVICE_CATCH_AND_THROW(L"MgdDrawingServiceUtil.OpenDrawingResource")

    return reader.release();
}


void MgdDrawingServiceUtil::CloseDrawingResource(bool& bOpenTempFile, CREFSTRING tempFileName)
{
    if (bOpenTempFile)
    {
        if (MgFileUtil::PathnameExists(tempFileName))
        {
            MgFileUtil::DeleteFile(tempFileName);
            bOpenTempFile = false;
        }
    }
}


void MgdDrawingServiceUtil::ParseDrawingResourceContent(
    MgByteReader* byteReader, REFSTRING dwfPathName, REFSTRING dwfCoordinateSpace)
{
    try
    {
        MgXmlUtil xmlUtil;
        xmlUtil.ParseString( MgUtil::GetTextFromReader( byteReader ).c_str() );
        DOMNodeList* nodeList = xmlUtil.GetNodeList( MgUtil::WideCharToMultiByte(ELEMENT_SOURCENAME).c_str() );

        // Get the fully qualified name of DWF file
        if (nodeList->getLength() == 1)
        {
            DOMNode* node = nodeList->item(0);
            DOMNode* childNode = node->getFirstChild();
            if (0 != childNode)
            {
                const XMLCh* nodeValue = childNode->getNodeValue();
                dwfPathName = X2W(nodeValue);
            }
        }

        // Get the coordinate space for the drawing
        nodeList = xmlUtil.GetNodeList( MgUtil::WideCharToMultiByte(ELEMENT_COORDINATE_SPACE).c_str() );
        if (nodeList->getLength() == 1)
        {
            DOMNode* node = nodeList->item(0);
            DOMNode* childNode = node->getFirstChild();
            if (0 != childNode)
            {
                const XMLCh* nodeValue = childNode->getNodeValue();

                dwfCoordinateSpace.append(X2W(nodeValue));
            }
        }
    }
    catch (const XMLException& e)
    {
        MgStringCollection arguments;
        arguments.Add(X2W(e.getMessage()));
        throw new MgXmlParserException(L"MgdDrawingServiceUtil.ParseDrawingResourceContent",
            __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch (const DOMException& e)
    {
        MgStringCollection arguments;
        arguments.Add(X2W(e.msg));
        throw new MgXmlParserException(L"MgdDrawingServiceUtil.ParseDrawingResourceContent",
            __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch (...)
    {
        throw new MgUnclassifiedException(L"MgdDrawingServiceUtil.ParseDrawingResourceContent",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }
}
