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

//-------------------------------------------------------------------------
// DESCRIPTION:
// The GraphicElement class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "GraphicElement.h"

using namespace MDFMODEL_NAMESPACE;

// initialize values for string properties
const wchar_t* GraphicElement::sResizeControlDefault = L"'ResizeNone'";                                // NOXLATE
const wchar_t* GraphicElement::sResizeControlValues  = L"ResizeNone|AddToResizeBox|AdjustToResizeBox"; // NOXLATE


//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the GraphicElement class.
//-------------------------------------------------------------------------
GraphicElement::GraphicElement()
{
    // default values
    // NOTE: values used in IOGraphicElement::Write must match these
    this->m_sResizeControl = GraphicElement::sResizeControlDefault;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all objects that have been created on the
//          heap or have been adopted.
//-------------------------------------------------------------------------
GraphicElement::~GraphicElement()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ResizeControl property.  The ResizeControl
//          property specifies how this graphic element interacts with the
//          resize box.  AddToResizeBox means the element's graphical extent
//          is added to the resize box, but the element is not resized or
//          repositioned if the resize box grows.  AdjustToResizeBox means
//          the element is resized and repositioned relative to the resize
//          box, but its extent is not added to the box.  ResizeNone, the
//          default, means the element does not interact with the resize box.
// RETURNS: The resize control for this graphic element.
//-------------------------------------------------------------------------
const MdfString& GraphicElement::GetResizeControl() const
{
    return this->m_sResizeControl;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ResizeControl property.  The ResizeControl
//          property specifies how this graphic element interacts with the
//          resize box.  AddToResizeBox means the element's graphical extent
//          is added to the resize box, but the element is not resized or
//          repositioned if the resize box grows.  AdjustToResizeBox means
//          the element is resized and repositioned relative to the resize
//          box, but its extent is not added to the box.  ResizeNone, the
//          default, means the element does not interact with the resize box.
// PARAMETERS:
//      Input:
//          resizeControl - The resize control for this graphic element.
//-------------------------------------------------------------------------
void GraphicElement::SetResizeControl(const MdfString& resizeControl)
{
    this->m_sResizeControl = resizeControl;
}
