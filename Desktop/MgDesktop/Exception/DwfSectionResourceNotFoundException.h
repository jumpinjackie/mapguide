#ifndef DESKTOP_DWF_SECTION_RESOURCE_NOT_FOUND_EXCEPTION_H
#define DESKTOP_DWF_SECTION_RESOURCE_NOT_FOUND_EXCEPTION_H

/// \ingroup Exceptions_Module

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Thrown when a DWF section resource is not found.
///
class MG_DESKTOP_API MgDwfSectionResourceNotFoundException : public MgApplicationException
{
    DECLARE_CLASSNAME(MgDwfSectionResourceNotFoundException)

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgDwfSectionResourceNotFoundException object.
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
    MgDwfSectionResourceNotFoundException(CREFSTRING methodName, INT32 lineNumber,
        CREFSTRING fileName, MgStringCollection* whatArguments,
        CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor for a MgDwfSectionResourceNotFoundException object.
    ///
    virtual ~MgDwfSectionResourceNotFoundException() throw();

INTERNAL_API:

    DECLARE_EXCEPTION_DEFAULTS(MgDwfSectionResourceNotFoundException)

CLASS_ID:

    static const INT32 m_cls_id = MapGuide_Desktop_Exception_MgDwfSectionResourceNotFoundException;
};

#endif