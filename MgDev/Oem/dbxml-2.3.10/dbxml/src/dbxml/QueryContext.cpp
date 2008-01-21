//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: QueryContext.cpp,v 1.77.2.2 2007/01/12 18:05:49 gmf Exp $
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
#include "dataItem/NodeHandleFunction.hpp"
#include "dataItem/LookupIndexFunction.hpp"

#include <set>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

#include <xqilla/utils/XQillaPlatformUtils.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/dom-api/XQillaNSResolver.hpp>
#include <xqilla/dom-api/impl/XQillaNSResolverImpl.hpp>
#include <xqilla/framework/XPath2MemoryManager.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/items/AnyAtomicType.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/context/VariableStore.hpp>
#include <xqilla/context/VariableTypeStore.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/exceptions/QueryTimeoutException.hpp>
#include <xqilla/exceptions/QueryInterruptedException.hpp>

using namespace DbXml;
using namespace std;

// From DB, used by QueryInterrupt code
extern "C" int __os_clock(DB_ENV *, u_int32_t *, u_int32_t *);

namespace DbXml {
/**
   QueryInterrupt is private to QueryContext, used to
   track the desire to interrupt a query or time it out
*/
class QueryInterrupt {
public:
	QueryInterrupt() :
		stop(false), timeCount(0), timeoutStart(0),
		timeoutValue(0) {}
	QueryInterrupt(const QueryInterrupt &o) :
		stop(false), timeCount(o.timeCount),
		timeoutStart(o.timeoutStart),
		timeoutValue(o.timeoutValue) {}
public:
	bool stop;
	uint32_t timeCount;
	uint32_t timeoutStart;
	uint32_t timeoutValue;
};
}

QueryContext::QueryContext(XmlManager &mgr, XmlQueryContext::ReturnType rt, XmlQueryContext::EvaluationType et)
	: baseURI_(DbXmlUri::dbxmlBaseURI),
	  returnType_(rt),
	  evaluationType_(et),
	  defaultCollection_(),
	  mgr_(mgr),
	  qInt_(new QueryInterrupt()),
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
	  qInt_(new QueryInterrupt()),
	  operationContext_(0),
	  qec_(0),
	  minder_(0),
	  db_flags_(0)
{
}

