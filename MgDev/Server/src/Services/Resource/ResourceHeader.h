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

#ifndef MGRESOURCEHEADER_H_
#define MGRESOURCEHEADER_H_

#include "ResourceInfo.h"

class MgResourceHeader;

typedef std::map<string, MgResourceHeader*> MgResourceHeaderMap;

class MgResourceHeader
{
// Constructors/Destructor

    public:

        MgResourceHeader();
        virtual ~MgResourceHeader();

    private:

        // Unimplemented Constructors/Methods

        MgResourceHeader(const MgResourceHeader&);
        MgResourceHeader& operator=(const MgResourceHeader&);

// Methods

    public:

        void Initialize(const MgResourceIdentifier& resourceIdentifier,
            const XmlDocument& xmlDoc, INT32 properties,
            MgPermissionInfo* permissionInfo = NULL);

        MgResourceInfo& GetResourceInfo();

        INT32 GetNumberOfFolders() const;
        void IncrementNumberOfFolders();

        INT32 GetNumberOfDocuments() const;
        void IncrementNumberOfDocuments();

        bool IsSecurityInfoInherited() const;
        bool IsSecurityInfoFromParent() const;

        const XmlValue& GetMetadata(MgResourceInfo::Attribute attr) const;
        void GetDocument(const MgResourceHeaderMap* resourceHeaderMap,
            string& document);

    private:

        void CheckInitialization() const;

        bool IsFilteredOn(INT32 properties) const;

        const DOMElement* GetSecurity() const;
        void SetSecurity(const DOMElement* securityNode);

// Data Members

    private:

        auto_ptr<MgXmlUtil> m_xmlUtil;
        INT32 m_properties;
        INT32 m_numFolders;
        INT32 m_numDocuments;
        bool m_inherited;
        auto_ptr<MgResourceInfo> m_resourceInfo;
        bool m_securityInfoFromParent;
};

// Inline Methods

inline INT32 MgResourceHeader::GetNumberOfFolders() const
{
    return m_numFolders;
}

inline INT32 MgResourceHeader::GetNumberOfDocuments() const
{
    return m_numDocuments;
}

inline bool MgResourceHeader::IsSecurityInfoInherited() const
{
    return m_inherited;
}

inline bool MgResourceHeader::IsSecurityInfoFromParent() const
{
    return m_securityInfoFromParent;
}

inline bool MgResourceHeader::IsFilteredOn(INT32 properties) const
{
    return (0 != (m_properties & properties));
}

#endif
