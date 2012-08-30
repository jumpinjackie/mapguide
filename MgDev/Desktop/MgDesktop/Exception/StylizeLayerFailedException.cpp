#include "MgDesktop.h"

IMPLEMENT_EXCEPTION_DEFAULTS(MgdStylizeLayerFailedException, MgApplicationException)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct a MgdStylizeLayerFailedException object.
///
MgdStylizeLayerFailedException::MgdStylizeLayerFailedException(CREFSTRING methodName,
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
MgdStylizeLayerFailedException::~MgdStylizeLayerFailedException() throw()
{
}
