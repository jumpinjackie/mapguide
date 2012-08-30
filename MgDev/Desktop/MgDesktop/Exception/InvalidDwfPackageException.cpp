#include "MgDesktop.h"

IMPLEMENT_EXCEPTION_DEFAULTS(MgdInvalidDwfPackageException, MgApplicationException)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct a MgdInvalidDwfPackageException object.
///
MgdInvalidDwfPackageException::MgdInvalidDwfPackageException(CREFSTRING methodName,
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
MgdInvalidDwfPackageException::~MgdInvalidDwfPackageException() throw()
{
}
