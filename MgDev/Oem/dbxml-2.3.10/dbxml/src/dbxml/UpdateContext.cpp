//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: UpdateContext.cpp,v 1.19 2006/10/30 17:45:53 bostic Exp $
//

#include "DbXmlInternal.hpp"
#include "UpdateContext.hpp"
#include "Manager.hpp"

using namespace DbXml;

UpdateContext::UpdateContext(XmlManager &db)
	: db_(db),
	  applyChanges_(true),
	  indexer_(((Manager&)db).getImplicitTimezone())
{}

UpdateContext::~UpdateContext()
{}

void UpdateContext::init(Transaction *txn, Container *container)
{
	indexer_.resetContext(container, &oc_);
	is_.read(container->getConfigurationDB(), txn, /*lock=*/false);
	oc_.set(txn);
}

Indexer &UpdateContext::getIndexer()
{
	return indexer_;
}

KeyStash &UpdateContext::getKeyStash(bool reset)
{
	if(reset) stash_.reset();
	return stash_;
}

IndexSpecification &UpdateContext::getIndexSpecification()
{
	return is_;
}

OperationContext &UpdateContext::getOperationContext()
{
	return oc_;
}
