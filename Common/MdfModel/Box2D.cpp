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

//-------------------------------------------------------------------------
// DESCRIPTION:
// The Box2D class implementation.
// TODO: The exact Role of this class has yet to be determined.  Does it
//       play a functional role or is it simply a data store for other classes
//       to pull values off.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "Box2D.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the Box2D
//          class.
// PARAMETERS:
//        Input:
//              dX, dY -  point in 2D space.
//-------------------------------------------------------------------------
Box2D::Box2D(const double& dX, const double& dY)
{
    this->m_dX1 = this->m_dX2 = dX;
    this->m_dY1 = this->m_dY2 = dY;
}

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the Box2D
//-------------------------------------------------------------------------
Box2D::Box2D(const double& dX1, const double& dY1,
             const double& dX2, const double& dY2)
{
    // set the x coordinates
    this->m_dX1 = dX1;
    this->m_dX2 = dX2;

    // set the y coordinates
    this->m_dY1 = dY1;
    this->m_dY2 = dY2;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the minimum value in the X Axis.
//-------------------------------------------------------------------------
double Box2D::GetMinX() const
{
    return (this->m_dX1 < this->m_dX2)? this->m_dX1 : this->m_dX2;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the maximum value in the X Axis.
//-------------------------------------------------------------------------
double Box2D::GetMaxX() const
{
    return (this->m_dX1 > this->m_dX2)? this->m_dX1 : this->m_dX2;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the minimum value in the Y Axis.
//-------------------------------------------------------------------------
double Box2D::GetMinY() const
{
    return (this->m_dY1 < this->m_dY2)? this->m_dY1 : this->m_dY2;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the maximum value in the Y Axis.
//-------------------------------------------------------------------------
double Box2D::GetMaxY() const
{
    return (this->m_dY1 > this->m_dY2)? this->m_dY1 : this->m_dY2;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the first value in the X Axis.
//-------------------------------------------------------------------------
double Box2D::GetX1() const
{
    return this->m_dX1;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the second value in the X Axis.
//-------------------------------------------------------------------------
double Box2D::GetX2() const
{
    return this->m_dX2;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the first value in the Y Axis.
//-------------------------------------------------------------------------
double Box2D::GetY1() const
{
    return this->m_dY1;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the second value in the Y Axis.
//-------------------------------------------------------------------------
double Box2D::GetY2() const
{
    return this->m_dY2;
}
