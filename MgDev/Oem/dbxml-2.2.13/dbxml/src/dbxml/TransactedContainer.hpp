//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: TransactedContainer.hpp,v 1.19 2005/08/31 17:14:20 gmf Exp $
//

#ifndef __TRANSACTEDCONTAINER_HPP
#define	__TRANSACTEDCONTAINER_HPP

#include "DbXmlInternal.hpp"
#include "dbxml/DbXmlFwd.hpp"
#include "ID.hpp"
#include "Container.hpp"
#include <db_cxx.h>

#include <string>

namespace DbXml
{

class Functor;
class UpdateContext;
class Document;
	
/**
 * TransactedContainer wraps a Container with a Berkeley DB
 * environment, and the update methods with Berkeley DB
 * transactions (if they not provided by the caller).
 */
class TransactedContainer : public Container
{
public:
	TransactedContainer(Manager &mgr, const std::string &name,
			    Transaction *txn, u_int32_t flags,
			    u_int32_t pagesize, u_int32_t seqIncr, int mode,
			    XmlContainer::ContainerType type,
			    bool doVersionCheck);
	virtual ~TransactedContainer();

	// Container Operations
	int addDocument(Transaction *txn, Document &document,
			UpdateContext &context, u_int32_t flags);
	int deleteDocument(Transaction *txn, const std::string &name,
			   UpdateContext &context);
	int deleteDocument(Transaction *txn, Document &document,
			   UpdateContext &context);
	int updateDocument(Transaction *txn, Document &document,
			   UpdateContext &context);
	int setIndexSpecification(Transaction *txn,
				  const XmlIndexSpecification &index,
				  UpdateContext &context);
private:
	// no need for copy and assignment
	TransactedContainer(const TransactedContainer&);
	void operator=(const TransactedContainer &);

	int transactedMethod(Transaction *txn, u_int32_t flags, const Functor &f);

	bool autoCommit_;
};

}

#endif

