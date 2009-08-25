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

MG_IMPL_DYNCREATE(MgParameter);

#define CHECK_NULL_PARAMETER(method_name) \
    if (NULL == m_parameter) \
        throw new MgInvalidOperationException(method_name, __LINE__, __WFILE__, NULL, L"", NULL);

///////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs an MgParameter object with the specified parameter
/// name, value, and direction.
/// </summary>
/// <param name="prop">
/// An MgNullableProperty instance which contains 
/// the name and value of the parameter.
/// </param>
/// <param name="direction">
/// The function direction of the parameter value.
/// </param>
MgParameter::MgParameter(MgNullableProperty* prop, INT32 direction)
{
    m_parameter = prop;
    SAFE_ADDREF(m_parameter);
    m_parameterDirection = direction;
}

///////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs an MgParameter object. The default parameter direction
/// is input.
/// </summary>
/// <param name="prop">
/// An MgNullableProperty instance which contains 
/// the name and value of the parameter.
/// </param>
MgParameter::MgParameter(MgNullableProperty* prop)
{
    m_parameter = prop;
    SAFE_ADDREF(m_parameter);
    m_parameterDirection = MgParameterDirection::Input;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor.
/// </summary>
MgParameter::MgParameter()
{
    m_parameterDirection = MgParameterDirection::Input;
    m_parameter = NULL;
}

////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor. Deletes a property and any information contained
/// in it.
/// </summary>
MgParameter::~MgParameter()
{
    SAFE_RELEASE(m_parameter);
}

///////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the function direction of the parameter value, which
/// is defined in MgParameterDirection.
/// </summary>
/// <param name="value">
/// Input the direction value.
/// </param>
/// <returns>
/// Returns nothing
/// </returns>
void MgParameter::SetDirection(INT32 value)
{
    m_parameterDirection = value;
}

///////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the function direction value of the the command parameter, which
/// is defined in MgParameterDirection.
/// </summary>
/// <returns>
/// Returns the direction value defined in MgParameterDirection.
/// </returns>
INT32 MgParameter::GetDirection()
{
    return m_parameterDirection;
}

///////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the parameter name.
/// </summary>
/// <returns>
/// Returns the parameter name.
/// </returns>
STRING MgParameter::GetName()
{
    CHECK_NULL_PARAMETER(L"MgParameter.GetName");
    return m_parameter->GetName();
}

///////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the parameter name.
/// </summary>
/// <returns>
/// Returns nothing.
/// </returns>
void MgParameter::SetName(CREFSTRING name)
{
    CHECK_NULL_PARAMETER(L"MgParameter.SetName");
    m_parameter->SetName(name);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Serializes data to a TCP/IP stream.
/// </summary>
/// <param name="stream">
/// Stream
/// </param>
void MgParameter::Serialize(MgStream* stream)
{
    CHECK_NULL_PARAMETER(L"MgParameter.Serialize");
    m_parameter->Serialize(stream);
    stream->WriteInt32(m_parameterDirection);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Deserializes data from a TCP/IP stream.
/// </summary>
/// <param name="stream">
/// Stream
/// </param>
void MgParameter::Deserialize(MgStream* stream)
{
    CHECK_NULL_PARAMETER(L"MgParameter.Deserialize");
    m_parameter->Deserialize(stream);
    stream->GetInt32(m_parameterDirection);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Test whether it allow to set the name.
/// </summary>
bool MgParameter::CanSetName()
{
    return true;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the name and value of the parameter.
/// </summary>
/// <returns>
/// Returns an MgNullableProperty instance which contains 
/// the name and value of the parameter.
/// </returns>
MgNullableProperty* MgParameter::GetProperty()
{
    SAFE_ADDREF(m_parameter);
    return m_parameter;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the name and value of the parameter.
/// </summary>
/// <param name="value">
/// Input an MgNullableProperty instance which contains 
/// the name and value of the parameter.
/// </param>
/// <returns>
/// Returns nothing.
/// </returns>
void MgParameter::SetProperty(MgNullableProperty* value)
{
    SAFE_RELEASE(m_parameter);
    m_parameter = value;
    SAFE_ADDREF(m_parameter);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the unique identifier for the class
/// </summary>
/// <returns>
/// Class Identifider.
/// </returns>
INT32 MgParameter::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Disposes this object.
/// </summary>
void MgParameter::Dispose()
{
    delete this;
}
