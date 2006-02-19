//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: QueryContext.cpp,v 1.58 2005/11/30 17:12:14 jsnelson Exp $
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlQueryContext.hpp"
#include "dbxml/XmlNamespace.hpp"
#include "QueryContext.hpp"
#include "QueryExecutionContext.hpp"
#include "Value.hpp"
#include "OperationContext.hpp"
#include "UTF8.hpp"
#include "dataItem/DbXmlContextImpl.hpp"
#include "Manager.hpp"
#include "dataItem/DbXmlURIResolver.hpp"
#include "dataItem/DbXmlUri.hpp"
#include "dataItem/MetaDataFunction.hpp"

#include <set>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

#include <pathan/PathanPlatformUtils.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/PathanEngine.hpp>
#include <pathan/PathanNSResolver.hpp>
#include <pathan/internal/dom-extensions/PathanNSResolverImpl.hpp>
#include <pathan/PathanImplementation.hpp>
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/AnyAtomicType.hpp>
#include <pathan/Node.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/VariableStore.hpp>
#include <pathan/simpleVariables/VariableTypeStore.hpp>
#include <pathan/XPath2NSUtils.hpp>

using namespace DbXml;
using namespace std;

QueryContext::QueryContext(XmlManager &mgr, XmlQueryContext::ReturnType rt, XmlQueryContext::EvaluationType et)
	: baseURI_(DbXmlUri::dbxmlScheme),
	  returnType_(rt),
	  evaluationType_(et),
	  defaultCollection_(),
	  mgr_(mgr),
	  operationContext_(0),
	  qec_(0),
	  minder_(0),
	  db_flags_(0)
{
	setNamespace("dbxml", "http://www.sleepycat.com/2002/dbxml");
}

QueryContext::QueryContext(const QueryContext &o)
	: namespaces_(o.namespaces_),
	  variables_(o.variables_),
	  baseURI_(o.baseURI_),
	  returnType_(o.returnType_),
	  evaluationType_(o.evaluationType_),
	  defaultCollection_(o.defaultCollection_),
	  mgr_(o.mgr_),
	  // We deliberately don't copy the parts of the QueryContext that
	  // are only used during execution time.
	  operationContext_(0),
	  qec_(0),
	  minder_(0),
	  db_flags_(0)
{
}

QueryContext::~QueryContext()
{
	delete operationContext_;
	delete qec_;
}

void QueryContext::setVariableValue(const std::string &name, const XmlResults &value)
{
	variables_.setVariableValue(name, value);
}

bool QueryContext::getVariableValue(const std::string &name, XmlResults &value) const
{
	return variables_.getVariableValue(name, value);
}

void QueryContext::setBaseURI(const std::string &baseURI)
{
	// validate the URI
	if (!baseURI.empty() && !DbXmlUri::isValidBase(baseURI)) {
		std::ostringstream s;
		s << "Malformed baseURI: " << baseURI;
		throw XmlException(XmlException::INVALID_VALUE,
				   s.str().c_str());
	}
	baseURI_ = baseURI;
}

void QueryContext::setNamespace( const std::string &prefix, const std::string &uri )
{
	namespaces_[prefix] = uri;
}

std::string QueryContext::getNamespace( const std::string &prefix )
{
	return namespaces_[prefix];
}

void QueryContext::removeNamespace( const std::string &prefix )
{
	namespaces_[prefix] = "";
}

void QueryContext::clearNamespaces()
{
	namespaces_.clear();
}

