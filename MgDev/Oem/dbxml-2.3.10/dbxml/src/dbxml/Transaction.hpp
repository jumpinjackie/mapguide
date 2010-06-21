//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: Transaction.hpp,v 1.11 2006/10/30 17:45:53 bostic Exp $
//

#ifndef __TRANSACTION_HPP
#define	__TRANSACTION_HPP

#include "dbxml/XmlManager.hpp"
#include "ReferenceCounted.hpp"
#include "db_cxx.h"
#include <vector>

extern "C" {
	typedef int (*db_abort_func)(DB_TXN*);
	typedef int (*db_commit_func)(DB_TXN*, u_int32_t);
	typedef struct {
		db_abort_func saved_abort_func;
		db_commit_func saved_commit_func;
	} db_txn_funcs;
};

namespace DbXml
{

/// Reference counted by XmlTransaction
class Transaction : public ReferenceCounted
{
public:
	class Notify {
	public:
		virtual ~Notify() {}
		virtual void preNotify(bool commit) {}
		virtual void postNotify(bool commit) {}
	};

	Transaction(XmlManager &db, DbTxn *dbtxn);
	Transaction(XmlManager &db, u_int32_t flags);
	/** Aborts the transaction, if it hasn't been committed or
	    aborted already. */
	~Transaction();

	void abort();
	void commit(u_int32_t flags = 0);

	Transaction *createChild(u_int32_t flags = 0);

	DbTxn *getDbTxn();

	void registerNotify(Notify *notify) {
		notify_.push_back(notify);
	}
	void unregisterNotify(Notify *notify);

	static DbTxn *toDbTxn(Transaction *t) {
		return t ? t->getDbTxn() : 0;
	}

	// a special release function to handle wrapping of DbTxn
	void releaseTransaction();
	
	// these need to be public for commit/abort C functions
	int runDbNotification(DB_TXN *dbtxn, bool isCommit,
			      bool fromDB, u_int32_t commitFlags);
	bool isOwned() const { return owned_; }
private:
	// TODO deal with child transactions, notifications etc. - jpcs
	Transaction(XmlManager &db, DbTxn *parent, u_int32_t flags);
	Transaction(const Transaction &);
	Transaction &operator=(const Transaction &);
	void setDbNotification(DB_TXN *txn);
	void clearDbNotification(DB_TXN *txn);
	void preNotify(bool commit) const;
	void postNotify(bool commit) const;
private:
	typedef std::vector<Notify*> NotifyList;
	DbTxn *txn_;
	XmlManager mgr_;
	NotifyList notify_;
	db_txn_funcs dbfuncs_;
	bool owned_;
};

}

#endif
