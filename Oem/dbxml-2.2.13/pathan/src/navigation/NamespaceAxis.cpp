/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/internal/navigation/NamespaceAxis.hpp>
#include <pathan/XPath2NSUtils.hpp>
#include "../dom-extensions/impl/XPathNamespaceImpl.hpp"

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

NamespaceAxis::NamespaceAxis(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, const Node *nodeObj,
       const NodeTest *nodeTest, DynamicContext *context, const AxisNodeFactory &factory)
  : Axis(contextNode, nodeObj, nodeTest, context, factory),
    node_(contextNode),
    nodeMap_(node_->getAttributes()),
    i_(0)
{
}

const DOMNode *NamespaceAxis::nextNode()
{
  const DOMNode *result = 0;

  if(toDo_) {
    // Output the obligatory namespace node for the "xml" prefix
    toDo_ = false;
    result = XPathNamespaceImpl::createXPathNamespace(XMLUni::fgXMLString, XMLUni::fgXMLURIName, static_cast<DOMElement*>(const_cast<DOMNode*>(node_)));
  }

  while(result == 0) {
    if(nodeMap_ == 0 || i_ >= nodeMap_->getLength()) {
      node_ = XPath2NSUtils::getParent(node_);
      if(node_ != 0) {
        nodeMap_ = node_->getAttributes();
        i_ = 0;
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

    // If the prefix wasn't empty, then the uri can't be empty
    // In other words, consider the case of "" being a valid uri for a default binding
    // when namespace 1.1 comes in this is not true!!!!REVISIT
    if(prefix != 0 && uri == 0) {
      continue;
    }
 
    // Add namespace, if not already there
    if(done_.insert(prefix).second) {
      result = XPathNamespaceImpl::createXPathNamespace(prefix, uri, static_cast<DOMElement*>(const_cast<DOMNode*>(node_)));
    }
  }

  return result;  
}

std::string NamespaceAxis::asString(DynamicContext *context, int indent) const
{
  return "NamespaceNodesResult";
}

