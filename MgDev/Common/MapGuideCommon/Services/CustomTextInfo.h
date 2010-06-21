//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef MG_CUSTOM_TEXT_INFO_H
#define MG_CUSTOM_TEXT_INFO_H

/// \cond INTERNAL
class MG_MAPGUIDE_API MgCustomTextInfo
{
    DECLARE_CLASSNAME(MgCustomTextInfo)

public:
    /// Default Constructor
    MgCustomTextInfo();
    ~MgCustomTextInfo();

    /// Accessors
    void SetX(double xCoord);
    void SetY(double yCoord);
    void SetPositionUnits(STRING units);
    void SetValue(STRING value);
    void SetFontName(STRING fontName);
    void SetFontHeight(double height);
    void SetSizeUnits(STRING units);
    double GetX();
    double GetY();
    STRING GetPositionUnits();
    STRING GetValue();
    STRING GetFontName();
    double GetFontHeight();
    STRING GetSizeUnits();

private:
    double m_dXPosition;
    double m_dYPosition;
    STRING m_positionUnits;
    STRING m_fontName;
    double m_fontHeight;
    STRING m_sizeUnits;
    STRING m_value;

};
/// \endcond

#endif
