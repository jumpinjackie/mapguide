//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: XmlTransaction.hpp,v 1.8 2005/04/05 16:43:56 bostic Exp $
//

#ifndef __XMLTRANSACTION_HPP
#define	__XMLTRANSACTION_HPP

#include <db_cxx.h>
#include "DbXmlFwd.hpp"

namespace DbXml
{

class Transaction;
	
/** Constructed using the method on XmlManager. If the XmlTransaction
    has no more references left, and has not already been committed or
    aborted, it aborts itself. */
class DBXML_EXPORT XmlTransaction
{
public:
	/** @name For Reference Counting */
	XmlTransaction();
	XmlTransaction(const XmlTransaction &);
	XmlTransaction &operator=(const XmlTransaction &);
	~XmlTransaction();
	bool isNull() const { return transaction_ == 0; }
	
	/** @name Transaction Methods */
	void abort();
	void commit(u_int32_t flags = 0);
	
	/**
	 * Valid flags:
	 * DB_DIRTY_READ, DB_TXN_NOSYNC, DB_TXN_NOWAIT, DB_DEGREE_2, DB_TXN_SYNC
	 */
	XmlTransaction createChild(u_int32_t flags = 0);
	
	/** @name Berkeley DB Compatibility */
	DbTxn *getDbTxn();
	
	/** @name Private Methods (for internal use) */
	// @{
	
	XmlTransaction(Transaction *transaction);
	operator Transaction *();
	operator Transaction &();
	
	//@}
	
private:
	Transaction *transaction_;
};

}

#endif
