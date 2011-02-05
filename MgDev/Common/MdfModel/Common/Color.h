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

#ifndef COLOR_H_
#define COLOR_H_

#include "../MdfModel.h"

BEGIN_NAMESPACE_MDFMODEL

//-------------------------------------------------------------------------
// DESCRIPTION: The Color class declaration.
//-------------------------------------------------------------------------
class MDFMODEL_API Color
{
public:
    // Constructor(s)/Destructor
    Color();
    Color(int red, int green, int blue, int alpha);
    virtual ~Color();

    // Operations
    int GetRed() const;
    void SetRed(int red);

    int GetGreen() const;
    void SetGreen(int green);

    int GetBlue() const;
    void SetBlue(int blue);

    int GetAlpha() const;
    void SetAlpha(int alpha);

private:
    // Data Members
    int m_red;
    int m_green;
    int m_blue;
    int m_alpha;
};

// Inline Methods
inline int Color::GetRed() const
{
    return m_red;
}

inline int Color::GetGreen() const
{
    return m_green;
}

inline int Color::GetBlue() const
{
    return m_blue;
}

inline int Color::GetAlpha() const
{
    return m_alpha;
}

END_NAMESPACE_MDFMODEL
#endif // COLOR_H_
