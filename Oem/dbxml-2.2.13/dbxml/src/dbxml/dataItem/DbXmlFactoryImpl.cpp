//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: DbXmlFactoryImpl.cpp,v 1.9 2005/09/08 09:45:42 jsnelson Exp $
//

#include "../DbXmlInternal.hpp"
#include "DbXmlFactoryImpl.hpp"
#include "DbXmlNodeImpl.hpp"
#include "DbXmlContext.hpp"
#include "../QueryContext.hpp"
#include "../ReferenceMinder.hpp"

#include <xquery/XQContext.hpp>
#include <xquery/XQDebugCallback.hpp>

#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/functions/FunctionConstructor.hpp> // for datatype-uri
#include <pathan/internal/items/impl/ATBooleanOrDerivedImpl.hpp>
#include <pathan/internal/items/impl/ATDecimalOrDerivedImpl.hpp>
#include <pathan/internal/items/impl/ATDoubleOrDerivedImpl.hpp>
#include <pathan/internal/items/impl/ATFloatOrDerivedImpl.hpp>
#include <pathan/internal/items/impl/ATQNameOrDerivedImpl.hpp>
#include <pathan/ATDurationOrDerived.hpp>
#include <pathan/ATUntypedAtomic.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/exceptions/TypeNotFoundException.hpp>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/validators/datatype/DatatypeValidator.hpp>
#include <xercesc/dom/DOM.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

DbXmlFactoryImpl::DbXmlFactoryImpl(const DocumentCache* dc, MemoryManager* memMgr)
	: datatypeLookup_(dc, memMgr),
	  outputDocument_(0),
	  memMgr_(memMgr),
	  pooledNodes_(0),
	  allocatedNodes_(0)
{
}

DbXmlFactoryImpl::~DbXmlFactoryImpl()
{
	// Remove the factory from all the allocated nodes
	DbXmlNodeImpl *tmp;
	while(allocatedNodes_ != 0) {
		tmp = allocatedNodes_;
		allocatedNodes_ = tmp->allocNext;

		tmp->setFactory(0);
		tmp->allocNext = 0;
	}

	// Delete the nodes in the pool
	while(pooledNodes_ != 0) {
		tmp = pooledNodes_;
		pooledNodes_ = tmp->poolNext;
		delete tmp;
	}
}

void DbXmlFactoryImpl::release()
{
	this->~DbXmlFactoryImpl();
	memMgr_->deallocate(this);
}

void DbXmlFactoryImpl::addToNodePool(DbXmlNodeImpl *node) const
{
	node->poolNext = pooledNodes_;
	pooledNodes_ = node;
}

Node::Ptr DbXmlFactoryImpl::createNode(Document *doc, const DynamicContext *context) const
{
	DbXmlNodeImpl *result = pooledNodes_;

	if(result == 0) {
		result = new DbXmlNodeImpl(doc, context, this);
		result->allocNext = allocatedNodes_;
		allocatedNodes_ = result;
	} else {
		result->init(doc);
		pooledNodes_ = result->poolNext;
		result->poolNext = 0;
	}

	return result;
}

Node::Ptr DbXmlFactoryImpl::createNode(const DOMNode *node, Document *doc, const DynamicContext *context) const
{
	DbXmlNodeImpl *result = pooledNodes_;

	if(result == 0) {
		result = new DbXmlNodeImpl(node, doc, context, this);
		result->allocNext = allocatedNodes_;
		allocatedNodes_ = result;
	} else {
		result->init(node, doc);
		pooledNodes_ = result->poolNext;
		result->poolNext = 0;
	}

	return result;
}

DOMDocument *DbXmlFactoryImpl::getOutputDocument(const DynamicContext *context) const
{
	// Lazily create the output document
	if(outputDocument_.isNull()) {
		QueryContext &qc = CAST_TO_CONST_DBXMLCONTEXT(context)->getQueryContext();
		outputDocument_ = qc.getManager().createDocument();
	}
	return outputDocument_.getContentAsDOM();
}

Node::Ptr DbXmlFactoryImpl::createTextNode(const XMLCh *value, const DynamicContext *context) const
{
	getOutputDocument(context);
	return createNode(getOutputDocument(context)->createTextNode(value), outputDocument_, context);
}

