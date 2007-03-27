//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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
    //default settings
    this->SetBlockColor(L"");
    this->SetLayerColor(L"");
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all adopted objects.
//-------------------------------------------------------------------------
BlockSymbol::~BlockSymbol()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the SymbolLibrary property defined in this BlockSymbol.
//          The SymbolLibrary property is the library on the server containing
//          the shape. Each shape is a 2D polygon.
// RETURNS: The SymbolLibrary string.
//-------------------------------------------------------------------------
const MdfString& BlockSymbol::GetDrawingName()const
{
    return this->m_strDrawingName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the SymbolName property defined in this BlockSymbol.
//          The SymbolName property is the name for the shape stored on the
//          server in the SymbolLibrary. Each shape is a 2D polygon.
// PARAMETERS:
//      Input:
//          strSymbolLibrary - The SymbolLibrary string.
//-------------------------------------------------------------------------
void BlockSymbol::SetDrawingName(const MdfString& strDrawingName)
{
    this->m_strDrawingName = strDrawingName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the SymbolName property defined in this BlockSymbol.
//          The SymbolName property is the name of the shape stored on the
//          server in the SymbolLibrary. Each shape is a 2D polygon.
// RETURNS: The SymbolName string.
//-------------------------------------------------------------------------
const MdfString& BlockSymbol::GetBlockName()const
{
    return this->m_strBlockName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the SymbolName property defined in this BlockSymbol.
//          The SymbolName property is the name of the shape stored on the
//          server in the SymbolLibrary. Each shape is a 2D polygon.
// PARAMETERS:
//      Input:
//          strSymbolName - The SymbolName string.
//-------------------------------------------------------------------------
void BlockSymbol::SetBlockName(const MdfString& strBlockName)
{
    this->m_strBlockName = strBlockName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the AreaColor property in this BlockSymbol.
//          AreaColor is of type Expression:Color, which is an expression
//          that evaluates to a color. This property represents the color that
//          will be applied to the fill for vector areas.
// RETURNS: The string representation of the Expression:Color.
//-------------------------------------------------------------------------
const MdfString& BlockSymbol::GetBlockColor()const
{
    return this->m_strBlockColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the AreaColor property in this BlockSymbol.
//          AreaColor is of type Expression:Color, which is an expression
//          that evaluates to a color. This property represents the color that
//          will be applied to the fill for vector areas.
// PARAMETERS:
//      Input:
//         strAreaColor - The string representation of the Expression:Color.
//-------------------------------------------------------------------------
void BlockSymbol::SetBlockColor(const MdfString& strBlockColor)
{
    this->m_strBlockColor = strBlockColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the LineColor property in this BlockSymbol.
//          LineColor is of type Expression:Color, which is an expression
//          that evaluates to a color. This property represents the color that
//          will be applied to vector lines.
// RETURNS: The string representation of the Expression:Color.
//-------------------------------------------------------------------------
const MdfString& BlockSymbol::GetLayerColor()const
{
    return this->m_strLayerColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the LineColor property in this BlockSymbol.
//          LineColor is of type Expression:Color, which is an expression
//          that evaluates to a color. This property represents the color that
//          will be applied to vector lines.
// PARAMETERS:
//      Input:
//         strLineColor - The string representation of the Expression:Color.
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
//          isvVisitor - The ISymbolVisitor interface which sports methods
//                       that accept the final concrete type this BlockSymbol
//                       represents as an argument.
//-------------------------------------------------------------------------
void BlockSymbol::AcceptVisitor(ISymbolVisitor& isvVisitor)
{
    isvVisitor.VisitBlockSymbol(*this);
}
