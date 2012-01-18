#include "MgDesktop.h"

IMPLEMENT_EXCEPTION_DEFAULTS(MgDwfSectionNotFoundException, MgApplicationException)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct a MgDwfSectionNotFoundException object.
///
MgDwfSectionNotFoundException::MgDwfSectionNotFoundException(CREFSTRING methodName,
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
MgDwfSectionNotFoundException::~MgDwfSectionNotFoundException() throw()
{
}