Node::Ptr DbXmlFactoryImpl::createCommentNode(const XMLCh *value, const DynamicContext *context) const
{
	getOutputDocument(context);
	return createNode(getOutputDocument(context)->createComment(value), outputDocument_, context);
}

Node::Ptr DbXmlFactoryImpl::createPINode(const XMLCh *name, const XMLCh *value, const DynamicContext *context) const
{
	getOutputDocument(context);
	return createNode(getOutputDocument(context)->createProcessingInstruction(name, value), outputDocument_, context);
}

Node::Ptr DbXmlFactoryImpl::createAttributeNode(const XMLCh *uri, const XMLCh *prefix, const XMLCh *name, const XMLCh *value, const DynamicContext *context) const
{
	getOutputDocument(context);
	DOMAttr *attr = getOutputDocument(context)->createAttributeNS(uri, name);
	if(prefix != 0 && !XPath2Utils::equals(XMLUni::fgZeroLenString, prefix))
		attr->setPrefix(prefix);
	attr->setNodeValue(value);
	return createNode(attr, outputDocument_, context);
}

Node::Ptr DbXmlFactoryImpl::createElementNode(const XMLCh *uri, const XMLCh *prefix, const XMLCh *name,
	const std::vector<Node::Ptr> &attrList, const std::vector<XQFactory::ElementChild> &childList,
	const DynamicContext *ctx) const
{
	const XQContext *context = CAST_TO_CONST_XQCONTEXT(ctx);
	getOutputDocument(context);

// 	XQStaticContext::ConstructionMode constrMode=context->getConstructionMode();

	DOMDocument *document = getOutputDocument(context);
	DOMElement *element = document->createElementNS(uri, name);
	if(prefix != 0 && !XPath2Utils::equals(XMLUni::fgZeroLenString, prefix))
		element->setPrefix(prefix);

	for(std::vector<Node::Ptr>::const_iterator a = attrList.begin(); a != attrList.end(); ++a) {
		const DbXmlNodeImpl *nodeImpl = (const DbXmlNodeImpl *)(*a)->getInterface(DbXmlNodeImpl::gDbXml);
		DBXML_ASSERT(nodeImpl != 0);

		DOMAttr* imported = (DOMAttr*)document->importNode(const_cast<DOMNode*>(nodeImpl->getDOMNode()),true);
// 		if(constrMode == XQStaticContext::CONSTRUCTION_MODE_PRESERVE)
// 			XPath2Utils::copyAttributeType(document, imported, (const DOMAttr*)nodeImpl->getDOMNode());

		element->setAttributeNodeNS(imported); // TBD - check that the attribute isn't a duplicate - jpcs
	}

	for(std::vector<XQFactory::ElementChild>::const_iterator i = childList.begin(); i != childList.end(); ++i) {
		const DbXmlNodeImpl *nodeImpl = (const DbXmlNodeImpl *)(*i)->getInterface(DbXmlNodeImpl::gDbXml);
		DBXML_ASSERT(nodeImpl != 0);

		DOMNode *newChild = NULL;
		if(nodeImpl->getDOMNode()->getOwnerDocument() == document) {
			if(i->clone) {
				newChild = nodeImpl->getDOMNode()->cloneNode(true);
// 				if(constrMode == XQStaticContext::CONSTRUCTION_MODE_PRESERVE && nodeImpl->dmNodeKind()==Node::element_string)
// 					XPath2Utils::copyElementType(newChild->getOwnerDocument(), (DOMElement*)newChild, (DOMElement*)nodeImpl->getDOMNode());
				if(context->getDebugCallback())
					context->getDebugCallback()->ReportClonedNode(const_cast<XQContext*>(context), nodeImpl->getDOMNode(), newChild);
			}
			else {
				newChild = const_cast<DOMNode*>(nodeImpl->getDOMNode());
			}
		}
		else {
			newChild = document->importNode(const_cast<DOMNode*>(nodeImpl->getDOMNode()),true);
// 			if(constrMode == XQStaticContext::CONSTRUCTION_MODE_PRESERVE && nodeImpl->dmNodeKind()==Node::element_string)
// 				XPath2Utils::copyElementType(newChild->getOwnerDocument(), (DOMElement*)newChild, (DOMElement*)nodeImpl->getDOMNode());
			if(context->getDebugCallback()) context->getDebugCallback()->ReportClonedNode(const_cast<XQContext*>(context), nodeImpl->getDOMNode(), newChild);
		}

		element->appendChild(newChild);
	}  

// 	if(constrMode == XQStaticContext::CONSTRUCTION_MODE_PRESERVE)
// 		XPath2Utils::setElementType(document, element, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, SchemaSymbols::fgATTVAL_ANYTYPE);

	return createNode(element, outputDocument_, context);
}

