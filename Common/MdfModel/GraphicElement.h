//
//  Copyright (C) 2007 by Autodesk, Inc.
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
        // The ResizeControl enum specifies how this graphic element interacts
        // with the resize box.  AddToResizeBox means the element's graphical
        // extent is added to the resize box, but the element is not resized
        // or repositioned if the resize box grows.  AdjustToResizeBox means
        // the element is resized and repositioned relative to the resize box,
        // but its extent is not added to the box.  ResizeNone, the default,
        // means the element does not interact with the resize box.
        enum ResizeControl
        {
            ResizeNone,
            AddToResizeBox,
            AdjustToResizeBox
        };

        // Destruction
        virtual ~GraphicElement();

        // Operations
        // Property : ResizeControl
        ResizeControl GetResizeControl() const;
        void SetResizeControl(ResizeControl resizeControl);

        // Visitor Pattern method defined in GraphicElement.
        virtual void AcceptVisitor(IGraphicElementVisitor& igeVisitor) = 0;

    protected:
        // Construction, initialization
        // Default constructor is protected to make this class abstract.
        GraphicElement();

    private:
        // Data members
        ResizeControl m_eResizeControl;
    };

    typedef MdfOwnerCollection<GraphicElement> GraphicElementCollection;
    EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<GraphicElement>;

END_NAMESPACE_MDFMODEL
#endif // GRAPHICELEMENT_H_
