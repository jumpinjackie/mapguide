#include "MgDesktop.h"

IMPLEMENT_EXCEPTION_DEFAULTS(MgDwfSectionResourceNotFoundException, MgApplicationException)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct a MgDwfSectionResourceNotFoundException object.
///
MgDwfSectionResourceNotFoundException::MgDwfSectionResourceNotFoundException(CREFSTRING methodName,
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
MgDwfSectionResourceNotFoundException::~MgDwfSectionResourceNotFoundException() throw()
{
}
