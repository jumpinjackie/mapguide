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
#include "ServerManager.h"
#include "ServerResourceService.h"
#include "PackageManager.h"
#include "PackageLogReader.h"

// Process-wide MgPackageManager
Ptr<MgPackageManager> MgPackageManager::sm_packageManager = (MgPackageManager*)NULL;

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgPackageManager::MgPackageManager()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgPackageManager::~MgPackageManager()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgPackageManager::~MgPackageManager()\n")));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Disposes the object.
///
void MgPackageManager::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the pointer to a process-wide MgPackageManager.
///
MgPackageManager* MgPackageManager::GetInstance()
{
    MG_TRY()

    ACE_TRACE("MgPackageManager::GetInstance");

    if (MgPackageManager::sm_packageManager == NULL)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance(), NULL));

        if (MgPackageManager::sm_packageManager == NULL)
        {
            MgPackageManager::sm_packageManager = new MgPackageManager;
        }
    }

    MG_CATCH_AND_THROW(L"MgPackageManager.GetInstance")

    // To avoid overheads and maintain thread safety,
    // do not assign this returned static singleton to a Ptr object.
    return MgPackageManager::sm_packageManager;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the resource package path from the server configuration.
///
STRING MgPackageManager::GetPackagePath()
{
    STRING packagePath;

    MgConfiguration* configuration = MgConfiguration::GetInstance();
    ACE_ASSERT(NULL != configuration);

    if (NULL == configuration)
    {
        throw new MgNullReferenceException(
            L"MgPackageManager.GetPackagePath",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    configuration->GetStringValue(
        MgConfigProperties::ResourceServicePropertiesSection,
        MgConfigProperties::ResourceServicePropertyPackagesPath,
        packagePath,
        MgConfigProperties::DefaultResourceServicePropertyPackagesPath);

    MgFileUtil::AppendSlashToEndOfPath(packagePath);

    return packagePath;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the name of the specified package.
///
STRING MgPackageManager::GetPackageName(CREFSTRING packagePathname)
{
    if (packagePathname.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(
            L"MgPackageManager.GetPackageName",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    STRING packageName;
    size_t index = packagePathname.find_last_of(L"\\/");

    if (STRING::npos != index)
    {
        packageName = packagePathname.substr(index + 1,
            packagePathname.length());
    }

    if (packageName.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(packagePathname);

        throw new MgInvalidArgumentException(
            L"MgPackageManager.GetPackageName",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    return packageName;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the pathname of the specified package.
///
STRING MgPackageManager::GetPackagePathname(CREFSTRING packageName)
{
    MgFileUtil::ValidateFileName(packageName);

    STRING packagePathname = GetPackagePath();

    packagePathname += packageName;

    return packagePathname;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the log pathname for the specified package.
///
STRING MgPackageManager::GetLogPathname(CREFSTRING packagePathname)
{
    // If package is "blah.mgp", then log is "blah.mgp.log".
    STRING logPathname = packagePathname + MgFileExtension::Log;

    return logPathname;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Enumerates a list the packages that are in the package directory.
///
MgStringCollection* MgPackageManager::EnumeratePackages()
{
    Ptr<MgStringCollection> packages;
    ACE_DIR* directory = NULL;

    MG_TRY()

    STRING packagePath = GetPackagePath();

    if (!MgFileUtil::PathnameExists(packagePath))
    {
        MgStringCollection arguments;
        arguments.Add(packagePath);

        throw new MgDirectoryNotFoundException(
            L"MgPackageManager.EnumeratePackages",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // Open the packages directory.
    directory = ACE_OS::opendir(ACE_TEXT_WCHAR_TO_TCHAR(packagePath.c_str()));

    if (NULL == directory)
    {
        MgStringCollection arguments;
        arguments.Add(packagePath);

        throw new MgFileIoException(
            L"MgPackageManager.EnumeratePackages",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    packages = new MgStringCollection();
    dirent* direntry = NULL;

    // Go through the directory entries.
    while ((direntry = ACE_OS::readdir(directory)) != NULL)
    {
        STRING packageName = MG_TCHAR_TO_WCHAR(direntry->d_name);

        // Check if the directory entry ends with the package extension.
        if (MgFileUtil::EndsWithExtension(packageName, MgFileExtension::Mgp))
        {
            STRING packagePathname = packagePath + packageName;

            // Add to list of packages if it is a file.
            if (MgFileUtil::IsFile(packagePathname))
            {
                packages->Add(packageName);
            }
        }
    }

    MG_CATCH(L"MgPackageManager.EnumeratePackages");

    if (NULL != directory)
    {
        ACE_OS::closedir(directory);
    }

    MG_THROW()

    return packages.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Loads the specified package into a resource repository.
///
void MgPackageManager::LoadPackage(CREFSTRING packageName)
{
    MG_TRY()

    STRING packagePathname = GetPackagePathname(packageName);
    MgServiceManager* serviceManager = MgServiceManager::GetInstance();
    ACE_ASSERT(NULL != serviceManager);
    Ptr<MgService> service = serviceManager->RequestService(MgServiceType::ResourceService);
    MgServerResourceService* resourceService = dynamic_cast<MgServerResourceService*>(service.p);

    if (NULL == resourceService)
    {
        throw new MgServiceNotAvailableException(
            L"MgPackageManager.LoadPackage",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }
    else
    {
        resourceService->LoadResourcePackage(packagePathname, true);
    }

    MG_CATCH_AND_THROW(L"MgPackageManager.LoadPackage")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Creates a package from the specified resource, and then saves it into
/// the specified name.
///
void MgPackageManager::MakePackage(MgResourceIdentifier* resource,
    CREFSTRING packageName, CREFSTRING packageDescription)
{
    MG_TRY()

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.ApplyResourcePackage",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    STRING packagePathname = GetPackagePathname(packageName);

    if (!MgFileUtil::EndsWithExtension(packagePathname, MgFileExtension::Mgp))
    {
        packagePathname += MgFileExtension::Mgp;
    }

    MgServiceManager* serviceManager = MgServiceManager::GetInstance();
    ACE_ASSERT(NULL != serviceManager);
    Ptr<MgService> service = serviceManager->RequestService(MgServiceType::ResourceService);
    MgServerResourceService* resourceService = dynamic_cast<MgServerResourceService*>(service.p);

    if (NULL == resourceService)
    {
        throw new MgServiceNotAvailableException(
            L"MgPackageManager.MakePackage",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }
    else
    {
        resourceService->MakeResourcePackage(resource, packagePathname,
            packageDescription, true);
    }

    MG_CATCH_AND_THROW(L"MgPackageManager.MakePackage")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Deletes the specified package and its associated status log file.
///
void MgPackageManager::DeletePackage(CREFSTRING packageName)
{
    MG_TRY()

    STRING packagePathname = GetPackagePathname(packageName);

    MgFileUtil::DeleteFile(packagePathname);

    STRING logPathname = GetLogPathname(packagePathname);

    MgFileUtil::DeleteFile(logPathname);

    MG_CATCH_AND_THROW(L"MgPackageManager.DeletePackage");
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the current status of the specified package.
///
MgPackageStatusInformation* MgPackageManager::GetPackageStatus(CREFSTRING packageName)
{
    Ptr<MgPackageStatusInformation> statusInfo;

    MG_TRY()

    STRING packagePathname = GetPackagePathname(packageName);
    MgPackageLogReader logReader(packagePathname);

    statusInfo = logReader.GetStatus();

    MG_CATCH_AND_THROW(L"MgPackageManager.GetPackageStatus");

    return statusInfo.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the current log of the specified package.
///
MgByteReader* MgPackageManager::GetPackageLog(CREFSTRING packageName)
{
    Ptr<MgByteReader> byteReader;

    MG_TRY()

    STRING packagePathname = GetPackagePathname(packageName);
    MgPackageLogReader logReader(packagePathname);

    byteReader = logReader.GetLog();

    MG_CATCH_AND_THROW(L"MgPackageManager.GetPackageLog");

    return byteReader.Detach();
}
