#include "ResourceDefs.h"
#include "ResourcePackageHandler.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgdResourcePackageHandler::MgdResourcePackageHandler(
    MgResourceService& repositoryManager) :
    m_repositoryManager(repositoryManager),
    m_opsSucceeded(0),
    m_opsReceived(0)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgdResourcePackageHandler::~MgdResourcePackageHandler()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Initializes the package status information.
///
void MgdResourcePackageHandler::InitializeStatus(CREFSTRING packageApiName,
    CREFSTRING packagePathname, bool logActivities)
{
    m_packagePathname = packagePathname;

    /*
    // Create the package log writer.
    if (logActivities)
    {
        m_packageLogWriter = new MgPackageLogWriter(packageApiName,
            m_packagePathname);
    }

    if (m_packageLogWriter != NULL)
    {
        m_opsSucceeded = 0;
        m_opsReceived = 0;

        MgServerManager* serverManager = MgServerManager::GetInstance();
        ACE_ASSERT(NULL != serverManager && serverManager->IsSiteServer());

        MgPackageStatusInformation& statusInfo = m_packageLogWriter->GetStatusInfo();

        Ptr<MgDateTime> startTime = new MgDateTime();
        statusInfo.SetEndTime(startTime);

        Ptr<MgUserInformation> currUserInfo = m_repositoryManager.GetCurrentUserInfo();
        ACE_ASSERT(NULL != currUserInfo.p);

        if (NULL != currUserInfo)
        {
            statusInfo.SetUserName(currUserInfo->GetUserName());
        }

        STRING serverName = serverManager->GetServerName();
        STRING serverAddress = serverManager->GetLocalServerAddress();

        if (serverName.empty())
        {
            MgIpUtil::HostAddressToName(serverAddress, serverName, false);
        }

        statusInfo.SetServerName(serverName);
        statusInfo.SetServerAddress(serverAddress);

        // Write the log file.
        m_packageLogWriter->UpdateLog();
    }*/
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Updates the package status information.
///
void MgdResourcePackageHandler::UpdateStatus(MgException* except)
{
    /*
    if (m_packageLogWriter != NULL)
    {
        m_packageLogWriter->SetError(except);
        MgPackageStatusInformation& statusInfo = m_packageLogWriter->GetStatusInfo();

        Ptr<MgDateTime> modifiedTime = new MgDateTime(
            MgFileUtil::GetFileModificationTime(m_packagePathname));

        statusInfo.SetPackageDate(modifiedTime);
        statusInfo.SetPackageSize(MgFileUtil::GetFileSize(m_packagePathname));

        Ptr<MgDateTime> endTime = new MgDateTime();
        statusInfo.SetEndTime(endTime);

        statusInfo.SetOperationsFailed(m_opsReceived - m_opsSucceeded);
        statusInfo.SetOperationsReceived(m_opsReceived);

        // Write the log file.
        m_packageLogWriter->UpdateLog();
    }*/
}
