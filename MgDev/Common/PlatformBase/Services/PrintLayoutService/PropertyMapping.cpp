//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#include "PlatformBase.h"

MG_IMPL_DYNCREATE(MgPropertyMapping)

//////////////////////////////////////////////////////////////
/// <summary>
/// Constructor
/// </summary>
MgPropertyMapping::MgPropertyMapping()
{
    m_sourceProperty = L"";
    m_sourceUnits = L"";
    m_targetProperty = L"";
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Destructor
/// </summary>
MgPropertyMapping::~MgPropertyMapping()
{
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Gets the name of the source property in this class.
/// </summary>
/// <returns>Returns the name of the source property</returns>
STRING MgPropertyMapping::GetSourceProperty()
{
    return m_sourceProperty;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Gets the name of the target property in this class.
/// </summary>
/// <returns>Returns the name of the target property</returns>
STRING MgPropertyMapping::GetTargetProperty()
{
    return m_targetProperty;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Gets the name of the source property units in this class.
/// </summary>
/// <returns>Returns the source property units</returns>
STRING MgPropertyMapping::GetSourceUnits()
{
    return m_sourceUnits;
}

//////////////////////////////////////////////////////////////
void MgPropertyMapping::Serialize(MgStream* stream)
{
    stream->WriteString(m_sourceProperty);
    stream->WriteString(m_sourceUnits);
    stream->WriteString(m_targetProperty);
}

//////////////////////////////////////////////////////////////
void MgPropertyMapping::Deserialize(MgStream* stream)
{
    stream->GetString(m_sourceProperty);
    stream->GetString(m_sourceUnits);
    stream->GetString(m_targetProperty);
}

//////////////////////////////////////////////////////////////
INT32 MgPropertyMapping::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////////////////////
void MgPropertyMapping::Dispose()
{
    delete this;
}
