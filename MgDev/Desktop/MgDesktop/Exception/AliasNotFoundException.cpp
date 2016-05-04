#include "MgDesktop.h"

IMPLEMENT_EXCEPTION_DEFAULTS(MgdAliasNotFoundException, MgApplicationException)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct a MgdConnectionFailedException object.
///
MgdAliasNotFoundException::MgdAliasNotFoundException(CREFSTRING methodName,
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
MgdAliasNotFoundException::~MgdAliasNotFoundException() throw()
{
}
