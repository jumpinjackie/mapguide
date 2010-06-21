//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

/////////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GwsQueryEngineImp.h"


/////////////////////////////////////////////////////////////////////
//
// class CGwsPooledFeature
//
/////////////////////////////////////////////////////////////////////

CGwsPooledFeature::CGwsPooledFeature (IGWSExtendedFeatureDescription * iResdscs)
{
    Initialize (iResdscs);
    InitializePropertyValues ();
}


CGwsPooledFeature::~CGwsPooledFeature () throw()
{
}


void CGwsPooledFeature::Set (IGWSFeature * feature)
{
    FdoInt32 revision = 0;
    GWSFeatureId featureId;
    if(feature)
    {
        revision = feature->GetRevisionNumber();
        featureId = feature->GetFeatureId();
    }

    SetRevisionNumber(revision);
    SetFeatureId(featureId);
    SetPropertyValues (feature);
}


FdoInt32 CGwsPooledFeature::GetRevisionNumber ()
{
    return CGwsMutableFeature::GetRevisionNumber ();
}

GWSFeatureId CGwsPooledFeature::GetFeatureId ()
{
    return CGwsMutableFeature::GetFeatureId ();
}
