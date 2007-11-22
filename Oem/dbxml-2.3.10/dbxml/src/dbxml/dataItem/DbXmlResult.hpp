//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlResult.hpp,v 1.3 2006/10/30 17:45:55 bostic Exp $
//

#ifndef __DBXMLRESULT_HPP
#define	__DBXMLRESULT_HPP

#include <xqilla/runtime/Result.hpp>

namespace DbXml
{

class DocID;
class NsNid;

class DbXmlResultImpl : public ResultImpl
{
public:
	virtual Item::Ptr seek(const DocID &did, const NsNid &nid, DynamicContext *context) = 0;
	virtual std::string asString(DynamicContext *context, int indent) const { return ""; }

	virtual ResultImpl *asResultImpl() const { return const_cast<DbXmlResultImpl*>(this); }

protected:
	DbXmlResultImpl(const LocationInfo *o)
		: ResultImpl(o) {}
};

class ResultAdapter : public DbXmlResultImpl
{
public:
	ResultAdapter(ResultImpl *result)
		: DbXmlResultImpl(result),
		  result_(result)
	{
	}

	virtual Item::Ptr next(DynamicContext *context)
	{
		return result_->next(context);
	}
	virtual Item::Ptr seek(const DocID &did, const NsNid &nid, DynamicContext *context)
	{
		// This is a sufficient implementation for now
		return result_->next(context);
	}

	virtual ResultImpl *asResultImpl() const
	{
		return const_cast<ResultImpl*>(result_.get());
	}

private:
	Result result_;
};

class EmptyDbXmlResult : public DbXmlResultImpl
{
public:
	EmptyDbXmlResult() : DbXmlResultImpl(0) {}

	virtual Item::Ptr next(DynamicContext *context) { return 0; }
	virtual Item::Ptr seek(const DocID &did, const NsNid &nid, DynamicContext *context) { return 0; }
};

class DbXmlResult
{
public:
	DbXmlResult(int)
		: impl_(0)
	{
	}

	DbXmlResult(DbXmlResultImpl *impl)
		: impl_(impl)
	{
		if(impl_) impl_->incrementRefCount();
	}

	DbXmlResult(ResultImpl *impl)
		: impl_(new ResultAdapter(impl))
	{
		if(impl_) impl_->incrementRefCount();
	}

	DbXmlResult(const Result &impl)
		: impl_(impl.isNull() ? 0 : new ResultAdapter(const_cast<ResultImpl*>(impl.get())))
	{
		if(impl_) impl_->incrementRefCount();
	}

	DbXmlResult(const DbXmlResult &o)
		: impl_(o.impl_)
	{
		if(impl_) impl_->incrementRefCount();
	}

	DbXmlResult &operator=(const DbXmlResult &o)
	{
		if(o.impl_ != impl_) {
			if(impl_) impl_->decrementRefCount();
			impl_ = o.impl_;
			if(impl_) impl_->incrementRefCount();
		}
		return *this;
	}

	~DbXmlResult()
	{
		if(impl_) impl_->decrementRefCount();
	}

	DbXmlResultImpl *operator->()
	{
		if(impl_) return impl_;
		return &empty_;
	}

	const DbXmlResultImpl *operator->() const
	{
		if(impl_) return impl_;
		return &empty_;
	}

	DbXmlResultImpl *get()
	{
		if(impl_) return impl_;
		return &empty_;
	}

	const DbXmlResultImpl *get() const
	{
		if(impl_) return impl_;
		return &empty_;
	}

	bool isNull() const { return impl_ == 0; }

	operator Result() const { return impl_ == 0 ? 0 : impl_->asResultImpl(); }

private:
	DbXmlResultImpl *impl_;

	static EmptyDbXmlResult empty_;
};

}

#endif
