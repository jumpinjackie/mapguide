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
 * $Id: XQDocumentConstructor.cpp,v 1.1 2006/11/03 17:04:20 jpcs Exp $
 */

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/ast/XQDocumentConstructor.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/exceptions/ASTException.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/items/Node.hpp>

#include <xercesc/dom/DOM.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

XQDocumentConstructor::XQDocumentConstructor(ASTNode *value, XPath2MemoryManager* mm)
  : XQDOMConstructor(mm),
    m_value(value)
{
  setType(ASTNode::DOM_CONSTRUCTOR);
}

static bool isTextNode(const Node::Ptr &node)
{
  return node->dmNodeKind() == Node::text_string;
}

Sequence XQDocumentConstructor::collapseTreeInternal(DynamicContext *context, int flags) const 
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  try
  {
    std::vector<Node::Ptr> childList;

    Result oneChild = m_value->collapseTree(context);
    Item::Ptr child;
    bool lastWasAtomic = false;
    while((child = oneChild->next(context)) != NULLRCP)
    {
      if(child->isNode())
      {
        lastWasAtomic = false;
        Node::Ptr sourceNode=(Node::Ptr)child;
        // If the content sequence contains an attribute node following a node that is not an attribute node, a type
        // error is raised [err:XPTY0004].
        if(sourceNode->dmNodeKind()==Node::attribute_string)
          XQThrow(ASTException,X("DOM Constructor"),
                  X("An attribute node cannot be a child of a document [err:XPTY0004]."));
        // If the content sequence contains a document node, the document node is replaced in the content 
        // sequence by its children.
        else if(sourceNode->dmNodeKind()==Node::document_string) {
          Result children = sourceNode->dmChildren(context, this);
          Node::Ptr childNode;
          while((childNode = children->next(context)).notNull()) {
            // merge consecutive text nodes
            if(isTextNode(childNode) && !childList.empty() && isTextNode(childList.back())) {
              const XMLCh* buff=XPath2Utils::concatStrings(childList.back()->dmStringValue(context),
                                                           childNode->dmStringValue(context), mm);
              childList.pop_back();
              childList.push_back(context->getItemFactory()->createTextNode(buff, context));
            }
            else
              childList.push_back(childNode);
          }
        }
        // Adjacent text nodes in the content sequence are merged into a single text node by concatenating 
        // their contents, with no intervening blanks. After concatenation, any text node whose content is 
        // a zero-length string is deleted from the content sequence.
        else if(isTextNode(sourceNode) && !childList.empty() && isTextNode(childList.back())) {
          const XMLCh* buff=XPath2Utils::concatStrings(childList.back()->dmStringValue(context),
                                                       sourceNode->dmStringValue(context), mm);
          childList.pop_back();
          childList.push_back(context->getItemFactory()->createTextNode(buff, context));
        }
        else {
          // if it's a text node, ensure it's not empty
          if(!isTextNode(sourceNode) || !XPath2Utils::equals(sourceNode->dmStringValue(context),0))
            childList.push_back(sourceNode);
        }
      }
      else
      {
        const XMLCh* valueStr=child->asString(context);
        if(lastWasAtomic)
        {
          XMLCh space[]={ ' ', 0 };
          valueStr=XPath2Utils::concatStrings(space,valueStr,mm);
        }
        lastWasAtomic = true;
        // empty strings are stripped
        if(valueStr && *valueStr)
        {
          if(!childList.empty() && isTextNode(childList.back())) {
            const XMLCh* buff=XPath2Utils::concatStrings(childList.back()->dmStringValue(context),valueStr,mm);

            childList.pop_back();
            childList.push_back(context->getItemFactory()->createTextNode(buff, context));
          }
          else {
            childList.push_back(context->getItemFactory()->createTextNode(valueStr, context));
          }
        }
      }
    }

    return Sequence(context->getItemFactory()->createDocumentNode(childList, context), mm);
  }
  catch(DOMException& e) {
    XQThrow(ASTException,X("DocumentConstructor"),e.getMessage());
  }
}

ASTNode* XQDocumentConstructor::staticResolution(StaticContext *context)
{
  m_value = m_value->staticResolution(context);
  return this;
}

ASTNode* XQDocumentConstructor::staticTyping(StaticContext *context)
{
  _src.clear();

  m_value = m_value->staticTyping(context);
  _src.add(m_value->getStaticResolutionContext());

  _src.getStaticType().flags = StaticType::DOCUMENT_TYPE;
  _src.forceNoFolding(true);
  _src.creative(true);
  return this; // Never constant fold
}

const XMLCh* XQDocumentConstructor::getNodeType() const
{
  return Node::document_string;
}

const ASTNode *XQDocumentConstructor::getValue() const
{
  return m_value;
}

void XQDocumentConstructor::setValue(ASTNode *value)
{
  m_value = value;
}

