//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

#include "Foundation.h"

// Process-wide MgResources
Ptr<MgResources> MgResources::m_resources = (MgResources*)NULL;

const STRING MgResources::DefaultMessageLocale       = L"en";           // Do not translate (ISO 639-1)
const STRING MgResources::ResourceFilenameExtension  = L".res";         // Do not translate
const STRING MgResources::ResourceFilenameUnderscore = L"_";            // Do not translate
const STRING MgResources::ResourceComponent          = L"mapguide";     // Do not translate

// The following resouce messages are not in the resource file because there may be the case where the resource file failed to load.
const STRING MgResources::FailedToLoadResourcesFile     = L"Failed to load the resource file \"%1\" for locale \"%2\".";
const STRING MgResources::FailedToLoadStartupFile       = L"Startup file %1 cannot be found or cannot be opened.";
const STRING MgResources::FailedToLoadConfigurationFile = L"Configuration file \"%1\" cannot be found or cannot be opened.";
const STRING MgResources::FailedToRetrieveMessage       = L"Failed to retrieve message for \"%1\".";
const STRING MgResources::FormatExceptionLocationMethods = L"- ";
const STRING MgResources::FormatExceptionLocationLines  = L" line ";
const STRING MgResources::FormatExceptionLocationFiles  = L" file ";
const STRING MgResources::MentorDictionaryPath          = L"%1 (using path: %2)";
const STRING MgResources::ServerStarting                = L"Server starting...";
const STRING MgResources::ServerShuttingDown            = L"Server shutting down...";
const STRING MgResources::ServerStarted                 = L"Server started.";
const STRING MgResources::ServerStopped                 = L"Server stopped.";
const STRING MgResources::ServerServiceName             = L"MapGuideServer3.1"; // Do not translate
const STRING MgResources::ServerServiceDisplayName      = L"MapGuide Server v3.1"; // Do not translate
const STRING MgResources::ServerServiceDescription      = L"The MapGuide Server process.";
const STRING MgResources::UsingDefaultResourceFile      = L"Defaulting to the \"en\" resource file.";
const STRING MgResources::RegisteredLicenseDetected     = L"Registered license detected...";
const STRING MgResources::EvaluationLicenseDetected     = L"Evaluation license detected.  Evaluation will expire in: %1 day(s).";
const STRING MgResources::AlphaReleaseDetected          = L"Alpha release detected.  Alpha version will expire in: %1 day(s).";
const STRING MgResources::BetaReleaseDetected           = L"Beta release detected.  Beta version will expire in: %1 day(s).";
const STRING MgResources::PreviewReleaseDetected        = L"Preview release detected.  Preview version will expire in: %1 day(s).";
const STRING MgResources::InvalidLicense                = L"You have an invalid license.";
const STRING MgResources::LicenseAlreadyExists          = L"Cannot create a new license, one already exists.";
const STRING MgResources::LicenseCheckoutCancel         = L"The license checkout was canceled by the user.";
const STRING MgResources::LicenseInvalidSerialNumber    = L"Invalid serial number.";
const STRING MgResources::LicenseCheckoutError          = L"Generic license checkout error.";
const STRING MgResources::LicenseNotFound               = L"The license was not found.";
const STRING MgResources::PreCacheMapsStart             = L"Precaching the following Maps:";



// The following resource strings are for the commandline options for the server. These are not in the resources file because
// the server has not even started at this point.
const STRING MgResources::ServerCmdDaemon               = L"daemon"; // Do not translate
const STRING MgResources::ServerCmdDaemonInfo           = L"Running the server as a daemon (service). \n\n";
const STRING MgResources::ServerCmdDaemonDescription    = L"  daemon\n"\
                                                          L"      Runs the server in the background as a daemon. \n\n";
const STRING MgResources::ServerCmdHelp                 = L"help"; // Do not translate
const STRING MgResources::ServerCmdHelpInfo1            = L"\n%s %s\n\n"\
                                                          L"The following is a list of supported command line options:\n\n";
