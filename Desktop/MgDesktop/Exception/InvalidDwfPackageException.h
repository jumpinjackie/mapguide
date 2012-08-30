#ifndef DESKTOP_INVALID_DWF_PACKAGE_EXCEPTION_H
#define DESKTOP_INVALID_DWF_PACKAGE_EXCEPTION_H

/// \ingroup Desktop_Exceptions_Module

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Thrown when a DWF package is not valid.
///
class MG_DESKTOP_API MgdInvalidDwfPackageException : public MgApplicationException
{
    DECLARE_CLASSNAME(MgdInvalidDwfPackageException)

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgdInvalidDwfPackageException object.
    ///
    /// \param methodName
    /// Name of the method where the exception occurred.
    /// \param lineNumber
    /// Line number where the exception occurred.
    /// \param fileName
    /// File name where the exception occurred.
    /// \param whatArguments
    /// Collection of arguments used to format the message that describes what the exception is.
    /// \param whyMessageId
    /// ID of the message that describes why the exception occurs.
    /// \param whyArguments
    /// Collection of arguments used to format the message that describes why the exception occurs.
    ///
    MgdInvalidDwfPackageException(CREFSTRING methodName, INT32 lineNumber,
        CREFSTRING fileName, MgStringCollection* whatArguments,
        CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor for a MgdInvalidDwfPackageException object.
    ///
    virtual ~MgdInvalidDwfPackageException() throw();

INTERNAL_API:

    DECLARE_EXCEPTION_DEFAULTS(MgdInvalidDwfPackageException)

CLASS_ID:

    static const INT32 m_cls_id = MapGuide_Desktop_Exception_InvalidDwfPackageException;
};

#endif