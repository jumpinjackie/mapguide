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

#ifndef MGOPERATIONPARAMETER_H_
#define MGOPERATIONPARAMETER_H_

class MgOperationParameter
{
/// Constructors/Destructor

public:
    MgOperationParameter();
    MgOperationParameter(const MgOperationParameter& opParam);
    virtual ~MgOperationParameter();

/// Methods

public:
    MgOperationParameter& operator=(const MgOperationParameter& opParam);

    CREFSTRING GetValue() const;
    void SetValue(CREFSTRING value);

    CREFSTRING GetContentType() const;
    void SetContentType(CREFSTRING contentType);

/// Data Members

INTERNAL_API:
    static const STRING Type;
    static const STRING Depth;
    static const STRING ResourceId;
    static const STRING ResourceContent;
    static const STRING ResourceHeader;
    static const STRING SourceResourceId;
    static const STRING DestinationResourceId;
    static const STRING DataName;
    static const STRING DataType;
    static const STRING DataLength;
    static const STRING Data;
    static const STRING OldDataName;
    static const STRING NewDataName;
    static const STRING Overwrite;
    static const STRING Package;
    static const STRING Owner;
    static const STRING IncludeDescendants;

private:
    STRING m_value;
    STRING m_contentType;
};


/// Inline Methods

inline CREFSTRING MgOperationParameter::GetValue() const
{
    return m_value;
}


inline CREFSTRING MgOperationParameter::GetContentType() const
{
    return m_contentType;
}

#endif
