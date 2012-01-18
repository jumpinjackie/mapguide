#include "MgDesktop.h"

IMPLEMENT_EXCEPTION_DEFAULTS(MgInvalidFeatureSourceException, MgApplicationException)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct a MgInvalidFeatureSourceException object.
///
MgInvalidFeatureSourceException::MgInvalidFeatureSourceException(CREFSTRING methodName,
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
MgInvalidFeatureSourceException::~MgInvalidFeatureSourceException() throw()
{
}
