#include "MgDesktop.h"

IMPLEMENT_EXCEPTION_DEFAULTS(MgDuplicateParameterException, MgApplicationException)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct a MgDuplicateParameterException object.
///
MgDuplicateParameterException::MgDuplicateParameterException(CREFSTRING methodName,
    INT32 lineNumber, CREFSTRING fileName, MgStringCollection* whatArguments,
    CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw() :
    MgApplicationException(methodName, lineNumber, fileName,
        whatArguments, whyMessageId, whyArguments)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the MgDuplicateParameterException object.
///
MgDuplicateParameterException::~MgDuplicateParameterException() throw()
{
}
