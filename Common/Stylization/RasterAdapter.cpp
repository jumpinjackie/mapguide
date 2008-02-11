//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
#include "Renderer.h"
#include "RasterAdapter.h"
#include "FeatureTypeStyleVisitor.h"
#include "GridData.h"
#include "GridStylizer.h"


//////////////////////////////////////////////////////////////////////////////
RasterAdapter::RasterAdapter(LineBufferPool* lbp) : GeometryAdapter(lbp)
{
    m_pGridData = NULL;
    m_pGridStylizer = NULL;
}


//////////////////////////////////////////////////////////////////////////////
RasterAdapter::~RasterAdapter()
{
    if(m_pGridData != NULL)
        delete m_pGridData;
    if(m_pGridStylizer != NULL)
        delete m_pGridStylizer;
}


void RasterAdapter::Stylize(Renderer*                   renderer,
                            RS_FeatureReader*           features,
                            bool                        initialPass,
                            FdoExpressionEngine*        exec,
                            RS_Raster*                  raster,
                            MdfModel::GridColorStyle*   style,
                            MdfModel::GridSurfaceStyle* surfStyle,
                            const MdfModel::MdfString*  /*tooltip*/,
                            const MdfModel::MdfString*  /*url*/,
                            RS_ElevationSettings*       /*elevSettings*/)
{
    m_exec = exec;

    //
    //now compute how big we want the image to be, in pixels
    //

    //first get the map data extent
    RS_Bounds mapExt = renderer->GetBounds();

    //get the map extent of the image
    //TODO: need to transform to destination coordinate system
    RS_Bounds imgExt = raster->GetExtent();

    //intersect the image extent with the requested map extent
    RS_Bounds intExt = RS_Bounds::Intersect(mapExt, imgExt);

    //if raster bounds outside map extent, don't stylize
    if (intExt.IsValid())
    {
        //compute the needed image size
        double factor = rs_max(intExt.width() / mapExt.width(), intExt.height() / mapExt.height());
        if (factor > 1.0)
            factor = 1.0;

        //compute the size of the viewer screen -- since we don't know it directly
        double pixelsPerMapUnit = renderer->GetMetersPerUnit() / METERS_PER_INCH * renderer->GetDpi() / renderer->GetMapScale();
        int devW = (int)(mapExt.width() * pixelsPerMapUnit);
        int devH = (int)(mapExt.height() * pixelsPerMapUnit);

        int imgW = (int)(factor * devW + 0.5);
        int imgH = (int)(factor * devH + 0.5);

        while (imgW >= 2048 || imgH >= 2048)
        {
            imgW >>= 1;
            imgH >>= 1;
        }

        //NOTE: Switching back to the code above since RESAMPLE doesn't scale to the
        //correct image size -- it scales to the full screen size instead
        /*
        //NOTE: since we use a RESAMPLE query for rasters for both RFP and WMS,
        //we know they are already at the correct size, so we do not need to
        //compute a window size here anymore (i.e. the code above should no longer
        //be needed -- it is here for reference or if things change in the future
        int imgW = raster->GetOriginalWidth();
        int imgH = raster->GetOriginalHeight();
        */

        //TODO: check if we need to move Map's AdjustResolutionWithExtent method and
        //adjust the resolution imgW, imgH here.
        m_pGridData = new GridData(Point2D(imgExt.minx, imgExt.miny),
                                   imgExt.width(), imgExt.height(),
                                   imgW, imgH);
        m_pGridStylizer = new GridStylizer();

        wchar_t bandName[10];
        //NOTE!!!Only supporting 1 band at this time. Otherwize we have to call SetCurrentBand()
        //on the FdoIRaster which we don't have here. TODO: Modify MgRaster::GetStream() to request a specific
        //band.
        swprintf(bandName, 10, /*NOXLATE*/L"%d", 1);
        //raster->SetCurrentBand(i);
        m_pGridData->ReadRaster(raster, bandName, imgW, imgH,
                                imgExt.minx, imgExt.miny, imgExt.maxx, imgExt.maxy, true);

        bool succeeded = m_pGridStylizer->ApplyStyles(m_pGridData, surfStyle, style);
        if(succeeded)
        {
           //use GDRenderer
            Band* pColorBand = m_pGridData->GetColorBand();
            renderer->StartFeature(features, initialPass, NULL, NULL, NULL);

            MdfModel::HillShade* hillShadeStyle = style->GetHillShade();
            if (NULL != hillShadeStyle)
            {
                //for elevation data we are not resampling we just pass in the whole image always and let the renderer figure out the pixed extents
                renderer->ProcessRaster(pColorBand->GetRawPointer(), imgW * imgH * 4, RS_ImageFormat_ARGB, imgW, imgH, intExt);
            }
            else
            {
                renderer->ProcessRaster(pColorBand->GetRawPointer(), imgW * imgH * 4, RS_ImageFormat_ARGB, imgW, imgH, imgExt);
            }
        }

        // Just for handle the default MG raster layer style, which is created by DataConnect.
        // TODO: Erase them when the MG studio support edit style for raster like MAP
        else
        {
            //get style rules -- currently used for bitonal only
            MdfModel::RuleCollection* rules = style->GetRules();

            int bpp = raster->GetBitsPerPixel();

            RS_InputStream* reader = raster->GetStream(RS_ImageFormat_ABGR, imgW, imgH);

            if (reader)
            {
                if (imgW > 0 && imgH > 0) //this is only a sanity check... should not happen
                {
                    //allocate destination image
                    //currently we will always stylize into an RGBA32 destination image
                    size_t imgBytes = imgW * imgH * 4; //hardcoded for 32 bit output
                    unsigned char* dst = new unsigned char[imgBytes];

                    switch (bpp)
                    {
                    case 32: DecodeRGBA(reader, dst, imgW, imgH); break;
                    case 24: DecodeRGB(reader, dst, imgW, imgH); break;
                    case 8: {
                                RS_InputStream* pal = raster->GetPalette();
                                DecodeMapped(reader, pal, dst, imgW, imgH);
                                if (pal)
                                    delete pal;
                            }
                            break;
                    case 1: {
                                //for bitonal, get the fore- and background colors first
                                RS_Color fg(0,0,0,255);
                                RS_Color bg(255, 255, 255, 0);

                                //just assume two rules, one for each of the colors
                                if (rules->GetCount() == 2)
                                {
                                    MdfModel::GridColorRule* gcr = (MdfModel::GridColorRule*)rules->GetAt(0);
                                    MdfModel::GridColorExplicit* gce = dynamic_cast<MdfModel::GridColorExplicit*>(gcr->GetGridColor());

                                    if (gce)
                                        EvalColor(gce->GetExplicitColor(), fg);

                                    gcr = (MdfModel::GridColorRule*)rules->GetAt(1);
                                    gce = dynamic_cast<MdfModel::GridColorExplicit*>(gcr->GetGridColor());

                                    if (gce)
                                        EvalColor(gce->GetExplicitColor(), bg);
                                }

                                DecodeBitonal(reader, fg, bg, dst, imgW, imgH);
                            }
                            break;
                    default: break;
                    }

                    RS_String tip;
                    RS_String eurl;
    //              if (tooltip && !tooltip->empty())
    //                  EvalString(tooltip, tip);
    //              if (url && !url->empty())
    //                  EvalString(*url, eurl);

                    renderer->StartFeature(features, true, tip.empty()? NULL : &tip, eurl.empty()? NULL : &eurl, NULL);
                    renderer->ProcessRaster(dst, imgW * imgH * 4, RS_ImageFormat_ABGR, imgW, imgH, intExt);

                    delete [] dst;
                }

                delete reader; //caller deletes reader
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// reads a 32 bpp RGBA image stream
void RasterAdapter::DecodeRGBA(RS_InputStream* is, unsigned char* dst, int w, int h)
{
    //find if rows are padded or not
    //ATIL pads on 8 byte boundary, WMS doesn't for example

    //we have to copy row by row, since rasters returned by
    //RFP/ATIL are padded to multiples of 8 bytes
    int pitch = (int)is->available() / h;
    int rowSize = w * 4;

    //how many padding bytes we need to read at the end of the row
    int pad = 0;

    if (pitch != rowSize)
        pad = pitch - rowSize;

    unsigned char* padding = (unsigned char*)alloca(pad);

    for (int j=0; j<h; j++)
    {
        int read = -1;
        int totalRead = 0;

        //read enough bytes to fill the row
        while (totalRead < rowSize && read != 0)
        {
            read = (int)is->read(dst + j*rowSize + totalRead, rowSize - totalRead);
            totalRead += read;
        }

        //now read the padding bytes if any
        if (pad)
        {
            int read = 0;
            read = (int)is->read(padding, pad);
            _ASSERT(pad == read);
        }
    }
}


//////////////////////////////////////////////////////////////////////////////
// reads a 24 bpp RGB stream
void RasterAdapter::DecodeRGB(RS_InputStream* is, unsigned char* dst, int w, int h)
{
    //we have to copy row by row, since rasters returned by
    //RFP/ATIL are padded to multiples of 8 bytes
    int pitch = (int)is->available() / h;

    //we will process the raster row by row, for better efficiency (hopefully)
    unsigned char* row = (unsigned char*)alloca(pitch);
    unsigned int* dstptr = (unsigned int*)dst;

    for (int j=0; j<h; j++)
    {
        int read = -1;
        int totalRead = 0;

        //read enough bytes to fill the row, including pad
        while (totalRead < pitch && read != 0)
        {
            read = (int)is->read(row + totalRead, pitch - totalRead);
            totalRead += read;
        }

        //now convert from RGB to RGBA and copy to destination
        unsigned char* px = row;
        for (int i=0; i<w; i++)
        {
            //inefficient, but whoever gave us an RGB-aligned raster
            //deserves it!
            int r = *px++;
            int g = *px++;
            int b = *px++;

            *dstptr++ = 0xFF000000 | (b << 16) | (g << 8) | (r);
        }
    }
}


//////////////////////////////////////////////////////////////////////////////
// 8 bit mapped
void RasterAdapter::DecodeMapped(RS_InputStream* is, RS_InputStream* pal, unsigned char* dst, int w, int h)
{
    //read off the color map
    unsigned char* palette = NULL;

    if (pal)
    {
        palette = (unsigned char*)alloca(pal->available());
        pal->read(palette, pal->available());
    }
    else
    {
        //if there is no palette, use a standard grey scale mapping
        palette = (unsigned char*)alloca(256*sizeof(int));

        for (int i=0; i<256; i++)
            ((int*)palette)[i] = 255 << 24 | i << 16 | i << 8 | i;
    }

    //we have to copy row by row, since rasters returned by
    //RFP/ATIL are padded to multiples of 8 bytes
    int pitch = (int)is->available() / h;

    //we will process the raster row by row, for better efficiency (hopefully)
    unsigned char* row = (unsigned char*)alloca(pitch);
    unsigned int* dstptr = (unsigned int*)dst;

    for (int j=0; j<h; j++)
    {
        int read = -1;
        int totalRead = 0;

        //read enough bytes to fill the row, including any pad
        while (totalRead < pitch && read != 0)
        {
            read = (int)is->read(row + totalRead, pitch - totalRead);
            totalRead += read;
        }

        //now convert from mapped to RGBA and copy to destination
        unsigned char* px = row;
        for (int i=0; i<w; i++) *dstptr++ = ((int*)palette)[*px++];
    }
}


//////////////////////////////////////////////////////////////////////////////
// bitonal
void RasterAdapter::DecodeBitonal(RS_InputStream* is, const RS_Color& fg, const RS_Color& bg, unsigned char* dst, int w, int h)
{
    int fgc = fg.abgr();
    int bgc = bg.abgr();

    //we have to copy row by row, since rasters returned by
    //RFP/ATIL are padded to multiples of 8 bytes
    int pitch = (int)is->available() / h;
    int rowSize = w / 8; //8 pixels per byte

    //we will process the raster row by row, for better efficiency (hopefully)
    unsigned char* row = (unsigned char*)alloca(pitch);
    unsigned int* dstptr = (unsigned int*)dst;

    for (int j=0; j<h; j++)
    {
        int read = -1;
        int totalRead = 0;

        //read enough bytes to fill the row, including padding bytes
        while (totalRead < pitch && read != 0)
        {
            read = (int)is->read(row + totalRead, pitch - totalRead);
            totalRead += read;
        }

        //now convert from bitonal to RGBA and copy to destination
        unsigned char* px = row;

        for (int i=0; i<rowSize; i++)
        {
            unsigned char bits = *px++;

            *dstptr++ = (bits & 0x80)? fgc : bgc;
            *dstptr++ = (bits & 0x40)? fgc : bgc;
            *dstptr++ = (bits & 0x20)? fgc : bgc;
            *dstptr++ = (bits & 0x10)? fgc : bgc;
            *dstptr++ = (bits & 0x08)? fgc : bgc;
            *dstptr++ = (bits & 0x04)? fgc : bgc;
            *dstptr++ = (bits & 0x02)? fgc : bgc;
            *dstptr++ = (bits & 0x01)? fgc : bgc;
        }

        //do the last pixels of the row -- we may not need
        //all bits of the last byte, so loop accordingly
        int last_bit_mask = 0x80 >> (w % 8);
        unsigned char bits = *px++;

        for (int mask = 0x80; mask > last_bit_mask; mask /= 2)
            *dstptr++ = (bits & mask)? fgc : bgc;
    }
}

