/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _DOMSERIALISE_HPP
#define _DOMSERIALISE_HPP


#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

class Item;

/** Outputs a const XMLCh *to the specified stream, after transcoding it
    to the local codepage. **/
std::ostream& operator<<(std::ostream& target, const XMLCh *toWrite);

/** Outputs a XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *and all of its child nodes by serialising them
    to the specified stream, including transcoding to the local
    codepage. **/
std::ostream& operator<<(std::ostream& target, const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *toWrite);

std::ostream& operator<<(std::ostream& target, const XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList *toWrite);

// can't serialize Items directly anymore, since we need the context.  
// Use operator<<(target, toWrite->asString(context)) instead
// std::ostream& operator<<(std::ostream& target, const Item::Ptr toWrite);

#endif // _DOMSERIALISE_HPP