const STRING MgResources::ServerCmdHelpInfo2            = L"\nNote: Only 1 command line option can be used at a time.\n\n";
const STRING MgResources::ServerCmdHelpDescription      = L"  ? or help\n"\
                                                          L"      Displays this information.\n\n";

const STRING MgResources::ServerCmdInstall              = L"install"; // Do not translate
const STRING MgResources::ServerCmdInstallInfo          = L"Installing server service \"%s\".\nServer service display name \"%s\".\nService will be automatically started if installed successfully.\n";
const STRING MgResources::ServerCmdInstallDescription   = L"  install <Name>\n"\
                                                          L"      Installs the server as a service. Automatically starts the service.\n\n"\
                                                          L"      Default service display name installed:\n"\
                                                          L"      \"%s\"\n\n"\
                                                          L"      You can use a different display name for the service if you specify an\n"\
                                                          L"      optional \"Name\".\n\n";

const STRING MgResources::ServerCmdUninstall            = L"uninstall"; // Do not translate
const STRING MgResources::ServerCmdUninstallInfo        = L"Uninstalling server service \"%s\".\n";
const STRING MgResources::ServerCmdUninstallDescription = L"  uninstall\n"\
                                                          L"      Uninstalls an installed server service. Automatically stops the service\n"\
                                                          L"      before uninstalling.\n\n";

const STRING MgResources::ServerCmdTestFdo              = L"testfdo"; // Do not translate
const STRING MgResources::ServerCmdTestFdoInfo          = L"Running the FDO unit tests.\n\n";
const STRING MgResources::ServerCmdTestFdoDescription   = L"  testfdo <Filename>\n"\
                                                          L"      Runs the FDO unit tests.\n\n"\
                                                          L"      Default output of the FDO unit tests will be the console unless you\n"\
                                                          L"      specify an optional \"Filename\". The output to the \"Filename\" will be\n"\
                                                          L"      in XML.\n\n";

const STRING MgResources::ServerCmdTestMode             = L"testmode"; // Do not translate
const STRING MgResources::ServerCmdTest                 = L"test"; // Do not translate
const STRING MgResources::ServerCmdTestInfo             = L"Running the server unit tests.\n\n";
const STRING MgResources::ServerCmdTestDescription      = L"  test <Test> <Filename>\n"\
                                                          L"      Runs the server unit test(s).\n\n"\
                                                          L"      By default all of the unit tests will be run unless you specify an\n"\
                                                          L"      optional \"Test\" to run. If you specify \"List\" as the test to run you\n"\
                                                          L"      will get a list of the available unit tests.\n\n"\
                                                          L"      Default output of the unit tests will be the console unless you specify\n"\
                                                          L"      an optional \"Filename\". The output to the \"Filename\" will be in XML.\n\n";

const STRING MgResources::ServerCmdTestDefaultFileName  = L"results.xml";
const STRING MgResources::ServerCmdTestDefaultTests     = L"all"; // Do not translate
const STRING MgResources::ServerCmdTestListTests        = L"list"; // Do not translate


const STRING MgResources::ServerCmdInteractive          = L"interactive"; // Do not translate
const STRING MgResources::ServerCmdRun                  = L"run"; // Do not translate
const STRING MgResources::ServerCmdRunInfo              = L"Running the server interactively as an application instead of as a service.\n\n";
const STRING MgResources::ServerCmdRunDescription       = L"  run\n"\
                                                            L"      Runs the server interactively as an application instead of a service.\n\n";

const STRING MgResources::ServerCmdStart                = L"start"; // Do not translate
const STRING MgResources::ServerCmdStartInfo            = L"Starting the server service.\n\n";
const STRING MgResources::ServerCmdStartDescription     = L"  start\n"\
                                                          L"      Starts the server service. Note: The service must be installed.\n\n";

