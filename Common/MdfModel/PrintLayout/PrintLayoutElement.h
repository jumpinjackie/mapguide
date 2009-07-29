//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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
    const MdfString& GetResourceId() const;
    void SetResourceId(const MdfString& resourceId);

    bool GetVisibility() const;
    void SetVisibility(bool visible);

    double GetOpacity() const;
    void SetOpacity(double opacity);

private:
    // Hidden copy constructor and assignment operator.
    PrintLayoutElement(const PrintLayoutElement&);
    PrintLayoutElement& operator=(const PrintLayoutElement&);

    // Data Members
    MdfString m_resourceId;
    bool m_visible;
    double m_opacity;
};

typedef MdfOwnerCollection<PrintLayoutElement> PrintLayoutElementCollection;
EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<PrintLayoutElement>;

// Inline Methods
inline const MdfString& PrintLayoutElement::GetResourceId() const
{
    return m_resourceId;
}

inline bool PrintLayoutElement::GetVisibility() const
{
    return m_visible;
}

inline double PrintLayoutElement::GetOpacity() const
{
    return m_opacity;
}

END_NAMESPACE_MDFMODEL
#endif // PRINTLAYOUTELEMENT_H_
