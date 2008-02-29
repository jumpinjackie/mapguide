//
//  Copyright (C) 2007-2008 by Autodesk, Inc.
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

#ifndef SE_RENDERER_H_
#define SE_RENDERER_H_

#include "Renderer.h"
#include "SE_SymbolDefProxies.h"
#include "SE_RenderProxies.h"
#include "SE_Tuple.h"

class RS_FontEngine;
class SE_IJoinProcessor;


class SE_Renderer : public Renderer
{
public:
    STYLIZATION_API SE_Renderer();
    STYLIZATION_API virtual ~SE_Renderer();

    // SE_RenderStyle, under associated xform, is in screen space, and the
    // geometry (in the context) is in screen space.  For ProcessPoint, the
    // bounds are returned in screen units.
    STYLIZATION_API virtual void ProcessPoint(SE_ApplyContext* ctx, SE_RenderPointStyle* style, RS_Bounds* bounds = NULL);
    STYLIZATION_API virtual void ProcessLine(SE_ApplyContext* ctx, SE_RenderLineStyle* style);
    STYLIZATION_API virtual void ProcessArea(SE_ApplyContext* ctx, SE_RenderAreaStyle* style);

    // Draws the specified symbol using the given transform.  The supplied angle
    // is in radians CCW, and should correspond to the rotation encoded in the
    // transform.  Note that since the transform converts to renderer space, its
    // rotation component must take into account whether y points up.
    STYLIZATION_API virtual void DrawSymbol(SE_RenderPrimitiveList& symbol, const SE_Matrix& xform,
                                            double angleRad, bool excludeRegion = false,
                                            SE_IJoinProcessor* processor = NULL);

    // Turns selection mode rendering on/off.
    STYLIZATION_API virtual void SetRenderSelectionMode(bool mode);
    STYLIZATION_API virtual void SetRenderSelectionMode(bool mode, int rgba);

    // Screen-space draw functions.  All angles are in degrees CCW.
    virtual void DrawScreenPolyline(LineBuffer* polyline, const SE_Matrix* xform, const SE_LineStroke& lineStroke) = 0;
    virtual void DrawScreenPolygon(LineBuffer* polygon, const SE_Matrix* xform, unsigned int fill) = 0;
    virtual void DrawScreenRaster(unsigned char* data, int length,
                                  RS_ImageFormat format, int native_width, int native_height,
                                  double x, double y, double w, double h, double angleDeg) = 0;
    virtual void DrawScreenText(const RS_String& txt, RS_TextDef& tdef, double insx, double insy,
                                RS_F_Point* path, int npts, double param_position) = 0;

    virtual bool YPointsUp() = 0;
    virtual void GetWorldToScreenTransform(SE_Matrix& xform) = 0;
    virtual void WorldToScreenPoint(double& inx, double& iny, double& ox, double& oy) = 0;
    virtual void ScreenToWorldPoint(double& inx, double& iny, double& ox, double& oy) = 0;

    // returns the viewport rotation angle, in radians CCW
    STYLIZATION_API virtual double GetWorldToScreenRotation();

    virtual double GetPixelsPerMillimeterScreen() = 0;
    virtual double GetPixelsPerMillimeterWorld() = 0;

    virtual RS_FontEngine* GetRSFontEngine() = 0;

    // Labeling -- this is the entry API for adding SE labels to the label mananger
    virtual void ProcessSELabelGroup(SE_LabelInfo*    labels,
                                     int              nlabels,
                                     RS_OverpostType  type,
                                     bool             exclude,
                                     LineBuffer*      path = NULL) = 0;

    virtual void AddExclusionRegion(RS_F_Point* fpts, int npts) = 0;

    // miscellaneous
    STYLIZATION_API void SetBufferPool(SE_BufferPool* pool);
    const RS_F_Point* GetLastSymbolExtent();
    SE_RenderStyle* CloneRenderStyle(SE_RenderStyle* symbol);

    // angles are in radians CCW
    void AddLabel(LineBuffer* geom, SE_RenderStyle* style, const SE_Matrix& xform, double angleRad);

    // helper methods
    void ProcessLineLabels(LineBuffer* geometry, SE_RenderLineStyle* style);

private:
    void ComputeSegmentLengths(LineBuffer* geometry, double* segLens);
    void ComputeGroupLengths(double* segLens, int numGroups, int* segGroups, double* groupLens);
    int ComputeSegmentGroups(LineBuffer* geometry, int contour, double vertexAngleLimit, double* segLens, int* segGroups);
    void ComputeGroupDistribution(double groupLen, double startOffset, double endOffset, double repeat, double symWidth,
                                  double& startPos, double& gap, int& numSymbols);
    void ProcessLineOverlapNone(LineBuffer* geometry, SE_RenderLineStyle* style, double* segLens);
    void ProcessLineOverlapDirect(LineBuffer* geometry, SE_RenderLineStyle* style, double* segLens);

    // TODO: integrate when joins work with rasters, text
    void ProcessLineOverlapWrap(LineBuffer* geometry, SE_RenderLineStyle* style);

    RS_F_Point m_lastSymbolExtent[4];

protected:
    SE_BufferPool* m_bp;
    bool m_bSelectionMode;

    SE_LineStroke m_selLineStroke;
    unsigned int m_selFillColor;
    RS_Color m_textForeColor;
    RS_Color m_textBackColor;
};

#endif
