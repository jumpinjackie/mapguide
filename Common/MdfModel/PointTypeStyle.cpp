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
// The PointTypeStyle class implementation. PointTypeStyle is a
// concrete subclass of FeatureTypeStyle.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "PointTypeStyle.h"
#include "IFeatureTypeStyleVisitor.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the PointTypeStyle class.
//-------------------------------------------------------------------------
PointTypeStyle::PointTypeStyle() : FeatureTypeStyle()
{
    // default values
    this->m_bAllowOverpost = false;
    this->m_bDisplayAsText = false;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor.
//-------------------------------------------------------------------------
PointTypeStyle::~PointTypeStyle()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the DisplayAsText property of this PointTypeStyle.
//          Determines whether features styled by this PointTypeStyle will
//          be displayed as text labels.
// RETURNS: Default is false.
//-------------------------------------------------------------------------
bool PointTypeStyle::IsDisplayAsText() const
{
    return this->m_bDisplayAsText;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the DisplayAsText property of this PointTypeStyle.
// PARAMETERS:
//      Input:
//          bDisplayAsText - Determines whether features styled by this
//          PointTypeStyle will be displayed as text labels.
//-------------------------------------------------------------------------
void PointTypeStyle::SetDisplayAsText(bool bDisplayAsText)
{
    this->m_bDisplayAsText = bDisplayAsText;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the AllowOverpost property of this PointTypeStyle.
//          Determines whether labels may overpost the features drawn using
//          this PointTypeStyle.
// RETURNS: Default is false.
//-------------------------------------------------------------------------
bool PointTypeStyle::IsAllowOverpost() const
{
    return this->m_bAllowOverpost;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the AllowOverpost property of this PointTypeStyle.
// PARAMETERS:
//      Input:
//          bAllowOverpost - Determines whether labels may overpost the
//          features drawn using this PointTypeStyle.
//-------------------------------------------------------------------------
void PointTypeStyle::SetAllowOverpost(bool bAllowOverpost)
{
    this->m_bAllowOverpost = bAllowOverpost;
}

//-------------------------------------------------------------------------
// PURPOSE: Implementation of the Visitor pattern. The pure, virtual method
//          is defined in FeatureTypeStyle and is implemented by all of its
//          concrete subclasses.
// PARAMETERS:
//      Input:
//          iftsVisitor - The IFeatureTypeStyleVisitor interface which sports
//                        methods that accept the final concrete type this
//                        PointTypeStyle represents as an argument.
//-------------------------------------------------------------------------
void PointTypeStyle::AcceptVisitor(IFeatureTypeStyleVisitor& iftsVisitor)
{
    iftsVisitor.VisitPointTypeStyle(*this);
}
