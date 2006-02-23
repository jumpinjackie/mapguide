/*
 * Copyright (c) 2002, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _URIRESOLVER_HPP
#define _URIRESOLVER_HPP

#include <pathan/Pathan.hpp>
#include <xercesc/util/XercesDefs.hpp>

class DynamicContext;
class Sequence;

XERCES_CPP_NAMESPACE_BEGIN
class DOMDocument;
class DOMNodeList;
XERCES_CPP_NAMESPACE_END

/** 
 * This is an abstract class used to resolve URIs in different ways
**/
class PATHAN_EXPORT URIResolver {
  public:
    /* virtual destructor, does nothing */
    virtual ~URIResolver() {};      

    /* Resolve the given uri (and baseUri) to a Sequence (reference parameter). If the uri
       is relative, the base uri can be obtained from the context. If the uri is not handled
       by this URIResolver, returns false, otherwise returns true. */
    virtual bool resolveDocument(Sequence &result, const XMLCh* uri, DynamicContext* context) = 0;

    /* Resolve the given uri (and baseUri) to a Sequence (reference parameter). If the uri
       is relative, the base uri can be obtained from the context. If the uri is not handled
       by this URIResolver, returns false, otherwise returns true. */
    virtual bool resolveCollection(Sequence &result, const XMLCh* uri, DynamicContext* context) = 0;

    /* Resolve the default collection. If it is not defined, returns false, otherwise returns true. */
    virtual bool resolveDefaultCollection(Sequence &result, DynamicContext* context) = 0;
};
#endif // _URIRESOLVER_HPP
