//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DbXmlContext.hpp,v 1.25 2006/10/30 17:45:55 bostic Exp $
//

#ifndef __DBXMLCONTEXT_HPP
#define	__DBXMLCONTEXT_HPP

#include <xqilla/context/DynamicContext.hpp>
#include "DbXmlURIResolver.hpp"
#include "../QueryContext.hpp"

#define CAST_TO_CONST_DBXMLCONTEXT(x) (static_cast<const DbXmlContext*>(x))
#define CAST_TO_DBXMLCONTEXT(x) (static_cast<DbXmlContext*>(x))

namespace DbXml
{

class DbXmlContext : public DynamicContext
{
public:
	DbXmlContext(QueryContext &qc, Transaction *txn);
	virtual ~DbXmlContext();

	// these are required by DynamicContext (XQilla)

	virtual Item::Ptr getContextItem() const;
	virtual void setContextItem(const Item::Ptr &item);
	virtual void setExternalContextNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node);

	virtual unsigned int getContextPosition() const;
	virtual void setContextPosition(unsigned int pos);

	virtual unsigned int getContextSize() const;
	virtual void setContextSize(unsigned int size);

	virtual void trace(const XMLCh* message1, const XMLCh* message2);

	virtual void setModuleResolver(ModuleResolver *mr) { moduleResolver_ = mr; }
	virtual ModuleResolver *getModuleResolver() const { return moduleResolver_; }
	virtual VectorOfStrings* resolveModuleURI(const XMLCh* ) const;

	// Neither of these is ever used in DB XML
	virtual void incrementDocumentRefCount(const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* document) const {}
	virtual void decrementDocumentRefCount(const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* document) const {}

	virtual ATDurationOrDerived::Ptr getImplicitTimezone() const;
	virtual void setImplicitTimezone(const ATDurationOrDerived::Ptr &timezoneAsDuration);
	virtual void testInterrupt() const {
		qc_->testInterrupt();
	}

	// Added in DbXmlContext

	bool runQueryPlans() const { return runQPs_; }
	void setRunQueryPlans(bool value) { runQPs_ = value; }
	QueryContext &getQueryContext() const { return *qc_; }
	void setQueryContext(QueryContext &qc) { qc_ = &qc; }
	void setTransaction(Transaction *txn) {
		resolver_.setTransaction(txn);
	}

protected:
	QueryContext *qc_;
	DbXmlURIResolver resolver_;
	bool runQPs_;

	ModuleResolver *moduleResolver_;

	Item::Ptr contextItem_;
	unsigned int contextPosition_;
	unsigned int contextSize_;
};

class AutoRunQueryPlansReset
{
public:
  AutoRunQueryPlansReset(DbXmlContext* context, bool runQPs = false)
  {
    context_ = context;
    oldRunQPs_ = context->runQueryPlans();
    context->setRunQueryPlans(runQPs);
  }

  ~AutoRunQueryPlansReset()
  {
    context_->setRunQueryPlans(oldRunQPs_);
  }

protected:
  DbXmlContext* context_;
  bool oldRunQPs_;
};

}

#endif
