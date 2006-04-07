//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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
#include "GDRenderer.h"
#include "GDW2DRewriter.h"
#include "gd.h"
#include "GDUtils.h"

////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------
//
//                    I/O Overloads
//
//------------------------------------------------------------------
////////////////////////////////////////////////////////////////////

WT_Result gdr_open (WT_File & /*file*/)
{
    return WT_Result::Success;
}


WT_Result gdr_close (WT_File & file)
{
    file.set_stream_user_data(WD_Null);
    return WT_Result::Success;
}


WT_Result gdr_read (WT_File & file, int desired_bytes, int & bytes_read, void * buffer)
{
    bytes_read = (int)((GDRenderer*)file.stream_user_data())->_GetInputStream()->read(buffer, desired_bytes);

    if (!bytes_read)
        return WT_Result::Unknown_File_Read_Error;

    return WT_Result::Success;
}


WT_Result gdr_seek(WT_File & file, int distance, int & amount_seeked)
{
    off_t prev = 0;
    ((GDRenderer*)file.stream_user_data())->_GetInputStream()->seek(distance, prev);
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


WT_Result gdr_process_units (WT_Units & units, WT_File & file)
{
    GDRenderer* rewriter = (GDRenderer*)file.stream_user_data();
    //do not write the units -- the destination W2D has different units
    //units.serialize(*rewriter->_GetW2D());

    //Instead save the in the desired rendition of the source file
    //so that we can get to them when transforming coordinates for DWF layers
    //Note that for DWF point symbols we do not do that
    if (!rewriter->IsSymbolW2D())
        file.desired_rendition().drawing_info().units() = units;

    return WT_Result::Success;
}


WT_Result gdr_process_color (WT_Color & color, WT_File & file)
{
    GDRenderer* rewriter = (GDRenderer*)file.stream_user_data();

    //note reversal of r and b
    WT_RGBA32 rgba = color.rgba();
    RS_Color c(rgba.m_rgb.b,rgba.m_rgb.g,rgba.m_rgb.r,rgba.m_rgb.a);

    int gdc = ConvertColor((gdImagePtr)rewriter->GetImage(), c);
    gdImageSetAntiAliased((gdImagePtr)rewriter->GetImage(), gdc);

    file.rendition().color() = color;

    return WT_Result::Success;
}


WT_Result gdr_process_lineWeight (WT_Line_Weight & lineWeight, WT_File & file)
{
//  GDRenderer* rewriter = (GDRenderer*)file.stream_user_data();

    //rescale the line weight
    //int dstWeight = rewriter->ScaleW2DNumber(file, lineWeight.weight_value());
    file.rendition().line_weight() = lineWeight;

    return WT_Result::Success;
}


WT_Result gdr_process_viewport (WT_Viewport & viewport, WT_File & file)
{
    GDRenderer* rewriter = (GDRenderer*)file.stream_user_data();

    //IMPORTANT: AutoCAD defines a viewport for each view of the
    //drawing displayed in paper space. These views are arranged
    //to fit on the page via several of the viewports.
    //For symbols coming from the import engine, only one viewport
    //is set, corresponding to the extent all the symbol entities.
    //We use this viewport setting in order to determine the extent
    //of the W2D entities in the symbol W2D, since they are not the
    //same as the extent that Studio will use when generating symbol
    //W2Ds (0,0,SYMBOL_MAX,SYMBOL_MAX).

    //In the case this code is used for DWF layers, things are more complicated
    //as there are often multiple viewports, defining extents for part of the data.
    //We cannot handle this for now

    if (rewriter->IsSymbolW2D() && ! rewriter->IsViewportSet())
        rewriter->UpdateSymbolTrans(file, viewport);

    //(*rewriter->GetImage()).desired_rendition().viewport() = viewport;

    return WT_Result::Success;
}


WT_Result gdr_process_colorMap (WT_Color_Map & colorMap, WT_File & file)
{
    file.rendition().color_map() = colorMap;
    return WT_Result::Success;
}


WT_Result gdr_process_contourSet (WT_Contour_Set & contourSet, WT_File & file)
{
    if (file.rendition().visibility().visible() == WD_False)
        return WT_Result::Success;

    GDRenderer* rewriter = (GDRenderer*)file.stream_user_data();

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

    int gdc = ConvertColor((gdImagePtr)rewriter->GetImage(), color);

    //transform point and draw the contour set
    int numcntrs = contourSet.contours();
    WT_Integer32* cntrcounts = contourSet.counts();
    int totalPts = contourSet.total_points();

    const RS_D_Point* dst_cntr = rewriter->ProcessW2DPoints(
        file, (WT_Logical_Point*)contourSet.points(), totalPts, true);

    if (dst_cntr)
    {
        rs_gdImageMultiPolygon((gdImagePtr)rewriter->GetImage(),
                    (int*)cntrcounts, numcntrs,
                    (gdPointPtr)(dst_cntr), totalPts,
                    gdc);
    }

    return WT_Result::Success;
}


WT_Result gdr_process_fill (WT_Fill & fill, WT_File & file)
{
    file.rendition().fill() = fill;
    return WT_Result::Success;
}


WT_Result gdr_process_font (WT_Font & font, WT_File & file)
{
    /*GDRenderer* rewriter = (GDRenderer*)file.stream_user_data();*/

    file.rendition().font() = font;
    return WT_Result::Success;
}


WT_Result gdr_process_gouraudPolyline (WT_Gouraud_Polyline & /*gouraudPolyline*/, WT_File & /*file*/)
{
    //GDRenderer* rewriter = (GDRenderer*)file.stream_user_data();
    //TODO: if the polyline is clipped, the colors will be wrong
    //WT_Gouraud_Polyline poly2(numpts, dstpts, gouraudPolyline.colors(), true);
    //poly2.serialize(*rewriter->GetImage());
    return WT_Result::Success;
}


WT_Result gdr_process_gouraudPolytriangle (WT_Gouraud_Polytriangle & /*gouraudPolytriangle*/, WT_File & /*file*/)
{
    //GDRenderer* rewriter = (GDRenderer*)file.stream_user_data();
    //TODO: clipping for polytriangles is needed
    //gouraudPolytriangle.serialize(*rewriter->GetImage());
    return WT_Result::Success;
}


WT_Result gdr_process_image (WT_Image & image, WT_File & file)
{
    if (file.rendition().visibility().visible() == WD_False)
        return WT_Result::Success;

    GDRenderer* rewriter = (GDRenderer*)file.stream_user_data();

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

    const RS_D_Point* dstpts = rewriter->ProcessW2DPoints(file, endpts, 4, false);

    gdImagePtr src = NULL;

    if (image.format() == WT_Image::RGBA)
    {
        src = gdImageCreateTrueColor(image.columns(), image.rows());
        gdImageAlphaBlending(src, 0);

        for (int j=0; j<image.rows(); j++)
            for (int i=0; i<image.columns(); i++)
            {
                WT_RGBA32 rgba = ((WT_RGBA32*)image.data())[i + j*image.columns()];
                RS_Color rsc(rgba.m_rgb.b, rgba.m_rgb.g, rgba.m_rgb.r, rgba.m_rgb.a);
                int gdc = ConvertColor(src, rsc);
                gdImageSetPixel(src, i, j, gdc);
            }

        gdImageAlphaBlending(src, 1);
    }
    else if (image.format() == WT_Image::RGB)
    {
        src = gdImageCreateTrueColor(image.columns(), image.rows());

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
                gdImageSetPixel(src, i, j, ConvertColor(src, rsc));
            }
    }
    else if (image.format() == WT_Image::JPEG)
    {
        src = gdImageCreateFromJpegPtr(image.data_size(), (void*)image.data());
    }

    if (src)
    {
        //case of no rotation
        if (   dstpts[0].y == dstpts[1].y
            && dstpts[1].x == dstpts[2].x)
        {
            //case of no rotation
            int left = rs_min(dstpts[0].x, dstpts[1].x);
            int top = rs_min(dstpts[1].y, dstpts[2].y);
            int width = dstpts[1].x - dstpts[0].x;
            int height = dstpts[1].y - dstpts[2].y; //y is inverted

            gdImageCopyResampled ((gdImagePtr)rewriter->GetImage(), src,
                            left,top,  //dstX, dstY
                            0, 0, //srcX, srcY
                            width, height, //int dstW, int dstH,
                            image.columns(), image.rows() //srcW, srcH
                            );
        }
        else
        {
            int dx = dstpts[1].x - dstpts[0].x;
            int dy = dstpts[1].y - dstpts[0].y;

            int width = (int)sqrt((double)dx*dx + dy*dy);

            dx = dstpts[2].x - dstpts[1].x;
            dy = dstpts[2].y - dstpts[1].y;

            int height = (int)sqrt((double)dx*dx + dy*dy);

            //see if we need to rescale the original image
            gdImagePtr resized = NULL;

            if (width != src->sx || height != src->sy)
            {
                resized = gdImageCreateTrueColor(width, height);

                //make it transparent
                gdImageAlphaBlending(resized, 0);
                gdImageFilledRectangle(resized, 0, 0, width-1, height, 0x7f000000);
                gdImageAlphaBlending(resized, 1);

                gdImageCopyResampled(resized, src,
                            0, 0,  //dstX, dstY
                            0, 0, //srcX, srcY
                            width, height, //int dstW, int dstH,
                            src->sx, src->sy );//srcW, srcH
            }

            double midx = 0.5 * (dstpts[0].x + dstpts[2].x);
            double midy = 0.5 * (dstpts[0].y + dstpts[2].y);

            double angle = atan2((double)(dstpts[1].y - dstpts[0].y), (double)(dstpts[1].x - dstpts[0].x));

            gdImageCopyRotated ((gdImagePtr)rewriter->GetImage(),
                            resized ? resized : src,
                            midx, midy,  //dstX, dstY
                            0, 0, //srcX, srcY
                            resized ? resized->sx : src->sx,
                            resized ? resized->sx : src->sy, //srcW, srcH
                            (int)(-angle * 180.0 / M_PI)
                            );

            if (resized)
                gdImageDestroy(resized);
        }

        gdImageDestroy(src);
    }

    return WT_Result::Success;
}


