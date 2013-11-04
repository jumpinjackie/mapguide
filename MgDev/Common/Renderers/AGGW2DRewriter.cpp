//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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
#include "AGGRenderer.h"
#include "AGGImageIO.h"
#include "AGGW2DRewriter.h"

#include "agg_context.h"


////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------
//
//                    I/O Overloads
//
//------------------------------------------------------------------
////////////////////////////////////////////////////////////////////

WT_Result agr_open(WT_File & /*file*/)
{
    return WT_Result::Success;
}


WT_Result agr_close(WT_File & file)
{
    file.set_stream_user_data(WD_Null);
    return WT_Result::Success;
}


WT_Result agr_read(WT_File & file, int desired_bytes, int & bytes_read, void * buffer)
{
    bytes_read = (int)((AGGRenderer*)file.stream_user_data())->_GetInputStream()->read(buffer, desired_bytes);

    if (!bytes_read)
        return WT_Result::Unknown_File_Read_Error;

    return WT_Result::Success;
}


WT_Result agr_seek(WT_File & file, int distance, int & amount_seeked)
{
    off_t prev = 0;
    ((AGGRenderer*)file.stream_user_data())->_GetInputStream()->seek(distance, prev);
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


WT_Result agr_process_units(WT_Units & units, WT_File & file)
{
    AGGRenderer* rewriter = (AGGRenderer*)file.stream_user_data();
    //do not write the units -- the destination W2D has different units
    //units.serialize(*rewriter->_GetW2D());

    //Instead save the in the desired rendition of the source file
    //so that we can get to them when transforming coordinates for DWF layers
    //Note that for DWF point symbols we do not do that
    if (!rewriter->IsSymbolW2D())
        file.desired_rendition().drawing_info().units() = units;

    return WT_Result::Success;
}


WT_Result agr_process_color(WT_Color & color, WT_File & file)
{
    file.rendition().color() = color;

    return WT_Result::Success;
}


WT_Result agr_process_lineWeight(WT_Line_Weight & lineWeight, WT_File & file)
{
    file.rendition().line_weight() = lineWeight;

    return WT_Result::Success;
}


WT_Result agr_process_viewport(WT_Viewport & viewport, WT_File & file)
{
    AGGRenderer* rewriter = (AGGRenderer*)file.stream_user_data();

    //IMPORTANT: AutoCAD defines a viewport for each view of the
    //drawing displayed in paper space. These views are arranged
    //to fit on the page via several of the viewports.
    //For symbols coming from the import engine, only one viewport
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

    //(*rewriter->GetW2DTargetImage()).desired_rendition().viewport() = viewport;

    return WT_Result::Success;
}


WT_Result agr_process_colorMap(WT_Color_Map & colorMap, WT_File & file)
{
    file.rendition().color_map() = colorMap;
    return WT_Result::Success;
}


WT_Result agr_process_contourSet(WT_Contour_Set & contourSet, WT_File & file)
{
    if (file.rendition().visibility().visible() == WD_False)
        return WT_Result::Success;

    AGGRenderer* rewriter = (AGGRenderer*)file.stream_user_data();

    if (!rewriter->LayerPassesFilter())
        return WT_Result::Success;

    WT_RGBA32 rgba = file.rendition().color().rgba();
    RS_Color color = RS_Color(rgba.m_rgb.r, rgba.m_rgb.g, rgba.m_rgb.b, rgba.m_rgb.a);

    //check for override color
    if (rewriter->IsSymbolW2D())
    {
        RS_Color override = rewriter->GetOverrideColorMarker().style().color();

        if (override.argb() != RS_Color::EMPTY_COLOR_ARGB)
            color = override;
    }

    //transform point and draw the contour set
    int numcntrs = contourSet.contours();
    WT_Integer32* cntrcounts = contourSet.counts();
    int totalPts = contourSet.total_points();

    LineBuffer* dst_cntr = rewriter->ProcessW2DPoints(file, (WT_Logical_Point*)contourSet.points(), totalPts, true);
    std::auto_ptr<LineBuffer> spDstLB(dst_cntr);

    if (dst_cntr)
    {
        //construct a line buffer with the transformed points and the original
        //contour information
        LineBuffer* lb = LineBufferPool::NewLineBuffer(rewriter->GetBufferPool(), totalPts);
        std::auto_ptr<LineBuffer> spLB(lb);
        int index = 0;
        for (int i=0; i<numcntrs; i++)
        {
            int j = cntrcounts[i];

            if (j)
            {
                lb->MoveTo(dst_cntr->x_coord(index), dst_cntr->y_coord(index));
                index++;
            }

            for (int k=1; k<j; k++)
            {
                lb->LineTo(dst_cntr->x_coord(index), dst_cntr->y_coord(index));
                index++;
            }
        }

        if (color.alpha() != 0)
            AGGRenderer::DrawScreenPolygon((agg_context*)rewriter->GetW2DTargetImage(), lb, NULL, color.argb());

        LineBufferPool::FreeLineBuffer(rewriter->GetBufferPool(), spLB.release());
        LineBufferPool::FreeLineBuffer(rewriter->GetBufferPool(), spDstLB.release());
    }

    return WT_Result::Success;
}


WT_Result agr_process_fill(WT_Fill & fill, WT_File & file)
{
    file.rendition().fill() = fill;
    return WT_Result::Success;
}


WT_Result agr_process_font(WT_Font & font, WT_File & file)
{
    /*AGGRenderer* rewriter = (AGGRenderer*)file.stream_user_data();*/
    file.rendition().font() = font;
    return WT_Result::Success;
}


WT_Result agr_process_gouraudPolyline(WT_Gouraud_Polyline & /*gouraudPolyline*/, WT_File & /*file*/)
{
    //AGGRenderer* rewriter = (AGGRenderer*)file.stream_user_data();
    //TODO: if the polyline is clipped, the colors will be wrong
    //WT_Gouraud_Polyline poly2(numpts, dstpts, gouraudPolyline.colors(), true);
    //poly2.serialize(*rewriter->GetW2DTargetImage());
    return WT_Result::Success;
}


WT_Result agr_process_gouraudPolytriangle(WT_Gouraud_Polytriangle & /*gouraudPolytriangle*/, WT_File & /*file*/)
{
    //AGGRenderer* rewriter = (AGGRenderer*)file.stream_user_data();
    //TODO: clipping for polytriangles is needed
    //gouraudPolytriangle.serialize(*rewriter->GetW2DTargetImage());
    return WT_Result::Success;
}


WT_Result agr_process_image(WT_Image & image, WT_File & file)
{
    if (file.rendition().visibility().visible() == WD_False)
        return WT_Result::Success;

    AGGRenderer* rewriter = (AGGRenderer*)file.stream_user_data();

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

    LineBuffer* dstpts = rewriter->ProcessW2DPoints(file, endpts, 4, false);
    std::auto_ptr<LineBuffer> spDstLB(dstpts);

    unsigned int* src = NULL;

    if (image.format() == WT_Image::RGBA)
    {
        src = new unsigned int[image.columns() * image.rows()];

        for (int j=0; j<image.rows(); j++)
            for (int i=0; i<image.columns(); i++)
            {
                //this code is slow, but not terrible for the purpose at hand (DWF symbols)
                WT_RGBA32 rgba = ((WT_RGBA32*)image.data())[i + j*image.columns()];
                RS_Color rsc(rgba.m_rgb.b, rgba.m_rgb.g, rgba.m_rgb.r, rgba.m_rgb.a);
                src[i + j * image.columns()] = rsc.argb();
            }
    }
    else if (image.format() == WT_Image::RGB)
    {
        src = new unsigned int[image.columns() * image.rows()];

        for (int j=0; j<image.rows(); j++)
            for (int i=0; i<image.columns(); i++)
            {
                WT_RGBA32 rgba;
                const unsigned char* ptr = image.data() + 3*(i + j * image.columns());
                rgba.m_rgb.b = *ptr++;
                rgba.m_rgb.g = *ptr++;
                rgba.m_rgb.r = *ptr++;
                rgba.m_rgb.a = 255;

                RS_Color rsc(rgba.m_rgb.r, rgba.m_rgb.g, rgba.m_rgb.b, rgba.m_rgb.a);
                src[i + j * image.columns()] = rsc.argb();
            }
    }
    else if (image.format() == WT_Image::JPEG)
    {
        int width;
        int height;
        src = AGGImageIO::DecodeJPEG(image.data(), image.data_size(), width, height);
    }

    if (src)
    {
        double angleRad = atan2((double)(dstpts->y_coord(1) - dstpts->y_coord(0)), (double)(dstpts->x_coord(1) - dstpts->x_coord(0)));
        double cx = 0.5 * (dstpts->x_coord(0) + dstpts->x_coord(2));
        double cy = 0.5 * (dstpts->y_coord(0) + dstpts->y_coord(2));

        double dx = dstpts->x_coord(1) - dstpts->x_coord(0);
        double dy = dstpts->y_coord(1) - dstpts->y_coord(0);

        double width = sqrt(dx*dx + dy*dy);

        dx = dstpts->x_coord(2) - dstpts->x_coord(1);
        dy = dstpts->y_coord(2) - dstpts->y_coord(1);

        double height = sqrt(dx*dx + dy*dy);

        // make the call with a negative height since the buffer contains the non-inverted image
        AGGRenderer::DrawScreenRaster((agg_context*)rewriter->GetW2DTargetImage(), (unsigned char*)src, image.columns() * image.rows() * 4, RS_ImageFormat_ARGB,
                                   image.columns(), image.rows(), cx, cy, width, -height, angleRad * M_180PI);

        delete[] src;
    }

    LineBufferPool::FreeLineBuffer(rewriter->GetBufferPool(), spDstLB.release());

    return WT_Result::Success;
}


WT_Result agr_process_origin(WT_Origin & origin, WT_File & file)
{
    //AGGRenderer* rewriter = (AGGRenderer*)file.stream_user_data();

    WT_Logical_Point pt = origin.origin();

    //WT_Origin origin2(pts[0]);
    file.update_current_point (origin.origin());

    //origin2.serialize(*rewriter->GetW2DTargetImage());

    return WT_Result::Success;
}


WT_Result agr_process_filledEllipse(WT_Filled_Ellipse & filledEllipse, WT_File & file)
{
    if (file.rendition().visibility().visible() == WD_False)
        return WT_Result::Success;

    AGGRenderer* rewriter = (AGGRenderer*)file.stream_user_data();

    if (!rewriter->LayerPassesFilter())
        return WT_Result::Success;

    WT_RGBA32 rgba = file.rendition().color().rgba();
    RS_Color color = RS_Color(rgba.m_rgb.r, rgba.m_rgb.g, rgba.m_rgb.b, rgba.m_rgb.a);

    //process override color, if any
    if (rewriter->IsSymbolW2D() && rewriter->GetOverrideColorMarker().style().color().argb() != RS_Color::EMPTY_COLOR_ARGB)
    {
        RS_Color override = rewriter->GetOverrideColorMarker().style().color();

        if (override.argb() != RS_Color::EMPTY_COLOR_ARGB)
            color = override;
    }

    WT_Logical_Point center = filledEllipse.position();

    LineBuffer* dstpts = rewriter->ProcessW2DPoints(file, (WT_Logical_Point*)&center, 1, false);
    std::auto_ptr<LineBuffer> spDstLB(dstpts);

    if (!dstpts)
        return WT_Result::Success;

    double major = rewriter->ScaleW2DNumber(file, filledEllipse.major());
    double minor = rewriter->ScaleW2DNumber(file, filledEllipse.minor());

    double start = filledEllipse.start_radian();
    double end = filledEllipse.end_radian();
    double tilt = filledEllipse.tilt_radian();

    // compute start point
    double rcos = cos(tilt);
    double rsin = sin(tilt);
    double tx = major * cos(start);
    double ty = minor * sin(start);
    double startX = dstpts->x_coord(0) + tx*rcos - ty*rsin;
    double startY = dstpts->y_coord(0) + ty*rcos + tx*rsin;

    LineBuffer* ell = LineBufferPool::NewLineBuffer(rewriter->GetBufferPool(), 20);
    std::auto_ptr<LineBuffer> spEllLB(ell);

    ell->SetDrawingScale(1.0);
    ell->MoveTo(startX, startY);
    ell->ArcTo(dstpts->x_coord(0), dstpts->y_coord(0), major, minor, start, end, tilt);

    AGGRenderer::DrawScreenPolygon((agg_context*)rewriter->GetW2DTargetImage(), ell, NULL, color.argb());

    LineBufferPool::FreeLineBuffer(rewriter->GetBufferPool(), spDstLB.release());
    LineBufferPool::FreeLineBuffer(rewriter->GetBufferPool(), spEllLB.release());

    return WT_Result::Success;
}


WT_Result agr_process_outlineEllipse(WT_Outline_Ellipse & outlineEllipse, WT_File & file)
{
    if (file.rendition().visibility().visible() == WD_False)
        return WT_Result::Success;

    AGGRenderer* rewriter = (AGGRenderer*)file.stream_user_data();

    if (!rewriter->LayerPassesFilter())
        return WT_Result::Success;

    WT_RGBA32 rgba = file.rendition().color().rgba();
    RS_Color color = RS_Color(rgba.m_rgb.r, rgba.m_rgb.g, rgba.m_rgb.b, rgba.m_rgb.a);

    //check for override color
    if (rewriter->IsSymbolW2D())
    {
        RS_Color override = rewriter->GetOverrideColorMarker().style().outline().color();

        if (override.argb() != RS_Color::EMPTY_COLOR_ARGB)
            color = override;
    }

    WT_Logical_Point center = outlineEllipse.position();

    LineBuffer* dstpts = rewriter->ProcessW2DPoints(file, (WT_Logical_Point*)&center, 1, false);
    std::auto_ptr<LineBuffer> spDstLB(dstpts);

    if (!dstpts)
        return WT_Result::Success;

    double major = rewriter->ScaleW2DNumber(file, outlineEllipse.major());
    double minor = rewriter->ScaleW2DNumber(file, outlineEllipse.minor());

    double start = outlineEllipse.start_radian();
    double end = outlineEllipse.end_radian();
    double tilt = outlineEllipse.tilt_radian();

    // compute start point
    double rcos = cos(tilt);
    double rsin = sin(tilt);
    double tx = major * cos(start);
    double ty = minor * sin(start);
    double startX = dstpts->x_coord(0) + tx*rcos - ty*rsin;
    double startY = dstpts->y_coord(0) + ty*rcos + tx*rsin;

    LineBuffer* ell = LineBufferPool::NewLineBuffer(rewriter->GetBufferPool(), 20);
    std::auto_ptr<LineBuffer> spEllLB(ell);

    ell->SetDrawingScale(1.0);
    ell->MoveTo(startX, startY);
    ell->ArcTo(dstpts->x_coord(0), dstpts->y_coord(0), major, minor, start, end, tilt);

    //get W2D line weight
    double weightpx = rewriter->ScaleW2DNumber(file, file.rendition().line_weight().weight_value());
    weightpx = rs_max(1.0, weightpx);

    SE_LineStroke lineStroke(color.argb(), weightpx);
    AGGRenderer::DrawScreenPolyline((agg_context*)rewriter->GetW2DTargetImage(), ell, NULL, lineStroke);

    LineBufferPool::FreeLineBuffer(rewriter->GetBufferPool(), spDstLB.release());
    LineBufferPool::FreeLineBuffer(rewriter->GetBufferPool(), spEllLB.release());

    return WT_Result::Success;
}


WT_Result agr_process_polygon(WT_Polygon & polygon, WT_File & file)
{
    if (file.rendition().visibility().visible() == WD_False)
        return WT_Result::Success;

    AGGRenderer* rewriter = (AGGRenderer*)file.stream_user_data();

    if (!rewriter->LayerPassesFilter())
        return WT_Result::Success;

    WT_RGBA32 rgba = file.rendition().color().rgba();
    RS_Color color = RS_Color(rgba.m_rgb.r, rgba.m_rgb.g, rgba.m_rgb.b, rgba.m_rgb.a);

    //check for override color
    if (rewriter->IsSymbolW2D())
    {
        RS_Color override = rewriter->GetOverrideColorMarker().style().color();

        if (override.argb() != RS_Color::EMPTY_COLOR_ARGB)
            color = override;
    }

    //do all necessary transformations
    LineBuffer* dstpts = rewriter->ProcessW2DPoints(file, polygon.points(), polygon.count(), true);
    std::auto_ptr<LineBuffer> spDstLB(dstpts);

    if (dstpts)
    {
        AGGRenderer::DrawScreenPolygon((agg_context*)rewriter->GetW2DTargetImage(), dstpts, NULL, color.argb());
        LineBufferPool::FreeLineBuffer(rewriter->GetBufferPool(), spDstLB.release());
    }

    return WT_Result::Success;
}


WT_Result agr_process_polytriangle(WT_Polytriangle & polytriangle, WT_File & file)
{
    if (file.rendition().visibility().visible() == WD_False)
        return WT_Result::Success;

    AGGRenderer* rewriter = (AGGRenderer*)file.stream_user_data();

    if (!rewriter->LayerPassesFilter())
        return WT_Result::Success;

    WT_RGBA32 rgba = file.rendition().color().rgba();
    RS_Color color = RS_Color(rgba.m_rgb.r, rgba.m_rgb.g, rgba.m_rgb.b, rgba.m_rgb.a);

    //process override color, if any
    if (rewriter->IsSymbolW2D() && rewriter->GetOverrideColorMarker().style().color().argb() != RS_Color::EMPTY_COLOR_ARGB)
    {
        RS_Color override = rewriter->GetOverrideColorMarker().style().color();

        if (override.argb() != RS_Color::EMPTY_COLOR_ARGB)
            color = override;
    }

    WT_Logical_Point* srcpts = polytriangle.points();

    int numPoints = polytriangle.count();
    if (numPoints <= 2)
        return WT_Result::Success;

    LineBuffer* dstpts = rewriter->ProcessW2DPoints(file, srcpts, numPoints, true);
    std::auto_ptr<LineBuffer> spDstLB(dstpts);

    if (dstpts)
    {
        //
        //          # of triangles
        //
        //      even                 odd
        //  1   3   5   7        1   3   5
        //  *---*---*---*        *---*---*
        //  |\  |\  |\  |        |\  |\  |\
        //  | \ | \ | \ |   or   | \ | \ | \
        //  |  \|  \|  \|        |  \|  \|  \
        //  *---*---*---*        *---*---*---*
        //  0   2   4   6        0   2   4   6
        //
        LineBuffer lb(numPoints+1);

        lb.MoveTo(dstpts->x_coord(0), dstpts->y_coord(0));

        // add the edge with even numbered points (lower edge in diagram above)
        for (int i=2; i<numPoints; i += 2)
            lb.LineTo(dstpts->x_coord(i), dstpts->y_coord(i));

        // add the edge with odd numbered points (upper edge in diagram above)
        int oddMax = (numPoints % 2)? numPoints-2 : numPoints-1;
        for (int i=oddMax; i>=1; i -= 2)
            lb.LineTo(dstpts->x_coord(i), dstpts->y_coord(i));

        lb.Close();

        agg_context* c = (agg_context*)rewriter->GetW2DTargetImage();

        // fix for MapGuide ticket 1987 - temporarily lower gamma to 0.3
        c->ras.gamma(agg::gamma_power(0.3));
        AGGRenderer::DrawScreenPolygon(c, &lb, NULL, color.argb());
        c->ras.gamma(agg::gamma_power(1.0)); 

        LineBufferPool::FreeLineBuffer(rewriter->GetBufferPool(), spDstLB.release());
    }

    return WT_Result::Success;
}


WT_Result agr_process_pngGroup4Image(WT_PNG_Group4_Image & pngGroup4Image, WT_File & file)
{
    if (file.rendition().visibility().visible() == WD_False)
        return WT_Result::Success;

    AGGRenderer* rewriter = (AGGRenderer*)file.stream_user_data();

    if (!rewriter->LayerPassesFilter())
        return WT_Result::Success;

    //transform the bounds
    WT_Logical_Box bounds = pngGroup4Image.bounds();

    WT_Logical_Point endpts[4];
    endpts[0].m_x = bounds.m_min.m_x;
    endpts[0].m_y = bounds.m_min.m_y;
    endpts[1].m_x = bounds.m_max.m_x;
    endpts[1].m_y = bounds.m_min.m_y;
    endpts[2].m_x = bounds.m_max.m_x;
    endpts[2].m_y = bounds.m_max.m_y;
    endpts[3].m_x = bounds.m_min.m_x;
    endpts[3].m_y = bounds.m_max.m_y;

    LineBuffer* dstpts = rewriter->ProcessW2DPoints(file, endpts, 4, false);
    std::auto_ptr<LineBuffer> spDstLB(dstpts);

    int width;
    int height;
    unsigned int* src = AGGImageIO::DecodePNG(pngGroup4Image.data(), pngGroup4Image.data_size(), width, height);

    if (src)
    {
        double angleRad = atan2((double)(dstpts->y_coord(1) - dstpts->y_coord(0)), (double)(dstpts->x_coord(1) - dstpts->x_coord(0)));
        double cx = 0.5 * (dstpts->x_coord(0) + dstpts->x_coord(2));
        double cy = 0.5 * (dstpts->y_coord(0) + dstpts->y_coord(2));

        double dx = dstpts->x_coord(1) - dstpts->x_coord(0);
        double dy = dstpts->y_coord(1) - dstpts->y_coord(0);

        double width = sqrt(dx*dx + dy*dy);

        dx = dstpts->x_coord(2) - dstpts->x_coord(1);
        dy = dstpts->y_coord(2) - dstpts->y_coord(1);

        double height = sqrt(dx*dx + dy*dy);

        // make the call with a positive height since DecodePNG already returns an inverted image
        AGGRenderer::DrawScreenRaster((agg_context*)rewriter->GetW2DTargetImage(), (unsigned char*)src, pngGroup4Image.columns() * pngGroup4Image.rows() * 4, RS_ImageFormat_ARGB,
                                   pngGroup4Image.columns(), pngGroup4Image.rows(), cx, cy, width, height, angleRad * M_180PI);

        delete[] src;
    }

    LineBufferPool::FreeLineBuffer(rewriter->GetBufferPool(), spDstLB.release());
    return WT_Result::Success;
}


WT_Result agr_process_polyline(WT_Polyline & polyline, WT_File & file)
{
    if (file.rendition().visibility().visible() == WD_False)
        return WT_Result::Success;

    AGGRenderer* rewriter = (AGGRenderer*)file.stream_user_data();

    if (!rewriter->LayerPassesFilter())
        return WT_Result::Success;

    WT_RGBA32 rgba = file.rendition().color().rgba();
    RS_Color color = RS_Color(rgba.m_rgb.r, rgba.m_rgb.g, rgba.m_rgb.b, rgba.m_rgb.a);

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

        RS_Color override = rewriter->GetOverrideColorMarker().style().outline().color();

        if (override.argb() != RS_Color::EMPTY_COLOR_ARGB)
            color = override;
    }

    //do all necessary transformations and clipping
    LineBuffer* dstpts = rewriter->ProcessW2DPoints(file, polyline.points(), polyline.count(), true);
    std::auto_ptr<LineBuffer> spDstLB(dstpts);

    if (dstpts)
    {
        double weightpx = rewriter->ScaleW2DNumber(file, file.rendition().line_weight().weight_value());
        weightpx = rs_max(1.0, weightpx);
        SE_LineStroke lineStroke(color.argb(), weightpx);
        AGGRenderer::DrawScreenPolyline((agg_context*)rewriter->GetW2DTargetImage(), dstpts, NULL, lineStroke);
        LineBufferPool::FreeLineBuffer(rewriter->GetBufferPool(), spDstLB.release());
    }

    return WT_Result::Success;
}


