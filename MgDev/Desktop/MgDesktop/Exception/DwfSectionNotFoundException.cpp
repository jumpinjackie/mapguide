#include "MgDesktop.h"

IMPLEMENT_EXCEPTION_DEFAULTS(MgdDwfSectionNotFoundException, MgApplicationException)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct a MgdDwfSectionNotFoundException object.
///
MgdDwfSectionNotFoundException::MgdDwfSectionNotFoundException(CREFSTRING methodName,
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
MgdDwfSectionNotFoundException::~MgdDwfSectionNotFoundException() throw()
{
}