Node::Ptr DbXmlFactoryImpl::createDocumentNode(const std::vector<Node::Ptr> &childList, const DynamicContext *ctx) const
{
	const XQContext *context = CAST_TO_CONST_XQCONTEXT(ctx);
	getOutputDocument(context);

// 	XQStaticContext::ConstructionMode constrMode=context->getConstructionMode();

	QueryContext &qc = CAST_TO_CONST_DBXMLCONTEXT(context)->getQueryContext();
	XmlDocument xmlDoc = qc.getManager().createDocument();

	DOMDocument *document = xmlDoc.getContentAsDOM();


	for(std::vector<Node::Ptr>::const_iterator i = childList.begin(); i != childList.end(); ++i) {
		const DbXmlNodeImpl *nodeImpl = (const DbXmlNodeImpl *)(*i)->getInterface(DbXmlNodeImpl::gDbXml);
		DBXML_ASSERT(nodeImpl != 0);

		DOMNode *newChild = document->importNode(const_cast<DOMNode*>(nodeImpl->getDOMNode()),true);
// 		if(constrMode == XQStaticContext::CONSTRUCTION_MODE_PRESERVE && nodeImpl->dmNodeKind()==Node::element_string)
// 			XPath2Utils::copyElementType(newChild->getOwnerDocument(), (DOMElement*)newChild, (DOMElement*)nodeImpl->getDOMNode());
		if(context->getDebugCallback()) context->getDebugCallback()->ReportClonedNode(const_cast<XQContext*>(context), nodeImpl->getDOMNode(), newChild);

		document->appendChild(newChild);
	}

	return createNode(document, xmlDoc, context);
}

AnyAtomicType::AtomicObjectType DbXmlFactoryImpl::getPrimitiveTypeIndex(const XMLCh* typeURI, const XMLCh* typeName) const
{
	bool isPrimitive;
	const DatatypeFactory* dtf = datatypeLookup_.lookupDatatype(typeURI, typeName, isPrimitive);
	return dtf->getPrimitiveTypeIndex();
}

ATQNameOrDerived::Ptr DbXmlFactoryImpl::createQName(const XMLCh* uri,
	const XMLCh *prefix,
	const XMLCh* name, 
	const DynamicContext* context
	) {
	return createQNameOrDerived(
		SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
		SchemaSymbols::fgDT_QNAME,
		uri,
		prefix,
		name,
		context
		);
}

ATDoubleOrDerived::Ptr DbXmlFactoryImpl::createDouble(const MAPM value, const DynamicContext* context) {
	return createDoubleOrDerived(
		SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
		SchemaSymbols::fgDT_DOUBLE,
		value, context);
}

ATDoubleOrDerived::Ptr DbXmlFactoryImpl::createDouble(const XMLCh* value, const DynamicContext* context) {
	return datatypeLookup_.getDoubleFactory()->createInstance(value, context);
}

ATFloatOrDerived::Ptr DbXmlFactoryImpl::createFloat(const MAPM value, const DynamicContext* context) {
	return createFloatOrDerived(
		SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
		SchemaSymbols::fgDT_FLOAT,
		value, context);
}

ATFloatOrDerived::Ptr DbXmlFactoryImpl::createFloat(const XMLCh* value, const DynamicContext* context) {
	return datatypeLookup_.getFloatFactory()->createInstance(value, context);
}

ATDecimalOrDerived::Ptr DbXmlFactoryImpl::createDecimal(const MAPM value, const DynamicContext* context) {
	return createDecimalOrDerived(
		SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
		SchemaSymbols::fgDT_DECIMAL,
		value, context);
}

ATDecimalOrDerived::Ptr DbXmlFactoryImpl::createDecimal(const XMLCh* value, const DynamicContext* context) {
	return datatypeLookup_.getDecimalFactory()->createInstance(value, context);
}

