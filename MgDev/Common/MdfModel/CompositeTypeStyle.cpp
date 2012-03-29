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
// The CompositeTypeStyle class implementation.  CompositeTypeStyle is a
// concrete subclass of FeatureTypeStyle.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "CompositeTypeStyle.h"
#include "IFeatureTypeStyleVisitor.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the CompositeTypeStyle class.
//-------------------------------------------------------------------------
CompositeTypeStyle::CompositeTypeStyle() : FeatureTypeStyle()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all objects that have been created on the
//          heap or have been adopted.
//-------------------------------------------------------------------------
CompositeTypeStyle::~CompositeTypeStyle()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Implementation of the Visitor pattern. The pure, virtual method
//          is defined in FeatureTypeStyle and is implemented by all of its
//          concrete subclasses.
// PARAMETERS:
//      Input:
//          iftsVisitor - The IFeatureTypeStyleVisitor interface which sports
//                        methods that accept the final concrete type this
//                        CompositeTypeStyle represents as an argument.
//-------------------------------------------------------------------------
void CompositeTypeStyle::AcceptVisitor(IFeatureTypeStyleVisitor& iftsVisitor)
{
     iftsVisitor.VisitCompositeTypeStyle(*this);
}
