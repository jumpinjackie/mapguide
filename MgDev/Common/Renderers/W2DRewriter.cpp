//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

#include "stdafx.h"
#include "W2DRewriter.h"
#include "DWFRenderer.h"

////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------
//
//                    I/O Overloads
//
//------------------------------------------------------------------
////////////////////////////////////////////////////////////////////


WT_Result simple_open(WT_File & /*file*/)
{
    return WT_Result::Success;
}


WT_Result simple_close(WT_File & file)
{
    file.set_stream_user_data(WD_Null);
    return WT_Result::Success;
}


WT_Result simple_read(WT_File & file, int desired_bytes, int & bytes_read, void * buffer)
{
    bytes_read = (int)((DWFRenderer*)file.stream_user_data())->_GetInputStream()->read(buffer, desired_bytes);

    if (!bytes_read)
        return WT_Result::Unknown_File_Read_Error;

    return WT_Result::Success;
}


WT_Result simple_seek(WT_File & file, int distance, int & amount_seeked)
{
    off_t prev = 0;
    ((DWFRenderer*)file.stream_user_data())->_GetInputStream()->seek(distance, prev);
    amount_seeked = distance;
    return WT_Result::Success;
}

////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------
//
//                  Implementation of Actions....
//
//------------------------------------------------------------------
////////////////////////////////////////////////////////////////////


