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

#ifndef MGOPERATIONINFO_H_
#define MGOPERATIONINFO_H_

#include "OperationParameter.h"

typedef std::map<STRING, MgOperationParameter> MgOpParamMap;

class MgOperationName
{
INTERNAL_API:
//    static const STRING CreateRepository;
//    static const STRING DeleteRepository;
    static const STRING UpdateRepository;
//    static const STRING ApplyResourcePackage;

    static const STRING SetResource;
    static const STRING DeleteResource;
    static const STRING MoveResource;
    static const STRING CopyResource;
    static const STRING ChangeResourceOwner;
    static const STRING InheritPermissionsFrom;

    static const STRING SetResourceData;
    static const STRING DeleteResourceData;
    static const STRING RenameResourceData;
};


class MgOperationInfo
{
/// Constructors/Destructor

public:
    MgOperationInfo();
    explicit MgOperationInfo(CREFSTRING name);
    MgOperationInfo(const MgOperationInfo& opInfo);
    virtual ~MgOperationInfo();

/// Methods

public:
    MgOperationInfo& operator=(const MgOperationInfo& opInfo);

    CREFSTRING GetName() const;
    void SetName(CREFSTRING name);

    CREFSTRING GetVersion() const;
    void SetVersion(CREFSTRING version);

    const MgOperationParameter& GetParameter(CREFSTRING name,
        bool required = true) const;
    void AddParameter(CREFSTRING name, const MgOperationParameter& opParam);
    void RemoveParameter(CREFSTRING name);

    const MgOpParamMap& GetParameters() const;

/// Data Members

public:
    static const STRING sm_currentVersion;
    static const MgOperationParameter sm_blankParameter;

private:
    STRING m_name;
    STRING m_version;
    MgOpParamMap m_parameters;
};


/// Inline Methods

inline CREFSTRING MgOperationInfo::GetName() const
{
    return m_name;
}


inline CREFSTRING MgOperationInfo::GetVersion() const
{
    return m_version;
}


inline const MgOpParamMap& MgOperationInfo::GetParameters() const
{
    return m_parameters;
}

#endif
