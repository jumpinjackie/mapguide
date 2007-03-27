//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: MetaDataFunction.hpp,v 1.4 2005/07/15 16:44:33 jsnelson Exp $
//

#ifndef __METADATAFUNCTION_HPP
#define	__METADATAFUNCTION_HPP

#include <pathan/internal/factory/FuncFactory.hpp>
#include <pathan/dataItem/DataItemFunction.hpp>
#include <pathan/dataItem/SingleResult.hpp>

namespace DbXml
{

class MetaDataFunction : public DataItemFunction
{
public:
	static const XMLCh name[];
	static const XMLCh XMLChFunctionURI[];

	MetaDataFunction(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

	virtual DataItem* staticResolution(StaticContext* context);
	virtual Result createResult(DynamicContext* context, int flags=0) const;

	bool getConstantQNameArg(const XMLCh *&uri, const XMLCh *&name, DynamicContext *context) const;

private:
	class MetaDataResult : public SingleResult
	{
	public:
		MetaDataResult(const MetaDataFunction *func, int flags, DynamicContext *context);
		Item::Ptr getSingleResult(DynamicContext *context) const;
		std::string asString(DynamicContext *context, int indent) const;
	private:
		int flags_;
		const MetaDataFunction *func_;
	};
};

class MetaDataFuncFactory : public FuncFactory
{
public:
	DataItem *createInstance(const VectorOfDataItems &args, XPath2MemoryManager* memMgr) const {
		return new (memMgr) MetaDataFunction(args, memMgr);
	}
	const XMLCh *getName() const {
		return MetaDataFunction::name;
	}
	const XMLCh *getURI() const {
		return MetaDataFunction::XMLChFunctionURI;
	}
	unsigned int getMinArgs() const {
		return 1;
	}
	unsigned int getMaxArgs() const {
		return 2;
	}
};

}

#endif
