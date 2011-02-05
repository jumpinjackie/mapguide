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

#ifndef THICKNESS_H_
#define THICKNESS_H_

#include "../MdfModel.h"

BEGIN_NAMESPACE_MDFMODEL

//-------------------------------------------------------------------------
// DESCRIPTION: The Thickness class declaration.
//-------------------------------------------------------------------------
class MDFMODEL_API Thickness
{
public:
    // Constructor(s)/Destructor
    Thickness();
    Thickness(double left, double top, double right, double bottom);
    virtual ~Thickness();

    // Operations
    double GetLeft() const;
    void SetLeft(double left);

    double GetTop() const;
    void SetTop(double top);

    double GetRight() const;
    void SetRight(double right);

    double GetBottom() const;
    void SetBottom(double bottom);

private:
    // Data Members
    double m_left;
    double m_top;
    double m_right;
    double m_bottom;
};

// Inline Methods
inline double Thickness::GetLeft() const
{
    return m_left;
}

inline double Thickness::GetTop() const
{
    return m_top;
}

inline double Thickness::GetRight() const
{
    return m_right;
}

inline double Thickness::GetBottom() const
{
    return m_bottom;
}

END_NAMESPACE_MDFMODEL
#endif // THICKNESS_H_