ATDecimalOrDerived::Ptr DbXmlFactoryImpl::createInteger(const int value, const DynamicContext* context) {
	return context->getMemoryManager()->createInteger(value);
}

ATDecimalOrDerived::Ptr DbXmlFactoryImpl::createInteger(const MAPM value, const DynamicContext* context) {
	return createDecimalOrDerived(
		SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
		SchemaSymbols::fgDT_INTEGER,
		value, context);
}

ATBooleanOrDerived::Ptr DbXmlFactoryImpl::createBoolean(bool value, const DynamicContext* context) {
	return createBooleanOrDerived(
		SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
		SchemaSymbols::fgDT_BOOLEAN,
		value, context);  
}

ATBooleanOrDerived::Ptr DbXmlFactoryImpl::createBoolean(const XMLCh* value, const DynamicContext* context) {
	return datatypeLookup_.getBooleanFactory()->createInstance(value, context);
}

ATDecimalOrDerived::Ptr DbXmlFactoryImpl::createNonNegativeInteger(const MAPM value, const DynamicContext* context) {
	return createDecimalOrDerived(
		SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
		SchemaSymbols::fgDT_NONNEGATIVEINTEGER,
		value, context);
}
      
ATDurationOrDerived::Ptr DbXmlFactoryImpl::createDayTimeDuration(const XMLCh* value, const DynamicContext* context) {
	return datatypeLookup_.getDurationFactory()->
		createInstance(FunctionConstructor::XMLChXPath2DatatypesURI, 
			ATDurationOrDerived::fgDT_DAYTIMEDURATION, value, context);
}

ATDurationOrDerived::Ptr DbXmlFactoryImpl::createYearMonthDuration(const XMLCh* value, const DynamicContext* context) {
	return datatypeLookup_.getDurationFactory()->
		createInstance(FunctionConstructor::XMLChXPath2DatatypesURI, 
			ATDurationOrDerived::fgDT_YEARMONTHDURATION, value, context);
}

ATDateOrDerived::Ptr DbXmlFactoryImpl::createDate(const XMLCh* value, const DynamicContext* context) {
	return datatypeLookup_.getDateFactory()->createInstance(value, context);
}

ATDateTimeOrDerived::Ptr DbXmlFactoryImpl::createDateTime(const XMLCh* value, const DynamicContext* context) {
	return datatypeLookup_.getDateTimeFactory()->createInstance(value, context);
}

ATTimeOrDerived::Ptr DbXmlFactoryImpl::createTime(const XMLCh* value, const DynamicContext* context) {
	return datatypeLookup_.getTimeFactory()->createInstance(value, context);
}

ATAnyURIOrDerived::Ptr DbXmlFactoryImpl::createAnyURI(const XMLCh* value, const DynamicContext* context) {
	return datatypeLookup_.getAnyURIFactory()->createInstance(value, context);
}

ATStringOrDerived::Ptr DbXmlFactoryImpl::createString(const XMLCh* value, const DynamicContext* context) {
	return datatypeLookup_.getStringFactory()->createInstance(value, context);
}

//////////////////////////
// All creation methods //
//////////////////////////


AnyAtomicType::Ptr DbXmlFactoryImpl::createDerivedFromAtomicType(AnyAtomicType::AtomicObjectType typeIndex, const XMLCh* typeURI,
	const XMLCh* typeName, const XMLCh* value, const DynamicContext* context)
{
	return datatypeLookup_.lookupDatatype(typeIndex)->createInstance(typeURI, typeName, value, context);
}

AnyAtomicType::Ptr DbXmlFactoryImpl::createDerivedFromAtomicType(const XMLCh* typeURI,
	const XMLCh* typeName, 
	const XMLCh* value, const DynamicContext* context) {
	bool isPrimitive;
	const DatatypeFactory* dtf = datatypeLookup_.lookupDatatype(typeURI, typeName, isPrimitive);
	if(isPrimitive) {
		return dtf->createInstance(value, context);
	}
	else {
		return dtf->createInstance(typeURI, typeName, value, context);
	}
}

