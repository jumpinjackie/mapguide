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

#ifndef FEATURETYPESTYLEVISITOR_H_
#define FEATURETYPESTYLEVISITOR_H_

#include "Stylization.h"

// A visitor implementation that determines the feature type style
// we are dealing with.
class FeatureTypeStyleVisitor : public MdfModel::IFeatureTypeStyleVisitor
{
public:
    enum eFeatureTypeStyle
    {
        ftsUnknown     = 0,
        ftsLine        = 1,
        ftsArea        = 2,
        ftsPoint       = 3,
        ftsComposite   = 4,
        ftsGridSurface = 5,
        ftsGridColor   = 6,
    };

    FeatureTypeStyleVisitor()
    {
        m_type = ftsUnknown;
    }

    // Operations
    virtual void VisitLineTypeStyle(MdfModel::LineTypeStyle& /*fsLineTypeStyle*/)
    {
        m_type = ftsLine;
    }

    virtual void VisitPointTypeStyle(MdfModel::PointTypeStyle& /*fsPointTypeStyle*/)
    {
        m_type = ftsPoint;
    }

    virtual void VisitAreaTypeStyle(MdfModel::AreaTypeStyle& /*fsAreaTypeStyle*/)
    {
        m_type = ftsArea;
    }

    virtual void VisitCompositeTypeStyle(MdfModel::CompositeTypeStyle& /*fsCompositeTypeStyle*/)
    {
        m_type = ftsComposite;
    }

    virtual void VisitGridColorStyle(MdfModel::GridColorStyle& /*fsGridColorStyle*/)
    {
        m_type = ftsGridColor;
    }

    virtual void VisitGridSurfaceStyle(MdfModel::GridSurfaceStyle& /*fsGridSurfaceStyle*/)
    {
        m_type = ftsGridSurface;
    }

    eFeatureTypeStyle GetFeatureTypeStyle()
    {
        return m_type;
    }

    //static helper
    static eFeatureTypeStyle DetermineFeatureTypeStyle(MdfModel::FeatureTypeStyle* style)
    {
        FeatureTypeStyleVisitor vis;
        style->AcceptVisitor(vis);
        return vis.GetFeatureTypeStyle();
    }

private:
    eFeatureTypeStyle m_type;
};

#endif
