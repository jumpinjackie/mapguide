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

#include "ResourceServiceDefs.h"
#include "ResourceContainer.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgResourceContainer::MgResourceContainer(MgDbEnvironment& environment,
    const string& fileName) :
    MgDatabase(environment),
    m_xmlContainer(NULL)
{
    assert(!fileName.empty());

    MG_RESOURCE_SERVICE_TRY()

    XmlManager& xmlMan = m_environment.GetXmlManager();
    assert(XmlContainer::WholedocContainer == xmlMan.getDefaultContainerType());
    u_int32_t flags = xmlMan.getDefaultContainerFlags();

    if (m_environment.IsTransacted())
    {
        assert(flags | DBXML_TRANSACTIONAL);
        XmlTransaction xmlTxn = xmlMan.createTransaction();

        m_xmlContainer = new XmlContainer(xmlMan.openContainer(xmlTxn, fileName, flags));
        xmlTxn.commit(0);
    }
    else
    {
        m_xmlContainer = new XmlContainer(xmlMan.openContainer(fileName, flags));
    }

#ifdef _DEBUG
    // These generate a lot of noise in DEBUG mode, so turn them on only when needed.
//    DbXml::setLogLevel(DbXml::LEVEL_ALL, true);
//    DbXml::setLogCategory(DbXml::CATEGORY_ALL, true);
#endif

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceContainer.MgResourceContainer")
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgResourceContainer::~MgResourceContainer()
{
    delete m_xmlContainer;
}

///----------------------------------------------------------------------------
/// <summary>
/// Adds the index specification for the specified attribute or node.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceContainer::AddIndex(const string& name)
{
    MG_RESOURCE_SERVICE_TRY()

    string uri;
    XmlIndexSpecification::Type indexType;
    XmlValue::Type syntaxType;

    if (MgResourceInfo::sm_metadataNames[MgResourceInfo::ModifiedDate] == name)
    {
        uri = MgResourceInfo::sm_metadataUri;
        indexType = (XmlIndexSpecification::Type)(
            XmlIndexSpecification::PATH_NODE |
            XmlIndexSpecification::NODE_METADATA |
            XmlIndexSpecification::KEY_EQUALITY);
        syntaxType = XmlValue::DATE_TIME;
    }
    else if (MgResourceInfo::sm_nodeResourceId == name)
    {
        indexType = (XmlIndexSpecification::Type)(
            XmlIndexSpecification::PATH_EDGE |
            XmlIndexSpecification::NODE_ELEMENT |
            XmlIndexSpecification::KEY_EQUALITY);
        syntaxType = XmlValue::STRING;
    }
    else
    {
        STRING buffer;
        MgUtil::MultiByteToWideChar(name, buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgResourceContainer.AddIndex",
            __LINE__, __WFILE__, &arguments, L"MgNameNotFound", NULL);
    }

    XmlManager& xmlMan = m_xmlContainer->getManager();
    XmlUpdateContext updateContext = xmlMan.createUpdateContext();

    if (m_environment.IsTransacted())
    {
        XmlTransaction xmlTxn = xmlMan.createTransaction();

        m_xmlContainer->addIndex(
            xmlTxn, uri, name, indexType, syntaxType, updateContext);
        xmlTxn.commit(0);
    }
    else
    {
        m_xmlContainer->addIndex(
            uri, name, indexType, syntaxType, updateContext);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceContainer.AddIndex")
}

///----------------------------------------------------------------------------
/// <summary>
/// Deletes the specified node's index specification from the container.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceContainer::DeleteIndex(const string& name)
{
    MG_RESOURCE_SERVICE_TRY()

    string uri;
    string index;

    if (MgResourceInfo::sm_metadataNames[MgResourceInfo::ModifiedDate] == name)
    {
        uri = MgResourceInfo::sm_metadataUri;
        index = "node-metadata-equality-date_time";
    }
    else if (MgResourceInfo::sm_nodeResourceId == name)
    {
        index = "edge-element-equality-string";
    }
    else
    {
        STRING buffer;
        MgUtil::MultiByteToWideChar(name, buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgResourceContainer.DeleteIndex",
            __LINE__, __WFILE__, &arguments, L"MgNameNotFound", NULL);
    }

    XmlManager& xmlMan = m_xmlContainer->getManager();
    XmlUpdateContext updateContext = xmlMan.createUpdateContext();

    if (m_environment.IsTransacted())
    {
        XmlTransaction xmlTxn = xmlMan.createTransaction();

        m_xmlContainer->deleteIndex(
            xmlTxn, uri, name, index, updateContext);
        xmlTxn.commit(0);
    }
    else
    {
        m_xmlContainer->deleteIndex(
            uri, name, index, updateContext);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceContainer.DeleteIndex")
}
