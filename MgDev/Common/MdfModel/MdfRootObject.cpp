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
// The MdfRootObject class implementation. MdfRootObject is an abstract class.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "MdfRootObject.h"
#include "Base64.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the UnknownXml property cached by an XML Parser.
//          The Xml stream is then serialized when writing to a future
//          schema version - to support round-tripping.
// RETURNS:
//-------------------------------------------------------------------------
const MdfString& MdfRootObject::GetUnknownXml() const
{
    return this->m_strUnknownXml;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor to the UnknownXml property cached in this MdfRootObject
//          by an Xml Parser.  It is stored here in case it is needed
//          during serialization.
// PARAMETERS:
//      Input:
//          pstrUnknownXml - an Xml stream containing unknown elements.
//-------------------------------------------------------------------------
void MdfRootObject::SetUnknownXml(const MdfString& strUnknownXml)
{
    this->m_strUnknownXml = strUnknownXml;
}
