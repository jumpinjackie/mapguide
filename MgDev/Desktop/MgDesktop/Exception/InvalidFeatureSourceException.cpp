#include "MgDesktop.h"

IMPLEMENT_EXCEPTION_DEFAULTS(MgdInvalidFeatureSourceException, MgApplicationException)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct a MgdInvalidFeatureSourceException object.
///
MgdInvalidFeatureSourceException::MgdInvalidFeatureSourceException(CREFSTRING methodName,
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
MgdInvalidFeatureSourceException::~MgdInvalidFeatureSourceException() throw()
{
}
