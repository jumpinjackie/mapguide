//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: Results.cpp,v 1.102 2006/10/30 17:45:53 bostic Exp $
//

#include <xqilla/simple-api/XQQuery.hpp>
#include <xqilla/context/DynamicContext.hpp>

#include "DbXmlInternal.hpp"
#include "dbxml/XmlNamespace.hpp"
#include "dbxml/XmlException.hpp"
#include "dbxml/XmlResults.hpp"
#include "Results.hpp"
#include "QueryContext.hpp"
#include "OperationContext.hpp"
#include "Container.hpp"
#include "QueryExpression.hpp"
#include "UTF8.hpp"
#include "Manager.hpp"
#include "HighResTimer.hpp"
#include "Document.hpp"
#include "IndexLookup.hpp"
#include "dataItem/DbXmlContext.hpp"

#include <xercesc/dom/DOM.hpp>
#if defined(XERCES_HAS_CPP_NAMESPACE)
  XERCES_CPP_NAMESPACE_USE
#endif

#include <xqilla/exceptions/XQillaException.hpp>
#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/exceptions/QueryInterruptedException.hpp>
#include <xqilla/exceptions/QueryTimeoutException.hpp>

using namespace DbXml;
using namespace std;


// big macro to encapsulate exceptions that might
// come from XQilla and Xerces
#define CATCH_XQUERY_EXCEPTIONS \
	catch (QueryInterruptedException &ie) { \
		throw XmlException(XmlException::OPERATION_INTERRUPTED, \
				   "Query was interrupted by the application");\
	} \
	catch (QueryTimeoutException &ie) { \
		throw XmlException(XmlException::OPERATION_TIMEOUT, \
				   "Query timed out"); \
	} \
	catch (const XQillaException &e) { \
		throw XmlException(XmlException::XPATH_EVALUATION_ERROR, XMLChToUTF8(e.getString()).str()); \
	} \
	catch(const XQException &e) { \
		throw XmlException(XmlException::XPATH_EVALUATION_ERROR, e); \
	} \
	catch(XERCES_CPP_NAMESPACE_QUALIFIER DOMException &e) { \
		throw XmlException(XmlException::XPATH_EVALUATION_ERROR, XMLChToUTF8(e.msg).str()); \
	} \
	catch(XERCES_CPP_NAMESPACE_QUALIFIER XMLException &e) { \
		throw XmlException(XmlException::XPATH_EVALUATION_ERROR, XMLChToUTF8(e.getMessage()).str()); \
	}

// Results from Values
ValueResults::ValueResults()
	: vvi_(0)
{
}

ValueResults::ValueResults(const XmlValue &value)
	: vvi_(0)
{
	if(!value.isNull())
		vv_.push_back(value);
}

ValueResults::ValueResults(Results *resultsToAdopt)
	: vvi_(0)
{
	if(resultsToAdopt) {
		XmlResults results(resultsToAdopt);
		XmlValue value;
		while(results.next(value)) {
			if (value.getType() == XmlValue::BINARY)
				throw XmlException(XmlException::INVALID_VALUE,
						   "XmlQueryContext::setVariableValue value cannot be binary");
			vv_.push_back(value);
		}
	}
}

ValueResults::~ValueResults()
{
	delete vvi_;
}

int ValueResults::reset()
{
	delete vvi_;
	vvi_ = 0;
	return 0;
}

size_t ValueResults::size() const
{
	return vv_.size();
}

// NOTE: algorithm keeps vvi_ pointing to "current" entry
int ValueResults::doNext(XmlValue &value, bool isPeek)
{
	if(vvi_ == 0) {
		if (vv_.size() == 0) {
			value = XmlValue();
			return 0;
		}
		vvi_ = new XmlValueVector::iterator;
		*vvi_ = vv_.begin();
	} else if (!isPeek)
		(*vvi_)++;		

	if(*vvi_ != vv_.end()) {
		value = **vvi_;
	} else {
		value = XmlValue();
	}

	return 0;
}

int ValueResults::next(XmlValue &value)
{
	return doNext(value, false);
}

