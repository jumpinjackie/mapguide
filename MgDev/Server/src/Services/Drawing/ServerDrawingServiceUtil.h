//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifndef MGSERVERDRAWINGSERVICEUTIL_H_
#define MGSERVERDRAWINGSERVICEUTIL_H_

#include "dwfcore/String.h"
#include "dwf/package/reader/PackageReader.h"
#include "dwf/whiptk/whip_toolkit.h"

// disable C4244 of XSerializeEngine about __w64 int assigned to unsigned long
#pragma warning(push)
#pragma warning(disable: 4244)
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#pragma warning(pop)

#include "ServerResourceService.h"

#include <assert.h>

using namespace xercesc;
using namespace DWFToolkit;

struct TargetLayer
{
    STRING name;
    INT32 number;
    WT_File* pFile;
};


class MG_SERVER_DRAWING_API MgServerDrawingServiceUtil
{
    /// Constructors/Destructor

    public:

        MgServerDrawingServiceUtil();
        virtual ~MgServerDrawingServiceUtil();

    private:

        // Unimplemented Methods

        MgServerDrawingServiceUtil(const MgServerDrawingServiceUtil& util);
        MgServerDrawingServiceUtil& operator=(const MgServerDrawingServiceUtil& util);

/// Methods

    public:

        static WT_Result MgWt_open (WT_File &file);
        static WT_Result MgWt_close (WT_File &file);
        static WT_Result MgWt_read (WT_File &file, int desired_bytes, int &bytes_read, void * buffer);
        static WT_Result MgWt_process_layer(WT_Layer &layer, WT_File &file);

        static WT_Result MgWt_process_color(WT_Color &color, WT_File &file);
        static WT_Result MgWt_process_contour_set(WT_Contour_Set &contour_set, WT_File &file);
        static WT_Result MgWt_process_filled_ellipse(WT_Filled_Ellipse &filled_ellipse, WT_File &file);
        static WT_Result MgWt_process_gouraud_polyline(WT_Gouraud_Polyline &gouraud_polyline, WT_File &file);
        static WT_Result MgWt_process_gouraud_polytriangle(WT_Gouraud_Polytriangle &gouraud_polytriangle, WT_File &file);
        static WT_Result MgWt_process_image(WT_Image &image, WT_File &file);
        static WT_Result MgWt_process_png_group4_image(WT_PNG_Group4_Image &image, WT_File &file);
        static WT_Result MgWt_process_outline_ellipse(WT_Outline_Ellipse &outline_ellipse, WT_File &file);
        static WT_Result MgWt_process_polygon(WT_Polygon &polygon, WT_File &file);
        static WT_Result MgWt_process_polyline(WT_Polyline &polyline, WT_File &file);
        static WT_Result MgWt_process_polymarker(WT_Polymarker &polymarker, WT_File &file);
        static WT_Result MgWt_process_polytriangle(WT_Polytriangle &polytriangle, WT_File &file);
        static WT_Result MgWt_process_text(WT_Text &text, WT_File &file);

        static bool DetectTargetLayer(WT_File &file);
        static DWFPackageReader* OpenDrawingResource(
            MgResourceService* resourceService, MgResourceIdentifier* resource,
            bool& bOpenTempFile, REFSTRING tempFileName);
        static void CloseDrawingResource(bool& bOpenTempFile, CREFSTRING tempFileName);
        static void ParseDrawingResourceContent(
            MgByteReader* byteReader, REFSTRING dwfPathName, REFSTRING dwfCoordinateSpace);

};

#endif
