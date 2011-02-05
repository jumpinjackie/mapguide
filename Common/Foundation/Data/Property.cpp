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

#include "Foundation.h"

/////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor.  Initializes an empty property
/// </summary>
MgProperty::MgProperty()
{
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor.  Deletes property and any contained information
/// </summary>
MgProperty::~MgProperty()
{
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgProperty::GetClassId()
{
    return m_cls_id;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the property name
/// </summary>
/// <returns>
/// The property name
/// </returns>
STRING MgProperty::GetName()
{
    return m_propertyName;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the property name
/// <param name="name">
/// Name of property
/// </param>
/// </summary>
void MgProperty::SetName(CREFSTRING name)
{
    if (name.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgProperty.SetName",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    m_propertyName = name;
}

//////////////////////////////////////////////////////////////
/// <summary>
// Does it allow to set the name
/// </summary>
bool MgProperty::CanSetName()
{
    return true;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Self destructing method
/// </summary>
void MgProperty::Dispose()
{
    delete this;
}
