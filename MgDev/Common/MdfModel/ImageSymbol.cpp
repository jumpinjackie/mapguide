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

//-------------------------------------------------------------------------
// DESCRIPTION:
// ImageSymbol class implementation. ImageSymbol is a concrete subclass of
// the Symbol class.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "ImageSymbol.h"
#include "ISymbolVisitor.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the ImageSymbol class.
//-------------------------------------------------------------------------
ImageSymbol::ImageSymbol()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all adopted objects.
//-------------------------------------------------------------------------
ImageSymbol::~ImageSymbol()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accesses the optional static image data for this symbol.
// RETURNS: The string representing the RGB image.  NULL means no static data.
//-------------------------------------------------------------------------
const MdfString& ImageSymbol::GetContent() const
{
    return this->m_content;
}

//-------------------------------------------------------------------------
// PURPOSE: Sets the static image data for this symbol.
// PARAMETERS:
//      Input:
//          content - the text representing an RGB image.
//-------------------------------------------------------------------------
void ImageSymbol::SetContent(const MdfString& content)
{
    this->m_content = content;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the ImageLibrary property defined in this ImageSymbol.
//          The ImageLibrary property is the library on the server containing
//          the shape. Each shape is a 2D polygon.
// RETURNS: The ImageLibrary string.
//-------------------------------------------------------------------------
const MdfString& ImageSymbol::GetImageLibrary() const
{
    return this->m_strImageLibrary;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the ImageName property defined in this ImageSymbol.
//          The ImageName property is the name for the shape stored on the
//          server in the ImageLibrary. Each shape is a 2D polygon.
// PARAMETERS:
//      Input:
//          strImageLibrary - The ImageLibrary string.
//-------------------------------------------------------------------------
void ImageSymbol::SetImageLibrary(const MdfString& strImageLibrary)
{
    this->m_strImageLibrary = strImageLibrary;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the ImageName property defined in this ImageSymbol.
//          The ImageName property is the name of the shape stored on the
//          server in the ImageLibrary. Each shape is a 2D polygon.
// RETURNS: The ImageName string.
//-------------------------------------------------------------------------
const MdfString& ImageSymbol::GetImageName() const
{
    return this->m_strImageName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the ImageName property defined in this ImageSymbol.
//          The ImageName property is the name of the shape stored on the
//          server in the ImageLibrary. Each shape is a 2D polygon.
// PARAMETERS:
//      Input:
//          strImageName - The ImageName string.
//-------------------------------------------------------------------------
void ImageSymbol::SetImageName(const MdfString& strImageName)
{
    this->m_strImageName = strImageName;
}

//-------------------------------------------------------------------------
// PURPOSE: Implementation of the Visitor pattern. The pure, virtual method
//          is defined in Symbol and is implemented by all of its concrete
//          subclasses.
// PARAMETERS:
//      Input:
//          isymVisitor - The ISymbolVisitor interface which sports methods
//                       that accept the final concrete type this ImageSymbol
//                       represents as an argument.
//-------------------------------------------------------------------------
void ImageSymbol::AcceptVisitor(ISymbolVisitor& isymVisitor)
{
    isymVisitor.VisitImageSymbol(*this);
}
