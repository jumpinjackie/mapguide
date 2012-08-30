#include "MgDesktop.h"

IMPLEMENT_EXCEPTION_DEFAULTS(MgdParameterNotFoundException, MgApplicationException)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct a MgdParameterNotFoundException object.
///
MgdParameterNotFoundException::MgdParameterNotFoundException(CREFSTRING methodName,
    INT32 lineNumber, CREFSTRING fileName, MgStringCollection* whatArguments,
    CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw() :
    MgApplicationException(methodName, lineNumber, fileName,
        whatArguments, whyMessageId, whyArguments)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the MgdParameterNotFoundException object.
///
MgdParameterNotFoundException::~MgdParameterNotFoundException() throw()
{
}
