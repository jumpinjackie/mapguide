//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: XmlTransaction.cpp,v 1.8 2005/11/28 17:41:05 gmf Exp $
//

#include "DbXmlInternal.hpp"
#include "Transaction.hpp"
#include "dbxml/XmlTransaction.hpp"

using namespace DbXml;

static const char *className = "XmlTransaction";
#define CHECK_POINTER checkNullPointer(transaction_,className)

XmlTransaction::XmlTransaction()
	: transaction_(0)
{
}

XmlTransaction::XmlTransaction(Transaction *transaction)
	: transaction_(transaction)
{
	if (transaction_ != 0)
		transaction_->acquire();
}

XmlTransaction::XmlTransaction(const XmlTransaction &o)
	: transaction_(o.transaction_)
{
	if (transaction_ != 0)
		transaction_->acquire();
}

XmlTransaction &XmlTransaction::operator=(const XmlTransaction &o)
{
	if(transaction_ != o.transaction_) {
		if (transaction_ != 0)
			transaction_->releaseTransaction();
		transaction_ = o.transaction_;
		if (transaction_ != 0)
			transaction_->acquire();
	}
	return *this;
}

XmlTransaction::~XmlTransaction()
{
	if (transaction_ != 0)
		transaction_->releaseTransaction();
}

void XmlTransaction::abort()
{
	if (transaction_ != 0)
		transaction_->abort();
}

void XmlTransaction::commit(u_int32_t flags)
{
	if (transaction_ != 0)
		transaction_->commit(flags);
}

XmlTransaction XmlTransaction::createChild(u_int32_t flags)
{
	CHECK_POINTER;
	return transaction_->createChild(flags);
}

DbTxn *XmlTransaction::getDbTxn()
{
	CHECK_POINTER;
	return transaction_->getDbTxn();
}

XmlTransaction::operator Transaction *()
{
	return transaction_;
}

XmlTransaction::operator Transaction &()
{
	CHECK_POINTER;
	return *transaction_;
}
