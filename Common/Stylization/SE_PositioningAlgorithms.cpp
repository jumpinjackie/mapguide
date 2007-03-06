
#include "stdafx.h"
#include "SE_Include.h"
#include "SE_PositioningAlgorithms.h"
#include "SE_Renderer.h"
#include "SE_Bounds.h"


SE_RenderPointStyle* DeepClonePointStyle(SE_RenderPointStyle* st)
{
    SE_RenderPointStyle* ret = new SE_RenderPointStyle(); //LEAK
    *ret = *st;
    ret->bounds = st->bounds->Clone(); //LEAK
    SE_RenderText* txt = new SE_RenderText(); //LEAK
    *txt = *((SE_RenderText*)st->symbol[0]);
    ret->symbol[0] = txt;

    return ret;
}

void SE_PositioningAlgorithms::EightBall(SE_Renderer*    renderer, 
                          SE_LineBuffer*  geometry, 
                          SE_Matrix&      xform, 
                          SE_Style*       style, 
                          SE_RenderStyle* rstyle, 
                          double          mm2px
                          )
{
    //this placement algorithm implements the MapGuide dynamic point labeling algorithm 
    //which means 8 candidate labels generated for each symbol
    
    double cx = 0.0;
    double cy = 0.0;
    double dummy;

    //get actual feature point
    geometry->Centroid(LineBuffer::ctPoint, &cx, &cy, &dummy);

    //assume there is a single text label in the render symbol
    //and extract it from in there
    SE_RenderPointStyle* rstyle2 = new SE_RenderPointStyle(); //LEAK
    rstyle2->addToExclusionRegions = rstyle->addToExclusionRegions;
    rstyle2->checkExclusionRegions = rstyle->checkExclusionRegions;
    rstyle2->drawLast = rstyle->drawLast;
    rstyle2->renderPass = rstyle->renderPass;
    rstyle2->bounds = rstyle->bounds->Clone();
    SE_RenderText* txt = NULL;
    
    for (SE_RenderSymbol::iterator iter = rstyle->symbol.begin(); iter != rstyle->symbol.end(); iter++)
    {
        if ((*iter)->type == SE_TextPrimitive)
        {
            SE_RenderText* rt = (SE_RenderText*)(*iter);

            txt = new SE_RenderText();
            *txt = *rt;
            rstyle2->symbol.push_back(txt);

            break;
        }
    }

    //also need to get the bounds of the last drawn point symbol, so that we know how much to offset the label
    //TODO:
    RS_Bounds symbol_bounds(1,1,0,0); //init to invalid
    double symbol_width = 0.001 * 32.0 / renderer->GetPixelsPerMillimeterScreen(); //symbol width in meters
    double symbol_height = 0.001 * 32.0 / renderer->GetPixelsPerMillimeterScreen(); //symbol height in meters
    double symbol_rot_deg = 0.0;

    double op_pts[16];
    
    // calculate a 2 pixel offset to allow for label ghosting
    double offset = 0.001 * 2.0 / renderer->GetPixelsPerMillimeterScreen(); //2 pixels (in meters)
    
    //compute how far label needs to be offset from
    //center point of symbol

    double w = 0.5 * symbol_width;
    double h = 0.5 * symbol_height;
    double ch = 0;		// vertical center point
    double cw = 0;		// horizontal center point

    w += offset;
    h += offset;
    
    bool useBounds = symbol_bounds.IsValid();
    if (useBounds)
    {
        symbol_bounds.maxx += offset;    symbol_bounds.maxy += offset;
        symbol_bounds.minx -= offset;    symbol_bounds.miny -= offset;
        ch = (symbol_bounds.maxy + symbol_bounds.miny)/2.;
        cw = (symbol_bounds.maxx + symbol_bounds.minx)/2.;
    }

    //take into account rotation of the symbol
    //find increased extents of the symbol bounds
    //due to the rotation
    if (symbol_rot_deg != 0.0)
    {
        double rotRad = symbol_rot_deg * (M_PI/180.); //symbol_rot assumed to be in radians
        double cs = cos(rotRad);
        double sn = sin(rotRad);

        double wcs, nwcs, wsn, nwsn, hsn, nhsn, hcs, nhcs, cwsn, cwcs, chsn, chcs;
        // check to see if the bounds have been set
        if (useBounds)
        {
            wcs = symbol_bounds.maxx * cs;   nwcs = symbol_bounds.minx * cs;
            wsn = symbol_bounds.maxx * sn;   nwsn = symbol_bounds.minx * sn;
            hsn = symbol_bounds.maxy * sn;   nhsn = symbol_bounds.miny * sn;
            hcs = symbol_bounds.maxy * cs;   nhcs = symbol_bounds.miny * cs;
        }
        else
        {
            wcs = w * cs;   nwcs = -wcs;
            wsn = w * sn;   nwsn = -wsn;
            hsn = h * sn;   nhsn = -hsn;
            hcs = h * cs;   nhcs = -hcs;
        }
       
        cwsn = cw * sn;		chsn = ch * sn; 
        cwcs = cw * cs;		chcs = ch * cs;

        // find the octant that the marker is rotated into, and shift the points accordingly.
        // this way, the overpost points are still within 22.5 degrees of an axis-aligned box.
        // (position 0 will always be the closest to Center-Right)
        double nangle = fmod(symbol_rot_deg, 360);
        if (nangle < 0)
            nangle += 360.;
        int i = (((int)((nangle/45.) + .5)) << 1) & 0x0000000f; // i is 2 * the octant
        op_pts[i++] = wcs - chsn;  op_pts[i++] = wsn + chcs;   i &= 0x0000000f; // & 15 does (mod 16)
        op_pts[i++] = wcs - hsn;   op_pts[i++] = wsn + hcs;    i &= 0x0000000f;
        op_pts[i++] = cwcs - hsn;  op_pts[i++] = cwsn + hcs;   i &= 0x0000000f;
        op_pts[i++] = nwcs - hsn;  op_pts[i++] = nwsn + hcs;   i &= 0x0000000f;
        op_pts[i++] = nwcs - chsn; op_pts[i++] = nwsn + chcs;  i &= 0x0000000f;
        op_pts[i++] = nwcs - nhsn; op_pts[i++] = nwsn + nhcs;  i &= 0x0000000f;
        op_pts[i++] = cwcs -nhsn;  op_pts[i++] = cwsn + nhcs;  i &= 0x0000000f;
        op_pts[i++] = wcs - nhsn;  op_pts[i]   = wsn + nhcs;
    }
    else
    {
        if (!useBounds)
        {
            symbol_bounds.maxx = w;	symbol_bounds.minx = -w;
            symbol_bounds.maxy = h;	symbol_bounds.miny = -h;
        }
        op_pts[0] = symbol_bounds.maxx;	op_pts[1] = ch;
        op_pts[2] = symbol_bounds.maxx;	op_pts[3] = symbol_bounds.maxy;
        op_pts[4] = cw;				    op_pts[5] = symbol_bounds.maxy;
        op_pts[6] = symbol_bounds.minx;	op_pts[7] = symbol_bounds.maxy;
        op_pts[8] = symbol_bounds.minx;	op_pts[9] = ch;
        op_pts[10] = symbol_bounds.minx;op_pts[11] = symbol_bounds.miny;
        op_pts[12] = cw;			    op_pts[13] = symbol_bounds.miny;
        op_pts[14] = symbol_bounds.maxx;op_pts[15] = symbol_bounds.miny;
    }

    //OK, who says I can't write bad code? Behold:
    SE_LabelInfo candidates[8];

    SE_RenderPointStyle* st0 = DeepClonePointStyle(rstyle2);
    ((SE_RenderText*)st0->symbol[0])->tdef.halign() = RS_HAlignment_Left;
    ((SE_RenderText*)st0->symbol[0])->tdef.valign() = RS_VAlignment_Half;
    candidates[0] = SE_LabelInfo(cx, cy, op_pts[0], op_pts[1], RS_Units_Device, 0.0, st0);

    SE_RenderPointStyle* st1 = DeepClonePointStyle(st0);
    ((SE_RenderText*)st1->symbol[0])->tdef.valign() = RS_VAlignment_Descent;
    candidates[1] = SE_LabelInfo(cx, cy, op_pts[2], op_pts[3], RS_Units_Device, 0.0, st1);

    SE_RenderPointStyle* st2 = DeepClonePointStyle(st1);
    ((SE_RenderText*)st2->symbol[0])->tdef.halign() = RS_HAlignment_Center;
    candidates[2] = SE_LabelInfo(cx, cy, op_pts[4], op_pts[5], RS_Units_Device, 0.0, st2);

    SE_RenderPointStyle* st3 = DeepClonePointStyle(st2);
    ((SE_RenderText*)st3->symbol[0])->tdef.halign() = RS_HAlignment_Right;
    candidates[3] = SE_LabelInfo(cx, cy, op_pts[6], op_pts[7], RS_Units_Device, 0.0, st3);

    SE_RenderPointStyle* st4 = DeepClonePointStyle(st3);
    ((SE_RenderText*)st4->symbol[0])->tdef.valign() = RS_VAlignment_Half;
    candidates[4] = SE_LabelInfo(cx, cy, op_pts[8], op_pts[9], RS_Units_Device, 0.0, st4);

    SE_RenderPointStyle* st5 = DeepClonePointStyle(st4);
    ((SE_RenderText*)st5->symbol[0])->tdef.valign() = RS_VAlignment_Ascent;
    candidates[5] = SE_LabelInfo(cx, cy, op_pts[10], op_pts[11], RS_Units_Device, 0.0, st5);

    SE_RenderPointStyle* st6 = DeepClonePointStyle(st5);
    ((SE_RenderText*)st6->symbol[0])->tdef.halign() = RS_HAlignment_Center;
    candidates[6] = SE_LabelInfo(cx, cy, op_pts[12], op_pts[13], RS_Units_Device, 0.0, st6);

    SE_RenderPointStyle* st7 = DeepClonePointStyle(st6);
    ((SE_RenderText*)st7->symbol[0])->tdef.halign() = RS_HAlignment_Left;
    candidates[7] = SE_LabelInfo(cx, cy, op_pts[14], op_pts[15], RS_Units_Device, 0.0, st7);

    renderer->ProcessLabelGroup(candidates, 8, RS_OverpostType_FirstFit, true, NULL);
}
    

void SE_PositioningAlgorithms::MultipleHighwaysShields(SE_Renderer*    renderer, 
                          SE_LineBuffer*  geometry, 
                          SE_Matrix&      xform, 
                          SE_Style*       style, 
                          SE_RenderStyle* rstyle, 
                          double          mm2px
                          )
{




}


