#ifndef DESKTOP_DUPLICATE_PARAMETER_EXCEPTION_H
#define DESKTOP_DUPLICATE_PARAMETER_EXCEPTION_H


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Thrown when a duplicate parameter is found.
///
class MG_DESKTOP_API MgDuplicateParameterException : public MgApplicationException
{
    DECLARE_CLASSNAME(MgDuplicateParameterException)

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgDuplicateParameterException object.
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
    MgDuplicateParameterException(CREFSTRING methodName, INT32 lineNumber,
        CREFSTRING fileName, MgStringCollection* whatArguments,
        CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destruct the MgDuplicateParameterException object.
    ///
    virtual ~MgDuplicateParameterException() throw();

INTERNAL_API:

    DECLARE_EXCEPTION_DEFAULTS(MgDuplicateParameterException)

CLASS_ID:

    static const INT32 m_cls_id = MapGuide_Desktop_Exception_MgDuplicateParameterException;
};

#endif