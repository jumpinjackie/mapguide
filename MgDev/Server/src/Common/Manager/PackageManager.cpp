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
#include "ServerManager.h"
#include "ServerResourceService.h"
#include "PackageManager.h"

#define MGPACKAGEMANAGERLOGBUFFERSIZE 1024

// Process-wide MgPackageManager
Ptr<MgPackageManager> MgPackageManager::m_packageManager = (MgPackageManager*)NULL;

//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------
MgPackageManager::MgPackageManager(void)
{
}

//----------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------
MgPackageManager::~MgPackageManager(void)
{
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%P|%t) MgPackageManager::~MgPackageManager()\n")));
}

void MgPackageManager::Dispose(void)
{
    delete this;
}

//----------------------------------------------------------------------------
// Get pointer to a process-wide MgPackageManager.
//----------------------------------------------------------------------------
MgPackageManager* MgPackageManager::GetInstance()
{
    MG_TRY()

    ACE_TRACE ("MgPackageManager::GetInstance");

    if (MgPackageManager::m_packageManager == NULL)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance (), 0));
        if (MgPackageManager::m_packageManager == NULL)
        {
            MgPackageManager::m_packageManager = new MgPackageManager;
        }
    }

    MG_CATCH_AND_THROW(L"MgPackageManager.GetInstance")

    // To avoid overheads and maintain thread safety,
    // do not assign this returned static singleton to a Ptr object.
    return MgPackageManager::m_packageManager;
}

