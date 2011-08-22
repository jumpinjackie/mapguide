//
//  Copyright (C) 2011 by Autodesk, Inc.
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
// The ProfileResult class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "ProfileResult.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the ProfileResult class.
//-------------------------------------------------------------------------
ProfileResult::ProfileResult()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor.
//-------------------------------------------------------------------------
ProfileResult::~ProfileResult()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ProfileResultType attribute.
// RETURNS: A profile result type attribute
//-------------------------------------------------------------------------
ProfileResult::ProfileResultType ProfileResult::GetProfileResultType() const
{
    return this->m_eProfileResultType;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ProfileResultType attribute.
// PARAMETERS:
//      Input:
//          type - profile result type
//-------------------------------------------------------------------------
void ProfileResult::SetProfileResultType(ProfileResult::ProfileResultType type)
{
    this->m_eProfileResultType = type;
}

//-------------------------------------------------------------------------
// PURPOSE: For memory leak testing.
//-------------------------------------------------------------------------
#ifdef _WIN32
#ifdef _DEBUG
int ProfileResult::DumpMemoryLeaks()
{
    return _CrtDumpMemoryLeaks();
}
#endif
#endif