WT_Result gdr_process_origin (WT_Origin & origin, WT_File & file)
{
    GDRenderer* rewriter = (GDRenderer*)file.stream_user_data();

    WT_Logical_Point pt = origin.origin();

    //transform origin into new space
    const RS_D_Point* pts = rewriter->ProcessW2DPoints(file, &pt, 1, false);
    pts = NULL;

    //WT_Origin origin2(pts[0]);
    file.update_current_point (origin.origin());

    //origin2.serialize(*rewriter->GetImage());

    return WT_Result::Success;
}


WT_Result gdr_process_filledEllipse (WT_Filled_Ellipse & filledEllipse, WT_File & file)
{
    if (file.rendition().visibility().visible() == WD_False)
        return WT_Result::Success;

    GDRenderer* rewriter = (GDRenderer*)file.stream_user_data();

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

    WT_Logical_Point oldpos = filledEllipse.position();

    const RS_D_Point* dstpts = rewriter->ProcessW2DPoints(
        file, (WT_Logical_Point*)&oldpos, 1, false);

    WT_Integer32 major = rewriter->ScaleW2DNumber(file, filledEllipse.major());
    WT_Integer32 minor = rewriter->ScaleW2DNumber(file, filledEllipse.minor());

    //simple bounds check before we draw
    if ( !(dstpts[0].x + major < 0
        || dstpts[0].x - major > ((gdImagePtr)rewriter->GetImage())->sx
        || dstpts[0].y + minor < 0
        || dstpts[0].x - minor > ((gdImagePtr)rewriter->GetImage())->sy))
    {
        //negate because GD is left-handed coords
        float end   = 360.f - filledEllipse.start_degree();
        float start = 360.f - filledEllipse.end_degree();

        //gd does not like negative angles (it's sin/cos lookup table doesn't)
        while (start < 0.f)
        {
            start += 360.f;
            end   += 360.f;
        }

        //TODO: tilt. Need to tesselate into a line buffer in order to rotate.
        int gdc = ConvertColor((gdImagePtr)rewriter->GetImage(), color);
        gdImageFilledArc((gdImagePtr)rewriter->GetImage(),
                                dstpts[0].x, dstpts[0].y,
                                major, minor,
                                (int)start, (int)end, //negate because GD is left-handed coords
                                gdc, gdArc);
    }

    return WT_Result::Success;
}