WT_Result agr_process_text(WT_Text & text, WT_File & file)
{
    if (file.rendition().visibility().visible() == WD_False)
        return WT_Result::Success;

    AGGRenderer* rewriter = (AGGRenderer*)file.stream_user_data();

    if (!rewriter->LayerPassesFilter())
        return WT_Result::Success;

    WT_RGBA32 rgba = file.rendition().color().rgba();
    RS_Color color = RS_Color(rgba.m_rgb.r, rgba.m_rgb.g, rgba.m_rgb.b, rgba.m_rgb.a);

    //process override color, if any
    //text override color is stored in the bg color of the fill
    //which is otherwise unused
    if (rewriter->IsSymbolW2D() && rewriter->GetOverrideColorMarker().style().background().argb() != RS_Color::EMPTY_COLOR_ARGB)
    {
        RS_Color override = rewriter->GetOverrideColorMarker().style().background();

        if (override.argb() != RS_Color::EMPTY_COLOR_ARGB)
            color = override;
    }

    WT_Logical_Point pt = text.position();

    LineBuffer* dstpts = rewriter->ProcessW2DPoints(file, &pt, 1, true);
    std::auto_ptr<LineBuffer> spDstLB(dstpts);

    if (dstpts)
    {
        const WT_Font& font = file.rendition().font();

        //rescale font height
        double hgt = 0.75 * rewriter->ScaleW2DNumber(file, font.height().height());
        double angleRad = (double)font.rotation().rotation() * 2.0 * M_PI / 65536.0;

        //match the font
        int style = RS_FontStyle_Regular;

        if (font.style().bold() == WD_True)
            style |= (int)RS_FontStyle_Bold;
        if (font.style().italic() == WD_True)
            style |= (int)RS_FontStyle_Italic;
        if (font.style().underlined() == WD_True)
            style |= (int)RS_FontStyle_Underline;

        wchar_t* uni_name = WT_String::to_wchar(font.font_name().name().length(),
                                                font.font_name().name().unicode());
        RS_String rs_uni_name = uni_name;
        RS_FontDef fdef(rs_uni_name, hgt, (RS_FontStyle_Mask)style, RS_Units_Model);
        delete [] uni_name;

        const RS_Font* rsfont = rewriter->FindFont(fdef);

        //draw the text
        wchar_t* uni_text = WT_String::to_wchar(text.string().length(), text.string().unicode());

        //width is not used by AGGRenderer - can leave at zero
        AGGRenderer::DrawString((agg_context*)rewriter->GetW2DTargetImage(), uni_text, dstpts->x_coord(0), dstpts->y_coord(0), 0.0, hgt, rsfont, color, angleRad);

        delete [] uni_text;

        LineBufferPool::FreeLineBuffer(rewriter->GetBufferPool(), spDstLB.release());
    }

    //TODO: optionally text position can be specified using
    //a bounds box. This case needs to be handled here too
    return WT_Result::Success;
}


