#include "MgDesktop.h"

IMPLEMENT_EXCEPTION_DEFAULTS(MgdDwfException, MgThirdPartyException)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct a MgdDwfException object.
///
MgdDwfException::MgdDwfException(CREFSTRING methodName,
    INT32 lineNumber, CREFSTRING fileName, MgStringCollection* whatArguments,
    CREFSTRING whyMessageId, MgStringCollection* whyArguments) throw() :
    MgThirdPartyException(methodName, lineNumber, fileName,
        whatArguments, whyMessageId, whyArguments)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgdDwfException::~MgdDwfException() throw()
{
}
