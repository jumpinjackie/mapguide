//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: Transaction.cpp,v 1.16 2006/10/30 17:45:53 bostic Exp $
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlException.hpp"
#include "Transaction.hpp"
#include "Manager.hpp"
#include "Log.hpp"

using namespace DbXml;

extern "C" {
	static int abortFunction(DB_TXN *dbtxn);
	static int commitFunction(DB_TXN *dbtxn, u_int32_t flags);
};

// NOTE: Because a DbTxn is external, this constructor
// creates a hard reference on the
// object, which is only released by a commit or abort
// operation.  This allows the caller to commit or abort
// directly on the DbTxn object, even after a containing
// XmlTransaction object has been deleted.
Transaction::Transaction(XmlManager &mgr, DbTxn *txn)
	: txn_(txn), mgr_(mgr), owned_(false)
{
	// caller ensures txn is non-null
	setDbNotification(txn_->get_DB_TXN());
	acquire();
}

Transaction::Transaction(XmlManager &mgr, u_int32_t flags)
	: mgr_(mgr), owned_(true)
{
	try {
		int err = mgr_.getDbEnv()->txn_begin(0, &txn_, flags);
		if(err) {
			throw XmlException(err);
		}
		setDbNotification(txn_->get_DB_TXN());
	}
	catch(DbException &e) {
		throw XmlException(e);
	}
}

Transaction::Transaction(XmlManager &mgr, DbTxn *parent, u_int32_t flags)
	: mgr_(mgr), owned_(true)
{
	try {
		int err = mgr_.getDbEnv()->txn_begin(parent, &txn_, flags);
		if(err) {
			throw XmlException(err);
		}
		setDbNotification(txn_->get_DB_TXN());
	}
	catch(DbException &e) {
		throw XmlException(e);
	}
}

Transaction::~Transaction()
{
	// Make the transaction exception safe
	if(txn_) {
		abort();
	}
}

void Transaction::abort()
{
	if(!txn_) {
		throw XmlException(XmlException::TRANSACTION_ERROR,
				   "Cannot abort, transaction already committed or aborted");
	}
	try {
		int err = runDbNotification(txn_->get_DB_TXN(),
					    false, // !isCommit
					    false, // !fromDB
					    0);
		if(err) {
			throw XmlException(err);
		}
	}
	catch(DbException &e) {
		throw XmlException(e);
	}
}

void Transaction::commit(u_int32_t flags)
{
	if(!txn_) {
		throw XmlException(XmlException::TRANSACTION_ERROR,
				   "Cannot commit, transaction already committed or aborted");
	}
	try {
		int err = runDbNotification(txn_->get_DB_TXN(),
					    true, // isCommit
					    false, // !fromDB
					    flags);
		if(err) {
			throw XmlException(err);
		}
	}
	catch(DbException &e) {
		throw XmlException(e);
	}
}

Transaction *Transaction::createChild(u_int32_t flags)
{
	if(!txn_) {
		throw XmlException(XmlException::TRANSACTION_ERROR,
				   "Cannot create child, transaction already committed or aborted");
	}
	((Manager&)mgr_).checkFlags(Log::misc_flag_info,
				    "XmlTransaction::createChild()", flags,
				    DB_READ_UNCOMMITTED|DB_TXN_NOSYNC|
				    DB_TXN_NOWAIT|DB_TXN_SNAPSHOT|
				    DB_TXN_SYNC|DB_READ_COMMITTED);
	return new Transaction(mgr_, txn_, flags);
}

DbTxn *Transaction::getDbTxn()
{
	if(!txn_) {
		throw XmlException(XmlException::TRANSACTION_ERROR,
				   "Cannot get DbTxn, transaction already committed or aborted");
	}
	return txn_;
}

void Transaction::preNotify(bool commit) const
{
	NotifyList::const_iterator end = notify_.end();
	NotifyList::const_iterator i = notify_.begin();
	for(; i != end; ++i) {
		(*i)->preNotify(commit);
	}
}

void Transaction::postNotify(bool commit) const
{
	NotifyList::const_iterator end = notify_.end();
	NotifyList::const_iterator i = notify_.begin();
	for(; i != end; ++i) {
		(*i)->postNotify(commit);
	}
}

void Transaction::unregisterNotify(Notify *notify)
{
	NotifyList::iterator end = notify_.end();
	NotifyList::iterator i = notify_.begin();
	while(i != end) {
		if(*i == notify) {
			i = notify_.erase(i);
			end = notify_.end();
		}
		else {
			++i;
		}
	}
}

void Transaction::setDbNotification(DB_TXN *txn)
{
	// cannot wrap the same DB_TXN twice -- there is only
	// one slot available
	if (txn->abort == abortFunction)
		throw XmlException(XmlException::TRANSACTION_ERROR,
				   "Cannot create XmlTransaction:  DbTxn is used by existing XmlTransaction");
	dbfuncs_.saved_abort_func = txn->abort;
	dbfuncs_.saved_commit_func = txn->commit;
	txn->abort = abortFunction;
	txn->commit = commitFunction;
	txn->xml_internal = (void *)this;
}

void Transaction::clearDbNotification(DB_TXN *txn)
{
	// caller will "release()" the reference
	txn->abort = dbfuncs_.saved_abort_func;
	txn->commit = dbfuncs_.saved_commit_func;
	txn->xml_internal = 0;
	txn_ = 0;
}

int Transaction::runDbNotification(DB_TXN *dbtxn, bool isCommit,
				   bool fromDB, u_int32_t commitFlags)

{
	int err = 0;
	DbTxn *dbTxn = txn_;
	clearDbNotification(dbtxn); // nulls txn_
	preNotify(isCommit);
	// The DbTxn method will delete the DbTxn.  This should only
	// happen if both (1) we own the DbTxn and (2) the call is not
	// coming via DB.  In the latter case, the DbTxn is going to
	// be deleted by DbTxn::commit().
	if (isCommit) {
		if (owned_ && !fromDB)
			err = dbTxn->commit(commitFlags);
		else
			err = dbfuncs_.saved_commit_func(dbtxn, commitFlags);
	} else {
		if (owned_ && !fromDB)
			err = dbTxn->abort();
		else
			err = dbfuncs_.saved_abort_func(dbtxn);
	}
	postNotify(isCommit);
	// If this object doesn't own the DbTxn, the hard reference
	// created during construction must be released
	if (!isOwned()) {
		owned_ = true; // keep state consistent
		release();
	}
	return err;
}

// Normally, this is a simple release()
// If
//  1) a DbTxn is wrapped (!isOwned)  and
//  2) there are no notifications registered and
//  3) there are only 2 references
// it is OK to release the extra hard reference
// created by the wrapping of DbTxn
// TBD -- if ReferenceCounted::release were virtual, that'd be better
void Transaction::releaseTransaction()
{
	if (!owned_ && (count() == 2) && (notify_.size() == 0)) {
		if (txn_)
			clearDbNotification(txn_->get_DB_TXN()); // nulls txn_
		release();
	}
	release();
}

extern "C" {
static int abortFunction(DB_TXN *txn)
{
	Transaction *xmlTxn = (Transaction *)txn->xml_internal;
	int err = 0;
	if (xmlTxn) {
		err = xmlTxn->runDbNotification(txn, false,
						true, // fromDB
						0);
	}
	return err;
}

static int commitFunction(DB_TXN *txn, u_int32_t flags)
{
	Transaction *xmlTxn = (Transaction *)txn->xml_internal;
	int err = 0;
	if (xmlTxn) {
		err = xmlTxn->runDbNotification(txn, true,
						true, // fromDB
						flags);
	}
	return err;
}
}
