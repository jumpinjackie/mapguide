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

//-------------------------------------------------------------------------
// DESCRIPTION:
// The GraphicElement class implementation.
//-------------------------------------------------------------------------

#include "GraphicElement.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the GraphicElement class.
//-------------------------------------------------------------------------
GraphicElement::GraphicElement()
{
    // NOTE: if you change any defaults then you must also update
    //       IOGraphicElement::Write to use the correct default values.
    this->m_eResizeControl = ResizeNone;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all objects that have been created on the
//          heap or have been adopted.
//-------------------------------------------------------------------------
GraphicElement::~GraphicElement()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ResizeControl property.
// RETURNS: The resize control for this graphic element.
//-------------------------------------------------------------------------
GraphicElement::ResizeControl GraphicElement::GetResizeControl() const
{
    return this->m_eResizeControl;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ResizeControl property.
// PARAMETERS:
//      Input:
//          eResizeControl - The resize control for this graphic element.
//-------------------------------------------------------------------------
void GraphicElement::SetResizeControl(GraphicElement::ResizeControl eResizeControl)
{
    this->m_eResizeControl = eResizeControl;
}
