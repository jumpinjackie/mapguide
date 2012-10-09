#ifndef DESKTOP_DUPLICATE_PARAMETER_EXCEPTION_H
#define DESKTOP_DUPLICATE_PARAMETER_EXCEPTION_H

/// \ingroup Desktop_Exceptions_Module

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Thrown when a duplicate parameter is found.
///
class MG_DESKTOP_API MgdDuplicateParameterException : public MgApplicationException
{
    DECLARE_CLASSNAME(MgdDuplicateParameterException)

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgdDuplicateParameterException object.
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
    MgdDuplicateParameterException(CREFSTRING methodName, INT32 lineNumber,
        CREFSTRING fileName, MgStringCollection* whatArguments,
        CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destruct the MgdDuplicateParameterException object.
    ///
    virtual ~MgdDuplicateParameterException() throw();

INTERNAL_API:

    DECLARE_EXCEPTION_DEFAULTS(MgdDuplicateParameterException)

CLASS_ID:

    static const INT32 m_cls_id = MapGuide_Desktop_Exception_DuplicateParameterException;
};

#endif