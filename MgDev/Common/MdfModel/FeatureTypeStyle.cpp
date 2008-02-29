//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
// The FeatureTypeStyle class implementation. FeatureTypeStyle is an
// abstract class that specifies a Visitor method for all of its subclasses
// to implement.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "FeatureTypeStyle.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the FeatureTypeStyle class.
//-------------------------------------------------------------------------
FeatureTypeStyle::FeatureTypeStyle()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor.
//-------------------------------------------------------------------------
FeatureTypeStyle::~FeatureTypeStyle()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the Rule collection in this FeatureTypeStyle.
// RETURNS: A pointer to the modifiable RuleCollection.
//-------------------------------------------------------------------------
RuleCollection* FeatureTypeStyle::GetRules()
{
    return &this->m_collRules;
}
