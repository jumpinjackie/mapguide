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

#ifndef IFEATURETYPESTYLEVISITOR_H_
#define IFEATURETYPESTYLEVISITOR_H_

#include "MdfModel.h"

BEGIN_NAMESPACE_MDFMODEL

    class LineTypeStyle;
    class PointTypeStyle;
    class AreaTypeStyle;
    class CompositeTypeStyle;
    class GridColorStyle;
    class GridSurfaceStyle;

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The IFeatureTypeStyleVisitor is an interface that is used for determining
    // the concrete class type of a pointer or reference to the abstract
    // FeatureTypeStyle class.
    //-------------------------------------------------------------------------
    class MDFMODEL_API IFeatureTypeStyleVisitor
    {
    public:
        // Operations
        virtual void VisitLineTypeStyle(LineTypeStyle& fsLineTypeStyle) = 0;
        virtual void VisitPointTypeStyle(PointTypeStyle& fsPointTypeStyle) = 0;
        virtual void VisitAreaTypeStyle(AreaTypeStyle& fsAreaTypeStyle) = 0;
        virtual void VisitCompositeTypeStyle(CompositeTypeStyle& fsCompositeTypeStyle) = 0;
        virtual void VisitGridColorStyle(GridColorStyle &fsGridStyle) = 0;
        virtual void VisitGridSurfaceStyle(GridSurfaceStyle &fsGridStyle) = 0;
    };

END_NAMESPACE_MDFMODEL
#endif // IFEATURETYPESTYLEVISITOR_H_
