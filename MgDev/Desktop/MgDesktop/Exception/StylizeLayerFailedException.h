#ifndef DESKTOP_STYLIZE_LAYER_FAILED_EXCEPTION_H
#define DESKTOP_STYLIZE_LAYER_FAILED_EXCEPTION_H

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Thrown when a stylize layer fails.
///
class MG_DESKTOP_API MgStylizeLayerFailedException : public MgApplicationException
{
    DECLARE_CLASSNAME(MgStylizeLayerFailedException)

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgStylizeLayerFailedException object.
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
    MgStylizeLayerFailedException(CREFSTRING methodName, INT32 lineNumber,
        CREFSTRING fileName, MgStringCollection* whatArguments,
        CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor for a MgStylizeLayerFailedException object.
    ///
    virtual ~MgStylizeLayerFailedException() throw();

INTERNAL_API:

    DECLARE_EXCEPTION_DEFAULTS(MgStylizeLayerFailedException)

CLASS_ID:

    static const INT32 m_cls_id = MapGuide_Desktop_Exception_MgStylizeLayerFailedException;
};

#endif