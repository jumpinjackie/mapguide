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
 * $Id: NamespaceAxis.cpp,v 1.15 2006/11/27 17:51:17 amassari Exp $
 */

#include "../config/xqilla_config.h"
#include <xqilla/axis/NamespaceAxis.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ItemFactory.hpp>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

NamespaceAxis::NamespaceAxis(const LocationInfo *info, const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, const Node *nodeObj,
       const NodeTest *nodeTest, const AxisNodeFactory &factory)
  : Axis(info, contextNode, nodeObj, nodeTest, factory),
    node_(contextNode),
    originalNode_(contextNode),
    nodeMap_(node_->getAttributes()),
    i_(0),
	defNsTested_(false),
    defXmlNs_(false)
{
}

const DOMNode *NamespaceAxis::nextNode(DynamicContext *context)
{
  const DOMNode *result = 0;

  while(node_!=0 && result == 0) {
    if(nodeMap_ == 0 || i_ >= nodeMap_->getLength()) {
      node_ = XPath2NSUtils::getParent(node_);
      if(node_ != 0) {
        nodeMap_ = node_->getAttributes();
        i_ = 0;
        // check if this node has a namespace, but the prefix is not declared in the attribute map
        const XMLCh* uri=node_->getNamespaceURI();
        const XMLCh* prefix=node_->getPrefix();
        if(uri && *uri && 
           nodeMap_->getNamedItemNS(XMLUni::fgXMLNSString, prefix)==0 &&
           done_.insert(prefix).second)
        {
           return context->getItemFactory()->createNamespaceNode(prefix, uri, originalNode_, context);
        }
        continue;
      }
      else {
        nodeMap_ = 0;
        break;
      }
    }

    DOMNode *tmpAttr = nodeMap_->item(i_);
    ++i_;

    const XMLCh* attrName = tmpAttr->getNodeName();

    // Check to see if this attribute starts with xmlns
    if(!XMLString::startsWith(attrName, XMLUni::fgXMLNSString)) {
      continue;
    }

    // Get uri
    const XMLCh* uri = tmpAttr->getNodeValue();

    // Figure out prefix
    const XMLCh* prefix = 0;
    if(XMLString::stringLen(attrName) != 5) {
      // A prefix was given

      // If the name doesn't start with xmlns: (and its not xmlns) then skip it
      // XXX: Is this necessary/allowed?
      if(attrName[5] != chColon) {
        continue;
      }

      prefix = attrName + 6;
    }

    // If the uri is empty, the prefix has been unset; add it to the map of "already reported" prefixes and go on
    if(uri == 0 || *uri==0) {
      done_.insert(prefix);
      continue;
    }
 
    // Add namespace, if not already there
    if(done_.insert(prefix).second) {
      result = context->getItemFactory()->createNamespaceNode(prefix, uri, originalNode_, context);
    }
  }

  if(result==0 && !defNsTested_)
  {
    defNsTested_=true;
    if(context->getDefaultElementAndTypeNS()!=0 && done_.insert(0).second)
      result = context->getItemFactory()->createNamespaceNode(NULL, context->getDefaultElementAndTypeNS(), originalNode_, context);
  }
  if(result==0 && !defXmlNs_)
  {
	defXmlNs_=true;
    // Output the obligatory namespace node for the "xml" prefix
    result = context->getItemFactory()->createNamespaceNode(XMLUni::fgXMLString, XMLUni::fgXMLURIName, originalNode_, context);
  }
  return result;  
}

std::string NamespaceAxis::asString(DynamicContext *context, int indent) const
{
  return "NamespaceNodesResult";
}

