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
// BlockSymbol class implementation. BlockSymbol is a concrete subclass of
// the FlatSymbol class.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "BlockSymbol.h"
#include "ISymbolVisitor.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the BlockSymbol class.
//-------------------------------------------------------------------------
BlockSymbol::BlockSymbol()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all adopted objects.
//-------------------------------------------------------------------------
BlockSymbol::~BlockSymbol()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the DrawingName property defined in this BlockSymbol.
//          The DrawingName property is the name of the drawing storing the
//          block symbol.
// RETURNS: The DrawingName string.
//-------------------------------------------------------------------------
const MdfString& BlockSymbol::GetDrawingName() const
{
    return this->m_strDrawingName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the DrawingName property defined in this BlockSymbol.
//          The DrawingName property is the name of the drawing storing the
//          block symbol.
// PARAMETERS:
//      Input:
//          strDrawingName - The DrawingName string.
//-------------------------------------------------------------------------
void BlockSymbol::SetDrawingName(const MdfString& strDrawingName)
{
    this->m_strDrawingName = strDrawingName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the BlockName property defined in this BlockSymbol.
//          The BlockName property is the name of the block stored in the
//          drawing.
// RETURNS: The BlockName string.
//-------------------------------------------------------------------------
const MdfString& BlockSymbol::GetBlockName() const
{
    return this->m_strBlockName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the BlockName property defined in this BlockSymbol.
//          The BlockName property is the name of the block stored in the
//          drawing.
// PARAMETERS:
//      Input:
//          strBlockName - The BlockName string.
//-------------------------------------------------------------------------
void BlockSymbol::SetBlockName(const MdfString& strBlockName)
{
    this->m_strBlockName = strBlockName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the BlockColor property in this BlockSymbol.
//          BlockColor is of type Expression:Color, which is an expression
//          that evaluates to a color. This property represents the block
//          override color that will be applied when drawing the block.
// RETURNS: The string representation of the Expression:Color.
//-------------------------------------------------------------------------
const MdfString& BlockSymbol::GetBlockColor() const
{
    return this->m_strBlockColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the BlockColor property in this BlockSymbol.
//          BlockColor is of type Expression:Color, which is an expression
//          that evaluates to a color. This property represents the block
//          override color that will be applied when drawing the block.
// PARAMETERS:
//      Input:
//         strBlockColor - The string representation of the Expression:Color.
//-------------------------------------------------------------------------
void BlockSymbol::SetBlockColor(const MdfString& strBlockColor)
{
    this->m_strBlockColor = strBlockColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the LayerColor property in this BlockSymbol.
//          LayerColor is of type Expression:Color, which is an expression
//          that evaluates to a color. This property represents the layer
//          override color that will be applied when drawing the block.
// RETURNS: The string representation of the Expression:Color.
//-------------------------------------------------------------------------
const MdfString& BlockSymbol::GetLayerColor() const
{
    return this->m_strLayerColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the LayerColor property in this BlockSymbol.
//          LayerColor is of type Expression:Color, which is an expression
//          that evaluates to a color. This property represents the layer
//          override color that will be applied when drawing the block.
// PARAMETERS:
//      Input:
//         strLayerColor - The string representation of the Expression:Color.
//-------------------------------------------------------------------------
void BlockSymbol::SetLayerColor(const MdfString& strLayerColor)
{
    this->m_strLayerColor = strLayerColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Implementation of the Visitor pattern. The pure, virtual method
//          is defined in Symbol and is implemented by all of its concrete
//          subclasses.
// PARAMETERS:
//      Input:
//          isymVisitor - The ISymbolVisitor interface which sports methods
//                        that accept the final concrete type this BlockSymbol
//                        represents as an argument.
//-------------------------------------------------------------------------
void BlockSymbol::AcceptVisitor(ISymbolVisitor& isymVisitor)
{
    isymVisitor.VisitBlockSymbol(*this);
}