WT_Result simple_process_author(WT_Author & /*author*/, WT_File & /*file*/)
{
    //DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    //author.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_creator(WT_Creator & /*creator*/, WT_File & /*file*/)
{
    //DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    //creator.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_created(WT_Creation_Time & /*created*/, WT_File & /*file*/)
{
    //DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    //created.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_modified(WT_Modification_Time & /*modified*/, WT_File & /*file*/)
{
    //DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    //modified.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_sourceFilename(WT_Source_Filename & /*sourceFilename*/, WT_File & /*file*/)
{
    //DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    //sourceFilename.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_sourceCreationTime(WT_Source_Creation_Time & /*sourceCreated*/, WT_File & /*file*/)
{
    //DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    //sourceCreated.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_sourceModificationTime(WT_Source_Modification_Time & /*sourceModified*/, WT_File & /*file*/)
{
    //DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    //sourceModified.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_units(WT_Units & units, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    //do not write the units -- the destination W2D has different units
    //units.serialize(*rewriter->_GetW2D());

    //Instead save the in the desired rendition of the source file
    //so that we can get to them when transforming coordinates for DWF layers
    //Note that for DWF point symbols we do not do that
    if (!rewriter->IsSymbolW2D())
        file.desired_rendition().drawing_info().units() = units;

    return WT_Result::Success;
}


WT_Result simple_process_embed(WT_Embed & /*embed*/, WT_File & /*file*/)
{
    //DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    //embed.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_namedView(WT_Named_View & namedView, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();

    //convert the named view box to new coord sys
    int npts = 0;
    std::vector<int>* cntrs = NULL;

    const WT_Logical_Point* pts = rewriter->ProcessW2DPoints(
        file, (WT_Logical_Point*)namedView.view(), 2, LineBuffer::ctNone, npts, &cntrs);

    //create and serialize the transformed view
    WT_Logical_Box box2(pts[0], pts[1]);
    WT_Named_View view2(box2, namedView.name());
    //view2.serialize(*rewriter->_GetW2D());

    return WT_Result::Success;
}


WT_Result simple_process_view(WT_View & view, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();

    //convert the view box to new coord sys
    int npts = 0;
    std::vector<int>* cntrs = NULL;

    const WT_Logical_Point* pts = rewriter->ProcessW2DPoints(
        file, (WT_Logical_Point*)&view.view(), 2, LineBuffer::ctNone, npts, &cntrs);

    //create and serialize the transformed view
    WT_Logical_Box box2(pts[0], pts[1]);
    WT_View view2(box2);
    //view2.serialize(*rewriter->_GetW2D());

    return WT_Result::Success;
}


WT_Result simple_process_plotInfo(WT_Plot_Info & /*plotInfo*/, WT_File & /*file*/)
{
    //DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    //plotInfo.serialize(*rewriter->_GetW2D());

    return WT_Result::Success;
}


WT_Result simple_process_background(WT_Background & background, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    background.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_color(WT_Color & color, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    (*rewriter->_GetW2D()).desired_rendition().color() = color;
    return WT_Result::Success;
}


WT_Result simple_process_lineWeight(WT_Line_Weight & lineWeight, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();

    //rescale the line weight
    WT_Integer32 dstWeight = rewriter->ScaleW2DNumber(file, lineWeight.weight_value());
    (*rewriter->_GetW2D()).desired_rendition().line_weight() = WT_Line_Weight(dstWeight);
    return WT_Result::Success;
}


WT_Result simple_process_lineStyle(WT_Line_Style & lineStyle, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    (*rewriter->_GetW2D()).desired_rendition().line_style() = lineStyle;
    return WT_Result::Success;
}


WT_Result simple_process_layer(WT_Layer & layer, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();

    //Most of the time layers are referred to by their integer id only.
    //Only the first time they are identified by name which we need to keep
    //track of.

    //first see if we already have this layer
    WT_Layer* layer2 = file.layer_list().find_layer_from_index(layer.layer_num());

    //if we don't have it in the list, add it
    if (!layer2)
    {
        file.layer_list().add_layer(layer);
        layer2 = &layer;
    }

    wchar_t* name = WT_String::to_wchar(layer2->layer_name().length(), layer2->layer_name().unicode());

    //check if the current layer was requested by looking it up
    //in the W2D layer filter that was passes to the renderer
    RS_String filter = rewriter->GetLayerFilter();

    if (!filter.empty())
    {
        rewriter->LayerPassesFilter() = false;

        wchar_t* strTok = (wchar_t*)alloca(sizeof(wchar_t) * (filter.length() + 1));
        wcscpy(strTok, filter.c_str());

        wchar_t* ptr;
        wchar_t* token = _wcstok(strTok, L",", &ptr);
        while (token)
        {
            if (wcscmp(token, name) == 0)
            {
                rewriter->LayerPassesFilter() = true;
                break;
            }

            token = _wcstok(NULL, L",", &ptr);
        }
    }

    delete [] name;

    if (rewriter->LayerPassesFilter())
        (*rewriter->_GetW2D()).desired_rendition().layer() = layer;

    return WT_Result::Success;
}


WT_Result simple_process_viewport(WT_Viewport & viewport, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();

    //IMPORTANT: AutoCAD defines a viewport for each view of the
    //drawing displayed in paper space. These views are arranged
    //to fit on the page via several of the viewports.
    //For symbols coming from the DWG Extract and Transform engine, only one viewport
    //is set, corresponding to the extent all the symbol entities.
    //We use this viewport setting in order to determine the extent
    //of the W2D entities in the symbol W2D, since they are not the
    //same as the extent that Studio will use when generating symbol
    //W2Ds (0, 0, SYMBOL_MAX, SYMBOL_MAX).

    //In the case this code is used for DWF layers, things are more complicated
    //as there are often multiple viewports, defining extents for part of the data.
    //We cannot handle this for now

    if (rewriter->IsSymbolW2D() && ! rewriter->IsViewportSet())
        rewriter->UpdateSymbolTrans(file, viewport);

    return WT_Result::Success;
}


WT_Result simple_process_visibility(WT_Visibility & visibility, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    visibility.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_codePage(WT_Code_Page & codePage, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    (*rewriter->_GetW2D()).desired_rendition().code_page() = codePage;
    return WT_Result::Success;
}


WT_Result simple_process_colorMap(WT_Color_Map & colorMap, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    (*rewriter->_GetW2D()).desired_rendition().color_map() = colorMap;
    return WT_Result::Success;
}


WT_Result simple_process_comments(WT_Comments & comments, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    comments.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_contourSet(WT_Contour_Set & contourSet, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();

    if (!rewriter->LayerPassesFilter())
        return WT_Result::Success;

    int numcntrs = contourSet.contours();
    WT_Integer32* cntrcounts = contourSet.counts();

    bool restoreColor = false;
    WT_Color rendColor;

    //process override color, if any
    if (rewriter->IsSymbolW2D() && rewriter->GetOverrideColorMarker().style().color().argb() != RS_Color::EMPTY_COLOR_ARGB)
    {
        rendColor = (*rewriter->_GetW2D()).desired_rendition().color();
        RS_Color override = rewriter->GetOverrideColorMarker().style().color();
        WT_Color c(override.red(), override.green(), override.blue(), override.alpha());
        (*rewriter->_GetW2D()).desired_rendition().color() = c;
        restoreColor = true;
    }

    //we will clip each contour as a polygon and then construct
    //a new contour set.
    //TODO: this can be optimized
    std::vector<WT_Integer32> newcounts;
    std::vector<WT_Logical_Point> newpts;

    int index = 0;

    for (int i=0; i< numcntrs; i++)
    {
        WT_Logical_Point* cur_cntr = (WT_Logical_Point*)(contourSet.points() + index);

        int numpts = 0;
        std::vector<int>* cntrs = NULL;

        //transform and clip polygon -- clipping may result in more than one new contour
        const WT_Logical_Point* dst_cntr = rewriter->ProcessW2DPoints(
            file, cur_cntr, cntrcounts[i], LineBuffer::ctArea, numpts, &cntrs);

        if (numpts > 2)
        {
            //process each contour returned by clipping
            int offset = 0;
            for (unsigned k=0; k<cntrs->size(); k++)
            {
                newcounts.push_back((*cntrs)[k]);

                for (int j=offset; j<(*cntrs)[k]+offset; j++)
                    newpts.push_back(dst_cntr[j]);

                offset += (*cntrs)[k];
            }
        }

        index += cntrcounts[i];
    }

    if (newcounts.size() > 0)
    {
        WT_Contour_Set cset2(file, (WT_Integer32)newcounts.size(), (WT_Integer32*)&newcounts.front(),
            (WT_Integer32)newpts.size(), (WT_Logical_Point*)&newpts.front(), true);
        cset2.serialize(*rewriter->_GetW2D());
        rewriter->IncrementDrawableCount();
    }

    if (restoreColor)
        (*rewriter->_GetW2D()).desired_rendition().color() = rendColor;

    return WT_Result::Success;
}


WT_Result simple_process_copyright(WT_Copyright & /*copyright*/, WT_File & /*file*/)
{
    //DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    //copyright.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_dashPattern(WT_Dash_Pattern & dashPattern, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    (*rewriter->_GetW2D()).desired_rendition().dash_pattern() = dashPattern;
    return WT_Result::Success;
}


WT_Result simple_process_description(WT_Description & /*description*/, WT_File & /*file*/)
{
    //DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    //description.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_fill(WT_Fill & fill, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    //fill.serialize(simple_output_file);
    (*rewriter->_GetW2D()).desired_rendition().fill() = fill;
    return WT_Result::Success;
}


WT_Result simple_process_filledEllipse(WT_Filled_Ellipse & filledEllipse, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();

    if (!rewriter->LayerPassesFilter())
        return WT_Result::Success;

    bool restoreColor = false;
    WT_Color rendColor;

    //process override color, if any
    if (rewriter->IsSymbolW2D() && rewriter->GetOverrideColorMarker().style().color().argb() != RS_Color::EMPTY_COLOR_ARGB)
    {
        rendColor = (*rewriter->_GetW2D()).desired_rendition().color();
        RS_Color override = rewriter->GetOverrideColorMarker().style().color();
        WT_Color c(override.red(), override.green(), override.blue(), override.alpha());
        (*rewriter->_GetW2D()).desired_rendition().color() = c;
        restoreColor = true;
    }

    WT_Logical_Point oldpos = filledEllipse.position();
    int numpts = 0;
    std::vector<int>* cntrs = NULL;
    const WT_Logical_Point* dstpts = rewriter->ProcessW2DPoints(
        file, (WT_Logical_Point*)&oldpos, 1, LineBuffer::ctPoint, numpts, &cntrs);

    if (numpts > 0)
    {
        WT_Integer32 major = rewriter->ScaleW2DNumber(file, filledEllipse.major());
        WT_Integer32 minor = rewriter->ScaleW2DNumber(file, filledEllipse.minor());

        WT_Filled_Ellipse ell2(dstpts[0], major, minor, (WT_Unsigned_Integer16)filledEllipse.start(), (WT_Unsigned_Integer16)filledEllipse.end(), filledEllipse.tilt());
        ell2.serialize(*rewriter->_GetW2D());
        rewriter->IncrementDrawableCount();
    }

    if (restoreColor)
        (*rewriter->_GetW2D()).desired_rendition().color() = rendColor;

    return WT_Result::Success;
}


WT_Result simple_process_font(WT_Font & font, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();

    //rescale font height
    font.height() = WT_Font_Option_Height(rewriter->ScaleW2DNumber(file, font.height().height()));

    (*rewriter->_GetW2D()).desired_rendition().font() = font;
    return WT_Result::Success;
}


WT_Result simple_process_gouraudPolyline(WT_Gouraud_Polyline & gouraudPolyline, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();

    if (!rewriter->LayerPassesFilter())
        return WT_Result::Success;

    //transform the points
    int numpts = 0;
    std::vector<int>* cntrs = NULL;
    const WT_Logical_Point* dstpts = rewriter->ProcessW2DPoints(
        file, gouraudPolyline.points(), gouraudPolyline.count(), LineBuffer::ctLine, numpts, &cntrs);

    //TODO: if the polyline is clipped, the colors will be wrong
    WT_Gouraud_Polyline poly2(numpts, dstpts, gouraudPolyline.colors(), true);
    poly2.serialize(*rewriter->_GetW2D());
    rewriter->IncrementDrawableCount();

    return WT_Result::Success;
}


WT_Result simple_process_gouraudPolytriangle(WT_Gouraud_Polytriangle & gouraudPolytriangle, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();

    if (!rewriter->LayerPassesFilter())
        return WT_Result::Success;

    WT_Logical_Point* srcpts = gouraudPolytriangle.points();

    const WT_Logical_Point* dstpts = rewriter->TransformW2DPoints(file, srcpts, gouraudPolytriangle.count());

    int index = 0;
    int count = 0;

    //now we need to iterate over the points and see which ones are outside the W2D extent
    //Note this clipping is not perfect by far -- since it works in W2D space and just drops
    //any points that are outside the extent instead of clipping triangle by triangle,
    //but we will need to see how adversely it will affect things before implementing a
    //different clipping strategy
    for (int i=0; i < gouraudPolytriangle.count(); i++)
    {
        if (dstpts[i].m_x < 0 || dstpts[i].m_x > INT_MAX-256 || dstpts[i].m_y < 0 || dstpts[i].m_y > INT_MAX-256)
        {
            //point is outside, so serialize what we have up to now
            if (count >=3)
            {
                WT_Gouraud_Polytriangle gpt2(count, dstpts + index, gouraudPolytriangle.colors() + index, false);
                gpt2.serialize(*rewriter->_GetW2D());
                rewriter->IncrementDrawableCount();
            }

            count = 0;
            index = i+1;
        }
        else
        {
            count++;
        }
    }

    //serialize whatever is left
    if (count >=3)
    {
        WT_Gouraud_Polytriangle gpt2(count, dstpts + index, gouraudPolytriangle.colors() + index, false);
        gpt2.serialize(*rewriter->_GetW2D());
        rewriter->IncrementDrawableCount();
    }

    return WT_Result::Success;
}


void RotateRGBAImage(const WT_Unsigned_Integer16 rows,
                     const WT_Unsigned_Integer16 columns,
                     const WT_Image::WT_Image_Format format,
                     const WT_Color_Map* color_map,
                     const WT_Byte* data,
                     const WT_Integer32 identifier,
                     const WT_Logical_Point* dstpts,
                     WT_File & file)
{
    // The difficult case when it is rotated -- make a new image which
    // encompasses the rotated extent.  We also have to use RGBA format
    // since we need the pieces which are in the bigger axis-aligned
    // bitmap but not overlapped by the rotated image to be transparent.

    //find new bounds for image -> basically axis-aligned bounds of
    //the rotated initial image bounds
    WT_Integer32 minx = rs_min(dstpts[0].m_x, rs_min(dstpts[1].m_x, rs_min(dstpts[2].m_x, dstpts[3].m_x)));
    WT_Integer32 maxx = rs_max(dstpts[0].m_x, rs_max(dstpts[1].m_x, rs_max(dstpts[2].m_x, dstpts[3].m_x)));

    WT_Integer32 miny = rs_min(dstpts[0].m_y, rs_min(dstpts[1].m_y, rs_min(dstpts[2].m_y, dstpts[3].m_y)));
    WT_Integer32 maxy = rs_max(dstpts[0].m_y, rs_max(dstpts[1].m_y, rs_max(dstpts[2].m_y, dstpts[3].m_y)));

    WT_Integer32 width  = maxx - minx /*??+ 1*/;
    WT_Integer32 height = maxy - miny /*??+ 1*/;

    //get basis vectors of transformed space
    double e1x = dstpts[1].m_x - dstpts[0].m_x;
    double e1y = dstpts[1].m_y - dstpts[0].m_y;
    double e2x = dstpts[3].m_x - dstpts[0].m_x;
    double e2y = dstpts[3].m_y - dstpts[0].m_y;

    //find width and height if image were not rotated (but just scaled)
    double nonrot_width  = sqrt(e1x*e1x + e1y*e1y);
    double nonrot_height = sqrt(e2x*e2x + e2y*e2y);

    //normalize basis vectors
    e1x /= nonrot_width;
    e1y /= nonrot_width;
    e2x /= nonrot_height;
    e2y /= nonrot_height;

    //determine size of new image in pixels
    double inv_old_cols = (double)columns / nonrot_width;
    double inv_old_rows = (double)rows / nonrot_height;
    int new_cols = (int)((double)width * inv_old_cols);
    int new_rows = (int)((double)height * inv_old_rows);

    //alloc mem for image
    int len = new_cols * new_rows;
    WT_RGBA32* pixels = new WT_RGBA32[new_cols * new_rows];

    //initialize to transparent color
    memset(pixels, 0, sizeof(WT_RGBA32)*len);

    //loop invariants
    double inv_new_cols = (double)width / new_cols;
    double inv_new_rows = (double)height/ new_rows;
    int old_rows = rows;
    int old_cols = columns;

    for (int j=0; j<new_rows; j++)
    {
        for (int i=0; i<new_cols; i++)
        {
            //compute W2D logical location of pixel
            double dstx = i * (inv_new_cols) + minx;
            double dsty = (new_rows - 1 -j) * (inv_new_rows) + miny;

            //convert location to a location vector
            //from origin of non-rotated image
            double ddx = dstx - dstpts[0].m_x;
            double ddy = dsty - dstpts[0].m_y;

            //project vector onto rotated basis vectors
            double dote1 = e1x * ddx + e1y * ddy;
            double dote2 = e2x * ddx + e2y * ddy;

            //convert projected values to pixel coordinates
            //inside the source image
            double pixelx = dote1 * inv_old_cols;
            double pixely = old_rows - dote2 * inv_old_rows;

            //if the pixel coordinates are inside the source
            //image we will obtain an interpolated color
            //for the destination image
            if (pixelx >=-1 && pixelx <= old_cols &&
                pixely >=-1 && pixely <= old_rows)
            {
                int xwhole = (int)floor(pixelx);
                int ywhole = (int)floor(pixely);

                //4 nearest pixels
                WT_RGBA32 c00(0, 0, 0, 0);
                WT_RGBA32 c10(0, 0, 0, 0);
                WT_RGBA32 c11(0, 0, 0, 0);
                WT_RGBA32 c01(0, 0, 0, 0);

                //get values for the 4 nearest pixels, taking care
                //not to go overboard (outside the image)
                switch (format)
                {
                case WT_Image::RGBA:
                    if (pixelx >= 0)
                    {
                        if (pixely >=0)
                            c00 = ((WT_RGBA32*)data)[xwhole + ywhole * old_cols];

                        if (pixely < old_rows - 1)
                            c01 = ((WT_RGBA32*)data)[xwhole + (ywhole + 1) * old_cols];
                    }

                    if (pixelx < old_cols - 1)
                    {
                        if (pixely >=0)
                            c10 = ((WT_RGBA32*)data)[xwhole + 1 + ywhole * old_cols];

                        if (pixely < old_rows - 1)
                            c11 = ((WT_RGBA32*)data)[xwhole + 1 + (ywhole + 1) * old_cols];
                    }
                    break;
                case WT_Image::RGB:
                    {
                        const WT_Byte* ptr = NULL;

                        if (pixelx >= 0)
                        {
                            if (pixely >= 0)
                            {
                                ptr = data + 3*(xwhole + ywhole * old_cols);
                                c00.m_rgb.b = *ptr++;
                                c00.m_rgb.g = *ptr++;
                                c00.m_rgb.r = *ptr++;
                                c00.m_rgb.a = 255;
                            }

                            if (pixely < old_rows - 1)
                            {
                                ptr = data + 3*(xwhole + (ywhole + 1) * old_cols);
                                c01.m_rgb.b = *ptr++;
                                c01.m_rgb.g = *ptr++;
                                c01.m_rgb.r = *ptr++;
                                c01.m_rgb.a = 255;
                            }
                        }

                        if (pixelx < old_cols - 1)
                        {
                            ptr = data + 3*(xwhole + 1 + ywhole * old_cols);

                            if (pixely >= 0)
                            {
                                c10.m_rgb.b = *ptr++;
                                c10.m_rgb.g = *ptr++;
                                c10.m_rgb.r = *ptr++;
                                c10.m_rgb.a = 255;
                            }

                            if (pixely < old_rows - 1)
                            {
                                ptr = data + 3*(xwhole + 1 + (ywhole + 1) * old_cols);
                                c11.m_rgb.b = *ptr++;
                                c11.m_rgb.g = *ptr++;
                                c11.m_rgb.r = *ptr++;
                                c11.m_rgb.a = 255;
                            }
                        }
                    }
                    break;
                }

                //bilinear interpolation of nearest pixels
                float ifx = (float)(pixelx - xwhole);
                float ify = (float)(pixely - ywhole);
                float fx = 1.0f - ifx;
                float fy = 1.0f - ify;
                float fxfy   = fx  * fy;
                float ifxfy  = ifx * fy;
                float ifxify = ifx * ify;
                float fxify  = fx  * ify;

                float c00a = fxfy * (float)c00.m_rgb.a / 255.0f;
                float c10a = ifxfy * (float)c10.m_rgb.a / 255.0f;
                float c11a = ifxify * (float)c11.m_rgb.a / 255.0f;
                float c01a = fxify * (float)c01.m_rgb.a / 255.0f;

                float a_normalize = (float)(1.0f / (c00a + c10a + c11a + c01a));

                c00a *= a_normalize;
                c10a *= a_normalize;
                c11a *= a_normalize;
                c01a *= a_normalize;

                float red =  c00a * c00.m_rgb.r
                           + c10a * c10.m_rgb.r
                           + c11a * c11.m_rgb.r
                           + c01a * c01.m_rgb.r;

                float green =  c00a * c00.m_rgb.g
                             + c10a * c10.m_rgb.g
                             + c11a * c11.m_rgb.g
                             + c01a * c01.m_rgb.g;

                float blue =  c00a * c00.m_rgb.b
                            + c10a * c10.m_rgb.b
                            + c11a * c11.m_rgb.b
                            + c01a * c01.m_rgb.b;

                float alpha =  fxfy  * c00.m_rgb.a
                             + ifxfy * c10.m_rgb.a
                             + ifxify* c11.m_rgb.a
                             + fxify * c01.m_rgb.a;

                WT_RGBA32 bilerp_col((int)(red+0.5f) & 0xFF, (int)(green+0.5f) & 0xFF, (int)(blue+0.5) & 0xFF, (int)(alpha+0.5f) & 0xFF);
                pixels[i + new_cols * j] = bilerp_col;
            }
        }
    }

    //make a new image out of the rotated data and send it to W2D
    WT_Image image((WT_Unsigned_Integer16)new_rows,
                   (WT_Unsigned_Integer16)new_cols,
                   WT_Image::RGBA,
                   identifier,
                   color_map,
                   len * sizeof(WT_RGBA32),
                   (WT_Byte*)pixels,
                   WT_Logical_Point(minx, miny),
                   WT_Logical_Point(maxx, maxy),
                   true);

    image.serialize(file);

    delete [] pixels;
}


int g_imageId = 1;//TODO:  may be this should be a variable in DWFRenderer

WT_Result simple_process_image(WT_Image & image, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();

    if (!rewriter->LayerPassesFilter())
        return WT_Result::Success;

    //transform the bounds
    WT_Logical_Box bounds = image.bounds();

    WT_Logical_Point endpts[4];
    endpts[0].m_x = bounds.m_min.m_x;
    endpts[0].m_y = bounds.m_min.m_y;
    endpts[1].m_x = bounds.m_max.m_x;
    endpts[1].m_y = bounds.m_min.m_y;
    endpts[2].m_x = bounds.m_max.m_x;
    endpts[2].m_y = bounds.m_max.m_y;
    endpts[3].m_x = bounds.m_min.m_x;
    endpts[3].m_y = bounds.m_max.m_y;

    int numpts = 0;
    std::vector<int>* cntrs = NULL;
    const WT_Logical_Point* dstpts = rewriter->ProcessW2DPoints(
        file, endpts, 4, LineBuffer::ctPoint, numpts, &cntrs);

    if (numpts == 4)
    {
        //check for rotation -- DWF does not support rotated images, so we need to
        //do it manually.

        if (dstpts[0].m_y == dstpts[1].m_y && dstpts[1].m_x == dstpts[2].m_x)
        {
            //case of no rotation -- direct copy of the image
            WT_Image image2(
                image.rows(),
                image.columns(),
                WT_Image::WT_Image_Format(image.format()),
                g_imageId++,
                image.color_map(),
                image.data_size(),
                (WT_Byte*)image.data(),
                dstpts[0], //min pt
                dstpts[2], //max pt
                true);

            image2.serialize(*rewriter->_GetW2D());
            rewriter->IncrementDrawableCount();
        }
        else if (image.format() == WT_Image::RGB || image.format() == WT_Image::RGBA)
        {
            RotateRGBAImage(image.rows(),
                            image.columns(),
                            (WT_Image::WT_Image_Format)image.format(),
                            image.color_map(),
                            image.data(),
                            g_imageId++,
                            dstpts,
                            *rewriter->_GetW2D());
            rewriter->IncrementDrawableCount();
        }
    }

    return WT_Result::Success;
}


WT_Result simple_process_keywords(WT_Keywords & /*keywords*/, WT_File & /*file*/)
{
    //DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    //keywords.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_markerSize(WT_Marker_Size & markerSize, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();

    //rescale the marker size
    WT_Integer32 size2 = rewriter->ScaleW2DNumber(file, markerSize.size());

    (*rewriter->_GetW2D()).desired_rendition().marker_size() = WT_Marker_Size(size2);
    return WT_Result::Success;
}


WT_Result simple_process_markerSymbol(WT_Marker_Symbol & markerSymbol, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();

    if (!rewriter->LayerPassesFilter())
        return WT_Result::Success;

    (*rewriter->_GetW2D()).desired_rendition().marker_symbol() = markerSymbol;
    return WT_Result::Success;
}


WT_Result simple_process_mergeControl(WT_Merge_Control & mergeControl, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    (*rewriter->_GetW2D()).desired_rendition().merge_control() = mergeControl;
    return WT_Result::Success;
}


WT_Result simple_process_namedViewList(WT_Named_View_List & /*namedViewList*/, WT_File & /*file*/)
{
    //DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    //namedViewList.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_origin(WT_Origin & origin, WT_File & file)
{
    //DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();

    //this call updates the source file point transform
    //to take into account the new origin so that points
    //we get are correctly offset
    file.update_current_point (origin.origin());

    //NOTE: do not serialize this to destination file since
    //it is already taken into account by the incoming point coordinates
    //WT_Origin origin2(pts[0]);
    //origin2.serialize(*rewriter->_GetW2D());

    return WT_Result::Success;
}


WT_Result simple_process_outlineEllipse(WT_Outline_Ellipse & outlineEllipse, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();

    if (!rewriter->LayerPassesFilter())
        return WT_Result::Success;

    bool restoreColor = false;
    WT_Color rendColor;

    //process override color, if any
    if (rewriter->IsSymbolW2D() && rewriter->GetOverrideColorMarker().style().outline().color().argb() != RS_Color::EMPTY_COLOR_ARGB)
    {
        rendColor = (*rewriter->_GetW2D()).desired_rendition().color();
        RS_Color override = rewriter->GetOverrideColorMarker().style().outline().color();
        WT_Color c(override.red(), override.green(), override.blue(), override.alpha());
        (*rewriter->_GetW2D()).desired_rendition().color() = c;
        restoreColor = true;
    }

    WT_Logical_Point oldpos = outlineEllipse.position();
    int numpts = 0;
    std::vector<int>* cntrs = NULL;
    const WT_Logical_Point* dstpts = rewriter->ProcessW2DPoints(
        file, (WT_Logical_Point*)&oldpos, 1, LineBuffer::ctPoint, numpts, &cntrs);

    if (numpts > 0)
    {
        WT_Integer32 major = rewriter->ScaleW2DNumber(file, outlineEllipse.major());
        WT_Integer32 minor = rewriter->ScaleW2DNumber(file, outlineEllipse.minor());

        WT_Outline_Ellipse ell2(dstpts[0], major, minor, (WT_Unsigned_Integer16)outlineEllipse.start(), (WT_Unsigned_Integer16)outlineEllipse.end(), outlineEllipse.tilt());
        ell2.serialize(*rewriter->_GetW2D());
        rewriter->IncrementDrawableCount();
    }

    if (restoreColor)
        (*rewriter->_GetW2D()).desired_rendition().color() = rendColor;

    return WT_Result::Success;
}


WT_Result simple_process_polygon(WT_Polygon & /*polygon*/, WT_File & /*file*/)
{
    //DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();

    _ASSERT(false);

    //should never get here.
    //WHIP polygons come in as polylines with the rendition Fill flag
    //set to true.

    /*
    //do all necessary transformations and clipping
    int outpts = 0;
    std::vector<int>* cntrs = NULL;
    const WT_Logical_Point* dstpts = rewriter->ProcessW2DPoints(
        file, polygon.points(), polygon.count(), LineBuffer::ctArea, outpts, &cntrs);

    if (outpts > 0)
    {
        WT_Polygon poly2(outpts, dstpts, true);
        poly2.serialize(*rewriter->_GetW2D());
        rewriter->IncrementDrawableCount();
    }
    */

    return WT_Result::Success;
}


WT_Result simple_process_polymarker(WT_Polymarker & polymarker, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();

    if (!rewriter->LayerPassesFilter())
        return WT_Result::Success;

    //do all necessary transformations and clipping
    int outpts = 0;
    std::vector<int>* cntrs = NULL;
    const WT_Logical_Point* dstpts = rewriter->ProcessW2DPoints(
        file, polymarker.points(), polymarker.count(), LineBuffer::ctPoint, outpts, &cntrs);

    if (outpts > 0)
    {
        WT_Polymarker poly2(outpts, dstpts, true);
        poly2.serialize(*rewriter->_GetW2D());
        rewriter->IncrementDrawableCount();
    }

    return WT_Result::Success;
}


WT_Result simple_process_polytriangle(WT_Polytriangle & polytriangle, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();

    if (!rewriter->LayerPassesFilter())
        return WT_Result::Success;

    bool restoreColor = false;
    WT_Color rendColor;

    //process override color, if any
    if (rewriter->IsSymbolW2D() && rewriter->GetOverrideColorMarker().style().color().argb() != RS_Color::EMPTY_COLOR_ARGB)
    {
        rendColor = (*rewriter->_GetW2D()).desired_rendition().color();
        RS_Color override = rewriter->GetOverrideColorMarker().style().color();
        WT_Color c(override.red(), override.green(), override.blue(), override.alpha());
        (*rewriter->_GetW2D()).desired_rendition().color() = c;
        restoreColor = true;
    }

    WT_Logical_Point* srcpts = polytriangle.points();

    const WT_Logical_Point* dstpts = rewriter->TransformW2DPoints(file, srcpts, polytriangle.count());

    int index = 0;
    int count = 0;

    //now we need to iterate over the points and see which ones are outside the W2D extent
    //Note this clipping is not perfect by far -- since it works in W2D space and just drops
    //any points that are outside the extent instead of clipping triangle by triangle,
    //but we will need to see how adversely it will affect things before implementing a
    //different clipping strategy
    for (int i=0; i < polytriangle.count(); i++)
    {
        if (dstpts[i].m_x < 0 || dstpts[i].m_x > INT_MAX-256 || dstpts[i].m_y < 0 || dstpts[i].m_y > INT_MAX-256)
        {
            //point is outside, so serialize what we have up to now
            if (count >=3)
            {
                WT_Polytriangle gpt2(count, dstpts + index, false);
                gpt2.serialize(*rewriter->_GetW2D());
                rewriter->IncrementDrawableCount();
            }

            count = 0;
            index = i+1;
        }
        else
        {
            count++;
        }
    }

    //serialize whatever is left
    if (count >=3)
    {
        WT_Polytriangle gpt2(count, dstpts + index, false);
        gpt2.serialize(*rewriter->_GetW2D());
        rewriter->IncrementDrawableCount();
    }

    if (restoreColor)
        (*rewriter->_GetW2D()).desired_rendition().color() = rendColor;

    return WT_Result::Success;
}


WT_Result simple_process_projection(WT_Projection & projection, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    projection.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_subject(WT_Subject & /*subject*/, WT_File & /*file*/)
{
    //DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    //subject.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_title(WT_Title & /*title*/, WT_File & /*file*/)
{
    /*DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();*/
    //title.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_unknown(WT_Unknown & /*unknown*/, WT_File & /*file*/)
{
    //DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    // simply skiping the WT_Unknown object.
    //unknown.serialize(simple_output_file);
    return WT_Result::Success;
}


WT_Result simple_process_url(WT_URL & url, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    (*rewriter->_GetW2D()).desired_rendition().url() = url;
    return WT_Result::Success;
}


WT_Result simple_process_pngGroup4Image(WT_PNG_Group4_Image & pngGroup4Image, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();

    if (!rewriter->LayerPassesFilter())
        return WT_Result::Success;

    //transform the bounds
    WT_Logical_Box bounds = pngGroup4Image.bounds();

    int numpts = 0;
    std::vector<int>* cntrs = NULL;
    const WT_Logical_Point* dstpts = rewriter->ProcessW2DPoints(
        file, (WT_Logical_Point*)&bounds, 2, LineBuffer::ctPoint, numpts, &cntrs);

    if (numpts == 2)
    {
        //only transfer the raster if it is fully inside destination box
        //TODO: otherwise we need to clip the raster itself

        // Create a new WT_Drawable with the updated bounds
        WT_PNG_Group4_Image img2(pngGroup4Image.rows(),
            pngGroup4Image.columns(),
            WT_PNG_Group4_Image::PNG,
            pngGroup4Image.identifier(),
            pngGroup4Image.color_map(),
            pngGroup4Image.data_size(),
            (WT_Byte *)pngGroup4Image.data(),
            dstpts[0],
            dstpts[1],
            false);

        img2.update_bounds(&file);
        img2.serialize(*rewriter->_GetW2D());
        rewriter->IncrementDrawableCount();
    }

    return WT_Result::Success;
}


WT_Result simple_process_polyline(WT_Polyline & polyline, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();

    if (!rewriter->LayerPassesFilter())
        return WT_Result::Success;

    bool isPolygon = rewriter->_GetW2D()->desired_rendition().fill().fill() == WD_True;

    WT_Color rendColor;
    bool restoreColor = false;

    //check for override color
    if (rewriter->IsSymbolW2D())
    {
        //TODO: we have no easy way to determine if a polyline is
        //just a polyline or the outline of a polygon. Since we need to
        //use different override colors in these cases, we will take a guess.
        bool isPossiblyPolygonOutline = false;

        //if the first point and the last point of the polyline are equal,
        //we will treat it as a polygon outline
        if (polyline.count() > 2
            && polyline.points()[0] == polyline.points()[polyline.count() - 1])
            isPossiblyPolygonOutline = true;

        RS_Color overrideColor;

        if (isPolygon)
            overrideColor = rewriter->GetOverrideColorMarker().style().color();
        else
            overrideColor = rewriter->GetOverrideColorMarker().style().outline().color();

        if (overrideColor.argb() != RS_Color::EMPTY_COLOR_ARGB)
        {
            rendColor = (*rewriter->_GetW2D()).desired_rendition().color();
            WT_Color c(overrideColor.red(), overrideColor.green(), overrideColor.blue(), overrideColor.alpha());
            (*rewriter->_GetW2D()).desired_rendition().color() = c;
            restoreColor = true;
        }
    }

    //do all necessary transformations and clipping
    int outpts = 0;
    std::vector<int>* cntrs = NULL;
    const WT_Logical_Point* dstpts = rewriter->ProcessW2DPoints(
        file, polyline.points(), polyline.count(),
        isPolygon? LineBuffer::ctArea : LineBuffer::ctLine,
        outpts, &cntrs);

    if (outpts > 0)
    {
        int offset = 0;

        for (unsigned i=0; i<cntrs->size(); i++)
        {
            if (isPolygon)
            {
                WT_Polygon poly2((*cntrs)[i], dstpts+offset, true);
                poly2.serialize(*rewriter->_GetW2D());
                rewriter->IncrementDrawableCount();
            }
            else
            {
                WT_Polyline poly2((*cntrs)[i], dstpts+offset, true);
                poly2.serialize(*rewriter->_GetW2D());
                rewriter->IncrementDrawableCount();
            }

            offset += (*cntrs)[i];
        }
    }

    if (restoreColor)
        (*rewriter->_GetW2D()).desired_rendition().color() = rendColor;

    return WT_Result::Success;
}


WT_Result simple_process_text(WT_Text & text, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();

    if (!rewriter->LayerPassesFilter())
        return WT_Result::Success;

    WT_Logical_Point pt = text.position();

    bool restoreColor = false;
    WT_Color rendColor;

    //process override color, if any
    //text override color is stored in the background color of the
    //fill style, which is otherwise unused
    if (rewriter->IsSymbolW2D() && rewriter->GetOverrideColorMarker().style().background().argb() != RS_Color::EMPTY_COLOR_ARGB)
    {
        rendColor = (*rewriter->_GetW2D()).desired_rendition().color();
        RS_Color overrideColor = rewriter->GetOverrideColorMarker().style().background();
        WT_Color c(overrideColor.red(), overrideColor.green(), overrideColor.blue(), overrideColor.alpha());
        (*rewriter->_GetW2D()).desired_rendition().color() = c;
        restoreColor = true;
    }

    int numpts = 0;
    std::vector<int>* cntrs = NULL;
    const WT_Logical_Point* dstpts = rewriter->ProcessW2DPoints(
        file, &pt, 1, LineBuffer::ctPoint, numpts, &cntrs);

    if (numpts > 0)
    {
        WT_Text text2(dstpts[0], text.string(), NULL,
            text.overscore().count(), text.overscore().positions(),
            text.underscore().count(), text.underscore().positions()) ;
        text2.serialize(*rewriter->_GetW2D());
        rewriter->IncrementDrawableCount();
    }

    if (restoreColor)
        (*rewriter->_GetW2D()).desired_rendition().color() = rendColor;

    //TODO: optionally text position can be specified using
    //a bounds box. This case needs to be handled here too

    return WT_Result::Success;
}


WT_Result simple_process_inkedArea(WT_Inked_Area & inkedArea, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    inkedArea.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_plotOptimized(WT_Plot_Optimized & plotOptimized, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    plotOptimized.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_groupBegin(WT_Group_Begin & groupBegin, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    groupBegin.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_groupEnd(WT_Group_End & groupEnd, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    groupEnd.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_embeddedFont(WT_Embedded_Font & embeddedFont, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    embeddedFont.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_trustedFontList(WT_Trusted_Font_List & trustedFontList, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    trustedFontList.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_blockref(WT_BlockRef & blockref, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    (*rewriter->_GetW2D()).desired_rendition().blockref() = blockref;
    return WT_Result::Success;
}


WT_Result simple_process_blockMeaning(WT_Block_Meaning & blockMeaning, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    blockMeaning.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_encryption(WT_Encryption & encryption, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    encryption.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_orientation(WT_Orientation & orientation, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    orientation.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_alignment(WT_Alignment & alignment, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    alignment.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_password(WT_Password & password, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    password.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_guid(WT_Guid & guid, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    guid.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_fileTime(WT_FileTime & fileTime, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    fileTime.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_userData(WT_UserData & userData, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    userData.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_penPattern(WT_Pen_Pattern & penPattern, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    (*rewriter->_GetW2D()).desired_rendition().pen_pattern() = penPattern;
    return WT_Result::Success;
}


WT_Result simple_process_linePattern(WT_Line_Pattern & linePattern, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    (*rewriter->_GetW2D()).desired_rendition().line_pattern() = linePattern;
    return WT_Result::Success;
}


WT_Result simple_process_fillPattern(WT_Fill_Pattern & fillPattern, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    fillPattern.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_DigitalSign(WT_SignData & digitalSing, WT_File & file)
{
    DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    digitalSing.serialize(*rewriter->_GetW2D());
    return WT_Result::Success;
}


WT_Result simple_process_dwf_header(WT_DWF_Header & /*dwf_header*/, WT_File & /*file*/)
{
    //DWFRenderer* rewriter = (DWFRenderer*)file.stream_user_data();
    //simple_output_file.heuristics().set_target_version(file.rendition().drawing_info().decimal_revision());
    //simple_output_file.open();
    return WT_Result::Success;
}
