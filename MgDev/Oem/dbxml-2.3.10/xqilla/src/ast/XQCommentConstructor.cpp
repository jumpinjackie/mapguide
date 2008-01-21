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
 * $Id: XQCommentConstructor.cpp,v 1.1 2006/11/03 17:04:20 jpcs Exp $
 */

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/ast/XQCommentConstructor.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/exceptions/ASTException.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/ast/XQAtomize.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLChar.hpp>
#include <xercesc/framework/XMLBuffer.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

XQCommentConstructor::XQCommentConstructor(ASTNode *value, XPath2MemoryManager* mm)
  : XQDOMConstructor(mm),
    m_value(value)
{
  setType(ASTNode::DOM_CONSTRUCTOR);
}

Sequence XQCommentConstructor::collapseTreeInternal(DynamicContext *context, int flags) const 
{
  Node::Ptr result;
  try
  {
    XMLBuffer value;
    getStringValue(m_value, value, context);

    // Check for two dashes in a row, or a dash at the end
    bool foundDash = false;
    const XMLCh *ptr = value.getRawBuffer();
    const XMLCh *end = ptr + value.getLen();
    while(ptr != end) {
      if(*ptr == chDash) {
        if(foundDash) break;
        foundDash = true;
      }
      else foundDash = false;
      ++ptr;
    }
    if(foundDash)
      XQThrow(ASTException,X("DOM Constructor"),X("It is a dynamic error if the result of the content expression of "
                                                  "a computed comment constructor contains two adjacent hyphens or "
                                                  "ends with a hyphen. [err:XQDY0072]"));

    result = context->getItemFactory()->createCommentNode(value.getRawBuffer(), context);
  }
  catch(DOMException& e) {
    XQThrow(ASTException,X("DOM Constructor"),e.getMessage());
  }
  if(result.notNull())
    return Sequence(result, context->getMemoryManager());
  return Sequence(context->getMemoryManager());
}

ASTNode* XQCommentConstructor::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  m_value = new (mm) XQAtomize(m_value, mm);
  m_value->setLocationInfo(this);

  m_value = m_value->staticResolution(context);

  return this;
}

ASTNode* XQCommentConstructor::staticTyping(StaticContext *context)
{
  _src.clear();

  m_value = m_value->staticTyping(context);
  _src.add(m_value->getStaticResolutionContext());

  _src.getStaticType().flags = StaticType::COMMENT_TYPE;
  _src.forceNoFolding(true);
  _src.creative(true);
  return this; // Never constant fold
}

const XMLCh* XQCommentConstructor::getNodeType() const
{
  return Node::comment_string;
}

const ASTNode *XQCommentConstructor::getValue() const
{
  return m_value;
}

void XQCommentConstructor::setValue(ASTNode *value)
{
  m_value = value;
}

