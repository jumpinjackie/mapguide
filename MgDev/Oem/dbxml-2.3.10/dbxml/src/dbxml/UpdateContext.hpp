//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: UpdateContext.hpp,v 1.17 2006/10/30 17:45:53 bostic Exp $
//

#ifndef __UPDATECONTEXT_HPP
#define	__UPDATECONTEXT_HPP

#include "ReferenceCounted.hpp"
#include "Container.hpp"
#include "Indexer.hpp"
#include "IndexSpecification.hpp"
#include "OperationContext.hpp"
#include "KeyStash.hpp"

namespace DbXml
{

class UpdateContext : public ReferenceCounted
{
public:
	/// Constructor.
	UpdateContext(XmlManager &db);
	virtual ~UpdateContext();

	void setApplyChanges(bool applyChanges) {
		applyChanges_ = applyChanges;
	}
	bool getApplyChanges() const {
		return applyChanges_;
	}

	void init(Transaction *txn, Container *container);

	Indexer &getIndexer();
	KeyStash &getKeyStash(bool reset = true);
	IndexSpecification &getIndexSpecification();
	OperationContext &getOperationContext();
private:
	// no need for copy and assignment
	UpdateContext(const UpdateContext&);
	UpdateContext &operator=(const UpdateContext &);

	// Hold a reference to the XmlManager, so it isn't destroyed before we are
	XmlManager db_;

	// Update Context settings
	bool applyChanges_;

	// Avoid re-making each of these objects
	Indexer indexer_;
	KeyStash stash_;
	IndexSpecification is_;
	OperationContext oc_;
};

}

#endif

