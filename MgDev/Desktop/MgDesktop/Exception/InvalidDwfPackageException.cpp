#include "MgDesktop.h"

IMPLEMENT_EXCEPTION_DEFAULTS(MgInvalidDwfPackageException, MgApplicationException)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct a MgInvalidDwfPackageException object.
///
MgInvalidDwfPackageException::MgInvalidDwfPackageException(CREFSTRING methodName,
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
MgInvalidDwfPackageException::~MgInvalidDwfPackageException() throw()
{
}
