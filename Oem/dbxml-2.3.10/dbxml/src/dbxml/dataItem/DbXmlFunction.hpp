//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlFunction.hpp,v 1.3.2.1 2007/01/12 18:05:49 gmf Exp $
//

#ifndef __DBXMLFUNCTION_HPP
#define	__DBXMLFUNCTION_HPP

#include <xqilla/functions/FuncFactory.hpp>
#include <xqilla/ast/XQFunction.hpp>

namespace DbXml
{

class DbXmlFunction : public XQFunction
{
public:
	static const XMLCh XMLChFunctionURI[];

	void getQNameArg(unsigned int argNum, const XMLCh *&uri, const XMLCh *&name,
		DynamicContext *context) const;
	bool getConstantQNameArg(unsigned int argNum, const XMLCh *&uri, const XMLCh *&name,
		DynamicContext *context) const;

protected:
	DbXmlFunction(const XMLCh* name, unsigned int argsFrom, unsigned int argsTo,
		const char* paramDecl, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
		: XQFunction(name, argsFrom, argsTo, paramDecl, args, memMgr) {}
};

template<class TYPE>
class DbXmlFuncFactory : public FuncFactory
{
public:
	DbXmlFuncFactory(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr)
		: qname(1023, memMgr)
	{
		qname.set(getName());
		qname.append(getURI());
	}
	ASTNode *createInstance(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr) const {
		return new (memMgr) TYPE(args, memMgr);
	}
	const XMLCh *getName() const {
		return TYPE::name;
	}
	const XMLCh *getURI() const {
		return TYPE::XMLChFunctionURI;
	}
	const XMLCh *getQName() const {
		return qname.getRawBuffer();
	}
	unsigned int getMinArgs() const {
		return TYPE::minArgs;
	}
	unsigned int getMaxArgs() const {
		return TYPE::maxArgs;
	}
private:
	XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer qname;
};

}

#endif
