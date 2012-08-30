#include "MgDesktop.h"

IMPLEMENT_EXCEPTION_DEFAULTS(MgdDuplicateParameterException, MgApplicationException)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct a MgdDuplicateParameterException object.
///
MgdDuplicateParameterException::MgdDuplicateParameterException(CREFSTRING methodName,
    INT32 lineNumber, CREFSTRING fileName, MgStringCollection* whatArguments,
    CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw() :
    MgApplicationException(methodName, lineNumber, fileName,
        whatArguments, whyMessageId, whyArguments)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the MgdDuplicateParameterException object.
///
MgdDuplicateParameterException::~MgdDuplicateParameterException() throw()
{
}
