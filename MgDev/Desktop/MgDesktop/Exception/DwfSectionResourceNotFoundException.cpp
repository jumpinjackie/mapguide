#include "MgDesktop.h"

IMPLEMENT_EXCEPTION_DEFAULTS(MgdDwfSectionResourceNotFoundException, MgApplicationException)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct a MgdDwfSectionResourceNotFoundException object.
///
MgdDwfSectionResourceNotFoundException::MgdDwfSectionResourceNotFoundException(CREFSTRING methodName,
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
MgdDwfSectionResourceNotFoundException::~MgdDwfSectionResourceNotFoundException() throw()
{
}
