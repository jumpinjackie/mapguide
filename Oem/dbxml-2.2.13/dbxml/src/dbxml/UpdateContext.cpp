//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: UpdateContext.cpp,v 1.15 2005/08/16 13:29:31 jsnelson Exp $
//

#include "DbXmlInternal.hpp"
#include "UpdateContext.hpp"

using namespace DbXml;

UpdateContext::UpdateContext(XmlManager &db)
	: db_(db),
	  applyChanges_(true),
	  indexer_()
{}

UpdateContext::~UpdateContext()
{}

void UpdateContext::init(Transaction *txn, Container *container)
{
	indexer_.reset(txn, container);
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
