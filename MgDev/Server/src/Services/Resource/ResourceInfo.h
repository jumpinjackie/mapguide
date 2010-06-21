//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef MGRESOURCEINFO_H_
#define MGRESOURCEINFO_H_

const int MAX_RESOURCE_METADATA_ATTRIBUTE_NUMBER = 5;

class MgResourceInfo
{
/// Enumerations

    public:

        enum Attribute
        {
            Depth           =  0,
            Owner           =  1,
            CreatedDate     =  2,
            ModifiedDate    =  3,
            Tags            =  4
        };

/// Constructors/Destructor

    public:

        MgResourceInfo(const MgResourceIdentifier& identifier,
            MgUserInformation* = NULL, time_t accessedTime = -1,
            INT32 depth = -1);
        MgResourceInfo(const MgResourceIdentifier& identifier,
            const XmlDocument& xmlDoc);
        MgResourceInfo(const MgResourceInfo& resourceInfo);
        virtual ~MgResourceInfo();

private:

        MgResourceInfo();

/// Methods

    public:

        MgResourceInfo& operator=(const MgResourceInfo& resourceInfo);

        MgResourceIdentifier& GetIdentifier();
        const XmlValue& GetMetadata(Attribute name) const;

        void SetIdentifier(CREFSTRING pathname, INT32 depth = -1);
        void SetAttribute(Attribute name, const XmlValue& value);

/// Data Members

    public:

        static const string sm_elementName;
        static const string sm_elementMetadata;
        static const string sm_elementResourceId;
        static const string sm_metadataUri;
        static const string sm_metadataPrefix;
        static const string sm_metadataNames[MAX_RESOURCE_METADATA_ATTRIBUTE_NUMBER];

    private:

        MgResourceIdentifier m_identifier;
        XmlValue m_metadataValues[MAX_RESOURCE_METADATA_ATTRIBUTE_NUMBER];
};

/// Inline Methods

inline MgResourceIdentifier& MgResourceInfo::GetIdentifier()
{
    return m_identifier;
}

inline const XmlValue& MgResourceInfo::GetMetadata(Attribute name) const
{
    return m_metadataValues[name];
}

#endif
