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
    MgDatabase(environment)
{
    assert(!fileName.empty());

    MG_RESOURCE_SERVICE_TRY()

#ifdef _DEBUG
    // These generate a lot of noise in DEBUG mode, so turn them on only when needed.
//    DbXml::setLogLevel(DbXml::LEVEL_ALL, true);
//    DbXml::setLogCategory(DbXml::CATEGORY_ALL, true);
#endif

    XmlManager& xmlMan = m_environment.GetXmlManager();
    assert(XmlContainer::WholedocContainer == xmlMan.getDefaultContainerType());
    u_int32_t flags = xmlMan.getDefaultContainerFlags();

    if (m_environment.IsTransacted())
    {
        assert(flags | DBXML_TRANSACTIONAL);
        XmlTransaction xmlTxn = xmlMan.createTransaction();

        m_xmlContainer.reset(new XmlContainer(xmlMan.openContainer(
            xmlTxn, fileName, flags)));
        xmlTxn.commit(0);
    }
    else
    {
        m_xmlContainer.reset(new XmlContainer(xmlMan.openContainer(
            fileName, flags)));
    }

    m_opened = (NULL != m_xmlContainer.get());
    Reset();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceContainer.MgResourceContainer")
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgResourceContainer::~MgResourceContainer()
{
    Reset();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Return the name of the database.
///
string MgResourceContainer::GetName()
{
    string name;

    if (m_opened)
    {
        assert(NULL != m_xmlContainer.get());
        name = m_xmlContainer->getName();
    }

    return name;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Add an index specification.
///
void MgResourceContainer::AddIndex(const string& uri, const string& name,
    const string& index)
{
    assert(NULL != m_xmlContainer.get());

    MG_RESOURCE_SERVICE_TRY()

    XmlManager& xmlMan = m_xmlContainer->getManager();
    XmlUpdateContext updateContext = xmlMan.createUpdateContext();

    if (m_environment.IsTransacted())
    {
        XmlTransaction xmlTxn = xmlMan.createTransaction();

        m_xmlContainer->addIndex(
            xmlTxn, uri, name, index, updateContext);
        xmlTxn.commit(0);
    }
    else
    {
        m_xmlContainer->addIndex(
            uri, name, index, updateContext);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceContainer.AddIndex")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Delete an index specification.
///
void MgResourceContainer::DeleteIndex(const string& uri, const string& name,
    const string& index)
{
    assert(NULL != m_xmlContainer.get());

    MG_RESOURCE_SERVICE_TRY()

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