WT_Result gdr_process_outlineEllipse (WT_Outline_Ellipse & outlineEllipse, WT_File & file)
{
    if (file.rendition().visibility().visible() == WD_False)
        return WT_Result::Success;

    GDRenderer* rewriter = (GDRenderer*)file.stream_user_data();

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

    WT_Logical_Point oldpos = outlineEllipse.position();

    const RS_D_Point* dstpts = rewriter->ProcessW2DPoints(
        file, (WT_Logical_Point*)&oldpos, 1, false);

    WT_Integer32 major = rewriter->ScaleW2DNumber(file, outlineEllipse.major());
    WT_Integer32 minor = rewriter->ScaleW2DNumber(file, outlineEllipse.minor());

    //simple bounds check before we draw
    if (!(dstpts[0].x + major < 0
        || dstpts[0].x - major > ((gdImagePtr)rewriter->GetImage())->sx
        || dstpts[0].y + minor < 0
        || dstpts[0].x - minor > ((gdImagePtr)rewriter->GetImage())->sy))
    {
        //negate because GD is left-handed coords
        float end = 360.f - outlineEllipse.start_degree();
        float start = 360.f - outlineEllipse.end_degree();

        //gd does not like negative angles (it's sin/cos lookup table doesn't)
        while (start < 0.f)
        {
            start += 360.f;
            end   += 360.f;
        }

        //TODO: tilt. Need to tesselate into a line buffer in order to rotate.
        int gdc = ConvertColor((gdImagePtr)rewriter->GetImage(), color);

        ////////////////////////
        // handle thickness
        int thick = rewriter->ScaleW2DNumber(file, file.rendition().line_weight().weight_value());

        gdImagePtr brush1 = NULL;

        if (thick > 1)
        {
            brush1 = rs_gdImageThickLineBrush(thick, color);
            gdImageSetBrush((gdImagePtr)rewriter->GetImage(), brush1);
        }

        gdImageArc((gdImagePtr)rewriter->GetImage(),
                                dstpts[0].x, dstpts[0].y,
                                major * 2, minor * 2,
                                (int)start, (int)end,
                                brush1 ? gdBrushed : gdc);

        if (brush1)
        {
            gdImageSetBrush((gdImagePtr)rewriter->GetImage(), NULL);
            gdImageDestroy(brush1);
        }
    }

    return WT_Result::Success;

}


