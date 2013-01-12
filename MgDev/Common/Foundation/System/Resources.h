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

#ifndef MG_RESOURCES_H_
#define MG_RESOURCES_H_

#include <map>

class ACE_Configuration_Heap;

typedef std::map<STRING, STRING> ResourceSection;
typedef std::map<STRING, ResourceSection*> ResourceFile;
typedef std::map<STRING, ResourceFile*> ResourceCache;

///////////////////////////////////////////////////////////////////////////////
/// MapGuide Resources try/catch/throw macros.
///
#define MG_RESOURCES_TRY()                                                    \
    MG_TRY()                                                                  \

#define MG_RESOURCES_CATCH(methodName)                                        \
    MG_CATCH(methodName)                                                      \

#define MG_RESOURCES_THROW()                                                  \
    MG_THROW()                                                                \

#define MG_RESOURCES_CATCH_AND_THROW(methodName)                              \
    MG_CATCH_AND_THROW(methodName)                                            \

/// \cond INTERNAL
class MG_FOUNDATION_API MgResources : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgResources)

INTERNAL_API:
    /// \brief
    /// Destructor
    ///
    virtual ~MgResources();

    /// \brief
    /// Self Destructor
    ///
    virtual void Dispose();

    /// \brief
    /// Get pointer to a process-wide MgResources.
    ///
    static MgResources* GetInstance();

    /// \brief
    /// Sets the component for MgResources.
    /// By default this is the web. The server will set this at startup.
    ///
    /// Note: This method should only be called once at startup.
    ///
    void SetComponent(CREFSTRING component);

    /// \brief
    /// Initialize MgResources.
    ///
    /// Note: This method should only be called once at startup.
    ///
    void Initialize(CREFSTRING path);

    /// \brief
    /// Loads the resources for the specified locale
    ///
    /// \param locale
    /// The locale of the resources to load
    ///
    /// \exception MgResourcesException
    ///
    void LoadResources(CREFSTRING locale);

    /// \brief
    /// Gets the resource filename for the component and locale specified.
    /// The resource filenames use the following format:
    ///   <component>_<ISO 639-1 name>.res
    /// Ex: mapguide_en.res
    ///
    /// \param locale
    /// The locale.
    /// \param filename
    /// Out parameter for the resource filename.
    ///
    /// \return
    /// filename if successful
    ///
    /// \exception MgResourcesException
    ///
    STRING GetResourceFilename(CREFSTRING locale);

    /// \brief
    /// Get the string resource for the the specified locale, section and resourceId
    ///
    /// \param locale
    /// The locale of the resources
    /// \param section
    /// Section in the resource file
    /// \param resourceId
    /// Resource identifier in the section
    /// \param value
    /// Out parameter for the value of the resource string requested
    ///
    /// \return
    /// resource string if found
    ///
    /// \exception MgResourcesException
    ///
    STRING GetStringResource(CREFSTRING locale, CREFSTRING section, CREFSTRING resourceId);

    /// \brief
    /// Format a string resource that was retrieved using GetStringResource.  The number of arguments
    /// needed to be replaced in the stringResource must match the size of the collection given.
    ///
    /// Note: the argument place holder starts at %1 and goes to %9, so %0 will not get replaced.
    ///
    /// \param stringResource
    /// The STRING to be formatted. ie. "%1 %2"
    /// \param arguments
    /// A pointer to an MgStringCollection that contains the text to be placed into the formatted
    /// stringResource.  If arguments is NULL then that is treated as having 0 arguments provided.
    /// A maximum of 9 arguments is allowed.
    ///
    /// \return
    /// The formatted message string
    ///
    /// \exception MgInvalidCollectionSizeException
    ///
    STRING FormatMessage(CREFSTRING stringResource, MgStringCollection* arguments);

    ///////////////////////////////////////////////////////////////////////////
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
    STRING GetResourceMessage(CREFSTRING section, CREFSTRING resourceId,
        MgStringCollection* arguments);

    /// Resource Components
    static const STRING ResourceComponent;

    /// Default locale
    static const STRING DefaultMessageLocale;

    static const STRING ServerServiceName;
    static const STRING ServerServiceDisplayName;
    static const STRING ServerServiceDescription;

    static const STRING FailedToLoadResourcesFile;
    static const STRING FailedToLoadStartupFile;
    static const STRING FailedToLoadConfigurationFile;
    static const STRING FailedToRetrieveMessage;
    static const STRING FormatExceptionLocationMethods;
    static const STRING FormatExceptionLocationLines;
    static const STRING FormatExceptionLocationFiles;
    static const STRING MentorDictionaryPath;
    static const STRING ServerStarting;
    static const STRING ServerShuttingDown;
    static const STRING ServerStarted;
    static const STRING ServerStopped;
    static const STRING ServerCmdDaemon;
    static const STRING ServerCmdDaemonInfo;
    static const STRING ServerCmdDaemonDescription;
    static const STRING ServerCmdHelp;
    static const STRING ServerCmdHelpInfo1;
    static const STRING ServerCmdHelpInfo2;
    static const STRING ServerCmdHelpDescription;
    static const STRING ServerCmdInstall;
    static const STRING ServerCmdInstallInfo;
    static const STRING ServerCmdInstallDescription;
    static const STRING ServerCmdInteractive; // Backwards compatibility
    static const STRING ServerCmdRun;
    static const STRING ServerCmdRunInfo;
    static const STRING ServerCmdRunDescription;
    static const STRING ServerCmdStart;
    static const STRING ServerCmdStartInfo;
    static const STRING ServerCmdStartDescription;
    static const STRING ServerCmdStop;
    static const STRING ServerCmdStopInfo;
    static const STRING ServerCmdStopDescription;
    static const STRING ServerCmdTestFdo;
    static const STRING ServerCmdTestFdoInfo;
    static const STRING ServerCmdTestFdoDescription;
    static const STRING ServerCmdTest;
    static const STRING ServerCmdTestInfo;
    static const STRING ServerCmdTestDescription;
    static const STRING ServerCmdTestDefaultFileName;
    static const STRING ServerCmdTestDefaultTests;
    static const STRING ServerCmdTestListTests;
    static const STRING ServerCmdTestMode; // Backwards compatibility
    static const STRING ServerCmdUninstall;
    static const STRING ServerCmdUninstallInfo;
    static const STRING ServerCmdUninstallDescription;
    static const STRING ServerCmdUnrecognizedInfo;
    static const STRING ServerCmdInstallFailed;
    static const STRING ServerCmdUninstallFailed;
    static const STRING ServerErrorCodeMessage;
    static const STRING ServerServiceError;
    static const STRING ServerServiceStartFailure;
    static const STRING RegisteredLicenseDetected;
    static const STRING EvaluationLicenseDetected;
    static const STRING AlphaReleaseDetected;
    static const STRING BetaReleaseDetected;
    static const STRING PreviewReleaseDetected;
    static const STRING InvalidLicense;
    static const STRING LicenseAlreadyExists;
    static const STRING LicenseNotFound;
    static const STRING LicenseCheckoutCancel;
    static const STRING LicenseCheckoutError;
    static const STRING LicenseInvalidSerialNumber;
    static const STRING PreCacheMapsStart;


    static const STRING UsingDefaultResourceFile;

    static const STRING Resource_Exception_Section;

    static const STRING Unknown;
    static const STRING Success;
    static const STRING Failure;
    static const STRING UnauthorizedAccess;
    static const STRING PermissionDenied;
    static const STRING Error;
    static const STRING Warnings;   // Intentionally suffix the constant here with 's' to resolve a build conflict
    static const STRING StackTrace;
    static const STRING ErrorDescription;
    static const STRING BlankArgument;

    // Services
    static const STRING DrawingService;
    static const STRING FeatureService;
    static const STRING MappingService;
    static const STRING RenderingService;
    static const STRING ResourceService;
    static const STRING ServerAdminService;
    static const STRING SiteService;
    static const STRING TileService;
    static const STRING KmlService;
    static const STRING ProfilingService;

    // Other
    static const STRING Stylization;

private:
    static const STRING ResourceFilenameExtension;
    static const STRING ResourceFilenameUnderscore;
    static const STRING WhiteSpace;

private:
    /// \brief
    /// Constructor
    ///
    MgResources();

    /// Helper to get locale-specific resources
    ResourceCache::iterator GetResources(CREFSTRING locale, bool bLoad);

    /// Helper to delete ResourceFile map
    void DeleteResourceFile(ResourceFile* file);

    /// Helper function to read in UTF8 resource file and parse it
    bool ParseFile(CREFSTRING strResourceFileName, ResourceFile* resourceFile);

    /// Pointer to a process-wide singleton.
    static Ptr<MgResources> m_resources;

    STRING m_component;
    STRING m_path;
    ResourceCache m_resourceCache;
    ACE_Recursive_Thread_Mutex m_mutex;
};
/// \endcond

#endif
