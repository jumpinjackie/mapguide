//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlAtomize.cpp,v 1.3 2006/10/30 17:45:54 bostic Exp $
//

#include "DbXmlInternal.hpp"
#include "DbXmlAtomize.hpp"

#include <xqilla/context/DynamicContext.hpp>

using namespace DbXml;
using namespace std;

DbXmlAtomize::DbXmlAtomize(XQAtomize *atom, XPath2MemoryManager* memMgr)
	: XQAtomize(const_cast<ASTNode*>(atom->getExpression()), memMgr)
{
	staticTyping(0);
}

ASTNode* DbXmlAtomize::staticResolution(StaticContext *context)
{
	expr_ = expr_->staticResolution(context);
	return this;
}

ASTNode* DbXmlAtomize::staticTyping(StaticContext *context)
{
	_src.clear();

	if(context != 0)
		expr_ = expr_->staticTyping(context);
	_src.getStaticType() = expr_->getStaticResolutionContext().getStaticType();
	_src.add(expr_->getStaticResolutionContext());

	if(!_src.getStaticType().containsType(StaticType::NODE_TYPE)) {
		// If the expression has no nodes, this function does nothing
		return expr_;
	}

	static const unsigned int untyped_types = StaticType::ELEMENT_TYPE | StaticType::ATTRIBUTE_TYPE |
		StaticType::DOCUMENT_TYPE | StaticType::TEXT_TYPE;
	static const unsigned int string_types = StaticType::NAMESPACE_TYPE | StaticType::COMMENT_TYPE |
		StaticType::PI_TYPE;

	if(_src.getStaticType().containsType(untyped_types)) {
		_src.getStaticType().flags &= ~untyped_types;
		_src.getStaticType().flags |= StaticType::UNTYPED_ATOMIC_TYPE;
	}
	if(_src.getStaticType().containsType(string_types)) {
		_src.getStaticType().flags &= ~string_types;
		_src.getStaticType().flags |= StaticType::STRING_TYPE;
	}

	if(context != 0 && expr_->isConstant()) {
		return constantFold(context);
	}
	return this;
}

