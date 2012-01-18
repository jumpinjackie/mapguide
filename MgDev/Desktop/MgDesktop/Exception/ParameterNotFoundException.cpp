#include "MgDesktop.h"

IMPLEMENT_EXCEPTION_DEFAULTS(MgParameterNotFoundException, MgApplicationException)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct a MgParameterNotFoundException object.
///
MgParameterNotFoundException::MgParameterNotFoundException(CREFSTRING methodName,
    INT32 lineNumber, CREFSTRING fileName, MgStringCollection* whatArguments,
    CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw() :
    MgApplicationException(methodName, lineNumber, fileName,
        whatArguments, whyMessageId, whyArguments)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the MgParameterNotFoundException object.
///
MgParameterNotFoundException::~MgParameterNotFoundException() throw()
{
}
