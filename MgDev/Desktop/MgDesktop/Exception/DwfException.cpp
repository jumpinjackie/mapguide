#include "MgDesktop.h"

IMPLEMENT_EXCEPTION_DEFAULTS(MgDwfException, MgThirdPartyException)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct a MgDwfException object.
///
MgDwfException::MgDwfException(CREFSTRING methodName,
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
MgDwfException::~MgDwfException() throw()
{
}
