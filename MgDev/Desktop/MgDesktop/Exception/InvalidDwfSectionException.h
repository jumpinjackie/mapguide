#ifndef DESKTOP_INVALID_DWF_SECTION_EXCEPTION_H
#define DESKTOP_INVALID_DWF_SECTION_EXCEPTION_H

/// \ingroup Desktop_Exceptions_Module

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Thrown when an invalid section in a DWF is found.
///
class MG_DESKTOP_API MgdInvalidDwfSectionException : public MgApplicationException
{
    DECLARE_CLASSNAME(MgdInvalidDwfSectionException)

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgdInvalidDwfSectionException object.
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
    MgdInvalidDwfSectionException(CREFSTRING methodName, INT32 lineNumber,
        CREFSTRING fileName, MgStringCollection* whatArguments,
        CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor for a MgdInvalidDwfSectionException object.
    ///
    virtual ~MgdInvalidDwfSectionException() throw();

INTERNAL_API:

    DECLARE_EXCEPTION_DEFAULTS(MgdInvalidDwfSectionException)

CLASS_ID:

    static const INT32 m_cls_id = MapGuide_Desktop_Exception_InvalidDwfSectionException;
};

#endif