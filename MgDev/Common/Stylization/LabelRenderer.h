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

#ifndef LABELRENDERER_H
#define LABELRENDERER_H

#include "LabelRendererBase.h"
#include "SimpleOverpost.h"

class GDRenderer;


//////////////////////////////////////////////////////////////////////////////
//used to accumulate labels
//so that we can draw all of them on top of geometry in the end
struct LR_LabelInfo
{
    LR_LabelInfo(double x, double y, const RS_String& text, const RS_TextDef tdef)
        : m_x(x),
          m_y(y),
          m_text(text),
          m_tdef(tdef),
          m_pts(NULL),
          m_numpts(0)
    {
    }

    double m_x;
    double m_y;
    RS_String m_text;
    RS_TextDef m_tdef;

    // if set, defines the path which the label will follow
    RS_F_Point* m_pts;
    int m_numpts;
};


//////////////////////////////////////////////////////////////////////////////
struct LR_OverpostGroup
{
    LR_OverpostGroup(bool render, bool exclude, RS_OverpostType type)
        : m_render(render),
          m_exclude(exclude),
          m_algo(laSimple),
          m_type(type)
    {
    }

    bool m_render;
    bool m_exclude;
    LabelAlgo m_algo;
    RS_OverpostType m_type;
    std::vector<LR_LabelInfo> m_labels;
};


//////////////////////////////////////////////////////////////////////////////
class LabelRenderer : public LabelRendererBase
{

public:
    LabelRenderer(GDRenderer* renderer);
    virtual ~LabelRenderer();

    virtual void StartLabels();

    virtual void ProcessLabel(double x, double y,
                              const RS_String& text, RS_TextDef& tdef);

    virtual void ProcessLabelGroup(RS_LabelInfo*    labels,
                                   int              nlabels,
                                   const RS_String& text,
                                   RS_OverpostType  type,
                                   bool             exclude,
                                   LineBuffer*      path);

    virtual void BlastLabels();

    virtual void AddExclusionRegion(RS_F_Point* pts, int npts);

private:
    void BeginOverpostGroup(RS_OverpostType type, bool render, bool exclude);
    void EndOverpostGroup();

    bool ProcessLabelInternal(LR_LabelInfo& info, bool render, bool exclude, bool check);

    void ApplyTextAlignment(RS_TextDef& tdef, double actual_height, RS_F_Point* extent, RS_F_Point& ins_point);

    bool DrawSimpleLabel(LR_LabelInfo& info, bool render, bool exclude, bool check);
    bool DrawPathLabel(LR_LabelInfo& info, bool render, bool exclude, bool check);

    double ComputeCharacterPositions(LR_LabelInfo& info, double* seglens, double position, float* kerned_spacing, double measured_width, CharPos* ret);

    bool OverlapsStuff(RS_F_Point* pts, int npts);
    void DeviceToMappingBounds(RS_Bounds& b);
    void ComputeBounds(RS_F_Point* RESTRICT pts, int npts, RS_Bounds& b);
    void RotatedBounds(double x, double y, double width, double height, double angle_cw_rad, RS_F_Point* b);

    std::vector<LR_LabelInfo> StitchPolylines(std::vector<LR_LabelInfo>& labels);
    bool CloseEnough(RS_F_Point& p1, RS_F_Point& p2);

    // member data
    std::vector<LR_OverpostGroup>   m_labelGroups;
    std::map<RS_String, size_t>     m_hStitchTable;
    bool                            m_bOverpostGroupOpen;
    GDRenderer*                     m_renderer;
    SimpleOverpost                  m_overpost;
};

#endif