WT_Result agr_process_visibility(WT_Visibility & visibility, WT_File & file)
{
    //AGGRenderer* rewriter = (AGGRenderer*)file.stream_user_data();
    file.rendition().visibility() = visibility;
    return WT_Result::Success;
}


WT_Result agr_process_codePage(WT_Code_Page & /*codePage*/, WT_File & /*file*/)
{
    //AGGRenderer* rewriter = (AGGRenderer*)file.stream_user_data();
    //(*rewriter->GetW2DTargetImage()).desired_rendition().code_page() = codePage;
    return WT_Result::Success;
}


WT_Result agr_process_plotInfo(WT_Plot_Info & /*plotInfo*/, WT_File & /*file*/)
{
    //AGGRenderer* rewriter = (AGGRenderer*)file.stream_user_data();
    //plotInfo.serialize(*rewriter->GetW2DTargetImage());
    return WT_Result::Success;
}


WT_Result agr_process_background(WT_Background & /*background*/, WT_File & /*file*/)
{
    //AGGRenderer* rewriter = (AGGRenderer*)file.stream_user_data();
    //background.serialize(*rewriter->GetW2DTargetImage());
    return WT_Result::Success;
}


WT_Result agr_process_polymarker(WT_Polymarker & /*polymarker*/, WT_File & /*file*/)
{
    //AGGRenderer* rewriter = (AGGRenderer*)file.stream_user_data();

    //do all necessary transformations and clipping
    //const RS_D_Point* dstpts = rewriter->ProcessW2DPoints(file, polymarker.points(), polymarker.count(), true);
    //dstpts = NULL;

    //WT_Polymarker poly2(outpts, dstpts, true);
    //poly2.serialize(*rewriter->GetW2DTargetImage());

    return WT_Result::Success;
}