WT_Result gdr_process_polygon (WT_Polygon & polygon, WT_File & file)
{
    if (file.rendition().visibility().visible() == WD_False)
        return WT_Result::Success;

    GDRenderer* rewriter = (GDRenderer*)file.stream_user_data();

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

    int gdc = ConvertColor((gdImagePtr)rewriter->GetImage(), color);

    //do all necessary transformations
    const RS_D_Point* dstpts = rewriter->ProcessW2DPoints(
        file, polygon.points(), polygon.count(), true);

    if (dstpts)
    {
        gdImageFilledPolygon((gdImagePtr)rewriter->GetImage(),
                            (gdPointPtr)dstpts,
                            polygon.count(),
                            gdc);
    }

    return WT_Result::Success;
}


WT_Result gdr_process_polytriangle (WT_Polytriangle & polytriangle, WT_File & file)
{
    if (file.rendition().visibility().visible() == WD_False)
        return WT_Result::Success;

    GDRenderer* rewriter = (GDRenderer*)file.stream_user_data();

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

    int gdc = ConvertColor((gdImagePtr)rewriter->GetImage(), color);

    WT_Logical_Point* srcpts = polytriangle.points();

    const RS_D_Point* dstpts = rewriter->ProcessW2DPoints(file, srcpts, polytriangle.count(), true);

    if (dstpts)
    {
        for (int i=2; i < polytriangle.count(); i++)
        {
            gdImageFilledPolygon((gdImagePtr)rewriter->GetImage(),
                                (gdPointPtr)(dstpts + i - 2),
                                3,
                                gdc);
        }
    }

    return WT_Result::Success;
}


