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

#ifndef DESKTOP_CUSTOM_LOGO_INFO_H
#define DESKTOP_CUSTOM_LOGO_INFO_H

/// \cond INTERNAL
class MG_DESKTOP_API MgdCustomLogoInfo
{
    DECLARE_CLASSNAME(MgdCustomLogoInfo)

public:
    /// Default Constructor
    MgdCustomLogoInfo();
    ~MgdCustomLogoInfo();

    /// Accessors
    void SetX(double xCoord);
    void SetY(double yCoord);
    void SetPositionUnits(STRING units);
    void SetWidth(double width);
    void SetHeight(double height);
    void SetSizeUnits(STRING units);
    void SetRotation(double angle);
    void SetResourceId(STRING resId);
    void SetName(STRING name);
    double GetX();
    double GetY();
    STRING GetPositionUnits();
    double GetWidth();
    double GetHeight();
    STRING GetSizeUnits();
    double GetRotation();
    STRING GetResourceId();
    STRING GetName();

private:
    double m_dXPosition;
    double m_dYPosition;
    STRING m_positionUnits;
    double m_dSizeWidth;
    double m_dSizeHeight;
    STRING m_sizeUnits;
    double m_dRotation;
    STRING m_resId;
    STRING m_name;

};
/// \endcond

#endif