//----------------------------------------------------------------------------
// List the packages that are in the package directory
//----------------------------------------------------------------------------
MgStringCollection* MgPackageManager::EnumeratePackages()
{
    STRING path;
    Ptr<MgStringCollection> packages;
    ACE_DIR* directory = NULL;

    packages = new MgStringCollection();
    if (packages == NULL)
    {
        throw new MgOutOfMemoryException(L"MgPackageManager.EnumeratePackages", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_TRY()

    path = GetPackagesPath();

    // If the pathname doesn't exit, throw an exception
    if (!MgFileUtil::PathnameExists(path))
    {
        MgStringCollection arguments;
        arguments.Add(path);
        throw new MgFileNotFoundException(L"MgPackageManager.EnumeratePackages", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    directory = ACE_OS::opendir(ACE_TEXT_WCHAR_TO_TCHAR(path.c_str()));
    if (directory == NULL)
    {
        MgStringCollection arguments;
        arguments.Add(path);
        throw new MgFileIoException(L"MgPackageManager.EnumeratePackages", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // Open the logs directory
    dirent* direntry = NULL;

    STRING wname;
    STRING temppath;
    STRING tempName;
    int statResult;
#ifdef _WIN32
    struct _stat statBuf;
#else
    struct stat statBuf;
#endif

    // Go through the directory entries
    while ((direntry = ACE_OS::readdir(directory)) != NULL)
    {
        wname = MG_TCHAR_TO_WCHAR(direntry->d_name);

        // Check if the directory entry ends with the package extension
        if (MgFileUtil::EndsWithExtension(wname, MgFileExtensions::Package))
        {
            temppath = path + wname;

#ifdef _WIN32
            statResult = ::_wstat(temppath.c_str(), &statBuf);
#else
            statResult = ::stat(MgUtil::WideCharToMultiByte(temppath).c_str(),
                &statBuf);
#endif
            // Add to list of packages if it is not a folder
            if (statResult == 0 && (statBuf.st_mode & S_IFREG))
            {
                packages->Add(wname);
            }
        }
    }

    ACE_OS::closedir(directory);

    MG_CATCH(L"MgPackageManager.EnumeratePackages");
    if (mgException != NULL)
    {
        if (directory != NULL)
        {
            ACE_OS::closedir(directory);
        }
        (*mgException).AddRef();
        mgException->Raise();
    }

    return SAFE_ADDREF((MgStringCollection*)packages);
}

//----------------------------------------------------------------------------
// Load the package
//----------------------------------------------------------------------------
void MgPackageManager::LoadPackage(CREFSTRING packageName)
{
    MG_TRY()

    MgServerManager* serverManager = MgServerManager::GetInstance();
    assert(NULL != serverManager);

    if (!serverManager->IsSiteServer())
    {
        throw new MgLogicException(L"MgPackageManager.LoadPackage",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (packageName.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgPackageManager.LoadPackage",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    if (packageName.find(L"\\") != string::npos ||
        packageName.find(L"/") != string::npos)
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(packageName);

        throw new MgInvalidArgumentException(L"MgPackageManager.LoadPackage",
            __LINE__, __WFILE__, &arguments, L"MgStringContainsReservedCharacters", NULL);
    }

    // Check that package exists
    STRING packagePath = GetPackagesPath() + packageName;

    if (!MgFileUtil::PathnameExists(packagePath))
    {
        MgStringCollection arguments;
        arguments.Add(packagePath);
        throw new MgFileNotFoundException(L"MgPackageManager.LoadPackage", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // Obtain the resource service
    MgServiceManager* serviceManager = MgServiceManager::GetInstance();
    assert(NULL != serviceManager);
    Ptr<MgServerResourceService> resourceService = dynamic_cast<MgServerResourceService*>(
        serviceManager->RequestService(MgServiceType::ResourceService));
    assert(resourceService != NULL);

    resourceService->LoadResourcePackage(packagePath, true);

    MG_CATCH_AND_THROW(L"MgPackageManager.LoadPackage")
}

//----------------------------------------------------------------------------
// Delete the package and its associated status log
//----------------------------------------------------------------------------
void MgPackageManager::DeletePackage(CREFSTRING packageName)
{
    // Ensure that the packageName does not include any path seperation characters which
    // might allow the user to delete something that they shouldn't
    if (packageName.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgPackageManager.DeletePackage",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    if (packageName.find(L"\\") != string::npos ||
        packageName.find(L"/") != string::npos)
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(packageName);

        throw new MgInvalidArgumentException(L"MgPackageManager.DeletePackage",
            __LINE__, __WFILE__, &arguments, L"MgStringContainsReservedCharacters", NULL);
    }

    STRING path;
    STRING pathname;

    MG_TRY()

    path = GetPackagesPath();

    // Delete the package
    pathname = path + packageName;
    MgFileUtil::DeleteFile(pathname);

    // Delete the log
    pathname = path + packageName + MgFileExtensions::Log;
    MgFileUtil::DeleteFile(pathname);

    MG_CATCH_AND_THROW(L"MgPackageManager.DeletePackage");
}

//----------------------------------------------------------------------------
// Retrieve the status of the package
//----------------------------------------------------------------------------
STRING MgPackageManager::GetPackageStatus(CREFSTRING packageName)
{
    if (packageName.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgPackageManager.GetPackageStatus",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    if (packageName.find(L"\\") != string::npos ||
        packageName.find(L"/") != string::npos)
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(packageName);

        throw new MgInvalidArgumentException(L"MgPackageManager.GetPackageStatus",
            __LINE__, __WFILE__, &arguments, L"MgStringContainsReservedCharacters", NULL);
    }

    STRING status = MgPackageStatus::Unknown;

    MG_TRY()

    STRING path;
    STRING packagePath;
    STRING logPath;

    path = GetPackagesPath();
    packagePath = path + packageName;
    logPath = path + packageName + MgFileExtensions::Log;

    // Package can only have a status if it exists
    if (MgFileUtil::PathnameExists(packagePath))
    {
        // If a log file doesn't exist for the package, assume it has not been loaded yet
        if (!MgFileUtil::PathnameExists(logPath))
        {
            status = MgPackageStatus::NotStarted;
        }
        // If a lot file does exist, the status should be recorded within
        else
        {
            Ptr<MgStringCollection> pCollection = GetInformationFromLog(logPath);

            // The three pieces of information used to varify the log are not there.
            // Set the status to not started and attempt to delete the log.
            if (pCollection->GetCount() != 3)
            {
                status = MgPackageStatus::NotStarted;
                MgFileUtil::DeleteFile(logPath);
            }
            else
            {
                STRING recordedStatus = pCollection->GetItem(0);
                STRING recordedModDate = pCollection->GetItem(1);
                INT32 recordedSize = ACE_OS::atoi((pCollection->GetItem(2)).c_str());
                INT32 packageSize = MgFileUtil::GetFileSizeInBytes(packagePath);

                // Log information does not match the package information
                // Set the status to not started and attempt to delete the log.
                if (recordedModDate != MgFileUtil::GetLastModifiedDate(packagePath) ||
                    recordedSize != packageSize)
                {
                    status = MgPackageStatus::NotStarted;
                    MgFileUtil::DeleteFile(logPath);
                }
                // Check for valid status
                else if (recordedStatus == MgPackageStatus::Success || recordedStatus == MgPackageStatus::Failed
                    || recordedStatus == MgPackageStatus::Loading || recordedStatus == MgPackageStatus::NotStarted)
                {
                    status = recordedStatus;
                }
            }
        }
    }

    // If something went wrong with the process, exception will be ignored and
    // last determined status will be returned.
    MG_CATCH(L"MgPackageManager.GetPackageStatus");

    return status;
}

//----------------------------------------------------------------------------
// Retrieve the contents of the package log
//----------------------------------------------------------------------------
MgByteReader* MgPackageManager::GetPackageLog(CREFSTRING packageName)
{
    if (packageName.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgPackageManager.GetPackageLog",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }
    if (packageName.find(L"\\") != string::npos ||
        packageName.find(L"/") != string::npos)
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(packageName);

        throw new MgInvalidArgumentException(L"MgPackageManager.GetPackageLog",
            __LINE__, __WFILE__, &arguments, L"MgStringContainsReservedCharacters", NULL);
    }

    Ptr<MgByteReader> byteReader;
    byteReader = NULL;

    MG_TRY()

    STRING path;
    STRING logPath;
    STRING packagePath;

    path = GetPackagesPath();
    packagePath = path + packageName;
    logPath = path + packageName + MgFileExtensions::Log;

    if (!MgFileUtil::PathnameExists(logPath))
    {
        MgStringCollection arguments;
        arguments.Add(logPath);
        throw new MgFileNotFoundException(L"MgPackageManager.GetPackageLog", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    Ptr<MgStringCollection> pCollection = GetInformationFromLog(logPath);
    if (pCollection->GetCount() != 3)
    {
        MgFileUtil::DeleteFile(logPath);
        MgStringCollection arguments;
        arguments.Add(logPath);
        throw new MgFileNotFoundException(L"MgPackageManager.GetPackageLog", __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        STRING recordedModDate = pCollection->GetItem(1);
        INT32 recordedSize = ACE_OS::atoi((pCollection->GetItem(2)).c_str());
        INT32 packageSize = MgFileUtil::GetFileSizeInBytes(packagePath);

        // Log information does not match the package information
        if (recordedModDate != MgFileUtil::GetLastModifiedDate(packagePath) ||
            recordedSize != packageSize)
        {
            MgFileUtil::DeleteFile(logPath);
            MgStringCollection arguments;
            arguments.Add(logPath);
            throw new MgFileNotFoundException(L"MgPackageManager.GetPackageLog", __LINE__, __WFILE__, &arguments, L"", NULL);
        }
    }

    Ptr<MgByteSource> byteSource = new MgByteSource(logPath);
    byteSource->SetMimeType(MgMimeType::Text);
    byteReader = byteSource->GetReader();

    MG_CATCH_AND_THROW(L"MgPackageManager.GetPackageLog");

    return SAFE_ADDREF((MgByteReader*)byteReader);
}

//----------------------------------------------------------------------------
// Retrieve the current resource packages path from the server configuration.
//----------------------------------------------------------------------------
STRING MgPackageManager::GetPackagesPath()
{
    STRING path;

    MG_TRY()

    MgConfiguration* pConfiguration = MgConfiguration::GetInstance();
    pConfiguration->GetStringValue(MgConfigProperties::ResourceServicePropertiesSection, MgConfigProperties::ResourceServicePropertyPackagesPath, path, MgConfigProperties::DefaultResourceServicePropertyPackagesPath);

    // Check if path ends with a '/' if not, add one if needed
    MgFileUtil::AppendSlashToEndOfPath(path);

    MG_CATCH_AND_THROW(L"MgPackageManager.GetPackagePath");

    return path.c_str();
}

// Returns a string collection containing the status, last modified date of the package
// and the size of the package.
MgStringCollection* MgPackageManager::GetInformationFromLog(CREFSTRING logPath)
{
    if (logPath.empty())
    {
        throw new MgNullArgumentException(L"MgPackageManager.GetInformationFromLog", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgStringCollection> pCollection;
    pCollection = new MgStringCollection();
    if (pCollection == NULL)
    {
        throw new MgOutOfMemoryException(L"MgPackageManager.GetInformationFromLog", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    FILE* file = NULL;

    MG_TRY()

    if (MgFileUtil::PathnameExists(logPath))
    {
        file = ACE_OS::fopen(MG_WCHAR_TO_TCHAR(logPath), ACE_TEXT("rb"));
        if (file == NULL)
        {
            MgStringCollection arguments;
            arguments.Add(logPath);
            throw new MgFileIoException(L"MgPackageManager.GetInformationFromLog", __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        char buffer[MGPACKAGEMANAGERLOGBUFFERSIZE];
        wchar_t* wBuffer;
        STRING str;

        INT32 newLineCount = 0;
        bool keepReading = true;
        while (keepReading)
        {
            memset(buffer, 0, MGPACKAGEMANAGERLOGBUFFERSIZE);
            ACE_OS::fread(buffer, sizeof(char), (MGPACKAGEMANAGERLOGBUFFERSIZE-1)*sizeof(char), file);
            wBuffer = MgUtil::MultiByteToWideChar(buffer);

            INT32 i;
            for (i = 0; i < (MGPACKAGEMANAGERLOGBUFFERSIZE-1); i++)
            {
                if (wBuffer[i] == L'\n')
                {
                    newLineCount++;
                    if (newLineCount == 7)
                    {
                        keepReading = false;
                        break;
                    }
                }
            }

            str += wBuffer;
            delete [] wBuffer;
        }

        // Close the file
        ACE_OS::fclose(file);
        file = NULL;

        // str now contains the first 7 lines of the file, we need to extract the info from there.
        Ptr<MgStringCollection> lines = MgStringCollection::ParseCollection(str, L"\n");

        if (lines->GetCount() >= 7)
        {
            STRING tmp;
            size_t pos;

            // Isolate status (located on second line) and add to collection
            tmp = lines->GetItem(1);
            pos = tmp.find_last_of(L" ");
#ifdef _WIN32
            tmp = tmp.substr((pos+1), (tmp.size()-pos-2));
#else
            tmp = tmp.substr((pos+1), (tmp.size()-pos-1));
#endif
            pCollection->Add(tmp);

            // Add last modified date of package to collection

#ifdef _WIN32
            tmp = lines->GetItem(5);
            tmp = tmp.substr(0, (tmp.size()-1));
#else
            tmp = lines->GetItem(4);
#endif
            pCollection->Add(tmp);

            // Add size of package to collection
#ifdef _WIN32
            tmp = lines->GetItem(6);
            tmp = tmp.substr(0, (tmp.size()-1));
#else
            tmp = lines->GetItem(5);
#endif
            pCollection->Add(tmp);
        }
    }

    MG_CATCH(L"MgPackageManager.GetInformationFromLog")
    if (mgException != NULL)
    {
        if (file != NULL)
        {
            ACE_OS::fclose(file);
        }
        (*mgException).AddRef();
        mgException->Raise();
    }

    return SAFE_ADDREF((MgStringCollection*)pCollection);
}
