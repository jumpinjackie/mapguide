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
    XmlContainer::ContainerType defaultContType = xmlMan.getDefaultContainerType();
    assert(XmlContainer::WholedocContainer == defaultContType);

    if (m_environment.IsTransacted())
    {
        XmlTransaction xmlTxn = xmlMan.createTransaction();

        XmlContainerConfig cconfig;
        cconfig.setAllowCreate(true); // If the container does not exist, create it.
        cconfig.setTransactional(true); // Enable transactions.
        cconfig.setThreaded(true);
        cconfig.setContainerType(defaultContType);
        cconfig.setAllowValidation(true);
        cconfig.setIndexNodes(XmlContainerConfig::Off);
        
        m_xmlContainer.reset(new XmlContainer(xmlMan.openContainer(xmlTxn, fileName, cconfig)));
        xmlTxn.commit(0);

        XmlContainer::ContainerType contType = m_xmlContainer->getContainerType();
        assert(XmlContainer::WholedocContainer == contType);
    }
    else
    {
        XmlContainerConfig cconfig;
        cconfig.setAllowCreate(true); // If the container does not exist, create it.
        cconfig.setThreaded(true);
        cconfig.setContainerType(defaultContType);
        cconfig.setAllowValidation(true);
        cconfig.setIndexNodes(XmlContainerConfig::Off);

        m_xmlContainer.reset(new XmlContainer(xmlMan.openContainer(fileName, cconfig)));

        XmlContainer::ContainerType contType = m_xmlContainer->getContainerType();
        assert(XmlContainer::WholedocContainer == contType);
    }

#ifdef _DEBUG
    // Dump the flags used to create the container
    STRING containerFileName = MgUtil::MultiByteToWideChar(fileName);
    ACE_DEBUG ((LM_INFO, ACE_TEXT("Opened Container        : %s\n"), MG_WCHAR_TO_TCHAR(containerFileName)));

    bool bResult;
    int nResult;
    unsigned int unResult;
    std::string strResult;
    XmlContainer::ContainerType cResult;
    XmlContainerConfig::ConfigState sResult;

    XmlContainerConfig cconfig = m_xmlContainer->getFlags();
    bResult = cconfig.getAllowCreate();
    ACE_DEBUG ((LM_INFO, ACE_TEXT("getAllowCreate          : %s\n"), bResult ? ACE_TEXT("True") : ACE_TEXT("False")));
    bResult = cconfig.getAllowValidation();
    ACE_DEBUG ((LM_INFO, ACE_TEXT("getAllowValidation      : %s\n"), bResult ? ACE_TEXT("True") : ACE_TEXT("False")));
    bResult = cconfig.getChecksum();
    ACE_DEBUG ((LM_INFO, ACE_TEXT("getChecksum             : %s\n"), bResult ? ACE_TEXT("True") : ACE_TEXT("False")));
    strResult = cconfig.getCompressionName();
    STRING compressionName = MgUtil::MultiByteToWideChar(strResult);
    ACE_DEBUG ((LM_INFO, ACE_TEXT("getCompressionName      : %s\n"), MG_WCHAR_TO_TCHAR(compressionName)));
    cResult = cconfig.getContainerType();
    ACE_DEBUG ((LM_INFO, ACE_TEXT("getContainerType        : %s\n"), (cResult == XmlContainer::WholedocContainer) ? ACE_TEXT("WholedocContainer") : ACE_TEXT("NodeContainer")));
    bResult = cconfig.getEncrypted();
    ACE_DEBUG ((LM_INFO, ACE_TEXT("getEncrypted            : %s\n"), bResult ? ACE_TEXT("True") : ACE_TEXT("False")));
    bResult = cconfig.getExclusiveCreate();
    ACE_DEBUG ((LM_INFO, ACE_TEXT("getExclusiveCreate      : %s\n"), bResult ? ACE_TEXT("True") : ACE_TEXT("False")));
    sResult = cconfig.getIndexNodes();
    ACE_DEBUG ((LM_INFO, ACE_TEXT("getIndexNodes           : %d\n"), sResult));
    nResult = cconfig.getMode();
    ACE_DEBUG ((LM_INFO, ACE_TEXT("getMode                 : %d\n"), nResult));
    bResult = cconfig.getMultiversion();
    ACE_DEBUG ((LM_INFO, ACE_TEXT("getMultiversion         : %s\n"), bResult ? ACE_TEXT("True") : ACE_TEXT("False")));
    bResult = cconfig.getNoMMap();
    ACE_DEBUG ((LM_INFO, ACE_TEXT("getNoMMap               : %s\n"), bResult ? ACE_TEXT("True") : ACE_TEXT("False")));
    unResult = cconfig.getPageSize();
    ACE_DEBUG ((LM_INFO, ACE_TEXT("getPageSize             : %d\n"), unResult));
    bResult = cconfig.getReadOnly();
    ACE_DEBUG ((LM_INFO, ACE_TEXT("getReadOnly             : %s\n"), bResult ? ACE_TEXT("True") : ACE_TEXT("False")));
    bResult = cconfig.getReadUncommitted();
    ACE_DEBUG ((LM_INFO, ACE_TEXT("getReadUncommitted      : %s\n"), bResult ? ACE_TEXT("True") : ACE_TEXT("False")));
    unResult = cconfig.getSequenceIncrement();
    ACE_DEBUG ((LM_INFO, ACE_TEXT("getSequenceIncrement    : %d\n"), unResult));
    sResult = cconfig.getStatistics();
    ACE_DEBUG ((LM_INFO, ACE_TEXT("getStatistics           : %d\n"), sResult));
    bResult = cconfig.getThreaded();
    ACE_DEBUG ((LM_INFO, ACE_TEXT("getThreaded             : %s\n"), bResult ? ACE_TEXT("True") : ACE_TEXT("False")));
    bResult = cconfig.getTransactional();
    ACE_DEBUG ((LM_INFO, ACE_TEXT("getTransactional        : %s\n"), bResult ? ACE_TEXT("True") : ACE_TEXT("False")));
    bResult = cconfig.getTransactionNotDurable();
    ACE_DEBUG ((LM_INFO, ACE_TEXT("getTransactionNotDurable: %s\n"), bResult ? ACE_TEXT("True") : ACE_TEXT("False")));
    ACE_DEBUG ((LM_INFO, ACE_TEXT("\n")));
#endif

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
