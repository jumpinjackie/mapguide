//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: OperationContext.hpp,v 1.12 2006/10/30 17:45:53 bostic Exp $
//

#ifndef __OPERATIONCONTEXT_HPP
#define	__OPERATIONCONTEXT_HPP

#include "ScopedDbt.hpp"
#include "Transaction.hpp"

namespace DbXml
{

class OperationContext
{
public:
	OperationContext(Transaction *t = 0) : txn_(t)
	{
		if(txn_) txn_->acquire();
	}
	~OperationContext()
	{
		if(txn_) txn_->release();
	}

	Transaction *txn()
	{
		return txn_;
	}
	DbTxn *db_txn()
	{
		return Transaction::toDbTxn(txn_);
	}
	DbtOut &key()
	{
		return key_;
	}
	DbtOut &data()
	{
		return data_;
	}

	void set(Transaction *t)
	{
		if(txn_) txn_->release();
		txn_ = t;
		if(txn_) txn_->acquire();
	}
	OperationContext *copy() const
	{
		return new OperationContext(*this);
	}

private:
	OperationContext(const OperationContext &o)
		: key_(o.key_),
		  data_(o.data_),
		  txn_(o.txn_)
	{
		if(txn_) txn_->acquire();
	}
	// no need for assignment
	OperationContext &operator=(const OperationContext &);

	DbtOut key_;
	DbtOut data_;
	Transaction *txn_;
};

}

#endif
