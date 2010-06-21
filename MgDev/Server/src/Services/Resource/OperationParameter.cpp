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

#include "ResourceServiceDefs.h"
#include "OperationParameter.h"

// Resource Service operation parameters
// TODO: Share the following strings with the Web tier.

const STRING MgOperationParameter::Type=L"TYPE";
const STRING MgOperationParameter::Depth=L"DEPTH";
const STRING MgOperationParameter::ResourceId=L"RESOURCEID";
const STRING MgOperationParameter::ResourceContent=L"CONTENT";
const STRING MgOperationParameter::ResourceHeader=L"HEADER";
const STRING MgOperationParameter::SourceResourceId=L"SOURCE";
const STRING MgOperationParameter::DestinationResourceId=L"DESTINATION";
const STRING MgOperationParameter::DataName=L"DATANAME";
const STRING MgOperationParameter::DataType=L"DATATYPE";
const STRING MgOperationParameter::DataLength=L"DATALENGTH";
const STRING MgOperationParameter::Data=L"DATA";
const STRING MgOperationParameter::OldDataName=L"OLDDATANAME";
const STRING MgOperationParameter::NewDataName=L"NEWDATANAME";
const STRING MgOperationParameter::Overwrite=L"OVERWRITE";
const STRING MgOperationParameter::Package=L"PACKAGE";
const STRING MgOperationParameter::Owner=L"OWNER";
const STRING MgOperationParameter::IncludeDescendants=L"INCLUDEDESCENDANTS";


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgOperationParameter::MgOperationParameter()
{
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Copy constructor.
///
MgOperationParameter::MgOperationParameter(const MgOperationParameter& opParam)
{
    *this = opParam;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgOperationParameter::~MgOperationParameter()
{
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Assignment operator.
///
MgOperationParameter& MgOperationParameter::operator=(
    const MgOperationParameter& opParam)
{
    if (&opParam != this)
    {
        m_value = opParam.m_value;
        m_contentType = opParam.m_contentType;
    }

    return *this;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets the value of the parameter.
///
void MgOperationParameter::SetValue(CREFSTRING value)
{
    m_value = value;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets the content type of the parameter.
///
void MgOperationParameter::SetContentType(CREFSTRING contentType)
{
    m_contentType = contentType;
}
