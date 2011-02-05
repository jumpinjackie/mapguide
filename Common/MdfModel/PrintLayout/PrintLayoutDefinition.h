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

#ifndef PRINTLAYOUTDEFINITION_H_
#define PRINTLAYOUTDEFINITION_H_

#include "../MdfRootObject.h"
#include "../Common/Color.h"
#include "../Common/Extent3D.h"
#include "../Common/Size2D.h"
#include "Thickness.h"
#include "PrintLayoutElement.h"

BEGIN_NAMESPACE_MDFMODEL

///////////////////////////////////////////////////////////////////////////////
/// Defines names for the different types of orientations.
///
class MDFMODEL_API OrientationType
{
public:
    static const MdfString Orientation0;
    static const MdfString Orientation90;
    static const MdfString Orientation180;
    static const MdfString Orientation270;
};

//-------------------------------------------------------------------------
// DESCRIPTION: The PrintLayoutDefinition class declaration.
//-------------------------------------------------------------------------
class MDFMODEL_API PrintLayoutDefinition : public MdfRootObject
{
public:
    // Constructor(s)/Destructor
    PrintLayoutDefinition();
    virtual ~PrintLayoutDefinition();

    // Operations
    const MdfString& GetName() const;
    void SetName(const MdfString& name);

    Extent3D* GetExtent();

    PrintLayoutElementCollection* GetElements();

    Size2D* GetPaperSize();

    const MdfString& GetDeviceName() const;
    void SetDeviceName(const MdfString& deviceName);

    const MdfString& GetMediaName() const;
    void SetMediaName(const MdfString& mediaName);

    const MdfString& GetOrientation() const;
    void SetOrientation(const MdfString& orientation);

    Thickness* GetPaperMargin();

    const MdfString& GetUnits() const;
    void SetUnits(const MdfString& units);

    Color* GetBackgroundColor();

private:
    // Hidden copy constructor and assignment operator.
    PrintLayoutDefinition(const PrintLayoutDefinition&);
    PrintLayoutDefinition& operator=(const PrintLayoutDefinition&);

    // Data Members
    MdfString m_name;
    Extent3D m_extent;
    PrintLayoutElementCollection m_elements;
    Size2D m_paperSize;
    MdfString m_deviceName;
    MdfString m_mediaName;
    MdfString m_orientation;
    Thickness m_paperMargin;
    MdfString m_units;
    Color m_backgroundColor;
};

// Inline Methods
inline const MdfString& PrintLayoutDefinition::GetName() const
{
    return m_name;
}

inline Extent3D* PrintLayoutDefinition::GetExtent()
{
    return &m_extent;
}

inline PrintLayoutElementCollection* PrintLayoutDefinition::GetElements()
{
    return &m_elements;
}

inline Size2D* PrintLayoutDefinition::GetPaperSize()
{
    return &m_paperSize;
}

inline const MdfString& PrintLayoutDefinition::GetDeviceName() const
{
    return m_deviceName;
}

inline const MdfString& PrintLayoutDefinition::GetMediaName() const
{
    return m_mediaName;
}

inline const MdfString& PrintLayoutDefinition::GetOrientation() const
{
    return m_orientation;
}

inline Thickness* PrintLayoutDefinition::GetPaperMargin()
{
    return &m_paperMargin;
}

inline const MdfString& PrintLayoutDefinition::GetUnits() const
{
    return m_units;
}

inline Color* PrintLayoutDefinition::GetBackgroundColor()
{
    return &m_backgroundColor;
}

END_NAMESPACE_MDFMODEL
#endif // PRINTLAYOUTDEFINITION_H_
