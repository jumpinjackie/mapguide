//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQFunctionCall.cpp,v 1.13 2005/07/15 15:26:19 alby Exp $
//

//////////////////////////////////////////////////////////////////////
// XQFunctionCall.cpp: implementation of the XQFunctionCall class.
//////////////////////////////////////////////////////////////////////

#include "xquery/XQEngine.hpp"
#include "xquery/XQContext.hpp"
#include "xquery/dataItem/XQFunctionCall.hpp"
#include <pathan/exceptions/FunctionException.hpp>
#include <pathan/exceptions/StaticErrorException.hpp>
#include <pathan/internal/parser/QName.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/DynamicContext.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/framework/XMLBuffer.hpp>

////////////////////////////////////////////////////////////////////////////////////////
//
//

XQFunctionCall::XQFunctionCall(QualifiedName* qname, const VectorOfDataItems &args, XPath2MemoryManager* expr)
  : DataItemImpl(expr),
  _args(args),
  _qname(qname)
{
    setType((DataItem::whichType)XQContext::FUNCTION_CALL);
}

Result XQFunctionCall::createResult(DynamicContext* ctx, int flags) const
{
	DSLthrow(FunctionException,X("XQFunctionCall::collapseTreeInternal"), X("staticResolution has not been called!!!"));
}

DataItem* XQFunctionCall::staticResolution(StaticContext *context) 
{
  DataItem* functionImpl=context->lookUpFunction(_qname->getPrefix(),_qname->getName(), _args);
  if(functionImpl==NULL) {
    // get the uri for debugging purposes
    const XMLCh* uri = context->getUriBoundToPrefix(_qname->getPrefix());

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
    XERCES_CPP_NAMESPACE_QUALIFIER XMLString::binToText(_args.size(), szNumBuff, 19, 10);
    buf.append(szNumBuff);
    buf.append(X(" argument is undefined [err:XPST0017]"));

    //cerr << "reason1: " << XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(buf.getRawBuffer()) << endl;
    DSLthrow(StaticErrorException, X("XQFunctionCall::staticResolution"), buf.getRawBuffer());
  }
  functionImpl->addPredicates(getPredicates());
	return functionImpl->staticResolution(context);
}

const QualifiedName *XQFunctionCall::getName() const
{
  return _qname;
}

const VectorOfDataItems &XQFunctionCall::getArguments() const
{
  return _args;
}
