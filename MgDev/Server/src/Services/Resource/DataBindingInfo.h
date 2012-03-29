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

#ifndef MG_DATA_BINDING_INFO_H
#define MG_DATA_BINDING_INFO_H

class MgDataBindingInfo
{
/// Constructors/Destructor

    public:

        MgDataBindingInfo();
        explicit MgDataBindingInfo(const MgDataBindingInfo& dataBindingInfo);
        virtual ~MgDataBindingInfo();

/// Methods

    public:

        MgDataBindingInfo& operator=(const MgDataBindingInfo& dataBindingInfo);

        STRING GetPreProcessTags() const;
        STRING GetResourceDataFilePath() const;
        STRING GetLoginUsername() const;
        STRING GetLoginPassword() const;
        bool GetSubstituteUnmanagedDataMappings() const;

        void SetPreProcessTags(CREFSTRING preProcessTags);
        void SetResourceDataFilePath(CREFSTRING resourceDataFilePath);
        void SetLoginUsername(CREFSTRING loginUsername);
        void SetLoginPassword(CREFSTRING loginPassword);
        void SetSubstituteUnmanagedDataMappings(bool value);

/// Data Members

    private:

        STRING m_preProcessTags;
        STRING m_resourceDataFilePath;
        STRING m_loginUsername;
        STRING m_loginPassword;
        bool m_substituteUnmanagedDataMappings;
};

/// Inline Methods

inline STRING MgDataBindingInfo::GetPreProcessTags() const
{
    return m_preProcessTags;
}

inline STRING MgDataBindingInfo::GetResourceDataFilePath() const
{
    return m_resourceDataFilePath;
}

inline STRING MgDataBindingInfo::GetLoginUsername() const
{
    return m_loginUsername;
}

inline STRING MgDataBindingInfo::GetLoginPassword() const
{
    return m_loginPassword;
}

inline bool MgDataBindingInfo::GetSubstituteUnmanagedDataMappings() const
{
    return m_substituteUnmanagedDataMappings;
}

#endif