WT_Result agr_process_markerSize(WT_Marker_Size & /*markerSize*/, WT_File & /*file*/)
{
    //AGGRenderer* rewriter = (AGGRenderer*)file.stream_user_data();

    //rescale the marker size
    //WT_Integer32 size2 = (WT_Integer32)rewriter->ScaleW2DNumber(file, markerSize.size());

    //(*rewriter->GetW2DTargetImage()).desired_rendition().marker_size() = WT_Marker_Size(size2);
    return WT_Result::Success;
}


WT_Result agr_process_markerSymbol(WT_Marker_Symbol & /*markerSymbol*/, WT_File & /*file*/)
{
    return WT_Result::Success;
}


WT_Result agr_process_mergeControl(WT_Merge_Control & /*mergeControl*/, WT_File & /*file*/)
{
    return WT_Result::Success;
}


WT_Result agr_process_projection(WT_Projection & /*projection*/, WT_File & /*file*/)
{
    return WT_Result::Success;
}


WT_Result agr_process_inkedArea(WT_Inked_Area & /*inkedArea*/, WT_File & /*file*/)
{
    return WT_Result::Success;
}


WT_Result agr_process_alignment(WT_Alignment & /*alignment*/, WT_File & /*file*/)
{
    return WT_Result::Success;
}