/** create a xs:boolean */
ATBooleanOrDerived::Ptr DbXmlFactoryImpl::createBooleanOrDerived(const XMLCh* typeURI, 
	const XMLCh* typeName,
	const XMLCh* value, 
	const DynamicContext* context) {
	return (const ATBooleanOrDerived::Ptr)datatypeLookup_.getBooleanFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:boolean with a bool value */
ATBooleanOrDerived::Ptr DbXmlFactoryImpl::createBooleanOrDerived(const XMLCh* typeURI, 
	const XMLCh* typeName,
	bool value, 
	const DynamicContext* context) {
	// No need to validate
	return new ATBooleanOrDerivedImpl(typeURI, typeName, value, context);
}

/** create a xs:date */
ATDateOrDerived::Ptr DbXmlFactoryImpl::createDateOrDerived(const XMLCh* typeURI, 
	const XMLCh* typeName,
	const XMLCh* value, 
	const DynamicContext* context){
	return (const ATDateOrDerived::Ptr)datatypeLookup_.getDateFactory()->createInstance(typeURI, typeName, value, context);
}


/** create a xs:dateTime */
ATDateTimeOrDerived::Ptr DbXmlFactoryImpl::createDateTimeOrDerived(const XMLCh* typeURI, 
	const XMLCh* typeName,
	const XMLCh* value,
	const DynamicContext* context){
	return (const ATDateTimeOrDerived::Ptr)datatypeLookup_.getDateTimeFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:decimal */
ATDecimalOrDerived::Ptr DbXmlFactoryImpl::createDecimalOrDerived(const XMLCh* typeURI, 
	const XMLCh* typeName,
	const XMLCh* value,
	const DynamicContext* context){
	return (const ATDecimalOrDerived::Ptr)datatypeLookup_.getDecimalFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:decimal with a MAPM */
ATDecimalOrDerived::Ptr DbXmlFactoryImpl::createDecimalOrDerived(const XMLCh* typeURI, 
	const XMLCh* typeName,
	const MAPM value,
	const DynamicContext* context){
	// No need to validate
	return new ATDecimalOrDerivedImpl(typeURI, typeName, value, context);
}


/** create a xs:double */  
ATDoubleOrDerived::Ptr DbXmlFactoryImpl::createDoubleOrDerived(const XMLCh* typeURI, 
	const XMLCh* typeName,
	const XMLCh* value, 
	const DynamicContext* context){
	if(XPath2Utils::equals(value, Numeric::NAN_string)) {
		value= Numeric::NaN_string;
	}
	return (const ATDoubleOrDerived::Ptr)datatypeLookup_.getDoubleFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:double with a MAPM */
ATDoubleOrDerived::Ptr DbXmlFactoryImpl::createDoubleOrDerived(const XMLCh* typeURI, 
	const XMLCh* typeName,
	const MAPM value, 
	const DynamicContext* context){
	// No need to validate
	return  new ATDoubleOrDerivedImpl(typeURI, typeName, value, context);
}


/** create a xs:duration */
ATDurationOrDerived::Ptr DbXmlFactoryImpl::createDurationOrDerived(const XMLCh* typeURI, 
	const XMLCh* typeName,
	const XMLCh* value, 
	const DynamicContext* context){
	return (const ATDurationOrDerived::Ptr)datatypeLookup_.getDurationFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:float */
ATFloatOrDerived::Ptr DbXmlFactoryImpl::createFloatOrDerived(const XMLCh* typeURI, 
	const XMLCh* typeName,
	const XMLCh* value, 
	const DynamicContext* context){
	if(XMLString::equals(value, Numeric::NAN_string)) {
		value= Numeric::NaN_string;
	}
	return (const ATFloatOrDerived::Ptr)datatypeLookup_.getFloatFactory()->createInstance(typeURI, typeName, value, context);

}

/** create a xs:float with a MAPM */
ATFloatOrDerived::Ptr DbXmlFactoryImpl::createFloatOrDerived(const XMLCh* typeURI, 
	const XMLCh* typeName,
	const MAPM value, 
	const DynamicContext* context) {
	// No need to validate
	return  new ATFloatOrDerivedImpl(typeURI, typeName, value, context);
}

/** create a xs:gDay */
ATGDayOrDerived::Ptr DbXmlFactoryImpl::createGDayOrDerived(const XMLCh* typeURI, 
	const XMLCh* typeName,
	const XMLCh* value, 
	const DynamicContext* context) {
	return (const ATGDayOrDerived::Ptr)datatypeLookup_.getGDayFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:gMonth */
ATGMonthOrDerived::Ptr DbXmlFactoryImpl::createGMonthOrDerived(const XMLCh* typeURI, 
	const XMLCh* typeName,
	const XMLCh* value, 
	const DynamicContext* context) {
	return (const ATGMonthOrDerived::Ptr)datatypeLookup_.getGMonthFactory()->createInstance(typeURI, typeName, value, context);

}


/** create a xs:gMonthDay */
ATGMonthDayOrDerived::Ptr DbXmlFactoryImpl::createGMonthDayOrDerived(const XMLCh* typeURI, 
	const XMLCh* typeName,
	const XMLCh* value, 
	const DynamicContext* context) {
	return (const ATGMonthDayOrDerived::Ptr)datatypeLookup_.getGMonthDayFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:gYear */
ATGYearOrDerived::Ptr DbXmlFactoryImpl::createGYearOrDerived(const XMLCh* typeURI, 
	const XMLCh* typeName,
	const XMLCh* value, 
	const DynamicContext* context) {
	return (const ATGYearOrDerived::Ptr)datatypeLookup_.getGYearFactory()->createInstance(typeURI, typeName, value, context);
}


/** create a xs:gYearMonth */
ATGYearMonthOrDerived::Ptr DbXmlFactoryImpl::createGYearMonthOrDerived(const XMLCh* typeURI, 
	const XMLCh* typeName,
	const XMLCh* value, 
	const DynamicContext* context) {
	return (const ATGYearMonthOrDerived::Ptr)datatypeLookup_.getGYearMonthFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:QName with two parameters */
ATQNameOrDerived::Ptr DbXmlFactoryImpl::createQNameOrDerived(const XMLCh* typeURI, 
	const XMLCh* typeName,
	const XMLCh* uri,
	const XMLCh* prefix,
	const XMLCh* name, 
	const DynamicContext* context) {

	ATQNameOrDerivedImpl* tmp =  new ATQNameOrDerivedImpl(typeURI, typeName, uri, prefix, name, context);
  
	const DatatypeFactory* dtf_anyURI = datatypeLookup_.getAnyURIFactory();
	if(dtf_anyURI->checkInstance(uri, context)) {
		const DatatypeFactory* dtf_NCName = datatypeLookup_.getStringFactory();
		if (dtf_NCName->checkInstance(SchemaSymbols::fgURI_SCHEMAFORSCHEMA, SchemaSymbols::fgDT_NCNAME, name, context)) {
			return tmp;
		} else {
			// this call will obviously fail, but it is better for error reporting, 
			// since we actually get the XMLException's error message 
			return (const ATQNameOrDerived::Ptr )dtf_NCName->
				createInstance(SchemaSymbols::fgURI_SCHEMAFORSCHEMA, SchemaSymbols::fgDT_NCNAME, name, context);
		}
	} else {
		// this call will obviously fail, but it is better for error reporting, 
		// since we actually get the XMLException's error message 
		return (const ATQNameOrDerived::Ptr )dtf_anyURI->createInstance(uri, context);
	}
}


/** create a xs:string */
ATStringOrDerived::Ptr DbXmlFactoryImpl::createStringOrDerived(const XMLCh* typeURI, 
	const XMLCh* typeName,
	const XMLCh* value, 
	const DynamicContext* context) {
	return (const ATStringOrDerived::Ptr)datatypeLookup_.getStringFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:time */
ATTimeOrDerived::Ptr DbXmlFactoryImpl::createTimeOrDerived(const XMLCh* typeURI, 
	const XMLCh* typeName,
	const XMLCh* value, 
	const DynamicContext* context){
	return (const ATTimeOrDerived::Ptr)datatypeLookup_.getTimeFactory()->createInstance(typeURI, typeName, value, context);
}

/** create an xdt:untypedAtomic */
ATUntypedAtomic::Ptr DbXmlFactoryImpl::createUntypedAtomic(const XMLCh* value, const DynamicContext* context) {
	return (const ATUntypedAtomic::Ptr)datatypeLookup_.getUntypedAtomicFactory()->createInstance(value, context);
}
