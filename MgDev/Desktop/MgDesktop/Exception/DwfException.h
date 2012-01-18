#ifndef DESKTOP_DWF_EXCEPTION_H
#define DESKTOP_DWF_EXCEPTION_H

/// \ingroup Exceptions_Module

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Wrapper for DWF toolkit exceptions.
///
class MG_DESKTOP_API MgDwfException : public MgThirdPartyException
{
    DECLARE_CLASSNAME(MgDwfException)

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgDwfException object.
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
    MgDwfException(CREFSTRING methodName, INT32 lineNumber,
        CREFSTRING fileName, MgStringCollection* whatArguments,
        CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor for a MgDwfException object.
    ///
    virtual ~MgDwfException() throw();

INTERNAL_API:

    DECLARE_EXCEPTION_DEFAULTS(MgDwfException)

CLASS_ID:

    static const INT32 m_cls_id = MapGuide_Desktop_Exception_MgDwfException;
};

#endif