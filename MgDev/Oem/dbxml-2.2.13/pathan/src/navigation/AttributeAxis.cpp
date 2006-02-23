/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/internal/navigation/AttributeAxis.hpp>
#include <pathan/internal/navigation/NodeTest.hpp>

#include <xercesc/util/XMLUniDefs.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

AttributeAxis::AttributeAxis(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, const Node *nodeObj,
       const NodeTest *nodeTest, DynamicContext *context, const AxisNodeFactory &factory)
  : Axis(contextNode, nodeObj, nodeTest, context, factory),
    nodeMap_(contextNode->getAttributes()),
    i_(0)
{
}

const DOMNode *AttributeAxis::nextNode()
{
  const DOMNode *result = 0;
  if(nodeMap_ != 0) {
    unsigned int nLen=nodeMap_->getLength();
    if(i_==0 && nodeTest_!=0 && nodeTest_->getItemType()==0 && 
       !nodeTest_->getNamespaceWildcard() && !nodeTest_->getNameWildcard())
    {
      const XMLCh* uri=nodeTest_->getNodeUri();
      if(uri==0)
        result=nodeMap_->getNamedItem(nodeTest_->getNodeName());
      else
        result=nodeMap_->getNamedItemNS(uri,nodeTest_->getNodeName());
      i_=nLen;
    }
    while(result == 0 && i_ < nLen) {
      result = nodeMap_->item(i_);
      ++i_;
      //Check to see if this attribute starts with "xmlns:" or is equal to "xmlns"
      const XMLCh* name=result->getNodeName();
      const XMLCh* xmlns=XMLUni::fgXMLNSString;
      while(*name && *xmlns)
      {
        if(*name++!=*xmlns++)
          break;
      }
      if(*xmlns==0 && (*name==0 || *name==chColon))
        result = 0;
    }
  }

  return result;
}

std::string AttributeAxis::asString(DynamicContext *context, int indent) const
{
  return "AttributeAxis";
}

