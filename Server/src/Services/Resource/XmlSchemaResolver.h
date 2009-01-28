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

#ifndef MG_XML_SCHEMA_RESOLVER_H
#define MG_XML_SCHEMA_RESOLVER_H

class MgXmlSchemaResolver : public DbXml::XmlResolver
{
/// Constructors/Destructor

    public:

        MgXmlSchemaResolver();
        virtual ~MgXmlSchemaResolver();

    private:

        // Unimplemented Methods

        MgXmlSchemaResolver(const MgXmlSchemaResolver&);
        MgXmlSchemaResolver& operator=(const MgXmlSchemaResolver&);

/// Methods

    public:

        bool IsValidationEnabled() const;
        CREFSTRING GetSchemaFilePath() const;

        // overrides from XmlResolver

        virtual XmlInputStream *resolveSchema(
            XmlTransaction *txn, XmlManager &mgr,
            const std::string &schemaLocation,
            const std::string &nameSpace) const;

/// Data Members

    private:

        bool m_validationEnabled;
        STRING m_schemaFilePath;
};

/// Inline Methods

inline bool MgXmlSchemaResolver::IsValidationEnabled() const
{
    return m_validationEnabled;
}

inline CREFSTRING MgXmlSchemaResolver::GetSchemaFilePath() const
{
    return m_schemaFilePath;
}

#endif
