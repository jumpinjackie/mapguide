/*
 * Copyright (c) 2001-2006
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2006
 *     Progress Software Corporation. All rights reserved.
 * Copyright (c) 2004-2006
 *     Oracle. All rights reserved.
 *
 * See the file LICENSE for redistribution information.
 *
 * $Id: XPath2NSUtils.cpp,v 1.7 2006/11/01 16:37:22 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/utils/XStr.hpp>

const XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *XPath2NSUtils::returnOwnerElement(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
  if(!node == 0) {
    //this is to ensure that we have an element node
    const XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *parent;
    if(node->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ATTRIBUTE_NODE) {
      parent = static_cast<const XERCES_CPP_NAMESPACE_QUALIFIER DOMAttr *>(node)->getOwnerElement();
    }
    else if(node->getNodeType() != XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE) {
      parent = static_cast<const XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *>(node->getParentNode());
    }
    else {
      parent = static_cast<const XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *>(node);
    }
    
    return parent;
  } else {
    return 0;
  }
}


const XMLCh* XPath2NSUtils::getLocalName(const XMLCh* name)
{
  unsigned int len=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(name);

  for(unsigned int i = 0; i < len; i++) {
    if(name[i] == XERCES_CPP_NAMESPACE_QUALIFIER chColon) {
      return name+i+1;
    }
  }
  return name;
}

const XMLCh* XPath2NSUtils::getLocalName(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{

  const XMLCh* result = node->getLocalName();
  if(result == NULL) {
    result = node->getNodeName();
  }
  return result;
}


const XMLCh* XPath2NSUtils::getPrefix(const XMLCh* name, XPath2MemoryManager* memMgr)
{
  unsigned int len=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(name);
  
  for(unsigned int i = 0; i < len; i++) {
    if(name[i] == XERCES_CPP_NAMESPACE_QUALIFIER chColon) {
      return XPath2Utils::subString(name, 0, i, memMgr);
    }
  }
  return XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString;
}

const XMLCh* XPath2NSUtils::getPrefix(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node,
                                      const DynamicContext* context)
{
  const XMLCh* result=node->getPrefix();
	
  if(result == 0) {
    result = context->getPrefixBoundToUri(node->getNamespaceURI());
  }

  return result;
}

const XMLCh* XPath2NSUtils::qualifyName(const XMLCh* prefix, const XMLCh* name, XPath2MemoryManager* memMgr)
{
  if (prefix != NULL) {
    XMLCh colon[2] = {XERCES_CPP_NAMESPACE_QUALIFIER chColon, XERCES_CPP_NAMESPACE_QUALIFIER chNull};
    return XPath2Utils::concatStrings(prefix, colon, name, memMgr);
  } else {
    return name;
  }
}

const XMLCh* XPath2NSUtils::qualifyName(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const DynamicContext* context)
{
  const XMLCh *prefix = XPath2NSUtils::getPrefix(node, context);

  if(prefix != 0 && *prefix!=0) {
    XMLCh colon[2] = {XERCES_CPP_NAMESPACE_QUALIFIER chColon, XERCES_CPP_NAMESPACE_QUALIFIER chNull};
    return XPath2Utils::concatStrings(prefix, colon, getLocalName(node), context->getMemoryManager());
  } else {
    return getLocalName(node);
  }
}

XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *XPath2NSUtils::getParent(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
  if(node->getNodeType() == XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ATTRIBUTE_NODE) {
    return (static_cast<const XERCES_CPP_NAMESPACE_QUALIFIER DOMAttr *>(node))->getOwnerElement();
  }
  else {
    return node->getParentNode();
  }
}

const XMLCh* XPath2NSUtils::getExpandedName(const XMLCh* namespaceURI, const XMLCh* name, XPath2MemoryManager* memMgr)
{
  XMLCh separator[2] = {XERCES_CPP_NAMESPACE_QUALIFIER chOpenAngle, XERCES_CPP_NAMESPACE_QUALIFIER chNull};
  return XPath2Utils::concatStrings(namespaceURI, separator, name, memMgr);
}


namespaceMapType XPath2NSUtils::createNamespaceMap(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, XPath2MemoryManager* memMgr)
{
	namespaceMapType namespaceMap;
	
	//If this isn't an element, then return nothing
	if(contextNode->getNodeType() != XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE) {
		return namespaceMap;
	}//if

	//We always add the default namespace
	namespaceMap[XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXMLString]=XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXMLURIName;

	const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *tmpNode = contextNode;
	while(tmpNode != 0) {
		//For each element, go through the attributes, and add any namespace bindings..
		addBindings(tmpNode, &namespaceMap, memMgr);

		tmpNode = tmpNode->getParentNode();
	}//while

  return namespaceMap;

}//createNamespaceMap


std::vector< std::pair< XMLCh*, XMLCh*> > XPath2NSUtils::getBindingsForNode(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) {

    std::vector< std::pair< XMLCh*, XMLCh*> > bindings;

    return bindings;

}

void XPath2NSUtils::addBindings(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *AttrNode, namespaceMapType *namespaceMap, XPath2MemoryManager* memMgr)
{
    //add in the prefix binding for this element
	
    const XMLCh* prefix = AttrNode->getPrefix();
    if(prefix != 0) {
        namespaceMapType::iterator tmpIter = namespaceMap->find(prefix);
        if(tmpIter == namespaceMap->end()) {
            (*namespaceMap)[prefix] = AttrNode->getNamespaceURI();
        }//if
    }


	XERCES_CPP_NAMESPACE_QUALIFIER DOMNamedNodeMap *attrs = AttrNode->getAttributes();

	if (attrs != 0) {
		for (unsigned int cnt = 0; cnt < attrs->getLength(); cnt++) {
			XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *tmpAttr = attrs->item(cnt);
			
			//Check to see if this attribute starts with xmlns
			const XMLCh* attrName = tmpAttr->getNodeName();

			if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::startsWith(attrName, XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXMLNSString) == false) {
				continue;
			}//if

			//Get uri
			const XMLCh* uri = tmpAttr->getNodeValue();

			//Figure out prefix
			const XMLCh* prefix = 0;
			if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(attrName) != 5) {
				//A prefix was given

				//If the name doesn't start with xmlns: (and its not xmlns) then skip it
				//XXX: Is this necessary/allowed?
				if(attrName[5] != XERCES_CPP_NAMESPACE_QUALIFIER chColon) {
					continue;
				}//if

				prefix = XPath2Utils::subString(attrName, 6, XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(attrName)-6, memMgr);
			}//if

			//If the prefix wasn't empty, then the uri can't be empty
			//In other words, consider the case of "" being a valid uri for a default binding
            //when namespace 1.1 comes in this is not true!!!!REVISIT
			if(prefix != 0 && uri == 0) {
				continue;
			}//if
 
			//Add namespace, if not already there / shadowed
			namespaceMapType::iterator tmpIter = namespaceMap->find(prefix);
			if(tmpIter == namespaceMap->end()) {
				(*namespaceMap)[prefix] = uri;
			}//if
		}//for
	}//if (attrs != 0)
}//addBindings
