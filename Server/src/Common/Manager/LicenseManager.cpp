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

#include "MapGuideCommon.h"
#include "LicenseManager.h"

// Process-wide MgLicenseManager
Ptr<MgLicenseManager> MgLicenseManager::sm_licenseManager = (MgLicenseManager*)NULL;

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgLicenseManager::MgLicenseManager()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgLicenseManager::~MgLicenseManager()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Disposes the object.
///
void MgLicenseManager::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the pointer to a process-wide MgLicenseManager.
///
MgLicenseManager* MgLicenseManager::GetInstance()
{
    MG_TRY()

    ACE_TRACE("MgLicenseManager::GetInstance");

    if (MgLicenseManager::sm_licenseManager == NULL)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance(), NULL));

        if (MgLicenseManager::sm_licenseManager == NULL)
        {
            MgLicenseManager::sm_licenseManager = new MgLicenseManager();
        }
    }

    MG_CATCH_AND_THROW(L"MgLicenseManager.GetInstance")

    // To avoid overheads and maintain thread safety,
    // do not assign this returned static singleton to a Ptr object.
    return MgLicenseManager::sm_licenseManager;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Initializes the license manager.
/// This method must be called once during the server startup time.
///
void MgLicenseManager::Initialize()
{
    // Start your license management here.
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Cleans up the license manager.
/// This method must be called once during the server shutdown time.
///
void MgLicenseManager::Terminate()
{
    // End your license management here.
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Checks whether or not the license is valid.
///
void MgLicenseManager::CheckLicense()
{
    // Perform your license validation here.
}