int ValueResults::peek(XmlValue &value)
{
	return doNext(value, true);
}

bool ValueResults::hasNext()
{
	// Don't create the vector if it's not already done; just use size 
	if(vvi_ == 0)
		return (vv_.size() != 0);
	if(*vvi_ != vv_.end() && ((*vvi_) + 1 != vv_.end()))
		return true;
	return false;
}

int ValueResults::previous(XmlValue &value)
{
	if (hasPrevious()) { // share initialization code, below
		(*vvi_)--;
		value = **vvi_;
	} else {
		value = XmlValue();
	}
	return 0;
}

bool ValueResults::hasPrevious()
{
	if ((vvi_ == 0) ||
	    (*vvi_ == vv_.begin()))
		return false;
	return true;
}

void ValueResults::add(const XmlValue &value)
{
	if(!value.isNull())
		vv_.push_back(value);
}

void ValueResults::add(const XmlValueVector &values)
{
	vv_.insert(vv_.end(), values.begin(), values.end());
}

// Lazily evaluated results
void LazyResults::add(const XmlValue &value)
{
	throw XmlException(XmlException::LAZY_EVALUATION, "This result set is lazily evaluated. add() can only be called for eagerly evaluated result sets.");
}

void LazyResults::add(const XmlValueVector &vv)
{
	throw XmlException(XmlException::LAZY_EVALUATION, "This result set is lazily evaluated. add() can only be called for eagerly evaluated result sets.");
}

size_t LazyResults::size() const
{
	throw XmlException(XmlException::LAZY_EVALUATION, "This result set is lazily evaluated. size() can only be called for eagerly evaluated result sets.");
	return 0;
}

// Results from XQ tree, evaluated lazily
LazyDIResults::LazyDIResults(QueryContext *context, Value *contextItem, QueryExpression &expr, Transaction *txn, u_int32_t flags)
	: context_(context),
	  expr_(&expr),
	  contextItem_(contextItem),
	  lazyDocs_((flags & DBXML_LAZY_DOCS) != 0),
	  qec_(context_, /*debugging*/false),
	  xqc_(0),
	  result_(0),
	  nextItem_(0)
{
	((QueryContext &)context_).setTransaction(txn);
	((QueryContext &)context_).setMinder(&evaluationMinder_);
	((QueryContext &)context_).setQueryExecutionContext(&qec_);

	// Documents are always lazy, until they are returned to the user
	((QueryContext &)context_).setFlags(flags | DBXML_LAZY_DOCS);

	int err = reset();
	if(err != 0)
		throw XmlException(err);
}

LazyDIResults::~LazyDIResults()
{
	nextItem_ = 0; // [#14890]
	result_ = 0; // Destruct the Result tree before the context
	if(xqc_ != 0) delete xqc_;
}

int LazyDIResults::next(XmlValue &value)
{
	timer_.start();

	try {
		Item::Ptr item;
		if (nextItem_ != NULLRCP) {
			item = nextItem_;
			nextItem_ = 0;
		} else
			item = result_->next(xqc_);
		if(item == NULLRCP) {
			value = XmlValue();
		} else {
			if(item->isNode()) {
				value = Value::create(item, context_, lazyDocs_);
			}
			else {
				value = Value::create(item, xqc_);
			}
		}
	}
	CATCH_XQUERY_EXCEPTIONS
	timer_.stop();

	if(value.isNull() && !result_.isNull()) {
		result_ = 0;
		if(Log::isLogEnabled(Log::C_QUERY, Log::L_INFO)) {
			ostringstream s;
			s << "Finished query execution, time taken = "
			  << (timer_.durationInSeconds() * 1000) << "ms";
			((Manager &)((QueryContext &)context_).getManager())
				.log(Log::C_QUERY, Log::L_INFO, s);
		}
	}

	return 0;
}

static void
_throwNotImpl(const char * op)
{
	ostringstream s;
	s << "Operation not supported on Lazy XmlResults: ";
	s << op;
	throw XmlException(XmlException::INVALID_VALUE, s.str().c_str());
}

