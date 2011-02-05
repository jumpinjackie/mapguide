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

#ifndef ISYMBOLIZATIONVISITOR_H_
#define ISYMBOLIZATIONVISITOR_H_

#include "MdfModel.h"

BEGIN_NAMESPACE_MDFMODEL

    class LineSymbolization2D;
    class AreaSymbolization2D;
    class PointSymbolization2D;
    class CompositeSymbolization;

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The ISymbolizationVisitor is an interface that is used for determining
    // the concrete class type of a pointer or reference to the abstract
    // Symbolization class.
    //-------------------------------------------------------------------------
    class MDFMODEL_API ISymbolizationVisitor
    {
    public:
        // Operations
        virtual void VisitLineSymbolization2D(LineSymbolization2D& syLine) = 0;
        virtual void VisitAreaSymbolization2D(AreaSymbolization2D& syArea) = 0;
        virtual void VisitPointSymbolization2D(PointSymbolization2D& syPoint) = 0;
        virtual void VisitCompositeSymbolization(CompositeSymbolization& syComp) = 0;
    };

END_NAMESPACE_MDFMODEL
#endif // ISYMBOLIZATIONVISITOR_H_
