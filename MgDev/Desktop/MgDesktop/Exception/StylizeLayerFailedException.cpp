#include "MgDesktop.h"

IMPLEMENT_EXCEPTION_DEFAULTS(MgStylizeLayerFailedException, MgApplicationException)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct a MgStylizeLayerFailedException object.
///
MgStylizeLayerFailedException::MgStylizeLayerFailedException(CREFSTRING methodName,
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
MgStylizeLayerFailedException::~MgStylizeLayerFailedException() throw()
{
}
