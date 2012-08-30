#ifndef DESKTOP_PARAMETER_NOT_FOUND_EXCEPTION_H
#define DESKTOP_PARAMETER_NOT_FOUND_EXCEPTION_H


/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Thrown when a parameter is not found.
///
class MG_DESKTOP_API MgdParameterNotFoundException : public MgApplicationException
{
    DECLARE_CLASSNAME(MgdParameterNotFoundException)

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgdParameterNotFoundException object.
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
    MgdParameterNotFoundException(CREFSTRING methodName, INT32 lineNumber,
        CREFSTRING fileName, MgStringCollection* whatArguments,
        CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destruct the MgdParameterNotFoundException object.
    ///
    virtual ~MgdParameterNotFoundException() throw();

INTERNAL_API:

    DECLARE_EXCEPTION_DEFAULTS(MgdParameterNotFoundException)

CLASS_ID:

    static const INT32 m_cls_id = MapGuide_Desktop_Exception_ParameterNotFoundException;
};
/// \endcond

#endif