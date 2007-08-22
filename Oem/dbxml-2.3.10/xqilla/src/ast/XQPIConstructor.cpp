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
 * $Id: XQPIConstructor.cpp,v 1.1 2006/11/03 17:04:20 jpcs Exp $
 */

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/ast/XQPIConstructor.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/exceptions/ASTException.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/ast/XQAtomize.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <xercesc/util/XMLChar.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

XQPIConstructor::XQPIConstructor(ASTNode* name, ASTNode* value, XPath2MemoryManager* mm)
  : XQDOMConstructor(mm),
    m_name(name),
    m_value(value)
{
  setType(ASTNode::DOM_CONSTRUCTOR);
}

Sequence XQPIConstructor::collapseTreeInternal(DynamicContext *context, int flags) const 
{
  Node::Ptr result;
  try
  {
    Result resName=m_name->collapseTree(context);
    AnyAtomicType::Ptr itemName=resName->next(context);
    if(itemName==NULLRCP || resName->next(context)!=NULLRCP)
      XQThrow(ASTException,X("DOM Constructor"),X("The target for the processing instruction must be a single "
                                                  "xs:NCName, xs:string or xs:untypedAtomic item [err:XPTY0004]"));

    const XMLCh* nodeName=NULL;
    // the specs specify that a xs:NCName could be returned, but we create a xs:string in that case
    if(itemName->getPrimitiveTypeIndex()==AnyAtomicType::STRING ||
       itemName->getPrimitiveTypeIndex()==AnyAtomicType::UNTYPED_ATOMIC)
    {
      nodeName=itemName->asString(context);
      if(!XMLChar1_0::isValidNCName(nodeName, XMLString::stringLen(nodeName)))
        XQThrow(ASTException,X("DOM Constructor"),
                X("The target for the processing instruction must be a valid xs:NCName [err:XQDY0041]"));
    }
    else
      XQThrow(ASTException,X("DOM Constructor"),X("The target for the processing instruction must be a single "
                                                  "xs:NCName, xs:string or xs:untypedAtomic item [err:XPTY0004]"));

    if(XMLString::compareIString(nodeName, XMLUni::fgXMLString)==0)
      XQThrow(ASTException,X("DOM Constructor"),
              X("The target for the processing instruction must not be 'XML' [err:XQDY0064]"));
    int nIndex=XMLString::indexOf(nodeName, chQuestion);
    int nTargetLen=XMLString::stringLen(nodeName);
    while(nIndex!=-1)
    {
      if((nIndex+1)<nTargetLen && nodeName[nIndex+1]==chCloseAngle)
        XQThrow(ASTException,X("DOM Constructor"),
                X("The target for the processing instruction must not contain the string '?>' [err:XQDY0026]"));
      nIndex=XMLString::indexOf(nodeName, chQuestion, nIndex+1);
    }

    if(nodeName==NULL || *nodeName==0)
      XQThrow(ASTException,X("DOM Constructor"),X("The name for the processing instruction is empty"));
    XMLBuffer value;
    getStringValue(m_value, value, context);

    // Check for the substring "?>"
    bool foundQ = false;
    const XMLCh *ptr = value.getRawBuffer();
    const XMLCh *end = ptr + value.getLen();
    while(ptr != end) {
      if(*ptr == chCloseAngle && foundQ)
        XQThrow(ASTException,X("DOM Constructor"),
                X("The content for the processing instruction must not contain the string '?>' [err:XQDY0026]"));
      else if(*ptr == chQuestion) foundQ = true;
      else foundQ = false;
      ++ptr;
    }

    const XMLCh* piContent=value.getRawBuffer();
    // remove leading whitespace
    while(XMLChar1_0::isWhitespace(*piContent)) piContent++;

    result = context->getItemFactory()->createPINode(nodeName, piContent, context);
  }
  catch(DOMException& e) {
    XQThrow(ASTException,X("PIConstructor"),e.getMessage());
  }
  if(result.notNull())
    return Sequence(result, context->getMemoryManager());
  return Sequence(context->getMemoryManager());
}

ASTNode* XQPIConstructor::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  m_name = new (mm) XQAtomize(m_name, mm);
  m_name->setLocationInfo(this);
  m_name = m_name->staticResolution(context);

  m_value = new (mm) XQAtomize(m_value, mm);
  m_value->setLocationInfo(this);
  m_value = m_value->staticResolution(context);

  return this;
}

ASTNode* XQPIConstructor::staticTyping(StaticContext *context)
{
  _src.clear();

  m_name = m_name->staticTyping(context);
  _src.add(m_name->getStaticResolutionContext());

  m_value = m_value->staticTyping(context);
  _src.add(m_value->getStaticResolutionContext());

  _src.getStaticType().flags = StaticType::PI_TYPE;
  _src.forceNoFolding(true);
  _src.creative(true);
  return this; // Never constant fold
}

const XMLCh* XQPIConstructor::getNodeType() const
{
  return Node::processing_instruction_string;
}

const ASTNode *XQPIConstructor::getName() const
{
  return m_name;
}

const ASTNode *XQPIConstructor::getValue() const
{
  return m_value;
}

void XQPIConstructor::setName(ASTNode *name)
{
  m_name = name;
}

void XQPIConstructor::setValue(ASTNode *value)
{
  m_value = value;
}