int LazyDIResults::peek(XmlValue &value)
{
	Item::Ptr item = nextItem_;
	if (!item) {
		try {
			nextItem_ = result_->next(xqc_);
			item = nextItem_;
		}
		CATCH_XQUERY_EXCEPTIONS
	}
	int ret = next(value);
	// need to reset nextItem, so iterator doesn't really move
	nextItem_ = item;
	return ret;
}

int LazyDIResults::previous(XmlValue &)
{
	_throwNotImpl("previous");
	return 0;
}

bool LazyDIResults::hasNext()
{
	try {
		if (nextItem_ == NULLRCP)
			nextItem_ = result_->next(xqc_);
		if (nextItem_ == NULLRCP)
			return false;
	}
	CATCH_XQUERY_EXCEPTIONS
	return true;
}

bool LazyDIResults::hasPrevious()
{
	_throwNotImpl("hasPrevious");
	return false;
}
	
	
int LazyDIResults::reset()
{
	((Manager &)((QueryContext &)context_).getManager())
		.log(Log::C_QUERY, Log::L_INFO, "Starting query execution");

	timer_.reset();
	timer_.start();

	((QueryContext&)context_).startQuery();

	try {
		if(xqc_ != 0) delete xqc_;
		xqc_ = ((QueryContext&)context_).createDynamicContext(((QueryContext &)context_).getOperationContext()
			.txn(), ((QueryExpression*)expr_)->getDynamicContext());
		if(contextItem_)
			xqc_->setContextItem(Value::convertToItem((Value*)contextItem_, xqc_));

		result_ = ((QueryExpression*)expr_)->getCompiledExpression()->execute(xqc_);
	}
	CATCH_XQUERY_EXCEPTIONS

	timer_.stop();

	return 0;
}

static DbWrapper::Operation mapIndexOp(XmlIndexLookup::Operation op)
{
	DbWrapper::Operation ret;
	switch (op) {
	case XmlIndexLookup::EQ:
		ret = DbWrapper::EQUALITY;
		break;
	case XmlIndexLookup::LT:
		ret = DbWrapper::LTX;
		break;
	case XmlIndexLookup::LTE:
		ret = DbWrapper::LTE;
		break;
	case XmlIndexLookup::GT:
		ret = DbWrapper::GTX;
		break;
	case XmlIndexLookup::GTE:
		ret = DbWrapper::GTE;
		break;
	default:
		ret = DbWrapper::EQUALITY;
		break;
	}
	return ret;
}

LazyIndexResults::LazyIndexResults(Container &container,
				   QueryContext *context,
				   Transaction *txn, const Index &index,
				   const IndexLookup &il,
				   u_int32_t flags)
	: context_(context->copy()),
	  container_(&((TransactedContainer &)container)),
	  lowOp_(DbWrapper::PREFIX),
	  lowKey_(((Manager&)container.getManager()).getImplicitTimezone()),
	  highOp_(DbWrapper::NONE),
	  highKey_(((Manager&)container.getManager()).getImplicitTimezone()),
	  reverse_((flags & DBXML_REVERSE_ORDER) != 0),
	  docOnly_((flags & DBXML_NO_INDEX_NODES) != 0),
	  cacheDocuments_((flags & DBXML_CACHE_DOCUMENTS) != 0)
{
	// validate that IndexLookup object is self-consistent.
	// It checks value types and validity of range operations.
	// It will throw on invalid input.
	il.validate();
	
	bool isNull = il.getLowBoundValue().isNull();

	Name cname(il.getNodeURI(), il.getNodeName());
	string childUriName = cname.getURIName();
	string parentUriName;
	if (il.hasParent()) {
		Name pname(il.getParentURI(), il.getParentName());
		parentUriName = pname.getURIName();
	}
	((Manager &)((QueryContext &)context_).getManager())
		.log(Log::C_QUERY, Log::L_INFO, "Starting index lookup");

	((QueryContext &)context_).setTransaction(txn);
	((QueryContext &)context_).setFlags(flags & ~(DBXML_REVERSE_ORDER|DBXML_NO_INDEX_NODES|DBXML_CACHE_DOCUMENTS));
	OperationContext &oc = ((QueryContext*)context_)->getOperationContext();

	lowKey_.setIndex(index);

	if(!isNull && AtomicTypeValue::convertToSyntaxType(
		   il.getLowBoundValue().getType()) != lowKey_.getSyntaxType()) {
		throw XmlException(XmlException::INVALID_VALUE,
				   "Value type does not match index syntax type.");
	}
	
	if(!isNull && lowKey_.getSyntaxType() == Syntax::NONE) {
		throw XmlException(XmlException::INVALID_VALUE,
				   "A value has been specified for an index that does not require one.");
	}

	lowKey_.setIDsFromNames(oc, container_, parentUriName.c_str(),
		childUriName.c_str());
	if(index.getPath() == Index::PATH_EDGE && !il.hasParent()) {
		lowKey_.setNodeLookup(true);
	}

	// operation is prefix unless there is at least one value
	if (!isNull) {
		lowOp_ = mapIndexOp(il.getLowBoundOperation());
		lowKey_.setValue(il.getLowBoundValue());
		if (!il.getHighBoundValue().isNull()) {
			highKey_.setIndex(index);
			highKey_.setIDsFromNames(oc, container_,
				parentUriName.c_str(), childUriName.c_str());
			if(index.getPath() == Index::PATH_EDGE &&
			   !il.hasParent())
				highKey_.setNodeLookup(true);
			highOp_ = mapIndexOp(il.getHighBoundOperation());
			highKey_.setValue(il.getHighBoundValue());
		}
	}
	int err = reset();
	if(err != 0)
		throw XmlException(err);
}

