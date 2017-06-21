#include "LoadPackageCommand.h"

MgLoadPackageCommand::MgLoadPackageCommand(CREFSTRING locale, CREFSTRING path)
    : MgServerInteractiveCommand()
{
    m_locale = locale;
    m_path = path;
}

MgLoadPackageCommand::~MgLoadPackageCommand()
{

}

INT32 MgLoadPackageCommand::Execute()
{
    INT32 nResult = 0;
    try
    {
        if (MgFileUtil::IsFile(m_path))
        {
            // Let the site manager know that the check servers background thread needs to stop
            MgSiteManager* siteManager = MgSiteManager::GetInstance();
            siteManager->StopCheckServersThread();

            // Set the user information for the current thread to be administrator.
            //
            // This is server-local, so no authentication is done. The MgServiceManager just needs to know the user when
            // services are requested.
            Ptr<MgUserInformation> adminUserInfo = new MgUserInformation(MgUser::Administrator, L"");
            MgUserInformation::SetCurrentUserInfo(adminUserInfo);

            MgServiceManager* serviceManager = MgServiceManager::GetInstance();
            Ptr<MgResourceService> resSvc = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
            if (NULL == (MgResourceService*)resSvc)
            {
                throw new MgServiceNotAvailableException(L"MgLoadPackageCommand.Execute", __LINE__, __WFILE__, NULL, L"", NULL);
            }

            Ptr<MgByteSource> bs = new MgByteSource(m_path);
            Ptr<MgByteReader> br = bs->GetReader();

            ACE_DEBUG((LM_INFO, ACE_TEXT("Loading package file: %W...\n\n"), m_path.c_str()));
            resSvc->ApplyResourcePackage(br);
            ACE_DEBUG((LM_INFO, ACE_TEXT("Package loaded.\n")));
        }
        else
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT("Package file not found: %W\n\n"), m_path.c_str()));

            nResult = -1;
        }
    }
    catch (MgException* e)
    {
        ACE_DEBUG((LM_ERROR, ACE_TEXT("Unable to load the specified package.\n")));
        ACE_DEBUG((LM_ERROR, ACE_TEXT("%W\n"), e->GetStackTrace(m_locale).c_str()));
        SAFE_RELEASE(e);

        nResult = -1;
    }
    catch (...)
    {
        ACE_DEBUG((LM_ERROR, ACE_TEXT("Unable to load the specified package.\n")));

        nResult = -1;
    }

    return nResult;
}
