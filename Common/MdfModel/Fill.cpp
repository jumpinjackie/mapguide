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
// Fill Class implementation. Fill class is an abstract class.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "Fill.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the Fill class.
//-------------------------------------------------------------------------
Fill::Fill()
{
    // default values
    this->m_strFillPattern = L"Solid"; // NOXLATE
    this->m_strForeColor = L"ffffffff"; // NOXLATE
    this->m_strBackColor = L"ff000000"; // NOXLATE
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor.
//-------------------------------------------------------------------------
Fill::~Fill()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the FillPattern that is defined in this
//          Fill. The FillPattern is stored internally and referenced
//          via a name. The FillPattern is used for the fill during stylization
//          of feature geometry.
// RETURNS: the FillPattern .
//-------------------------------------------------------------------------
const MdfString& Fill::GetFillPattern() const
{
    return this->m_strFillPattern;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the FillPattern that is defined in this
//          Fill. The FillPattern is stored internally and referenced
//          via a name. The FillPattern is used for the fill during stylization
//          of feature geometry.
// PARAMETERS:
//      Input:
//          strFillPattern - the FillPattern to be used by this fill.
//-------------------------------------------------------------------------
void Fill::SetFillPattern(const MdfString& strFillPattern)
{
    this->m_strFillPattern = strFillPattern;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ForegroundColor property in this Fill.
//          ForegroundColor is of type Expression:Color, which is an expression
//          that evaluates to a color. This property represents the color that
//          will be applied to the Fill pattern.
// RETURNS: The string representation of the Expression:Color.
//-------------------------------------------------------------------------
const MdfString& Fill::GetForegroundColor() const
{
    return this->m_strForeColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the ForegroundColor property in this Fill.
//          ForegroundColor is of type Expression:Color, which is an expression
//          that evaluates to a color. This property represents the color that
//          will be applied to the Fill pattern. This method does not test
//          the argument to be a valid expression.
// PARAMETERS:
//      Input:
//         strForegroundColor - The string representation of the Expression:Color.
//-------------------------------------------------------------------------
void Fill::SetForegroundColor(const MdfString& strForegroundColor)
{
    this->m_strForeColor = strForegroundColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the BackgroundColor property in this Fill.
//          BackgroundColor is of type Expression:Color, which is an expression
//          that evaluates to a color. This property represents the color that
//          is applied to the background before the Fill pattern is applied.
// RETURNS: The string representation of the Expression:Color.
//-------------------------------------------------------------------------
const MdfString& Fill::GetBackgroundColor() const
{
    return this->m_strBackColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the BackgroundColor property in this Fill.
//          BackgroundColor is of type Expression:Color, which is an expression
//          that evaluates to a color. This property represents the color that
//          is applied to the background before the Fill pattern is applied.
//          This method does not test for the argument being a valid expression.
// PARAMETERS:
//      Input:
//          strBackgroundColor - The string representation of the
//                               Expression:Color.
//-------------------------------------------------------------------------
void Fill::SetBackgroundColor(const MdfString& strBackgroundColor)
{
    this->m_strBackColor = strBackgroundColor;
}
