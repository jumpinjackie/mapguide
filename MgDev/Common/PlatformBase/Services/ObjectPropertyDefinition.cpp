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

#include "PlatformBase.h"
#include "ObjectPropertyType.h"
#include "OrderingOption.h"

MG_IMPL_DYNCREATE(MgObjectPropertyDefinition)

//////////////////////////////////////////////////////////////////
/// <summary>
/// MgObjectPropertyDefinition defines a feature property which contains
/// or is collection of objects from another feature class.  The contained
/// objects have their own feature class definition.  The identity properties
/// for collections of objects is also defined.
/// </summary>

MgObjectPropertyDefinition::MgObjectPropertyDefinition()
{
    Initialize();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs a MgObjectPropertyDefinition object
/// </summary>
/// <param name="name">Name of the property</param>
MgObjectPropertyDefinition::MgObjectPropertyDefinition(CREFSTRING name) : MgPropertyDefinition(name, MgFeaturePropertyType::ObjectProperty)
{
    Initialize();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets a reference to the ClassDefinition that defines
/// the type of this property.
/// </summary>
/// <returns>Class definition</returns>
MgClassDefinition* MgObjectPropertyDefinition::GetClassDefinition()
{
    return SAFE_ADDREF((MgClassDefinition*)this->m_clsDef);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the type of this object property (value,
/// collection, or ordered collection).
/// </summary>
/// <returns>Returns the type of this object property</returns>
/// <remarks>The type is a value of the enum ObjectType</remarks>
INT32 MgObjectPropertyDefinition::GetObjectType()
{
    return this->m_objType;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the order type of this object property (ascending or descending).
/// This property is only applicable if the property type is an
/// ordered collection.</summary>
/// <returns>Returns the order type</returns>
/// <remarks>The type is a value of the enum OrderType</remarks>
INT32 MgObjectPropertyDefinition::GetOrderType()
{
    return this->m_orderType;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Gets a reference to a DataPropertyDefinition to use for uniquely identifying
/// instances of the contained class within a single parent object instance.
/// This value is only used for collection object property types
/// </summary>
/// <returns>Returns data property definition</returns>
MgDataPropertyDefinition* MgObjectPropertyDefinition::GetIdentityProperty()
{
    return SAFE_ADDREF((MgDataPropertyDefinition*)this->m_dataPropDef);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets a reference to the ClassDefinition that defines
/// the type of this property.
/// </summary>
/// <param name="classDef">Class definition</returns>
void MgObjectPropertyDefinition::SetClassDefinition(MgClassDefinition* classDef)
{
    this->m_clsDef = SAFE_ADDREF((MgClassDefinition*)classDef);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the type of this object property (value,
/// collection, or ordered collection).
/// </summary>
/// <param name="objType">type of this object property</param>
void MgObjectPropertyDefinition::SetObjectType(INT32 objType)
{
    MgObjectPropertyType::ValidateRange(objType);
    this->m_objType = objType;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the type of this object property (value,
/// collection, or ordered collection).
/// </summary>
/// <param name="orderType">order type</param>
/// <remarks>The type is a value of the enum OrderType</remarks>
void MgObjectPropertyDefinition::SetOrderType(INT32 orderType)
{
    MgOrderingOption::ValidateRange(orderType);
    this->m_orderType = orderType;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Sets a reference to a DataPropertyDefinition to use for uniquely identifying
/// instances of the contained class within a single parent object instance.
/// This value is only used for collection object property types
/// </summary>
/// <param name="propDef">Property definition</param>
void MgObjectPropertyDefinition::SetIdentityProperty(MgDataPropertyDefinition* propDef)
{
    this->m_dataPropDef = SAFE_ADDREF((MgDataPropertyDefinition*)propDef);
}

void MgObjectPropertyDefinition::Serialize(MgStream* stream)
{
    MgPropertyDefinition::Serialize(stream);
    stream->WriteObject(m_clsDef);
    stream->WriteObject(m_dataPropDef);
    stream->WriteInt32(m_objType);
    stream->WriteInt32(m_orderType);
}

void MgObjectPropertyDefinition::Deserialize(MgStream* stream)
{
    MgPropertyDefinition::Deserialize(stream);
    m_clsDef = (MgClassDefinition*)stream->GetObject();
    m_dataPropDef = (MgDataPropertyDefinition*)stream->GetObject();
    stream->GetInt32(m_objType);
    stream->GetInt32(m_orderType);
}

void MgObjectPropertyDefinition::Initialize()
{
    m_clsDef = (MgClassDefinition*)NULL;
    m_dataPropDef = (MgDataPropertyDefinition*)NULL;
    m_objType = MgObjectPropertyType::Value;
    m_orderType = MgOrderingOption::Ascending;
}