QueryContext::~QueryContext()
{
	delete operationContext_;
	delete qInt_;
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

// if uri is absolute, use it; otherwise, concat with
// the base URI, if set
void QueryContext::setDefaultCollection(const std::string &uri)
{
	DbXmlUri dbxmluri(baseURI_, uri, /*documentUri*/false);

	if(!dbxmluri.isResolved()) {
		std::ostringstream s;
		s << "setDefaultCollection: cannot construct a valid URI " <<
			"from uri: " << uri << ", and baseURI: " << baseURI_;
		throw XmlException(XmlException::INVALID_VALUE,
			s.str().c_str());
	}

	defaultCollection_ = dbxmluri.getResolvedUri();
}

void QueryContext::setNamespace( const std::string &prefix,
				 const std::string &uri )
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

DbXmlContext *QueryContext::createStaticContext(Transaction *txn)
{
	return new (Globals::defaultMemoryManager) DbXmlContextImpl(*this, txn,
		Globals::defaultMemoryManager);
}

void QueryContext::populateStaticContext(DbXmlContext *context)
{
	// maybe remove namespaces_ & variables_ and let the context set this info
	// in the XPath query
	XQillaNSResolver *nsResolver = (XQillaNSResolver*)context->getNSResolver();

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
				switch((XmlValue::NodeType)value.getNodeType()) {
				case XmlValue::ELEMENT_NODE:
					src->getStaticType().flags |= StaticType::ELEMENT_TYPE;
					break;
				case XmlValue::ATTRIBUTE_NODE:
					src->getStaticType().flags |= StaticType::ATTRIBUTE_TYPE;
					break;
				case XmlValue::TEXT_NODE:
				case XmlValue::CDATA_SECTION_NODE:
					src->getStaticType().flags |= StaticType::TEXT_TYPE;
					break;
				case XmlValue::PROCESSING_INSTRUCTION_NODE:
					src->getStaticType().flags |= StaticType::PI_TYPE;
					break;
				case XmlValue::COMMENT_NODE:
					src->getStaticType().flags |= StaticType::COMMENT_TYPE;
					break;
				case XmlValue::DOCUMENT_NODE:
					src->getStaticType().flags |= StaticType::DOCUMENT_TYPE;
					break;
				case XmlValue::DOCUMENT_TYPE_NODE:
				case XmlValue::ENTITY_NODE:
				case XmlValue::DOCUMENT_FRAGMENT_NODE:
				case XmlValue::ENTITY_REFERENCE_NODE:
				case XmlValue::NOTATION_NODE:
					src->getStaticType().flags |= StaticType::NODE_TYPE;
					break;
				}
				break;
			}
			case XmlValue::ANY_SIMPLE_TYPE: {
				src->getStaticType().flags |= StaticType::ANY_SIMPLE_TYPE;
				break;
			}
			case XmlValue::ANY_URI: {
				src->getStaticType().flags |= StaticType::ANY_URI_TYPE;
				break;
			}
			case XmlValue::BASE_64_BINARY: {
				src->getStaticType().flags |= StaticType::BASE_64_BINARY_TYPE;
				break;
			}
			case XmlValue::BOOLEAN: {
				src->getStaticType().flags |= StaticType::BOOLEAN_TYPE;
				break;
			}
			case XmlValue::DATE: {
				src->getStaticType().flags |= StaticType::DATE_TYPE;
				break;
			}
			case XmlValue::DATE_TIME: {
				src->getStaticType().flags |= StaticType::DATE_TIME_TYPE;
				break;
			}
			case XmlValue::DAY_TIME_DURATION: {
				src->getStaticType().flags |= StaticType::DAY_TIME_DURATION_TYPE;
				break;
			}
			case XmlValue::DECIMAL: {
				src->getStaticType().flags |= StaticType::DECIMAL_TYPE;
				break;
			}
			case XmlValue::DOUBLE: {
				src->getStaticType().flags |= StaticType::DOUBLE_TYPE;
				break;
			}
			case XmlValue::DURATION: {
				src->getStaticType().flags |= StaticType::DURATION_TYPE;
				break;
			}
			case XmlValue::FLOAT: {
				src->getStaticType().flags |= StaticType::FLOAT_TYPE;
				break;
			}
			case XmlValue::G_DAY: {
				src->getStaticType().flags |= StaticType::G_DAY_TYPE;
				break;
			}
			case XmlValue::G_MONTH: {
				src->getStaticType().flags |= StaticType::G_MONTH_TYPE;
				break;
			}
			case XmlValue::G_MONTH_DAY: {
				src->getStaticType().flags |= StaticType::G_MONTH_DAY_TYPE;
				break;
			}
			case XmlValue::G_YEAR: {
				src->getStaticType().flags |= StaticType::G_YEAR_TYPE;
				break;
			}
			case XmlValue::G_YEAR_MONTH: {
				src->getStaticType().flags |= StaticType::G_YEAR_MONTH_TYPE;
				break;
			}
			case XmlValue::HEX_BINARY: {
				src->getStaticType().flags |= StaticType::HEX_BINARY_TYPE;
				break;
			}
			case XmlValue::NOTATION: {
				src->getStaticType().flags |= StaticType::NOTATION_TYPE;
				break;
			}
			case XmlValue::QNAME: {
				src->getStaticType().flags |= StaticType::QNAME_TYPE;
				break;
			}
			case XmlValue::STRING: {
				src->getStaticType().flags |= StaticType::STRING_TYPE;
				break;
			}
			case XmlValue::TIME: {
				src->getStaticType().flags |= StaticType::TIME_TYPE;
				break;
			}
			case XmlValue::YEAR_MONTH_DURATION: {
				src->getStaticType().flags |= StaticType::YEAR_MONTH_DURATION_TYPE;
				break;
			}
			case XmlValue::UNTYPED_ATOMIC: {
				src->getStaticType().flags |= StaticType::UNTYPED_ATOMIC_TYPE;
				break;
			}
			default: break;
			}
		}

		// Calculate the properties of the variable
		unsigned int props = 0;
		if(src->getStaticType().isType(StaticType::NODE_TYPE) && results.size() == 1) {
			props = StaticResolutionContext::DOCORDER | StaticResolutionContext::GROUPED |
				StaticResolutionContext::PEER | StaticResolutionContext::SUBTREE |
				StaticResolutionContext::SAMEDOC | StaticResolutionContext::ONENODE;
		}
		src->setProperties(props);

		// Declare the variable
		UTF8ToXMLCh qname(vi->first.c_str());
		const XMLCh *uri = context->getUriBoundToPrefix(XPath2NSUtils::getPrefix(qname.str(), memMgr), 0);
		const XMLCh *name = memMgr->getPooledString(XPath2NSUtils::getLocalName(qname.str()));

		store->declareGlobalVar(uri, name, *src);
	}

	if(baseURI_ != "")
		context->setBaseURI(UTF8ToXMLCh(baseURI_).str());

	context->addCustomFunction(new (memMgr) DbXmlFuncFactory<MetaDataFunction>(memMgr));
	context->addCustomFunction(new (memMgr) DbXmlFuncFactory<NodeToHandleFunction>(memMgr));
	context->addCustomFunction(new (memMgr) DbXmlFuncFactory<HandleToNodeFunction>(memMgr));
	context->addCustomFunction(new (memMgr) DbXmlFuncFactory<LookupIndexFunction>(memMgr));
	context->addCustomFunction(new (memMgr) DbXmlFuncFactory<LookupAttributeIndexFunction>(memMgr));
	context->addCustomFunction(new (memMgr) DbXmlFuncFactory<LookupMetaDataIndexFunction>(memMgr));
}

DbXmlContext *QueryContext::createDynamicContext(Transaction *txn, const DynamicContext *staticContext)
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
                store->setGlobalVar(context->getUriBoundToPrefix(XPath2NSUtils::getPrefix(name.str(), memMgr), 0),
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

void QueryContext::interruptQuery()
{
	qInt_->stop = true;
}

void QueryContext::setQueryTimeoutSeconds(u_int32_t secs)
{
	qInt_->timeoutValue = secs;
}

u_int32_t QueryContext::getQueryTimeoutSeconds() const
{
	return qInt_->timeoutValue;
}

void QueryContext::startQuery()
{
	qInt_->stop = false;
	if(qInt_->timeoutValue) {
		qInt_->timeCount = 0;
		(void)__os_clock(NULL, &qInt_->timeoutStart, NULL);
	}
}

void QueryContext::testInterrupt()
{
	if(qInt_) {
		// exception information beyond the type is not useful
		if(qInt_->stop) {
			qInt_->stop = false;
			throw QueryInterruptedException(0, 0, 0, 0, 0);
		}
		if(qInt_->timeoutValue) {
			// only call __os_clock() every 100 calls; not
			// precise, but more efficient
			if(++(qInt_->timeCount) == 100) {
				qInt_->timeCount = 0;
				u_int32_t now;
				(void)__os_clock(NULL, &now, NULL);
				if ((now - qInt_->timeoutStart) >
					qInt_->timeoutValue) {
					qInt_->timeoutStart = 0;
					throw QueryTimeoutException(0, 0,
								    0, 0, 0);
				}
			}
		}
	}
}
