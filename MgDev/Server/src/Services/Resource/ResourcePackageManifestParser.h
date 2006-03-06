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

#ifndef MG_RESOURCE_PACKAGE_MANIFEST_PARSER_H
#define MG_RESOURCE_PACKAGE_MANIFEST_PARSER_H

#include "XmlUtil.h"
#include "OperationInfo.h"

typedef std::vector<MgOperationInfo> MgOpInfoVector;

class MgResourcePackageManifestParser
{
/// Constructors/Destructor

    public:

        MgResourcePackageManifestParser();
        virtual ~MgResourcePackageManifestParser();

    private:

        // Unimplemented Methods

        MgResourcePackageManifestParser(const MgResourcePackageManifestParser&);
        MgResourcePackageManifestParser& operator=(
            const MgResourcePackageManifestParser&);

/// Methods

    public:

        CREFSTRING GetDescription() const;
        const MgOpInfoVector& GetOperations() const;

        void Parse(const string& manifest);

/// Data Members

    private:

        auto_ptr<MgXmlUtil> m_xmlUtil;
        STRING m_description;
        MgOpInfoVector m_operations;
};

/// Inline Methods

inline CREFSTRING MgResourcePackageManifestParser::GetDescription() const
{
    return m_description;
}

inline const MgOpInfoVector& MgResourcePackageManifestParser::GetOperations() const
{
    return m_operations;
}

#endif
