#ifndef DESKTOP_RESOURCE_DEFS_H
#define DESKTOP_RESOURCE_DEFS_H

#include <assert.h>
#include <map>
#include <vector>
#include <dwfcore/ZipFileDescriptor.h>


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Package API names
///
class MG_DESKTOP_API MgdPackageApiName
{
INTERNAL_API:
    static const STRING LoadPackage;    /// value("LoadPackage")
    static const STRING MakePackage;    /// value("MakePackage")
};


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Status codes for packages
///
class MG_DESKTOP_API MgdPackageStatusCode
{
EXTERNAL_API:
    /// Indicates a package was successfully loaded/made.
    static const STRING Succeeded;      /// value("Succeeded")

    /// Indicates an error occurred while loading/making the package.
    static const STRING Failed;         /// value("Failed")

    /// Indicates a package is currently being loaded/made.
    static const STRING InProgress;     /// value("InProgress")

    /// Indicates the package has not started.
    static const STRING NotStarted;     /// value("NotStarted")

    /// Indicates the status of the package is not known.
    static const STRING Unknown;        /// value("Unknown")
};

///////////////////////////////////////////////////////////////////////////////
/// Mg Resource Service try/catch/throw macros.
///
#define MG_RESOURCE_SERVICE_TRY()                                             \
    MG_TRY()                                                                  \

#define MG_RESOURCE_SERVICE_CATCH(methodName)                                 \
    }                                                                           \
    catch (DWFException& e)                                                   \
    {                                                                         \
        MgStringCollection arguments;                                         \
        arguments.Add(STRING(e.message()));                                   \
        mgException = new MgdDwfException(methodName, __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments); \
    }                                                                         \
    catch (const XMLException& e)                                             \
    {                                                                         \
        MgStringCollection arguments;                                         \
        arguments.Add(X2W(e.getMessage()));                                   \
        mgException = new MgXmlParserException(methodName, __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments); \
                                                                              \
    MG_CATCH(methodName)                                                      \

#define MG_RESOURCE_SERVICE_THROW()                                           \
    MG_THROW()                                                                \

#define MG_RESOURCE_SERVICE_CATCH_AND_THROW(methodName)                       \
    MG_RESOURCE_SERVICE_CATCH(methodName)                                     \
                                                                              \
    MG_RESOURCE_SERVICE_THROW()                                               \

#define MG_RESOURCE_CONTAINER_CATCH_AND_THROW(methodName)                     \
    MG_RESOURCE_SERVICE_CATCH_AND_THROW(FormatMethodName(methodName))



#endif