const STRING MgResources::ServerCmdStop                 = L"stop"; // Do not translate
const STRING MgResources::ServerCmdStopInfo             = L"Stopping the server service.\n\n";
const STRING MgResources::ServerCmdStopDescription      = L"  stop\n"\
                                                          L"      Stops the server service. Note: The service must be installed.\n\n";

const STRING MgResources::ServerCmdUnrecognizedInfo     = L"Unrecognized option: \"%s\".\n";

const STRING MgResources::ServerCmdInstallFailed        = L"Failed to install the server service: \"%s\".\nError: %s";
const STRING MgResources::ServerCmdUninstallFailed      = L"Failed to uninstall the server service: \"%s\".\nError: %s";
const STRING MgResources::ServerErrorCodeMessage        = L"Error code %d.\n";
const STRING MgResources::ServerServiceError            = L"Error: %s.\n";
const STRING MgResources::ServerServiceStartFailure     = L"Service failed to start: %d\nError: %s\nYou cannot run the server from the command line unless you specify the\n\"%s\" command line option.\n\n";


const STRING MgResources::Resource_Exception_Section    = L"Exceptions"; // Do not translate

const STRING MgResources::Unknown                       = L"Unknown";
const STRING MgResources::Success                       = L"Success";
const STRING MgResources::Failure                       = L"Failure";
const STRING MgResources::UnauthorizedAccess            = L"Unauthorized Access";
const STRING MgResources::PermissionDenied              = L"Permission Denied";
const STRING MgResources::Error                         = L"Error"; // Do not translate
const STRING MgResources::Warnings                      = L"Warning"; // Do not translate
const STRING MgResources::StackTrace                    = L"StackTrace"; // Do not translate
const STRING MgResources::ErrorDescription              = L"ErrorDescription"; // Do not translate
const STRING MgResources::BlankArgument                 = L" ";

// Services
const STRING MgResources::DrawingService                = L"DrawingService"; // Do not translate
const STRING MgResources::FeatureService                = L"FeatureService"; // Do not translate
const STRING MgResources::MappingService                = L"MappingService"; // Do not translate
const STRING MgResources::RenderingService              = L"RenderingService"; // Do not translate
const STRING MgResources::ResourceService               = L"ResourceService"; // Do not translate
const STRING MgResources::ServerAdminService            = L"ServerAdminService"; // Do not translate
const STRING MgResources::SiteService                   = L"SiteService"; // Do not translate
const STRING MgResources::TileService                   = L"TileService"; // Do not translate
const STRING MgResources::KmlService                    = L"KmlService"; // Do not translate
const STRING MgResources::ProfilingService              = L"ProfilingService"; // Do not translate

// Other
const STRING MgResources::Stylization                   = L"Stylization"; // Do not translate
const STRING MgResources::WhiteSpace                    = L"\n\r\t ";

// for locale
static const STRING ResourceFilenameDash    = L"-";            // Do not translate
static const int ExtendedLocaleLength       = 5;
static const int ExtendedLocaleDashLocation = 2;
static const int ParentLocaleLength         = 2;

MgResources::MgResources()
{
    // default path
    m_path = L"Resources/";

    // default component
    m_component = MgResources::ResourceComponent;
}


MgResources::~MgResources()
{
    // Clean up the resource cache
    ResourceCache::iterator iterator;

    for(iterator = m_resourceCache.begin(); iterator != m_resourceCache.end(); ++iterator)
    {
        DeleteResourceFile(iterator->second);
        iterator->second = NULL;
    }
    m_resourceCache.clear();
}


void MgResources::Dispose()
{
    delete this;
}


MgResources* MgResources::GetInstance()
{
    MG_RESOURCES_TRY()

    ACE_TRACE ("MgResources::GetInstance");

    if (MgResources::m_resources == NULL)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance (), 0));
        if (MgResources::m_resources == NULL)
        {
            MgResources::m_resources = new MgResources();
        }
    }

    MG_RESOURCES_CATCH_AND_THROW(L"MgResources.GetInstance")

    // To avoid overheads and maintain thread safety,
    // do not assign this returned static singleton to a Ptr object.
    return MgResources::m_resources;
}


