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

#ifndef SIZE2D_H_
#define SIZE2D_H_

#include "../MdfModel.h"

BEGIN_NAMESPACE_MDFMODEL

//-------------------------------------------------------------------------
// DESCRIPTION: The Size2D class declaration.
//-------------------------------------------------------------------------
class MDFMODEL_API Size2D
{
public:
    // Constructor(s)/Destructor
    Size2D();
    Size2D(double width, double height);
    virtual ~Size2D();

    // Operations
    double GetWidth() const;
    void SetWidth(double width);

    double GetHeight() const;
    void SetHeight(double height);

private:
    // Data Members
    double m_width;
    double m_height;
};

// Inline Methods
inline double Size2D::GetWidth() const
{
    return m_width;
}

inline double Size2D::GetHeight() const
{
    return m_height;
}

END_NAMESPACE_MDFMODEL
#endif // SIZE2D_H_