int LazyIndexResults::reset()
{
	SyntaxDatabase *sdb = ((Container*)container_)->getIndexDB(lowKey_.getSyntaxType());
	int err = 0;
	if(sdb) {
		if (highOp_ != DbWrapper::NONE) {
			cursor_.reset(sdb->getIndexDB()->
				createCursor(
					((QueryContext &)context_).getOperationContext().txn(),
					lowOp_, &lowKey_, highOp_, &highKey_, reverse_));
		} else {
			cursor_.reset(sdb->getIndexDB()->
				createCursor(
					((QueryContext &)context_).getOperationContext().txn(),
					lowOp_, &lowKey_, reverse_));
		}
		err = cursor_->error();
		if(err == 0)
			err = cursor_->first(ie_);
	}
	return err;
}

int LazyIndexResults::doNext(XmlValue &value, bool isPeek)
{
	// ie_ holds the "next" one, which is the entry to process
	DocID docId = ie_.getDocID();
	if(docId == 0) {
		// done
		value = XmlValue();
		return 0;
	}

	XmlDocument document;
	// try to avoid re-fetching documents if the last result
	// was from the same doc
	if (!value.isNull() && (value.getType() == XmlValue::NODE) &&
	    docId == ((const Document&) value.asDocument()).getID())
		document = value.asDocument();
	else
		docId.fetchDocument(container_, context_, document,
				    (cacheDocuments_ ? &evaluationMinder_ : 0));

	DOMNode *node = 0;
	if(!docOnly_ && ie_.isSpecified(IndexEntry::NODE_ID)) {
		node = ie_.fetchNode(document);
	}

	value = new NodeValue(node, document);

	int err = 0;
	if(!isPeek)
		err = cursor_->next(ie_);
	return err;
}

int LazyIndexResults::next(XmlValue &value)
{
	return doNext(value, false);
}

int LazyIndexResults::peek(XmlValue &value)
{
	return doNext(value, true);
}

bool LazyIndexResults::hasNext()
{
	if (ie_.getDocID() != 0)
		return true;
	return false;
}

//
// these next 2 are harder to do, and have limited usefulness
//
inline void liOpNotSupp(const char *str)
{
	string s("XmlValue operation not supported for lazy index lookup: ");
	s += str;
	throw XmlException(XmlException::LAZY_EVALUATION, s.c_str());
}

int LazyIndexResults::previous(XmlValue &value)
{
	liOpNotSupp("previous");
	return 0;
}

bool LazyIndexResults::hasPrevious()
{
	liOpNotSupp("hasPrevious");
	return false;
}

