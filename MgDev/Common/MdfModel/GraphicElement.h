//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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

#ifndef GRAPHICELEMENT_H_
#define GRAPHICELEMENT_H_

#include "MdfModel.h"
#include "MdfRootObject.h"
#include "MdfOwnerCollection.h"
#include "IGraphicElementVisitor.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // Building block of the graphics of a SymbolDefinition.
    //-------------------------------------------------------------------------
    class MDFMODEL_API GraphicElement : public MdfRootObject
    {
    public:
        // Used with string properties
        static const wchar_t* sResizeControlDefault;
        static const wchar_t* sResizeControlValues;

    public:
        // Destruction
        virtual ~GraphicElement();

        // Operations
        // Property : ResizeControl
        const MdfString& GetResizeControl() const;
        void SetResizeControl(const MdfString& resizeControl);

        // Visitor Pattern method defined in GraphicElement.
        virtual void AcceptVisitor(IGraphicElementVisitor& igeVisitor) = 0;

    protected:
        // Construction, initialization
        // Default constructor is protected to make this class abstract.
        GraphicElement();

    private:
        // Data members
        MdfString m_sResizeControl;
    };

    typedef MdfOwnerCollection<GraphicElement> GraphicElementCollection;
    EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<GraphicElement>;

END_NAMESPACE_MDFMODEL
#endif // GRAPHICELEMENT_H_
