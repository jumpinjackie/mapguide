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

#ifndef MG_TAG_INFO_H
#define MG_TAG_INFO_H

const int MAX_RESOURCE_TAG_ATTRIBUTE_NUMBER = 3;

class MgTagInfo
{
/// Enumerations

    public:

        enum Attribute
        {
            StorageType = 0,
            TokenValue  = 1,
            MimeType    = 2
        };

/// Constructors/Destructor

    public:

        MgTagInfo();
        MgTagInfo(CREFSTRING storageType, CREFSTRING tokenValue,
            CREFSTRING mimeType);
        MgTagInfo(const MgTagInfo& tagInfo);
        virtual ~MgTagInfo();

/// Methods

    public:

        MgTagInfo& operator=(const MgTagInfo& tagInfo);

        CREFSTRING GetAttribute(Attribute name) const;
        void SetAttribute(Attribute name, CREFSTRING value);

/// Data Members

    private:

        STRING m_attributes[MAX_RESOURCE_TAG_ATTRIBUTE_NUMBER];
};

/// Inline Methods

inline CREFSTRING MgTagInfo::GetAttribute(Attribute name) const
{
    return m_attributes[name];
}

#endif
