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

#ifndef BOX2D_H_
#define BOX2D_H_

#include "MdfModel.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // Represents a two-dimensional box. There is no default constructor and
    // the user must provide two doubles or another Box2D etc. to construct
    // a Box2D. The Box2D class is used for computing the 2D bounding area
    // of geometry.
    //-------------------------------------------------------------------------
    class MDFMODEL_API Box2D
    {
    public:
        // Construction, initialization
        // Uses the default destructor as it passed around by value
        // and thus should not be subclassed.
        Box2D(const double& dX, const double& dY);

        Box2D(const double& dX1, const double& dY1,
              const double& dX2, const double& dY2);

        // Operations
        double GetMinX() const;
        double GetMaxX() const;
        double GetMinY() const;
        double GetMaxY() const;

        double GetX1() const;
        double GetX2() const;
        double GetY1() const;
        double GetY2() const;

    private:
        // Data Members
        // first X/Y for the box
        double m_dX1;
        double m_dY1;

        // second X/Y for the box
        double m_dX2;
        double m_dY2;
    };

END_NAMESPACE_MDFMODEL
#endif // BOX2D_H_
