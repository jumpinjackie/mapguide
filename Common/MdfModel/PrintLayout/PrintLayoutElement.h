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

#ifndef PRINTLAYOUTELEMENT_H_
#define PRINTLAYOUTELEMENT_H_

#include "../MdfRootObject.h"
#include "../MdfOwnerCollection.h"
#include "../Common/Point3D.h"
#include "../Common/StringObject.h"

BEGIN_NAMESPACE_MDFMODEL

//-------------------------------------------------------------------------
// DESCRIPTION: The PrintLayoutElement class declaration.
//-------------------------------------------------------------------------
class MDFMODEL_API PrintLayoutElement : public MdfRootObject
{
public:
    // Constructor(s)/Destructor
    PrintLayoutElement();
    virtual ~PrintLayoutElement();

    // Operations
    const MdfString& GetName() const;
    void SetName(const MdfString& name);

    const MdfString& GetResourceId() const;
    void SetResourceId(const MdfString& resourceId);

    Point3D* GetCenter();

    double GetWidth() const;
    void SetWidth(double width);

    double GetHeight() const;
    void SetHeight(double height);

    double GetRotation() const;
    void SetRotation(double rotation);

    const MdfString& GetUnits() const;
    void SetUnits(const MdfString& units);

    bool GetIsVisible() const;
    void SetIsVisible(bool visible);

    double GetOpacity() const;
    void SetOpacity(double opacity);

    StringObjectCollection* GetReferences();

private:
    // Hidden copy constructor and assignment operator.
    PrintLayoutElement(const PrintLayoutElement&);
    PrintLayoutElement& operator=(const PrintLayoutElement&);

    // Data Members
    MdfString m_name;
    MdfString m_resourceId;
    Point3D m_center;
    double m_width;
    double m_height;
    double m_rotation;
    MdfString m_units;
    bool m_isVisible;
    double m_opacity;
    StringObjectCollection m_references;
};

typedef MdfOwnerCollection<PrintLayoutElement> PrintLayoutElementCollection;
EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<PrintLayoutElement>;

// Inline Methods
inline const MdfString& PrintLayoutElement::GetName() const
{
    return m_name;
}

inline const MdfString& PrintLayoutElement::GetResourceId() const
{
    return m_resourceId;
}

inline Point3D* PrintLayoutElement::GetCenter()
{
    return &m_center;
}

inline double PrintLayoutElement::GetWidth() const
{
    return m_width;
}

inline double PrintLayoutElement::GetHeight() const
{
    return m_height;
}

inline double PrintLayoutElement::GetRotation() const
{
    return m_rotation;
}

inline const MdfString& PrintLayoutElement::GetUnits() const
{
    return m_units;
}

inline bool PrintLayoutElement::GetIsVisible() const
{
    return m_isVisible;
}

inline double PrintLayoutElement::GetOpacity() const
{
    return m_opacity;
}

inline StringObjectCollection* PrintLayoutElement::GetReferences()
{
    return &m_references;
}

END_NAMESPACE_MDFMODEL
#endif // PRINTLAYOUTELEMENT_H_
