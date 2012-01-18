#include "MgDesktop.h"

IMPLEMENT_EXCEPTION_DEFAULTS(MgInvalidDwfSectionException, MgApplicationException)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct a MgInvalidDwfSectionException object.
///
MgInvalidDwfSectionException::MgInvalidDwfSectionException(CREFSTRING methodName,
    INT32 lineNumber, CREFSTRING fileName, MgStringCollection* whatArguments,
    CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw() :
    MgApplicationException(methodName, lineNumber, fileName,
        whatArguments, whyMessageId, whyArguments)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgInvalidDwfSectionException::~MgInvalidDwfSectionException() throw()
{
}
