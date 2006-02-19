/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/internal/navigation/Axis.hpp>
#include <pathan/internal/navigation/NodeTest.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/XPath2NSUtils.hpp>
#include <pathan/SequenceType.hpp>
#include <pathan/DynamicContext.hpp>
#include "../dom-extensions/impl/XPathNamespaceImpl.hpp"

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

AxisNodeFactoryTemplate<NodeImpl> Axis::gNodeImplFactory = AxisNodeFactoryTemplate<NodeImpl>();

Axis::Axis(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *contextNode, const Node *nodeObj,
	const NodeTest *nodeTest, DynamicContext *context, const AxisNodeFactory &factory)
  : ResultImpl(context),
    nodeObj_(nodeObj),
    factory_(factory),
    contextNode_(contextNode),
    nodeTest_(nodeTest),
    toDo_(true)
{
}

Item::Ptr Axis::next(DynamicContext *context)
{
  const DOMNode *node = 0;
  while((node = nextNode()) != 0)
  {
    if(nodeTest_ == 0) return factory_.createNode(node, context);

    SequenceType::ItemType *itemType = nodeTest_->getItemType();
    if(itemType != 0)
    {
      Node::Ptr result = factory_.createNode(node, context);
      if(itemType->matches(result, context)) {
        return result;
      }
    }
    else
    {
      switch(node->getNodeType())
      {      
        case DOMNode::DOCUMENT_NODE: {
          if(!nodeTest_->getTypeWildcard() && nodeTest_->getNodeType() != Node::document_string) continue;
          if(!nodeTest_->getNameWildcard() || !nodeTest_->getNamespaceWildcard()) continue;
          break;
        }
        case DOMNode::ELEMENT_NODE: {
          if(!nodeTest_->getTypeWildcard() && nodeTest_->getNodeType() != Node::element_string) continue;
          if(!nodeTest_->getNameWildcard() && !XPath2Utils::equals(node->getLocalName(), nodeTest_->getNodeName())) continue;
          if(!nodeTest_->getNamespaceWildcard())
          {
              const XMLCh *uri = nodeTest_->getNodeUri();
              if(uri == 0)
                uri = context->getDefaultElementAndTypeNS();
              if(!XPath2Utils::equals(node->getNamespaceURI(), uri)) continue;
          }
          break;
        }
        case DOMNode::ATTRIBUTE_NODE: {
          if(nodeTest_->getTypeWildcard()) { if(nodeTest_->getHasChildren()) continue; }
          else if(nodeTest_->getNodeType() != Node::attribute_string) continue;
          if(!nodeTest_->getNameWildcard() && !XPath2Utils::equals(node->getLocalName(), nodeTest_->getNodeName())) continue;
          if(!nodeTest_->getNamespaceWildcard() && !XPath2Utils::equals(node->getNamespaceURI(), nodeTest_->getNodeUri())) continue;
          break;
        }
        case DOMNode::CDATA_SECTION_NODE:
        case DOMNode::TEXT_NODE: {
          if(nodeTest_->getTypeWildcard()) { if(nodeTest_->getHasChildren()) continue; }
          else if(nodeTest_->getNodeType() != Node::text_string) continue;
          if(!nodeTest_->getNameWildcard() || !nodeTest_->getNamespaceWildcard()) continue;
          break;
        }
        case DOMNode::PROCESSING_INSTRUCTION_NODE: {
          if(nodeTest_->getTypeWildcard()) { if(nodeTest_->getHasChildren()) continue; }
          else if(nodeTest_->getNodeType() != Node::processing_instruction_string) continue;
          if(!nodeTest_->getNameWildcard() && !XPath2Utils::equals(node->getNodeName(), nodeTest_->getNodeName())) continue;
          if(!nodeTest_->getNamespaceWildcard()) continue;
          break;
        }
        case DOMNode::COMMENT_NODE: {
          if(nodeTest_->getTypeWildcard()) { if(nodeTest_->getHasChildren()) continue; }
          else if(nodeTest_->getNodeType() != Node::comment_string) continue;
          if(!nodeTest_->getNameWildcard() || !nodeTest_->getNamespaceWildcard()) continue;
          break;
        }
        case DOMXPathNamespace::XPATH_NAMESPACE_NODE: {
          if(nodeTest_->getTypeWildcard()) { if(nodeTest_->getHasChildren()) continue; }
          else if(nodeTest_->getNodeType() != Node::namespace_string) continue;
          if(!nodeTest_->getNameWildcard() && !XPath2Utils::equals(node->getPrefix(), nodeTest_->getNodeName())) continue;
          if(!nodeTest_->getNamespaceWildcard() && !XPath2Utils::equals(XMLUni::fgZeroLenString, nodeTest_->getNodeUri())) continue;
          break;
        }
        default: {
          continue;
        }
      }

      return factory_.createNode(node, context);
    }
    
  }

  return 0;
}

