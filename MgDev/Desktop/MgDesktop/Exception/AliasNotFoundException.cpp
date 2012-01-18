#include "MgDesktop.h"

IMPLEMENT_EXCEPTION_DEFAULTS(MgAliasNotFoundException, MgApplicationException)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct a MgConnectionFailedException object.
///
MgAliasNotFoundException::MgAliasNotFoundException(CREFSTRING methodName,
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
MgAliasNotFoundException::~MgAliasNotFoundException() throw()
{
}
