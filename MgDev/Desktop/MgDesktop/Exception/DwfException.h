#ifndef DESKTOP_DWF_EXCEPTION_H
#define DESKTOP_DWF_EXCEPTION_H

/// \ingroup Desktop_Exceptions_Module

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Wrapper for DWF toolkit exceptions.
///
class MG_DESKTOP_API MgdDwfException : public MgThirdPartyException
{
    DECLARE_CLASSNAME(MgdDwfException)

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgdDwfException object.
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
    MgdDwfException(CREFSTRING methodName, INT32 lineNumber,
        CREFSTRING fileName, MgStringCollection* whatArguments,
        CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor for a MgdDwfException object.
    ///
    virtual ~MgdDwfException() throw();

INTERNAL_API:

    DECLARE_EXCEPTION_DEFAULTS(MgdDwfException)

CLASS_ID:

    static const INT32 m_cls_id = MapGuide_Desktop_Exception_DwfException;
};

#endif