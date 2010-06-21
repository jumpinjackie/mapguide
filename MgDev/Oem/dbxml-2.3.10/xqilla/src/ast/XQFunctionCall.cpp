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
 * $Id: XQFunctionCall.cpp,v 1.12 2006/11/01 16:37:18 jpcs Exp $
 */

//////////////////////////////////////////////////////////////////////
// XQFunctionCall.cpp: implementation of the XQFunctionCall class.
//////////////////////////////////////////////////////////////////////

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/ast/XQFunctionCall.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/exceptions/StaticErrorException.hpp>
#include <xqilla/parser/QName.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/framework/XMLBuffer.hpp>

////////////////////////////////////////////////////////////////////////////////////////
//
//

XQFunctionCall::XQFunctionCall(QualifiedName* qname, const VectorOfASTNodes &args, XPath2MemoryManager* expr)
  : ASTNodeImpl(expr),
  _args(args),
  _qname(qname)
{
    setType(ASTNode::FUNCTION_CALL);
}

Result XQFunctionCall::createResult(DynamicContext* ctx, int flags) const
{
	XQThrow(FunctionException,X("XQFunctionCall::collapseTreeInternal"), X("staticResolution has not been called!!!"));
}

ASTNode* XQFunctionCall::staticResolution(StaticContext *context) 
{
  ASTNode* functionImpl=context->lookUpFunction(_qname->getPrefix(),_qname->getName(), _args, this);

  if(functionImpl==NULL) {
    // get the uri for debugging purposes
    const XMLCh* uri = context->getUriBoundToPrefix(_qname->getPrefix(), this);

    if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(uri) == 0 && XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(_qname->getPrefix()) == 0)
      uri=context->getDefaultFuncNS();

    XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer buf;
    buf.set(X("Function '"));
    buf.append(_qname->getName());
    buf.append(X("' in namespace '"));
    if(uri)
      buf.append(uri);
    buf.append(X("' with "));
    XMLCh szNumBuff[20];
    XERCES_CPP_NAMESPACE_QUALIFIER XMLString::binToText((unsigned long)_args.size(), szNumBuff, 19, 10);
	buf.append(szNumBuff);
    buf.append(X(" argument is undefined [err:XPST0017]"));

    //cerr << "reason1: " << XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(buf.getRawBuffer()) << endl;
    XQThrow(StaticErrorException, X("XQFunctionCall::staticResolution"), buf.getRawBuffer());
  }
  functionImpl->setLocationInfo(this);
  return functionImpl->staticResolution(context);
}

ASTNode *XQFunctionCall::staticTyping(StaticContext *context)
{
  // Should never happen
  return this;
}

const QualifiedName *XQFunctionCall::getName() const
{
  return _qname;
}

const VectorOfASTNodes &XQFunctionCall::getArguments() const
{
  return _args;
}
