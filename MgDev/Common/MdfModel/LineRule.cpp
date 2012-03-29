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
// The LineRule class implementation. LineRule is a concrete subclass of
// Rule.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "LineRule.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the LineRule class.
//-------------------------------------------------------------------------
LineRule::LineRule()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Destroy all adopted objects.
//-------------------------------------------------------------------------
LineRule::~LineRule()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the LineSymbolization2D collection in this LineRule.
// RETURNS: A pointer to the modifiable LineSymbolization2DCollection.
//-------------------------------------------------------------------------
LineSymbolizationCollection* LineRule::GetSymbolizations()
{
    return &this->m_collSymbolizations;
}
