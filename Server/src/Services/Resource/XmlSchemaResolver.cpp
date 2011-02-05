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

#include "ResourceServiceDefs.h"
#include "XmlSchemaResolver.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgXmlSchemaResolver::MgXmlSchemaResolver()
{
    MG_RESOURCE_SERVICE_TRY()

    MgConfiguration* configuration = MgConfiguration::GetInstance();
    assert(NULL != configuration);

    configuration->GetBoolValue(
        MgConfigProperties::ResourceServicePropertiesSection,
        MgConfigProperties::ResourceServicePropertyResourceValidationEnabled,
        m_validationEnabled,
        MgConfigProperties::DefaultResourceServicePropertyResourceValidationEnabled);

    configuration->GetStringValue(
        MgConfigProperties::ResourceServicePropertiesSection,
        MgConfigProperties::ResourceServicePropertyResourceSchemaFilePath,
        m_schemaFilePath,
        MgConfigProperties::DefaultResourceServicePropertyResourceSchemaFilePath);

    MgFileUtil::AppendSlashToEndOfPath(m_schemaFilePath);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgXmlSchemaResolver.MgXmlSchemaResolver")
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgXmlSchemaResolver::~MgXmlSchemaResolver()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Resolves the given schema location and namespace to an XmlInputStream.
/// If the uri cannot be resolved by this XmlResolver, then the method should
/// return 0. The returned XmlInputStream is adopted by DbXml, and will be
/// deleted by it.
/// </summary>
///
/// <exceptions>
/// XmlException
/// </exceptions>
///----------------------------------------------------------------------------

XmlInputStream* MgXmlSchemaResolver::resolveSchema(
    XmlTransaction *txn, XmlManager &mgr,
    const std::string &schemaLocation, const std::string &nameSpace) const
{
    // Verify the existance of the schema file if XML validation is enabled.

    XmlInputStream* xmlInputStream = NULL;

    if (m_validationEnabled)
    {
        if (schemaLocation.empty())
        {
            // TODO: Add MgResources::GetMessage.
            STRING message = MgUtil::GetResourceMessage(
                MgResources::ResourceService, L"MgSchemaFileNotSpecified");

            throw XmlException(XmlException::INDEXER_PARSER_ERROR,
                MgUtil::WideCharToMultiByte(message), __FILE__, __LINE__);
        }
        else
        {
            STRING schemaPathname = m_schemaFilePath;
            schemaPathname += MgUtil::MultiByteToWideChar(schemaLocation);

            if (MgFileUtil::PathnameExists(schemaPathname))
            {
                xmlInputStream = mgr.createLocalFileInputStream(
                    MgUtil::WideCharToMultiByte(schemaPathname));
            }
            else
            {
                MgResources* resources = MgResources::GetInstance();
                assert(NULL != resources);

                // TODO: Add MgResources::GetMessage.
                STRING message = MgUtil::GetResourceMessage(
                    MgResources::ResourceService, L"MgSchemaFileNotFound");

                MgStringCollection arguments;
                arguments.Add(schemaPathname);

                message = resources->FormatMessage(message, &arguments);

                throw XmlException(XmlException::INDEXER_PARSER_ERROR,
                    MgUtil::WideCharToMultiByte(message), __FILE__, __LINE__);
            }
        }
    }

    return xmlInputStream;
}
