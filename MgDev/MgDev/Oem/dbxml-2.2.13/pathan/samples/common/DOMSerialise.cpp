/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 *  Please see LICENSE.TXT for more information.
 */


#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <pathan/XPath2Utils.hpp>
#include "DOMSerialise.hpp"
#include <assert.h>
#include <pathan/Item.hpp>

using namespace std;

ostream & operator << (ostream & target, const XMLCh * toWrite)
{
  target << XPath2Utils::StringToUTF8(toWrite).c_str();

  return target;
}

ostream & operator << (ostream & target, XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList * toWrite)
{
  for (unsigned int i = 0; i < toWrite->getLength (); i++)
    {
      target << i << ") ";
      XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *temp = toWrite->item (i);
      target << temp;
    }
  return target;
}

ostream & operator << (ostream & target, const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * toWrite)
{
	//this no longer works
	//  target << XPath2Utils::serialiseNode(toWrite);
  return target;
}