WT_Result gdr_process_pngGroup4Image (WT_PNG_Group4_Image & pngGroup4Image, WT_File & file)
{
    if (file.rendition().visibility().visible() == WD_False)
        return WT_Result::Success;

    GDRenderer* rewriter = (GDRenderer*)file.stream_user_data();

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

    const RS_D_Point* dstpts = rewriter->ProcessW2DPoints(
        file, endpts, 4, false);

    gdImagePtr src = gdImageCreateFromPngPtr(pngGroup4Image.data_size(), (void*)pngGroup4Image.data());

    //case of no rotation
    if (   dstpts[0].y == dstpts[1].y
        && dstpts[1].x == dstpts[2].x)
    {
        //case of no rotation
        int left = rs_min(dstpts[0].x, dstpts[1].x);
        int top = rs_min(dstpts[1].y, dstpts[2].y);
        int width = dstpts[1].x - dstpts[0].x;
        int height = dstpts[1].y - dstpts[2].y; //y is inverted

        gdImageCopyResampled ((gdImagePtr)rewriter->GetImage(), src,
                        left,top,  //dstX, dstY
                        0, 0, //srcX, srcY
                        width, height, //int dstW, int dstH,
                        pngGroup4Image.columns(), pngGroup4Image.rows() //srcW, srcH
                        );
    }
    else
    {
        int dx = dstpts[1].x - dstpts[0].x;
        int dy = dstpts[1].y - dstpts[0].y;

        int width = (int)sqrt((double)dx*dx + dy*dy);

        dx = dstpts[2].x - dstpts[1].x;
        dy = dstpts[2].y - dstpts[1].y;

        int height = (int)sqrt((double)dx*dx + dy*dy);

        //see if we need to rescale the original image
        gdImagePtr resized = NULL;

        if (width != src->sx || height != src->sy)
        {
            resized = gdImageCreateTrueColor(width, height);

            gdImageCopyResampled(resized, src,
                        0, 0,  //dstX, dstY
                        0, 0, //srcX, srcY
                        width, height, //int dstW, int dstH,
                        src->sx, src->sy );//srcW, srcH
        }

        double midx = 0.5 * (dstpts[0].x + dstpts[2].x);
        double midy = 0.5 * (dstpts[0].y + dstpts[2].y);

        double angle = atan2((double)(dstpts[1].y - dstpts[0].y), (double)(dstpts[1].x - dstpts[0].x));

        gdImageCopyRotated ((gdImagePtr)rewriter->GetImage(),
                        resized ? resized : src,
                        midx, midy,  //dstX, dstY
                        0, 0, //srcX, srcY
                        resized ? resized->sx : src->sx,
                        resized ? resized->sx : src->sy, //srcW, srcH
                        (int)(-angle * 180.0 / M_PI)
                        );

        if (resized)
            gdImageDestroy(resized);
    }

    gdImageDestroy(src);

    return WT_Result::Success;
}


WT_Result gdr_process_polyline (WT_Polyline & polyline, WT_File & file)
{
    if (file.rendition().visibility().visible() == WD_False)
        return WT_Result::Success;

    GDRenderer* rewriter = (GDRenderer*)file.stream_user_data();

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

    int gdc = ConvertColor((gdImagePtr)rewriter->GetImage(), color);

    //do all necessary transformations and clipping
    const RS_D_Point* dstpts = rewriter->ProcessW2DPoints(
        file, polyline.points(), polyline.count(), true);

    if (dstpts)
    {
        int thick = rewriter->ScaleW2DNumber(file, file.rendition().line_weight().weight_value());
        gdImagePtr brush1 = NULL;

        if (thick > 1)
        {
            brush1 = rs_gdImageThickLineBrush(thick, color);
            gdImageSetBrush((gdImagePtr)rewriter->GetImage(), brush1);
        }

        gdImageOpenPolygon((gdImagePtr)rewriter->GetImage(),
                            (gdPointPtr)dstpts,
                            polyline.count(),
                            brush1 ? gdBrushed : gdc);

        if (brush1)
        {
            gdImageSetBrush((gdImagePtr)rewriter->GetImage(), NULL);
            gdImageDestroy(brush1);
        }
    }

    return WT_Result::Success;
}


WT_Result gdr_process_text (WT_Text & text, WT_File & file)
{
    if (file.rendition().visibility().visible() == WD_False)
        return WT_Result::Success;

    GDRenderer* rewriter = (GDRenderer*)file.stream_user_data();

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

    const RS_D_Point* dstpts = rewriter->ProcessW2DPoints(file, &pt, 1, true);

    if (dstpts)
    {
        const WT_Font& font = file.rendition().font();

        //rescale font height
        double hgt = 0.75 * rewriter->ScaleW2DNumber(file, font.height().height());
        double rot = (double)font.rotation().rotation() * 2 * M_PI / 65536.;

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

        RS_String font_file = L"";
        bool found = rewriter->FindFont(fdef, font_file);

        if (!found)
            font_file = L"c:\\windows\\fonts\\arial.ttf";

        //draw the text
        wchar_t* uni_text = WT_String::to_wchar(text.string().length(), text.string().unicode());

        rewriter->DrawString(uni_text, dstpts[0].x, dstpts[0].y, hgt, font_file, color, rot);

        delete [] uni_text;
    }

    //TODO: optionally text position can be specified using
    //a bounds box. This case needs to be handled here too

    return WT_Result::Success;
}