void MgResources::SetComponent(CREFSTRING component)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));
    m_component = component;
}


void MgResources::Initialize(CREFSTRING path)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    MG_RESOURCES_TRY()

    m_path = path;

    // Check if path ends with a '/' if not, add one if needed
    MgFileUtil::AppendSlashToEndOfPath(m_path);

    MG_RESOURCES_CATCH_AND_THROW(L"MgResources.Initialize")
}


STRING MgResources::GetResourceFilename(CREFSTRING locale)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, L""));

    STRING filename;

    MG_RESOURCES_TRY()

    // Note: No validation is done on the locale.
    if (locale.length() > 0)
    {
        filename = m_path + m_component + ResourceFilenameUnderscore + locale + ResourceFilenameExtension;
    }

    MG_RESOURCES_CATCH_AND_THROW(L"MgResources.GetResourceFilename")

    return filename;
}


void MgResources::LoadResources(CREFSTRING locale)
{
    // allow only one thread to load at a time
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    MG_RESOURCES_TRY()

    // Check to see if this is already in the cache. If it is we need to remove it, load it
    // and then add it to the cache.  Otherwise, we just load it and add it to the cache.

    ResourceCache::iterator iterator = GetResources(locale, false);
    if (m_resourceCache.end() != iterator)
    {
        // Remove specified locale from the cache as we want to update it.
        DeleteResourceFile(iterator->second);
        iterator->second = NULL;
        m_resourceCache.erase(iterator);
    }

    STRING strResourceFilename = GetResourceFilename(locale);

    // five character locales are required to be of the form lower-upper - verify this
    if (ExtendedLocaleLength == locale.length())
    {
        STRING lowerPart = MgUtil::ToLower(locale.substr(0,3));
        STRING upperPart = MgUtil::ToUpper(locale.substr(3,2));
        if (lowerPart.compare(locale.substr(0,3)) != 0 ||
            upperPart.compare(locale.substr(3,2)) != 0)
        {
            MgStringCollection arguments;
            arguments.Add(strResourceFilename.c_str());
            arguments.Add(locale.c_str());

            throw new MgResourcesLoadFailedException(L"MgResources.LoadResources",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
    }

    ResourceFile* pResources = new ResourceFile();
    if (NULL != pResources)
    {
        bool bSuccess = false;
        if (MgFileUtil::PathnameExists(strResourceFilename) && true == ParseFile(strResourceFilename, pResources))
        {
            m_resourceCache[locale] = pResources;
            bSuccess = true;
        }
        else
        {
            if (ExtendedLocaleLength == locale.length() && ExtendedLocaleDashLocation == (int)locale.find(ResourceFilenameDash))
            {
                // if the locale is of the form "en-GB", and there wasn't a matching resource file,
                // try to find the resource file of the parent locale, i.e. "en"
                STRING strParentLocale = locale.substr(0, ParentLocaleLength);
                STRING strParentResourceFilename = GetResourceFilename(strParentLocale);
                if (MgFileUtil::PathnameExists(strParentResourceFilename) && true == ParseFile(strParentResourceFilename, pResources))
                {
                    m_resourceCache[locale] = pResources;
                    bSuccess = true;
                }
            }
        }

        if (!bSuccess)
        {
            MgStringCollection arguments;
            arguments.Add(strResourceFilename.c_str());
            arguments.Add(locale.c_str());

            throw new MgResourcesLoadFailedException(L"MgResources.LoadResources",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
    }

    MG_RESOURCES_CATCH_AND_THROW(L"MgResources.LoadResources")
}


// This will call the load method if the resources are not already cached.
// This (private) method is NOT thread-safe - callers are responsible for
// serializing access.
ResourceCache::iterator MgResources::GetResources(CREFSTRING locale, bool bLoad)
{
    ResourceCache::iterator iterator;

    MG_RESOURCES_TRY()

    iterator = m_resourceCache.find(locale);
    if (m_resourceCache.end() == iterator)
    {
        if(true == bLoad)
        {
            // The locale was NOT found in the cache, so we need to load it.
            LoadResources(locale);
            iterator = m_resourceCache.find(locale);
        }
    }

    MG_RESOURCES_CATCH_AND_THROW(L"MgResources.GetResources")

    return iterator;
}


STRING MgResources::GetStringResource(CREFSTRING locale, CREFSTRING section, CREFSTRING resourceId)
{
    // ensure thread-safe access to the ACE_Configuration_Heap below
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, L""));

    STRING value;

    MG_RESOURCES_TRY()

    ResourceCache::iterator iterator = GetResources(locale, true);
    if (m_resourceCache.end() != iterator)
    {
        ResourceFile* pResources = iterator->second;
        if (NULL != pResources)
        {
            ResourceFile::iterator sectionIter = pResources->find(section);
            if (pResources->end() != sectionIter)
            {
                ResourceSection* pSection = sectionIter->second;
                ResourceSection::iterator resourceIter = pSection->find(resourceId);
                if (pSection->end() != resourceIter)
                {
                    value = resourceIter->second;
                }
            }
        }
    }

    MG_RESOURCES_CATCH_AND_THROW(L"MgResources.GetStringResource")

    return value;
}


STRING MgResources::FormatMessage(CREFSTRING stringResource, MgStringCollection* arguments)
{
    STRING value = stringResource;

    INT32 numArgsProvided = 0;
    INT32 numArgsNeeded = 0;

    STRING currentReplace;
    wchar_t buff[4];
    string::size_type pos = 0;

    INT32 i; // used in for loops

    MG_RESOURCES_TRY()

    // Determine the number of arguments provided.
    if (arguments != NULL)
    {
        numArgsProvided = arguments->GetCount();
    }

    // Determine the number of arguments needed
    for (i = 9; i > 0; i--)
    {
        ACE_OS::itoa(i, buff, 10);
        currentReplace = L"%";
        currentReplace += buff;
        pos = stringResource.find(currentReplace, 0);

        // if placeholder for argument i exists then i
        // is the number of arguments needed.
        if (pos != string::npos)
        {
            numArgsNeeded = i;
            break;
        }
    }

    // If the argument collection size is valid, format message
    if ((numArgsProvided == numArgsNeeded) && (numArgsProvided <= 9))
    {
        STRING currentArg;
        string::size_type currentArgSize;

        for (i = 0; i < numArgsProvided; i++)
        {
            currentArg = arguments->GetItem(i);
            currentArgSize = currentArg.size();

            // Item at index 0 will replace %1 and so on
            ACE_OS::itoa((i + 1), buff, 10);
            currentReplace = L"%";
            currentReplace += buff;

            // Replace all occurances of the placeholder
            pos = value.find(currentReplace, 0);
            while (pos != string::npos)
            {
                value.replace(pos, 2, currentArg);
                // Search for the placeholder after the end of the inserted argument
                pos = value.find(currentReplace, (pos+currentArgSize));
            }
        }
    }
    // Otherwise, collection size is invalid so throw exception
    else
    {
        STRING buffer;
        MgUtil::Int32ToString(numArgsProvided, buffer);

        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgResources.FormatMessage",
            __LINE__, __WFILE__, &arguments, L"MgArgumentsMismatch", NULL);
    }

    MG_RESOURCES_CATCH_AND_THROW(L"MgResources.FormatMessage")

    return value;
}


void MgResources::DeleteResourceFile(ResourceFile* file)
{
    for (ResourceFile::iterator iter = file->begin(); iter != file->end(); ++iter)
    {
        if (NULL != iter->second)
        {
            iter->second->clear();
            delete iter->second;
            iter->second = NULL;
        }
    }
    file->clear();
    delete file;
}


bool MgResources::ParseFile(CREFSTRING strResourceFileName, ResourceFile* pResourceFile)
{
    bool bParsed = false;

    MG_RESOURCES_TRY()

    // Read the entire file in assuming it is UTF-8 encoded.
    Ptr<MgByteSource> resourceFile = new MgByteSource(strResourceFileName);
    resourceFile->SetMimeType(MgMimeType::Text);
    Ptr<MgByteReader> reader = resourceFile->GetReader();
    STRING fileString = reader->ToString();

    ResourceSection* pSection = NULL;

    STRING::size_type offset = 0;
    while (offset < fileString.length() && offset != fileString.npos)
    {
        // break at newlines
        STRING::size_type newline = fileString.find(L"\n", offset);
        if (newline != fileString.npos)
        {
            // Trim leading/trailing all whitespace
            STRING subString = fileString.substr(offset, newline-offset);
            STRING::size_type start = subString.find_first_not_of(MgResources::WhiteSpace);
            STRING::size_type end = subString.find_last_not_of(MgResources::WhiteSpace);

            offset = newline+1;

            if (subString.npos != start && subString.npos != end)
            {
                subString = subString.substr(start, end-start+1);

                if (L'#' == subString[0])
                {
                    // do nothing, skip to next line
                    continue;
                }

                if (L'[' == subString[0])
                {
                    // start of section, save section name and switch to new section
                    STRING brackets = L"[]";
                    STRING sectionName = MgUtil::Trim(subString, brackets);
                    pSection = new ResourceSection();
                    pResourceFile->operator[](sectionName) = pSection;
                    continue;
                }

                STRING::size_type equals = subString.find(L'=');
                if (subString.npos != equals)
                {
                    // This should be a configuration directive.  Add it to the section.
                    STRING directive = MgUtil::Trim(subString.substr(0, equals), MgResources::WhiteSpace);
                    STRING value = MgUtil::Trim(subString.substr(equals+1), MgResources::WhiteSpace);
                    if (NULL != pSection)
                    {
                        pSection->operator[](directive) = value;
                    }
                    else
                    {
                        throw new MgResourcesLoadFailedException(L"MgResources.ParseFile", __LINE__, __WFILE__, NULL, L"", NULL);
                    }
                }
            }
        }
    }

    bParsed = true;

    MG_RESOURCES_CATCH_AND_THROW(L"MgResources.ParseFile")

    return bParsed;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets a message that is retrieved from the specified resource ID and is
/// formatted with the specified arguments.
///
/// \param section
/// Section in the resource file.
/// \param resourceId
/// Resource identifier in the section.
/// \param arguments
/// Pointer to an MgStringCollection that contains the text to be placed
/// into the formatted message. If arguments is NULL then that is treated
/// as having 0 arguments provided. A maximum of 9 arguments is allowed.
///
/// \return
/// The formatted message string.
///
STRING MgResources::GetResourceMessage(CREFSTRING section, CREFSTRING resourceId,
    MgStringCollection* arguments)
{
    STRING message;

    MG_RESOURCES_TRY()

    STRING locale = DefaultMessageLocale;
    MgConfiguration* configuration = MgConfiguration::GetInstance();
    ACE_ASSERT(NULL != configuration);

    if (NULL != configuration)
    {
        configuration->GetStringValue(
            MgFoundationConfigProperties::GeneralPropertiesSection,
            MgFoundationConfigProperties::GeneralPropertyDefaultMessageLocale,
            locale,
            MgFoundationConfigProperties::DefaultGeneralPropertyDefaultMessageLocale);
    }

    STRING resourceStr = GetStringResource(locale, section, resourceId);

    message = FormatMessage(resourceStr, arguments);

    MG_RESOURCES_CATCH_AND_THROW(L"MgResources.GetResourceMessage")

    return message;
}