const DOMNode *Axis::getParent(const DOMNode *fNode)
{
  DOMNode *parent = XPath2NSUtils::getParent(fNode);

  // Skip out of entity reference nodes
  while(parent != 0 && parent->getNodeType() == DOMNode::ENTITY_REFERENCE_NODE) {
    parent = parent->getParentNode();
  }

  return parent;
}

const DOMNode *Axis::getFirstChild(const DOMNode *fNode)
{
  if(fNode->getNodeType() == DOMNode::ATTRIBUTE_NODE)
    return 0;

  DOMNode *result = fNode->getFirstChild();
  if(result == 0) return 0;

  // Skip into the contents of entity reference nodes
  while(result->getNodeType() == DOMNode::ENTITY_REFERENCE_NODE) {
    result = result->getFirstChild();
  }

  // Skip any other unused types
  while(result->getNodeType() == DOMNode::DOCUMENT_TYPE_NODE) {
    result = result->getNextSibling();
  }

  return result;
}

const DOMNode *Axis::getLastChild(const DOMNode *fNode)
{
  if(fNode->getNodeType() == DOMNode::ATTRIBUTE_NODE)
    return 0;

  DOMNode *result = fNode->getLastChild();
  if(result == 0) return 0;

  // Skip into the contents of entity reference nodes
  while(result->getNodeType() == DOMNode::ENTITY_REFERENCE_NODE) {
    result = result->getLastChild();
  }

  // Skip any other unused types
  while(result->getNodeType() == DOMNode::DOCUMENT_TYPE_NODE) {
    result = result->getPreviousSibling();
  }

  return result;
}

const DOMNode *Axis::getNextSibling(const DOMNode *fNode)
{
  DOMNode *result = fNode->getNextSibling();

  if(result == 0) {
    // Skip out of the contents of entity reference nodes
    DOMNode *parent = fNode->getParentNode();
    while(result == 0 && parent != 0 && parent->getNodeType() == DOMNode::ENTITY_REFERENCE_NODE) {
      result = parent->getNextSibling();
      parent = parent->getParentNode();
    }
  }

  // Skip any other unused types
  while(result != 0 && result->getNodeType() == DOMNode::DOCUMENT_TYPE_NODE) {
    result = result->getNextSibling();
  }

  return result;
}

const DOMNode *Axis::getPreviousSibling(const DOMNode *fNode)
{
  DOMNode *result = fNode->getPreviousSibling();

  if(result == 0) {
    // Skip out of the contents of entity reference nodes
    DOMNode *parent = fNode->getParentNode();
    while(result == 0 && parent != 0 && parent->getNodeType() == DOMNode::ENTITY_REFERENCE_NODE) {
      result = parent->getPreviousSibling();
      parent = parent->getParentNode();
    }
  }

  // Skip any other unused types
  while(result != 0 && result->getNodeType() == DOMNode::DOCUMENT_TYPE_NODE) {
    result = result->getPreviousSibling();
  }

  return result;
}