WT_Result gdr_process_visibility (WT_Visibility & visibility, WT_File & file)
{
    //GDRenderer* rewriter = (GDRenderer*)file.stream_user_data();
    file.rendition().visibility() = visibility;
    return WT_Result::Success;
}


WT_Result gdr_process_codePage (WT_Code_Page & /*codePage*/, WT_File & /*file*/)
{
    //GDRenderer* rewriter = (GDRenderer*)file.stream_user_data();
    //(*rewriter->GetImage()).desired_rendition().code_page() = codePage;
    return WT_Result::Success;
}


WT_Result gdr_process_plotInfo (WT_Plot_Info & /*plotInfo*/, WT_File & /*file*/)
{
    //GDRenderer* rewriter = (GDRenderer*)file.stream_user_data();
    //plotInfo.serialize(*rewriter->GetImage());
    return WT_Result::Success;
}


WT_Result gdr_process_background (WT_Background & /*background*/, WT_File & /*file*/)
{
    //GDRenderer* rewriter = (GDRenderer*)file.stream_user_data();
    //background.serialize(*rewriter->GetImage());
    return WT_Result::Success;
}


WT_Result gdr_process_polymarker (WT_Polymarker & polymarker, WT_File & file)
{
    GDRenderer* rewriter = (GDRenderer*)file.stream_user_data();

    //do all necessary transformations and clipping
    const RS_D_Point* dstpts = rewriter->ProcessW2DPoints(file, polymarker.points(), polymarker.count(), true);
    dstpts = NULL;

    //WT_Polymarker poly2(outpts, dstpts, true);
    //poly2.serialize(*rewriter->GetImage());

    return WT_Result::Success;
}


WT_Result gdr_process_markerSize (WT_Marker_Size & /*markerSize*/, WT_File & /*file*/)
{
    //GDRenderer* rewriter = (GDRenderer*)file.stream_user_data();

    //rescale the marker size
    //WT_Integer32 size2 = rewriter->ScaleW2DNumber(file, markerSize.size());

    //(*rewriter->GetImage()).desired_rendition().marker_size() = WT_Marker_Size(size2);
    return WT_Result::Success;
}


WT_Result gdr_process_markerSymbol (WT_Marker_Symbol & /*markerSymbol*/, WT_File & /*file*/)
{
    return WT_Result::Success;
}


WT_Result gdr_process_mergeControl (WT_Merge_Control & /*mergeControl*/, WT_File & /*file*/)
{
    return WT_Result::Success;
}


WT_Result gdr_process_projection (WT_Projection & /*projection*/, WT_File & /*file*/)
{
    return WT_Result::Success;
}


WT_Result gdr_process_inkedArea (WT_Inked_Area & /*inkedArea*/, WT_File & /*file*/)
{
    return WT_Result::Success;
}


WT_Result gdr_process_alignment (WT_Alignment & /*alignment*/, WT_File & /*file*/)
{
    return WT_Result::Success;
}


WT_Result gdr_process_penPattern (WT_Pen_Pattern & /*penPattern*/, WT_File & /*file*/)
{
    return WT_Result::Success;
}


WT_Result gdr_process_linePattern (WT_Line_Pattern & /*linePattern*/, WT_File & /*file*/)
{
    return WT_Result::Success;
}


WT_Result gdr_process_fillPattern (WT_Fill_Pattern & /*fillPattern*/, WT_File & /*file*/)
{
    return WT_Result::Success;
}


WT_Result gdr_process_dashPattern (WT_Dash_Pattern & /*dashPattern*/, WT_File & /*file*/)
{
    return WT_Result::Success;
}


WT_Result gdr_process_lineStyle (WT_Line_Style & /*lineStyle*/, WT_File & /*file*/)
{
    return WT_Result::Success;
}

WT_Result gdr_process_layer (WT_Layer & layer, WT_File & file)
{    
    GDRenderer* rewriter = (GDRenderer*)file.stream_user_data();
    wchar_t* name = WT_String::to_wchar(layer.layer_name().length(), layer.layer_name().unicode());

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

        while(token)
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
