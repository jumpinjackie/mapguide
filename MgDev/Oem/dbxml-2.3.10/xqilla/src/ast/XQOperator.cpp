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
 * $Id: XQOperator.cpp,v 1.12 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <assert.h>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xqilla/ast/XQOperator.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/XPath2TypeCastException.hpp>
#include <xqilla/items/AnyAtomicType.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/items/DatatypeFactory.hpp>

XQOperator::XQOperator(const XMLCh* opName, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ASTNodeImpl(memMgr),
  _args(args)
{
  _opName=opName;
  setType(ASTNode::OPERATOR);
}

void XQOperator::addArgument(ASTNode* arg)
{
	_args.push_back(arg);
}

ASTNode* XQOperator::getArgument(unsigned int index) const
{
	assert(index<_args.size());
	return _args[index];
}

void XQOperator::setArgument(unsigned int index, ASTNode *arg)
{
	assert(index<_args.size());
	_args[index] = arg;
}

void XQOperator::removeArgument(unsigned int index)
{
	assert(index<_args.size());
	_args.erase(_args.begin() + index);
}

unsigned int XQOperator::getNumArgs() const
{
  return _args.size();
}

const XMLCh* XQOperator::getOperatorName() const
{
  return _opName;
}

const VectorOfASTNodes &XQOperator::getArguments() const {
  return _args;
}
