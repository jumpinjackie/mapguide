#include "MgDesktop.h"

IMPLEMENT_EXCEPTION_DEFAULTS(MgdInvalidDwfSectionException, MgApplicationException)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct a MgdInvalidDwfSectionException object.
///
MgdInvalidDwfSectionException::MgdInvalidDwfSectionException(CREFSTRING methodName,
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
MgdInvalidDwfSectionException::~MgdInvalidDwfSectionException() throw()
{
}
