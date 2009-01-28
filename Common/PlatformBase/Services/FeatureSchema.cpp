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
#include "FeatureService.h"

MG_IMPL_DYNCREATE(MgFeatureSchema);

//////////////////////////////////////////////////////////////
MgFeatureSchema::MgFeatureSchema()
{
    m_collection = new MgClassDefinitionCollection();
}

//////////////////////////////////////////////////////////////
MgFeatureSchema::MgFeatureSchema(CREFSTRING name, CREFSTRING description)
{
    SetName(name);

    m_description = description;
    m_collection = new MgClassDefinitionCollection();
}

//////////////////////////////////////////////////////////////
void MgFeatureSchema::SetName(CREFSTRING name)
{
    if (name.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgFeatureSchema.SetName",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }
    m_name = name;
}

//////////////////////////////////////////////////////////////
STRING MgFeatureSchema::GetName()
{
    return m_name;
}

//////////////////////////////////////////////////////////////
void MgFeatureSchema::SetDescription(CREFSTRING description)
{
    m_description = description;
}

//////////////////////////////////////////////////////////////
STRING MgFeatureSchema::GetDescription()
{
    return m_description;
}

//////////////////////////////////////////////////////////////
MgClassDefinitionCollection* MgFeatureSchema::GetClasses()
{
    return SAFE_ADDREF((MgClassDefinitionCollection*)m_collection);
}

//////////////////////////////////////////////////////////////
void MgFeatureSchema::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////
INT32 MgFeatureSchema::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////////////////////
void MgFeatureSchema::Serialize(MgStream* stream)
{
    stream->WriteString(m_name);
    stream->WriteString(m_description);
    stream->WriteObject(m_collection);
}

//////////////////////////////////////////////////////////////
void MgFeatureSchema::Deserialize(MgStream* stream)
{
    stream->GetString(m_name);
    stream->GetString(m_description);
    m_collection = (MgClassDefinitionCollection*)stream->GetObject();
}

//////////////////////////////////////////////////////////////
bool MgFeatureSchema::CanSetName()
{
    return true;
}
