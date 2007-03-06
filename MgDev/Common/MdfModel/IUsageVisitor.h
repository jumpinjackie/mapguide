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

#ifndef IUSAGEVISITOR_H_
#define IUSAGEVISITOR_H_

#include "MdfModel.h"

BEGIN_NAMESPACE_MDFMODEL

    class PointUsage;
    class LineUsage;
    class AreaUsage;

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The IUsageVisitor is an interface that is used for determining
    // the concrete class type of a pointer or reference to the abstract
    // Usage class.
    //-------------------------------------------------------------------------
    class MDFMODEL_API IUsageVisitor
    {
    public:
        // Operations
        virtual void VisitPointUsage(PointUsage& pointUsage) = 0;
        virtual void VisitLineUsage(LineUsage& lineUsage) = 0;
        virtual void VisitAreaUsage(AreaUsage& areaUsage) = 0;
    };

END_NAMESPACE_MDFMODEL
#endif // IUSAGEVISITOR_H_