XQContext *QueryContext::createXQContext(Transaction *txn)
{
	// It would be nice to cache this rather large XPath2Context,
	// instead of creating one each time - but it would end up
	// leaking variable values (items and sequences), and pooled
	// strings from the namespace bindings - jpcs

	DbXmlContext *context = new (Globals::defaultMemoryManager) DbXmlContextImpl(*this, txn, Globals::defaultMemoryManager);

	// maybe remove namespaces_ & variables_ and let the context set this info
	// in the XPath query
	PathanNSResolver *nsResolver = (PathanNSResolver*)context->getNSResolver();

	NamespaceMap::iterator nsi;
	for (nsi = namespaces_.begin(); nsi != namespaces_.end(); ++nsi) {
		nsResolver->addNamespaceBinding(UTF8ToXMLCh(nsi->first).str(), UTF8ToXMLCh(nsi->second).str());
	}

	XPath2MemoryManager *memMgr = context->getMemoryManager();

	VariableTypeStore *store = context->getVariableTypeStore();
	VariableBindings::Values values = variables_.getValues();
	VariableBindings::Values::iterator vi;
	for (vi = values.begin(); vi != values.end(); ++vi) {
		// Create a StaticResolutionContext for the variable
		StaticResolutionContext *src = new (memMgr) StaticResolutionContext(memMgr);
		src->getStaticType().flags = 0;

		// Calculate the static type of the variable
		XmlResults results = vi->second;
		results.reset();
		XmlValue value;
		while(results.next(value)) {
			switch (value.getType()) {
			case XmlValue::NODE: {
				src->getStaticType().flags |= StaticType::NODE_TYPE;
				break;
			}
			case XmlValue::DECIMAL:
			case XmlValue::DOUBLE:
			case XmlValue::FLOAT: {
				src->getStaticType().flags |= StaticType::NUMERIC_TYPE;
				break;
			}
			default: {
				src->getStaticType().flags |= StaticType::OTHER_TYPE;
				break;
			}
			}
		}

		// Calculate the properties of the variable
		unsigned int props = 0;
		if(src->getStaticType().isNodesOnly()) {
			props |= StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED |
				StaticResolutionContext::PEER;
			if(results.size() == 1) {
				props |= StaticResolutionContext::ONENODE;
			}
		}
		src->setProperties(props);

		// Declare the variable
		UTF8ToXMLCh name(vi->first.c_str());
		store->declareGlobalVar(context->getUriBoundToPrefix(XPath2NSUtils::getPrefix(name.str(), memMgr)),
			XPath2NSUtils::getLocalName(name.str()), *src);
	}

	context->setBaseURI(UTF8ToXMLCh(baseURI_).str());

	context->addCustomFunction(new (memMgr) MetaDataFuncFactory());

	return context;
}

XQContext *QueryContext::createDynamicXQContext(Transaction *txn, XQContext *staticContext)
{
	DbXmlContext *context = CAST_TO_DBXMLCONTEXT(staticContext->createDynamicContext(Globals::defaultMemoryManager));
	context->setQueryContext(*this);
	context->setTransaction(txn);

	XPath2MemoryManager *memMgr = context->getMemoryManager();

	VariableStore *store = context->getVariableStore();
	VariableBindings::Values values = variables_.getValues();
	VariableBindings::Values::iterator vi;
	for (vi = values.begin(); vi != values.end(); ++vi) {
		// Create a Sequence for the variable
		Sequence seq(memMgr);

		// Populate the Sequence with the variable's value
		XmlResults results = vi->second;
		results.reset();
		XmlValue value;
		while(results.next(value)) {
			seq.addItem(Value::convertToItem(value, context));
		}

		// Set the variables value
		UTF8ToXMLCh name(vi->first.c_str());
		store->setGlobalVar(context->getUriBoundToPrefix(XPath2NSUtils::getPrefix(name.str(), memMgr)),
			XPath2NSUtils::getLocalName(name.str()), seq, context);
	}

	return context;
}

OperationContext &QueryContext::getOperationContext()
{
	if (operationContext_ == 0) {
		operationContext_ = new OperationContext();
	}
	return *operationContext_;
}

void QueryContext::setTransaction(Transaction *txn)
{
	getOperationContext().set(txn);
}

QueryExecutionContext &QueryContext::getQueryExecutionContext()
{
	if (qec_ == 0) {
		qec_ = new QueryExecutionContext(*this, /*debugging*/false);
	}
	return *qec_;
}

QueryContext *QueryContext::mergeIntoDynamicContext(const QueryContext &staticContext) const
{
	QueryContext *result = new QueryContext(*this);
	result->namespaces_ = staticContext.namespaces_;
	result->returnType_ = staticContext.returnType_;

	// Could do a check to make sure the dynamic variables haven't changed type -- jpcs

	return result;
}
