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

#include "MapGuideCommon.h"
#include "Services/ServerConnectionPool.h"


// Initialize only once per process
static bool m_bInitialized = false;


void MgInitializeWebTierInternal(CREFSTRING configFile)
{
    MG_TRY()

    if (m_bInitialized)
        return;

    // Initialize ACE, this should enable sockets as well
    ACE::init();

    // Load configuration file
    MgConfiguration* config = MgConfiguration::GetInstance();

    config->LoadConfiguration(configFile);

    // Initialize resource path as well
    MgResources* resources = MgResources::GetInstance();

    if (resources != NULL && config != NULL)
    {
        STRING resourcesPath;
        config->GetStringValue(MgConfigProperties::GeneralPropertiesSection, MgConfigProperties::GeneralPropertyResourcesPath, resourcesPath, MgConfigProperties::DefaultGeneralPropertyResourcesPath);

        STRING locale;
        config->GetStringValue(MgConfigProperties::GeneralPropertiesSection, MgConfigProperties::GeneralPropertyDefaultMessageLocale, locale, MgConfigProperties::DefaultGeneralPropertyDefaultMessageLocale);

        resources->Initialize(resourcesPath);
        resources->LoadResources(locale);

        // Get the logs path
        STRING path = L"";
        config->GetStringValue(MgConfigProperties::GeneralPropertiesSection, MgConfigProperties::GeneralPropertyLogsPath, path, MgConfigProperties::DefaultGeneralPropertyLogsPath);

        // Check if path ends with a '/' if not, add one if needed
        MgFileUtil::AppendSlashToEndOfPath(path);

        // Try and create the directory. If it already exists this will error and not a problem
        ACE_OS::mkdir(MG_WCHAR_TO_TCHAR(path));
    }

    m_bInitialized = true;

    MG_CATCH_AND_THROW(L"MgInitializeWebTierInternal")
}


void MgUninitializeWebTierInternal()
{
    MG_TRY()

    if (!m_bInitialized)
        return;

    // Close all open connections before we drop ACE
    MgServerConnectionPool::CloseConnections();

    // Uninitialize ACE
    ACE::fini();

    m_bInitialized = false;

    MG_CATCH_AND_THROW(L"MgUninitializeWebTierInternal")
}


void MgInitializeWebTier(CREFSTRING configFile)
{
    MG_TRY()

    ACE_MT (ACE_GUARD_ACTION(ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance(), MgInitializeWebTierInternal(configFile);, ;));

    MG_CATCH_AND_THROW(L"MgInitializeWebTier")
}


void MgUninitializeWebTier()
{
    // ACE::fini removes the thread mutex so we cannot use it to protect this call.
    MG_TRY()

    MgUninitializeWebTierInternal();

    MG_CATCH_AND_THROW(L"MgInitializeWebTier")
}

bool IsWebTierInitialized()
{
    return m_bInitialized;
}

