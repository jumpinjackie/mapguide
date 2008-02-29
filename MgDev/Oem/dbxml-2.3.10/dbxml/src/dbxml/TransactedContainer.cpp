//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: TransactedContainer.cpp,v 1.35 2006/11/03 17:26:10 gmf Exp $
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlException.hpp"
#include "dbxml/XmlIndexSpecification.hpp"
#include "dbxml/XmlTransaction.hpp"
#include "TransactedContainer.hpp"
#include "OperationContext.hpp"
#include "Manager.hpp"
#include "Transaction.hpp"

using namespace DbXml;

// This utility class wraps a method in a transaction.
//
class DbXml::Functor
{
public:
	virtual ~Functor() {}
	virtual int method(Container &container, DbXml::Transaction *txn,
			   u_int32_t flags) const = 0;
};

class OpenFunctor : public Functor
{
public:
	OpenFunctor(int mode, bool doVersionCheck)
		: mode_(mode), doVersionCheck_(doVersionCheck) { }
	virtual int method(Container &container, DbXml::Transaction *txn,
			   u_int32_t flags) const {
		return container.open(txn, flags, mode_, doVersionCheck_);
	}
private:
	int mode_;
	bool doVersionCheck_;
};

TransactedContainer::TransactedContainer(
	Manager &mgr, const std::string &name,
	Transaction *txn, u_int32_t flags,
	u_int32_t pagesize, u_int32_t seqIncr, int mode,
	XmlContainer::ContainerType type, bool doVersionCheck)
	: Container(mgr, name, pagesize, seqIncr, type),
	  autoCommit_((txn != 0) || (flags & DBXML_TRANSACTIONAL)),
	  usingCDB_(false)
{
	flags &= ~DBXML_TRANSACTIONAL;
	int err = 0;
	try {
		err = transactedMethod(txn, flags,
				       OpenFunctor(mode, doVersionCheck));
	} catch (DbException &e) {
		err = e.get_errno();
		if ((err != EEXIST) && (err != DB_NOTFOUND))
			throw XmlException(e);
		// fall through for DB_NOTFOUND and EEXIST
	}
	if (err == EEXIST) {
		std::ostringstream s;
		s << "XmlContainer already exists: " << name;
		throw XmlException(XmlException::CONTAINER_EXISTS,
				   s.str().c_str());
	}
	if (err == DB_NOTFOUND) {
		std::ostringstream s;
		s << "XmlContainer not found: " << name;
		throw XmlException(XmlException::CONTAINER_NOT_FOUND, 
				   s.str().c_str());
	}
	if (err != 0) {
		throw XmlException(err);
  }
}

TransactedContainer::~TransactedContainer()
{
	try {
		((Manager &)mgr_).openContainers_.closeContainer(this, 0);
	} catch (...) {
		// Destructors don't throw.
	}
}

class AddDocumentFunctor : public Functor
{
public:
	AddDocumentFunctor(Document &document, UpdateContext &context)
		: document_(document), context_(context)
		{ }
	virtual int method(Container &container, DbXml::Transaction *txn,
			   u_int32_t flags) const {
		return container.addDocument(txn, document_,
					     context_, flags);
	}
private:
	Document &document_;
	UpdateContext &context_;
};

int TransactedContainer::addDocument(Transaction *txn, Document &document,
				     UpdateContext &context, u_int32_t flags)
{
	return transactedMethod(txn, flags,
				AddDocumentFunctor(document, context));
}

class DeleteDocumentFunctor2 : public Functor
{
public:
	DeleteDocumentFunctor2(Document &document, UpdateContext &context)
		: document_(document), context_(context)
	{ }
	virtual int method(Container &container, DbXml::Transaction *txn,
			   u_int32_t flags) const {
		return container.deleteDocument(txn, document_,
						context_);
	}
private:
	Document &document_;
	UpdateContext &context_;
};

int TransactedContainer::deleteDocument(Transaction *txn, Document &document,
					UpdateContext &context)
{
	return transactedMethod(txn, 0, DeleteDocumentFunctor2(document,
							       context));
}

class DeleteDocumentFunctor3 : public Functor
{
public:
	DeleteDocumentFunctor3(const std::string &name, UpdateContext &context)
		: name_(name), context_(context)
	{ }
	virtual int method(Container &container, DbXml::Transaction *txn,
			   u_int32_t flags) const {
		return container.deleteDocument(txn, name_, context_);
	}
private:
	const std::string &name_;
	UpdateContext &context_;
};

int TransactedContainer::deleteDocument(Transaction *txn,
					const std::string &name,
					UpdateContext &context)
{
	return transactedMethod(txn, 0, DeleteDocumentFunctor3(
					name, context));
}

class UpdateDocumentFunctor : public Functor
{
public:
	UpdateDocumentFunctor(Document &document, UpdateContext &context)
		: document_(document), context_(context)
	{ }
	virtual int method(Container &container, DbXml::Transaction *txn,
			   u_int32_t flags) const {
		return container.updateDocument(txn, document_, context_);
	}
private:
	Document &document_;
	UpdateContext &context_;
};

int TransactedContainer::updateDocument(Transaction *txn, Document &document,
					UpdateContext &context)
{
	return transactedMethod(txn, 0, UpdateDocumentFunctor(document,
								  context));
}

class SetIndexSpecificationFunctor : public Functor
{
public:
	SetIndexSpecificationFunctor(const XmlIndexSpecification &index,
				     UpdateContext &context)
		: index_(index),
		  context_(context)
		{ }
	virtual int method(Container &container, DbXml::Transaction *txn,
			   u_int32_t flags) const {
		return container.setIndexSpecification(
			txn, (const IndexSpecification &)index_, context_);
	}
private:
	const XmlIndexSpecification &index_;
	UpdateContext &context_;
};

int TransactedContainer::setIndexSpecification(
	Transaction *txn,
	const XmlIndexSpecification &index, UpdateContext &context)
{
	return transactedMethod(txn, 0, SetIndexSpecificationFunctor(index,
								     context));
}

int TransactedContainer::transactedMethod(Transaction *txn,
					  u_int32_t flags, const Functor &f)
{
	int err = 0;
	if (txn == 0 && autoCommit_) {
		XmlTransaction transaction(mgr_.createTransaction());
		err = f.method(*this, transaction, flags);
		if(err == 0)
			transaction.commit();
	} else if (txn || !usingCDB_) {
		err = f.method(*this, txn, flags);
	} else {
		DBXML_ASSERT(!txn);
#ifdef CDB_ALLOWED
		DbTxn *dbtxn = 0;
		try {
			err = mgr_.getDbEnv()->cdsgroup_begin(&dbtxn);
			XmlTransaction xtxn = mgr_.createTransaction(dbtxn);
			if (err)
				return err;
			err = f.method(*this, xtxn, flags);
		}
		catch (...) {
			DBXML_ASSERT(dbtxn);
			dbtxn->commit(0);
			throw;
		}
		if (dbtxn)
			dbtxn->commit(0);
#else
		err = f.method(*this, txn, flags);
#endif
	}
	return err;
}
