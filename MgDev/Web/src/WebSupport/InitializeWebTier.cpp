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

#include "AceCommon.h"
#include "Configuration.h"
#include "Resources.h"


void MgInitializeWebTierInternal(CREFSTRING configFile)
{
    MG_TRY()

    // Initialize only once per process
    static bool m_bInitialized = false;

    if (m_bInitialized) return;

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
        config->GetStringValue(MgConfigProperties::GeneralPropertiesSection, MgConfigProperties::GeneralPropertyDefaultLocale, locale, MgConfigProperties::DefaultGeneralPropertyDefaultLocale);

        resources->Initialize(resourcesPath);
        resources->LoadResources(locale);

    }

    m_bInitialized = true;

    MG_CATCH_AND_THROW(L"MgInitializeWebTierInternal")
}

void MgInitializeWebTier(CREFSTRING configFile)
{
    MG_TRY()

    ACE_MT (ACE_GUARD_ACTION(ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance (), MgInitializeWebTierInternal(configFile);, ;));

    MG_CATCH_AND_THROW(L"MgInitializeWebTier")
}

