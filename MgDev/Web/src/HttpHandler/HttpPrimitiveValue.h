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

#ifndef _MG_HTTP_PRIMITIVE_VALUE_H
#define _MG_HTTP_PRIMITIVE_VALUE_H

#include "HttpPrimitiveValueTypes.h"

//////////////////////////////////////////////////////////////////
/// <summary>
/// Represents a primitive value returned by a call to a Mg Service.
/// </summary>
class MG_MAPAGENT_API MgHttpPrimitiveValue : public MgDisposable
{
EXTERNAL_API:
    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Destructor. This will clean up the value.
    /// </summary>
    virtual ~MgHttpPrimitiveValue();

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Constructor. Create a new MgHttpPrimitiveValue object initialized
    /// with a boolean value
    /// </summary>
    MgHttpPrimitiveValue(bool value);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Constructor. Create a new MgHttpPrimitiveValue object initialized
    /// with an integer value
    /// </summary>
    MgHttpPrimitiveValue(INT32 value);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Constructor. Create a new MgHttpPrimitiveValue object initialized
    /// with a string value
    /// </summary>
    MgHttpPrimitiveValue(CREFSTRING value);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Get the type of the data held by this object
    /// </summary>
    int GetType();

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Get the boolean primitive value
    /// </summary>
    bool GetBoolValue();

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Get the integer primitive value
    /// </summary>
    INT32 GetIntegerValue();

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Get the string primitive value
    /// </summary>
    STRING GetStringValue();

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Get a string representation of the primitive value
    /// </summary>
    STRING ToString();

protected:
    /// <summary>
    /// This method is responsible for releasing all
    /// its member variables and then destruct itself.
    /// </summary>
    /// <returns>Returns nothing</returns>
    virtual void Dispose();

    /// <summary>
    /// Get the class Id
    /// </summary>
    /// <returns>
    /// The integer value
    /// </returns>
    INT32 GetClassId();

private:

    union Val
    {
        bool    b;
        INT32   i;
        STRING* pstr;
    } m_value;

    INT32 m_type;

CLASS_ID:
    static const INT32 m_cls_id = HttpHandler_MapAgent_HttpPrimitiveValue;
};

#endif
