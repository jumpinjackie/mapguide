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

#ifndef LABELRENDERERLOCAL_H_
#define LABELRENDERERLOCAL_H_

#include "LabelRendererBase.h"
#include "SimpleOverpost.h"
#include "RS_FontEngine.h"
#include "BIDIConverter.h"

struct SE_RenderStyle;

//////////////////////////////////////////////////////////////////////////////
// Used to accumulate labels so that we can draw all
// of them on top of geometry in the end.
struct LabelInfoLocal
{
    LabelInfoLocal(double x, double y, const RS_String& text, const RS_TextDef& tdef)
        : m_x(x),
          m_y(y),
          m_text(text),
          m_tdef(tdef),
          m_pts(NULL),
          m_numpts(0),
          m_numelems(0),
          m_rotated_points(NULL),
          m_sestyle(NULL)
    {
    }

    LabelInfoLocal(double x, double y, SE_RenderStyle* style)
        : m_x(x),
          m_y(y),
          m_pts(NULL),
          m_numpts(0),
          m_numelems(0),
          m_rotated_points(NULL),
          m_sestyle(style)
    {
    }

    double m_x;
    double m_y;
    RS_String m_text;
    RS_TextDef m_tdef;

    // if set, defines the path which the label will follow
    RS_F_Point* m_pts;
    int m_numpts;

    RS_F_Point m_ins_point;

    // number of elements this label consists of
    // - a simple label has one element per line of text
    // - a path label has one element per character
    size_t m_numelems;

    // the rotated points associated with this label
    // - each group of four points defines one rotated extent
    // - there's one extent per element
    RS_F_Point* m_rotated_points;

    // stores matched font, measured text size, kerned char spacings
    // layout character positions
    RS_TextMetrics m_tm;

    // new SE labels keep the symbol here rather than in the m_tdef/m_text combo
    SE_RenderStyle* m_sestyle;
};


//////////////////////////////////////////////////////////////////////////////
struct OverpostGroupLocal
{
    OverpostGroupLocal(bool render, bool exclude, RS_OverpostType type)
        : m_render(render),
          m_exclude(exclude),
          m_algo(laSimple),
          m_type(type),
          m_scaleLimit(0.0)
    {
    }

    bool m_render;
    bool m_exclude;
    LabelAlgo m_algo;
    RS_OverpostType m_type;
    double m_scaleLimit;
    RS_Bounds m_feature_bounds;
    std::vector<LabelInfoLocal> m_labels;
};


//////////////////////////////////////////////////////////////////////////////
class LabelRendererLocal : public LabelRendererBase
{
public:
    STYLIZATION_API LabelRendererLocal(SE_Renderer* se_renderer, double tileExtentOffset);
    virtual ~LabelRendererLocal();

    virtual void StartLabels();

    // RS labels
    virtual void ProcessLabelGroup(RS_LabelInfo*    labels,
                                   int              nlabels,
                                   const RS_String& text,
                                   RS_OverpostType  type,
                                   bool             exclude,
                                   LineBuffer*      path,
                                   double           scaleLimit);

    // SE labels
    virtual void ProcessLabelGroup(SE_LabelInfo*    labels,
                                   int              nlabels,
                                   RS_OverpostType  type,
                                   bool             exclude,
                                   LineBuffer*      path);

    virtual void BlastLabels();

    virtual void AddExclusionRegion(RS_F_Point* pts, int npts);

private:
    void Cleanup();
    void BeginOverpostGroup(RS_OverpostType type, bool render, bool exclude);
    void EndOverpostGroup();

    bool ComputeSimpleLabelBounds(LabelInfoLocal& info);
    bool ComputePathLabelBounds(LabelInfoLocal& info, std::vector<LabelInfoLocal>& repeated_infos, double scaleLimit);
    bool ComputeSELabelBounds(LabelInfoLocal& info);

    void ProcessLabelGroupsInternal(SimpleOverpost* pMgr, std::vector<OverpostGroupLocal*>& groups);
    bool ProcessLabelInternal(SimpleOverpost* pMgr,
                              LabelInfoLocal& info,
                              bool render,
                              bool exclude,
                              bool check);

    void AddExclusionRegion(SimpleOverpost* pMgr, RS_F_Point* pts, int npts);

    bool OverlapsStuff(SimpleOverpost* pMgr, RS_F_Point* pts, int npts);

    std::vector<LabelInfoLocal> StitchPolylines(std::vector<LabelInfoLocal>& labels);
    std::vector<LabelInfoLocal> StitchPolylinesHelper(std::vector<LabelInfoLocal>& labels);

    // member data
    std::vector<OverpostGroupLocal>  m_labelGroups;
    std::map<RS_String, size_t>      m_hStitchTable;
    SimpleOverpost                   m_overpost;
    double                           m_tileExtentOffset;
    BIDIConverter                    m_bidiConverter;
};

#endif
