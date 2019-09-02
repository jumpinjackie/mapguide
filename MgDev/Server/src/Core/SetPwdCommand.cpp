#include "SetPwdCommand.h"

MgSetPwdCommand::MgSetPwdCommand(CREFSTRING locale, CREFSTRING userName, CREFSTRING password)
    : MgServerInteractiveCommand()
{
    m_locale = locale;
    m_strUser = userName;
    m_strPassword = password;
}


MgSetPwdCommand::~MgSetPwdCommand()
{
}

INT32 MgSetPwdCommand::Execute()
{
    INT32 nResult = 0;
    try
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
        Ptr<MgServerSiteService> pService = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        if (NULL == (MgServerSiteService*)pService)
        {
            throw new MgServiceNotAvailableException(L"MgSetPwdCommand.Execute", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        if (m_strUser.empty()) //Need to have passed username
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT("No username specified.\n")));
            nResult = -1;
        }
        else if (m_strPassword.empty()) //Need to have passed pwd
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT("No password specified.\n")));
            nResult = -1;
        }
        else
        {
            //Anonymous has no password, so there's nothing to set
            if (m_strUser == MgUser::Anonymous)
            {
                ACE_DEBUG((LM_INFO, ACE_TEXT("Cannot change password for Anonymous.\n")));
                nResult = -1;
            }
            else
            {
                ACE_DEBUG((LM_INFO, ACE_TEXT("Changing password for user: %W...\n\n"), m_strUser.c_str()));
                pService->UpdateUser(m_strUser, L"", L"", m_strPassword, L"");
                ACE_DEBUG((LM_INFO, ACE_TEXT("Password changed.\n")));
            }
        }
    }
    catch (MgException* e)
    {
        ACE_DEBUG((LM_ERROR, ACE_TEXT("Unable to set the password for the specified user.\n")));
        ACE_DEBUG((LM_ERROR, ACE_TEXT("%W\n"), e->GetStackTrace(m_locale).c_str()));
        SAFE_RELEASE(e);

        nResult = -1;
    }
    catch (...)
    {
        ACE_DEBUG((LM_ERROR, ACE_TEXT("Unable to set the password for the specified user.\n")));

        nResult = -1;
    }

    return nResult;
}
