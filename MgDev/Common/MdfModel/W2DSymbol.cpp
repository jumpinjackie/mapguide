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
// W2DSymbol class implementation. W2DSymbol is a concrete subclass of
// the FlatSymbol class.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "W2DSymbol.h"
#include "ISymbolVisitor.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the W2DSymbol class.
//-------------------------------------------------------------------------
W2DSymbol::W2DSymbol(const MdfString& strSymbolLibrary,
                     const MdfString& strSymbolName)
: m_strSymbolLibrary(strSymbolLibrary)
, m_strSymbolName(strSymbolName)
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all adopted objects.
//-------------------------------------------------------------------------
W2DSymbol::~W2DSymbol()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the SymbolLibrary property defined in this W2DSymbol.
//          The SymbolLibrary property is the library on the server containing
//          the shape. Each shape is a 2D polygon.
// RETURNS: The SymbolLibrary string.
//-------------------------------------------------------------------------
const MdfString& W2DSymbol::GetSymbolLibrary() const
{
    return this->m_strSymbolLibrary;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the SymbolName property defined in this W2DSymbol.
//          The SymbolName property is the name for the shape stored on the
//          server in the SymbolLibrary. Each shape is a 2D polygon.
// PARAMETERS:
//      Input:
//          strSymbolLibrary - The SymbolLibrary string.
//-------------------------------------------------------------------------
void W2DSymbol::SetSymbolLibrary(const MdfString& strSymbolLibrary)
{
    this->m_strSymbolLibrary = strSymbolLibrary;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the SymbolName property defined in this W2DSymbol.
//          The SymbolName property is the name of the shape stored on the
//          server in the SymbolLibrary. Each shape is a 2D polygon.
// RETURNS: The SymbolName string.
//-------------------------------------------------------------------------
const MdfString& W2DSymbol::GetSymbolName() const
{
    return this->m_strSymbolName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the SymbolName property defined in this W2DSymbol.
//          The SymbolName property is the name of the shape stored on the
//          server in the SymbolLibrary. Each shape is a 2D polygon.
// PARAMETERS:
//      Input:
//          strSymbolName - The SymbolName string.
//-------------------------------------------------------------------------
void W2DSymbol::SetSymbolName(const MdfString& strSymbolName)
{
    this->m_strSymbolName = strSymbolName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the FillColor property in this W2DSymbol.
//          FillColor is of type Expression:Color, which is an expression
//          that evaluates to a color. This property represents the color that
//          will be applied to the fill for vector areas.
// RETURNS: The string representation of the Expression:Color.
//-------------------------------------------------------------------------
const MdfString& W2DSymbol::GetFillColor() const
{
    return this->m_strFillColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the FillColor property in this W2DSymbol.
//          FillColor is of type Expression:Color, which is an expression
//          that evaluates to a color. This property represents the color that
//          will be applied to the fill for vector areas.
// PARAMETERS:
//      Input:
//         strFillColor - The string representation of the Expression:Color.
//-------------------------------------------------------------------------
void W2DSymbol::SetFillColor(const MdfString& strFillColor)
{
    this->m_strFillColor = strFillColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the LineColor property in this W2DSymbol.
//          LineColor is of type Expression:Color, which is an expression
//          that evaluates to a color. This property represents the color that
//          will be applied to vector lines.
// RETURNS: The string representation of the Expression:Color.
//-------------------------------------------------------------------------
const MdfString& W2DSymbol::GetLineColor() const
{
    return this->m_strLineColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the LineColor property in this W2DSymbol.
//          LineColor is of type Expression:Color, which is an expression
//          that evaluates to a color. This property represents the color that
//          will be applied to vector lines.
// PARAMETERS:
//      Input:
//         strLineColor - The string representation of the Expression:Color.
//-------------------------------------------------------------------------
void W2DSymbol::SetLineColor(const MdfString& strLineColor)
{
    this->m_strLineColor = strLineColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the TextColor property in this W2DSymbol.
//          TextColor is of type Expression:Color, which is an expression
//          that evaluates to a color. This property represents the color that
//          will be applied to fill for text areas.
// RETURNS: The string representation of the Expression:Color.
//-------------------------------------------------------------------------
const MdfString& W2DSymbol::GetTextColor() const
{
    return this->m_strTextColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the TextColor property in this W2DSymbol.
//          TextColor is of type Expression:Color, which is an expression
//          that evaluates to a color. This property represents the color that
//          will be applied to fill for text areas.
// PARAMETERS:
//      Input:
//         strTextColor - The string representation of the Expression:Color.
//-------------------------------------------------------------------------
void W2DSymbol::SetTextColor(const MdfString& strTextColor)
{
    this->m_strTextColor = strTextColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Implementation of the Visitor pattern. The pure, virtual method
//          is defined in Symbol and is implemented by all of its concrete
//          subclasses.
// PARAMETERS:
//      Input:
//          isymVisitor - The ISymbolVisitor interface which sports methods
//                        that accept the final concrete type this W2DSymbol
//                        represents as an argument.
//-------------------------------------------------------------------------
void W2DSymbol::AcceptVisitor(ISymbolVisitor& isymVisitor)
{
    isymVisitor.VisitW2DSymbol(*this);
}
