//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#ifndef MG_OPERATION_INFO_H
#define MG_OPERATION_INFO_H

#include "OperationParameter.h"

typedef std::map<STRING, MgOperationParameter> MgOpParamMap;

class MgOperationInfo
{
/// Constructors/Destructor

    public:

        MgOperationInfo();
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
        void SetParameter(CREFSTRING name, const MgOperationParameter& opParam);

/// Data Members

    public:

        static const MgOperationParameter m_defaultParameter;

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

#endif
