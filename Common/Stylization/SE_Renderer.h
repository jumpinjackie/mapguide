//
//  Copyright (C) 2007 by Autodesk, Inc.
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

#ifndef SE_RENDERER_H
#define SE_RENDERER_H

#include "SE_RenderProxies.h"

class RS_FontEngine;

class SE_Renderer
{
public:
    STYLIZATION_API SE_Renderer();
    STYLIZATION_API ~SE_Renderer();

    // SE_RenderStyle, under associated xform, is in screen space, and geometry is in screen space
    STYLIZATION_API virtual void ProcessPoint(LineBuffer* geometry, SE_RenderPointStyle* style);
    STYLIZATION_API virtual void ProcessLine(LineBuffer* geometry, SE_RenderLineStyle* style);
    STYLIZATION_API virtual void ProcessArea(LineBuffer* geometry, SE_RenderAreaStyle* style);

    // Draws the specified symbol using the given transform.  The supplied angle is in radians
    // CCW, and should correspond to the rotation encoded in the transform.  Note that since the
    // transform converts to renderer space, its rotation component must take into account whether
    // y points up.
    STYLIZATION_API virtual void DrawSymbol(SE_RenderPrimitiveList& symbol, const SE_Matrix& xform, double angleRad);

    // Screen-space draw functions.  All angles are in degrees CCW.
    virtual void DrawScreenPolyline(LineBuffer* polyline, const SE_Matrix* xform, unsigned int color, double weight) = 0;
    virtual void DrawScreenPolygon(LineBuffer* polygon, const SE_Matrix* xform, unsigned int fill) = 0;
    virtual void DrawScreenRaster(unsigned char* data, int length,
                                  RS_ImageFormat format, int native_width, int native_height,
                                  double x, double y, double w, double h, double angleDeg) = 0;
    virtual void DrawScreenText(const RS_String& txt, RS_TextDef& tdef, double insx, double insy,
                                double* path, int npts, double param_position) = 0;

    virtual bool YPointsUp() = 0;
    virtual void GetWorldToScreenTransform(SE_Matrix& xform) = 0;
    virtual void WorldToScreenPoint(double& inx, double& iny, double& ox, double& oy) = 0;
    virtual void ScreenToWorldPoint(double& inx, double& iny, double& ox, double& oy) = 0;

    virtual double GetPixelsPerMillimeterScreen() = 0;
    virtual double GetPixelsPerMillimeterWorld() = 0;

    virtual RS_FontEngine* GetFontEngine() = 0;

    // Labeling -- this is the entry API for adding SE labels to the label mananger
    virtual void ProcessLabelGroup(SE_LabelInfo*    labels,
                                   int              nlabels,
                                   RS_OverpostType  type,
                                   bool             exclude,
                                   LineBuffer*      path = NULL) = 0;

    virtual void AddExclusionRegion(RS_F_Point* fpts, int npts) = 0;

    //miscellaneous
    void SetBufferPool(SE_BufferPool* pool);
    const RS_F_Point* GetLastExclusionRegion();
    SE_RenderStyle* CloneRenderStyle(SE_RenderStyle* symbol);

protected:
    void SetRenderSelectionMode(bool mode);

private:
    // TODO: remove/integrate when joins work with rasters, text
    void ProcessLineJoin(LineBuffer* geometry, SE_RenderLineStyle* style);
    void DrawSymbolJoin(SE_RenderPrimitiveList& symbol, SE_PiecewiseTransform** xforms, int nxforms);
    void AddLabelJoin(LineBuffer* geom, SE_RenderStyle* style, SE_PiecewiseTransform** xforms, int nxforms);
    void AddExclusionRegionJoin(SE_PiecewiseTransform** xforms, int nxforms);

    // angles are in radians CCW
    void AddLabel(LineBuffer* geom, SE_RenderStyle* style, SE_Matrix& xform, double angleRad);
    void AddExclusionRegion(SE_RenderStyle* rstyle, SE_Matrix& xform, double angleRad);

    RS_F_Point m_lastExclusionRegion[4];

protected:
    SE_BufferPool* m_bp;
    bool m_bSelectionMode;

    double m_selWeight;
    unsigned int m_selColor;
    unsigned int m_selFill;
    RS_Color m_textForeColor;
    RS_Color m_textBackColor;
};

#endif // SE_RENDERER_H