WT_Result agr_process_penPattern(WT_Pen_Pattern & /*penPattern*/, WT_File & /*file*/)
{
    return WT_Result::Success;
}


WT_Result agr_process_linePattern(WT_Line_Pattern & /*linePattern*/, WT_File & /*file*/)
{
    return WT_Result::Success;
}


WT_Result agr_process_fillPattern(WT_Fill_Pattern & /*fillPattern*/, WT_File & /*file*/)
{
    return WT_Result::Success;
}


WT_Result agr_process_dashPattern(WT_Dash_Pattern & /*dashPattern*/, WT_File & /*file*/)
{
    return WT_Result::Success;
}


WT_Result agr_process_lineStyle(WT_Line_Style & /*lineStyle*/, WT_File & /*file*/)
{
    return WT_Result::Success;
}


WT_Result agr_process_layer(WT_Layer & layer, WT_File & file)
{
    AGGRenderer* rewriter = (AGGRenderer*)file.stream_user_data();

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

#ifdef _WIN32
        wchar_t* token = wcstok(strTok, L",");
#else
        wchar_t* ptr;
        wchar_t* token = wcstok(strTok, L",", &ptr);
#endif

        while (token)
        {
            if (wcscmp(token, name) == 0)
            {
                rewriter->LayerPassesFilter() = true;
                break;
            }

#ifdef _WIN32
            token = wcstok(NULL, L",");
#else
            token = wcstok(NULL, L",", &ptr);
#endif

        }
    }

    delete [] name;

    return WT_Result::Success;
}
