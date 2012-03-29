#ifndef DESKTOP_INVALID_FEATURE_SOURCE_EXCEPTION_H
#define DESKTOP_INVALID_FEATURE_SOURCE_EXCEPTION_H

/// \ingroup Exceptions_Module

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Thrown when FeatureService encounters a problem.
///
class MG_DESKTOP_API MgInvalidFeatureSourceException : public MgApplicationException
{
    DECLARE_CLASSNAME(MgInvalidFeatureSourceException)

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgInvalidFeatureSourceException object.
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
    MgInvalidFeatureSourceException(CREFSTRING methodName, INT32 lineNumber,
        CREFSTRING fileName, MgStringCollection* whatArguments,
        CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor for a MgInvalidFeatureSourceException object.
    ///
    virtual ~MgInvalidFeatureSourceException() throw();

INTERNAL_API:

    DECLARE_EXCEPTION_DEFAULTS(MgInvalidFeatureSourceException)

CLASS_ID:

    static const INT32 m_cls_id = MapGuide_Desktop_Exception_MgInvalidFeatureSourceException;
};

#endif
