//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQFactoryImpl.cpp,v 1.7 2005/08/30 10:39:04 jsnelson Exp $
//

#include <xquery/context/XQFactoryImpl.hpp>
#include <xquery/XQContext.hpp>
#include <xquery/XQDebugCallback.hpp>

#include <assert.h>

#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/functions/FunctionConstructor.hpp> // for datatype-uri
#include <pathan/internal/items/impl/ATBooleanOrDerivedImpl.hpp>
#include <pathan/internal/items/impl/ATDecimalOrDerivedImpl.hpp>
#include <pathan/internal/items/impl/ATDoubleOrDerivedImpl.hpp>
#include <pathan/internal/items/impl/ATFloatOrDerivedImpl.hpp>
#include <pathan/internal/items/impl/ATQNameOrDerivedImpl.hpp>
#include <pathan/internal/items/impl/NodeImpl.hpp>
#include <pathan/ATDurationOrDerived.hpp>
#include <pathan/ATUntypedAtomic.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/exceptions/TypeNotFoundException.hpp>
#include <pathan/exceptions/DataItemException.hpp>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLChar.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/validators/datatype/DatatypeValidator.hpp>
#include <xercesc/dom/DOM.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

XQFactoryImpl::XQFactoryImpl(const DocumentCache* dc, MemoryManager* memMgr)
  : datatypeLookup_(dc, memMgr),
    outputDocument_(0),
    memMgr_(memMgr)
{
}

XQFactoryImpl::~XQFactoryImpl()
{
}

void XQFactoryImpl::release()
{
  this->~XQFactoryImpl();
  memMgr_->deallocate(this);
}

DOMDocument *XQFactoryImpl::getOutputDocument(const DynamicContext *context) const
{
  // Lazily create the output document
  if(outputDocument_ == NULLRCP) {
    outputDocument_ = new NodeImpl(context->createNewDocument(), context);
  }
	return (DOMDocument*)outputDocument_->getDOMNode();
}

Node::Ptr XQFactoryImpl::createTextNode(const XMLCh *value, const DynamicContext *context) const
{
  return new NodeImpl(getOutputDocument(context)->createTextNode(value), context);
}

Node::Ptr XQFactoryImpl::createCommentNode(const XMLCh *value, const DynamicContext *context) const
{
  return new NodeImpl(getOutputDocument(context)->createComment(value), context);
}

Node::Ptr XQFactoryImpl::createPINode(const XMLCh *name, const XMLCh *value, const DynamicContext *context) const
{
  return new NodeImpl(getOutputDocument(context)->createProcessingInstruction(name, value), context);
}

Node::Ptr XQFactoryImpl::createAttributeNode(const XMLCh *uri, const XMLCh *prefix, const XMLCh *name, const XMLCh *value, const DynamicContext *context) const
{
  // check if it's an ID
  bool isID=false;
  static XMLCh xmlID[]={ chLatin_x, chLatin_m, chLatin_l, chColon, chLatin_i, chLatin_d, chNull }; 
  if(XPath2Utils::equals(uri, XMLUni::fgXMLURIName) && XPath2Utils::equals(name, xmlID))
  {
    if(!XMLChar1_0::isValidNCName(value, XMLString::stringLen(value)))
      DSLthrow(DataItemException,X("XQFactoryImpl::createAttributeNode"),X("The value of an attribute xml:id must be a valid xs:NCName [err:XQST0082]"));
    isID=true;
  }

  DOMAttr *attr = getOutputDocument(context)->createAttributeNS(uri, name);
  if(prefix != 0 && !XPath2Utils::equals(XMLUni::fgZeroLenString, prefix))
    attr->setPrefix(prefix);
  attr->setNodeValue(value);
/*
  // to mark it as an ID requires access to the internal headers of Xerces; maybe we can fake it's an ID inside
  // Node::dmIsId, or maybe we can change Xerces to automatically turn on the bit when he sees the xml:id name....
  DOMAttrNSImpl *attr = (DOMAttrNSImpl*)getOutputDocument(context)->createAttributeNS(uri, name);
  castToNodeImpl(attr)->isIdAttr(isID); // TODO: add it to the ID map of the document??
*/
  return new NodeImpl(attr, context);
}

Node::Ptr XQFactoryImpl::createElementNode(const XMLCh *uri, const XMLCh *prefix, const XMLCh *name,
                                           const std::vector<Node::Ptr> &attrList, const std::vector<XQFactory::ElementChild> &childList,
                                           const DynamicContext *ctx) const
{
  const XQContext *context = CAST_TO_CONST_XQCONTEXT(ctx);

  XQStaticContext::ConstructionMode constrMode=context->getConstructionMode();

  DOMDocument *document = getOutputDocument(context);
  DOMElement *element = document->createElementNS(uri, name);
  if(prefix != 0 && !XPath2Utils::equals(XMLUni::fgZeroLenString, prefix))
    element->setPrefix(prefix);

  for(std::vector<Node::Ptr>::const_iterator a = attrList.begin(); a != attrList.end(); ++a) {
    const NodeImpl *nodeImpl = (const NodeImpl *)(*a)->getInterface(Item::gPathan);
    assert(nodeImpl != 0);

    const DOMNode* attr=nodeImpl->getDOMNode();
    DOMAttr* exists=element->getAttributeNodeNS(attr->getNamespaceURI(), attr->getLocalName());
    if(exists!=0)
      DSLthrow(DataItemException,X("XQFactoryImpl::createElementNode"),X("An element has two attributes with the same expanded name [err:XQDY0025]"));

    DOMAttr* imported = (DOMAttr*)document->importNode(const_cast<DOMNode*>(attr),true);
    if(constrMode == XQStaticContext::CONSTRUCTION_MODE_PRESERVE)
      XPath2Utils::copyAttributeType(document, imported, (const DOMAttr*)attr);
    if(context->getDebugCallback()) context->getDebugCallback()->ReportClonedNode(const_cast<XQContext*>(context), attr, imported);

    element->setAttributeNodeNS(imported);
  }

  for(std::vector<XQFactory::ElementChild>::const_iterator i = childList.begin(); i != childList.end(); ++i) {
    const NodeImpl *nodeImpl = (const NodeImpl *)(*i)->getInterface(Item::gPathan);
    assert(nodeImpl != 0);

    DOMNode *newChild = NULL;
    if(nodeImpl->getDOMNode()->getOwnerDocument() == document) {
      if(i->clone) {
        newChild = nodeImpl->getDOMNode()->cloneNode(true);
        if(constrMode == XQStaticContext::CONSTRUCTION_MODE_PRESERVE && nodeImpl->dmNodeKind()==Node::element_string)
          XPath2Utils::copyElementType(newChild->getOwnerDocument(), (DOMElement*)newChild, (DOMElement*)nodeImpl->getDOMNode());
        if(context->getDebugCallback()) context->getDebugCallback()->ReportClonedNode(const_cast<XQContext*>(context), nodeImpl->getDOMNode(), newChild);
      }
      else {
        newChild = const_cast<DOMNode*>(nodeImpl->getDOMNode());
      }
    }
    else {
      newChild = document->importNode(const_cast<DOMNode*>(nodeImpl->getDOMNode()),true);
      if(constrMode == XQStaticContext::CONSTRUCTION_MODE_PRESERVE && nodeImpl->dmNodeKind()==Node::element_string)
        XPath2Utils::copyElementType(newChild->getOwnerDocument(), (DOMElement*)newChild, (DOMElement*)nodeImpl->getDOMNode());
      if(context->getDebugCallback()) context->getDebugCallback()->ReportClonedNode(const_cast<XQContext*>(context), nodeImpl->getDOMNode(), newChild);
    }

    element->appendChild(newChild);
  }  

  if(constrMode == XQStaticContext::CONSTRUCTION_MODE_PRESERVE)
    XPath2Utils::setElementType(document, element, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, SchemaSymbols::fgATTVAL_ANYTYPE);

  return new NodeImpl(element, context);
}

Node::Ptr XQFactoryImpl::createDocumentNode(const std::vector<Node::Ptr> &childList, const DynamicContext *ctx) const
{
  const XQContext *context = CAST_TO_CONST_XQCONTEXT(ctx);

  XQStaticContext::ConstructionMode constrMode=context->getConstructionMode();

  DOMDocument *document = context->createNewDocument();

  for(std::vector<Node::Ptr>::const_iterator i = childList.begin(); i != childList.end(); ++i) {
    const NodeImpl *nodeImpl = (const NodeImpl *)(*i)->getInterface(Item::gPathan);
    assert(nodeImpl != 0);

    DOMNode *newChild = document->importNode(const_cast<DOMNode*>(nodeImpl->getDOMNode()),true);
    if(constrMode == XQStaticContext::CONSTRUCTION_MODE_PRESERVE && nodeImpl->dmNodeKind()==Node::element_string)
      XPath2Utils::copyElementType(newChild->getOwnerDocument(), (DOMElement*)newChild, (DOMElement*)nodeImpl->getDOMNode());
    if(context->getDebugCallback()) context->getDebugCallback()->ReportClonedNode(const_cast<XQContext*>(context), nodeImpl->getDOMNode(), newChild);

    document->appendChild(newChild);
  }

  return new NodeImpl(document, context);
}

AnyAtomicType::AtomicObjectType XQFactoryImpl::getPrimitiveTypeIndex(const XMLCh* typeURI, const XMLCh* typeName) const
{
  bool isPrimitive;
  const DatatypeFactory* dtf = datatypeLookup_.lookupDatatype(typeURI, typeName, isPrimitive);
  return dtf->getPrimitiveTypeIndex();
}

ATQNameOrDerived::Ptr XQFactoryImpl::createQName(const XMLCh* uri,
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

ATDoubleOrDerived::Ptr XQFactoryImpl::createDouble(const MAPM value, const DynamicContext* context) {
  return createDoubleOrDerived(
    SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
    SchemaSymbols::fgDT_DOUBLE,
    value, context);
}

ATDoubleOrDerived::Ptr XQFactoryImpl::createDouble(const XMLCh* value, const DynamicContext* context) {
  return datatypeLookup_.getDoubleFactory()->createInstance(value, context);
}

ATFloatOrDerived::Ptr XQFactoryImpl::createFloat(const MAPM value, const DynamicContext* context) {
  return createFloatOrDerived(
    SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
    SchemaSymbols::fgDT_FLOAT,
    value, context);
}

ATFloatOrDerived::Ptr XQFactoryImpl::createFloat(const XMLCh* value, const DynamicContext* context) {
  return datatypeLookup_.getFloatFactory()->createInstance(value, context);
}

ATDecimalOrDerived::Ptr XQFactoryImpl::createDecimal(const MAPM value, const DynamicContext* context) {
  return createDecimalOrDerived(
    SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
    SchemaSymbols::fgDT_DECIMAL,
    value, context);
}

ATDecimalOrDerived::Ptr XQFactoryImpl::createDecimal(const XMLCh* value, const DynamicContext* context) {
  return datatypeLookup_.getDecimalFactory()->createInstance(value, context);
}

ATDecimalOrDerived::Ptr XQFactoryImpl::createInteger(const int value, const DynamicContext* context) {
  return context->getMemoryManager()->createInteger(value);
}

ATDecimalOrDerived::Ptr XQFactoryImpl::createInteger(const MAPM value, const DynamicContext* context) {
  return createDecimalOrDerived(
    SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
    SchemaSymbols::fgDT_INTEGER,
    value, context);
}

ATBooleanOrDerived::Ptr XQFactoryImpl::createBoolean(bool value, const DynamicContext* context) {
  return createBooleanOrDerived(
    SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
    SchemaSymbols::fgDT_BOOLEAN,
    value, context);  
}

ATBooleanOrDerived::Ptr XQFactoryImpl::createBoolean(const XMLCh* value, const DynamicContext* context) {
  return datatypeLookup_.getBooleanFactory()->createInstance(value, context);
}

ATDecimalOrDerived::Ptr XQFactoryImpl::createNonNegativeInteger(const MAPM value, const DynamicContext* context) {
  return createDecimalOrDerived(
    SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
    SchemaSymbols::fgDT_NONNEGATIVEINTEGER,
    value, context);
}
      
ATDurationOrDerived::Ptr XQFactoryImpl::createDayTimeDuration(const XMLCh* value, const DynamicContext* context) {
  return datatypeLookup_.getDurationFactory()->
    createInstance(FunctionConstructor::XMLChXPath2DatatypesURI, 
                   ATDurationOrDerived::fgDT_DAYTIMEDURATION, value, context);
}

ATDurationOrDerived::Ptr XQFactoryImpl::createYearMonthDuration(const XMLCh* value, const DynamicContext* context) {
  return datatypeLookup_.getDurationFactory()->
    createInstance(FunctionConstructor::XMLChXPath2DatatypesURI, 
                   ATDurationOrDerived::fgDT_YEARMONTHDURATION, value, context);
}

ATDateOrDerived::Ptr XQFactoryImpl::createDate(const XMLCh* value, const DynamicContext* context) {
  return datatypeLookup_.getDateFactory()->createInstance(value, context);
}

ATDateTimeOrDerived::Ptr XQFactoryImpl::createDateTime(const XMLCh* value, const DynamicContext* context) {
  return datatypeLookup_.getDateTimeFactory()->createInstance(value, context);
}

ATTimeOrDerived::Ptr XQFactoryImpl::createTime(const XMLCh* value, const DynamicContext* context) {
  return datatypeLookup_.getTimeFactory()->createInstance(value, context);
}

ATAnyURIOrDerived::Ptr XQFactoryImpl::createAnyURI(const XMLCh* value, const DynamicContext* context) {
  return datatypeLookup_.getAnyURIFactory()->createInstance(value, context);
}

ATStringOrDerived::Ptr XQFactoryImpl::createString(const XMLCh* value, const DynamicContext* context) {
  return datatypeLookup_.getStringFactory()->createInstance(value, context);
}

//////////////////////////
// All creation methods //
//////////////////////////


AnyAtomicType::Ptr XQFactoryImpl::createDerivedFromAtomicType(AnyAtomicType::AtomicObjectType typeIndex, const XMLCh* typeURI,
                                                                  const XMLCh* typeName, const XMLCh* value, const DynamicContext* context)
{
  return datatypeLookup_.lookupDatatype(typeIndex)->createInstance(typeURI, typeName, value, context);
}

AnyAtomicType::Ptr XQFactoryImpl::createDerivedFromAtomicType(const XMLCh* typeURI,
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
ATBooleanOrDerived::Ptr XQFactoryImpl::createBooleanOrDerived(const XMLCh* typeURI, 
                                                                  const XMLCh* typeName,
                                                                  const XMLCh* value, 
                                                                  const DynamicContext* context) {
  return (const ATBooleanOrDerived::Ptr)datatypeLookup_.getBooleanFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:boolean with a bool value */
ATBooleanOrDerived::Ptr XQFactoryImpl::createBooleanOrDerived(const XMLCh* typeURI, 
                                                                  const XMLCh* typeName,
                                                                  bool value, 
                                                                  const DynamicContext* context) {
  // No need to validate
  return new ATBooleanOrDerivedImpl(typeURI, typeName, value, context);
}

/** create a xs:date */
ATDateOrDerived::Ptr XQFactoryImpl::createDateOrDerived(const XMLCh* typeURI, 
                                                            const XMLCh* typeName,
                                                            const XMLCh* value, 
                                                            const DynamicContext* context){
  return (const ATDateOrDerived::Ptr)datatypeLookup_.getDateFactory()->createInstance(typeURI, typeName, value, context);
}


/** create a xs:dateTime */
ATDateTimeOrDerived::Ptr XQFactoryImpl::createDateTimeOrDerived(const XMLCh* typeURI, 
                                                                    const XMLCh* typeName,
                                                                    const XMLCh* value,
                                                                    const DynamicContext* context){
  return (const ATDateTimeOrDerived::Ptr)datatypeLookup_.getDateTimeFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:decimal */
ATDecimalOrDerived::Ptr XQFactoryImpl::createDecimalOrDerived(const XMLCh* typeURI, 
                                                                  const XMLCh* typeName,
                                                                  const XMLCh* value,
                                                                  const DynamicContext* context){
  return (const ATDecimalOrDerived::Ptr)datatypeLookup_.getDecimalFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:decimal with a MAPM */
ATDecimalOrDerived::Ptr XQFactoryImpl::createDecimalOrDerived(const XMLCh* typeURI, 
                                                                  const XMLCh* typeName,
                                                                  const MAPM value,
                                                                  const DynamicContext* context){
  // No need to validate
  return new ATDecimalOrDerivedImpl(typeURI, typeName, value, context);
}


/** create a xs:double */  
ATDoubleOrDerived::Ptr XQFactoryImpl::createDoubleOrDerived(const XMLCh* typeURI, 
                                                                const XMLCh* typeName,
                                                                const XMLCh* value, 
                                                                const DynamicContext* context){
  if(XPath2Utils::equals(value, Numeric::NAN_string)) {
    value= Numeric::NaN_string;
  }
  return (const ATDoubleOrDerived::Ptr)datatypeLookup_.getDoubleFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:double with a MAPM */
ATDoubleOrDerived::Ptr XQFactoryImpl::createDoubleOrDerived(const XMLCh* typeURI, 
                                                                const XMLCh* typeName,
                                                                const MAPM value, 
                                                                const DynamicContext* context){
  // No need to validate
  return  new ATDoubleOrDerivedImpl(typeURI, typeName, value, context);
}


/** create a xs:duration */
ATDurationOrDerived::Ptr XQFactoryImpl::createDurationOrDerived(const XMLCh* typeURI, 
                                                                    const XMLCh* typeName,
                                                                    const XMLCh* value, 
                                                                    const DynamicContext* context){
  return (const ATDurationOrDerived::Ptr)datatypeLookup_.getDurationFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:float */
ATFloatOrDerived::Ptr XQFactoryImpl::createFloatOrDerived(const XMLCh* typeURI, 
                                                              const XMLCh* typeName,
                                                              const XMLCh* value, 
                                                              const DynamicContext* context){
  if(XMLString::equals(value, Numeric::NAN_string)) {
    value= Numeric::NaN_string;
  }
  return (const ATFloatOrDerived::Ptr)datatypeLookup_.getFloatFactory()->createInstance(typeURI, typeName, value, context);

}

/** create a xs:float with a MAPM */
ATFloatOrDerived::Ptr XQFactoryImpl::createFloatOrDerived(const XMLCh* typeURI, 
                                                              const XMLCh* typeName,
                                                              const MAPM value, 
                                                              const DynamicContext* context) {
  // No need to validate
  return  new ATFloatOrDerivedImpl(typeURI, typeName, value, context);
}

/** create a xs:gDay */
ATGDayOrDerived::Ptr XQFactoryImpl::createGDayOrDerived(const XMLCh* typeURI, 
                                                            const XMLCh* typeName,
                                                            const XMLCh* value, 
                                                            const DynamicContext* context) {
  return (const ATGDayOrDerived::Ptr)datatypeLookup_.getGDayFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:gMonth */
ATGMonthOrDerived::Ptr XQFactoryImpl::createGMonthOrDerived(const XMLCh* typeURI, 
                                                                const XMLCh* typeName,
                                                                const XMLCh* value, 
                                                                const DynamicContext* context) {
  return (const ATGMonthOrDerived::Ptr)datatypeLookup_.getGMonthFactory()->createInstance(typeURI, typeName, value, context);

}


/** create a xs:gMonthDay */
ATGMonthDayOrDerived::Ptr XQFactoryImpl::createGMonthDayOrDerived(const XMLCh* typeURI, 
                                                                      const XMLCh* typeName,
                                                                      const XMLCh* value, 
                                                                      const DynamicContext* context) {
  return (const ATGMonthDayOrDerived::Ptr)datatypeLookup_.getGMonthDayFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:gYear */
ATGYearOrDerived::Ptr XQFactoryImpl::createGYearOrDerived(const XMLCh* typeURI, 
                                                              const XMLCh* typeName,
                                                              const XMLCh* value, 
                                                              const DynamicContext* context) {
  return (const ATGYearOrDerived::Ptr)datatypeLookup_.getGYearFactory()->createInstance(typeURI, typeName, value, context);
}


/** create a xs:gYearMonth */
ATGYearMonthOrDerived::Ptr XQFactoryImpl::createGYearMonthOrDerived(const XMLCh* typeURI, 
                                                                        const XMLCh* typeName,
                                                                        const XMLCh* value, 
                                                                        const DynamicContext* context) {
  return (const ATGYearMonthOrDerived::Ptr)datatypeLookup_.getGYearMonthFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:QName with two parameters */
ATQNameOrDerived::Ptr XQFactoryImpl::createQNameOrDerived(const XMLCh* typeURI, 
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
ATStringOrDerived::Ptr XQFactoryImpl::createStringOrDerived(const XMLCh* typeURI, 
                                                                const XMLCh* typeName,
                                                                const XMLCh* value, 
                                                                const DynamicContext* context) {
  return (const ATStringOrDerived::Ptr)datatypeLookup_.getStringFactory()->createInstance(typeURI, typeName, value, context);
}

/** create a xs:time */
ATTimeOrDerived::Ptr XQFactoryImpl::createTimeOrDerived(const XMLCh* typeURI, 
                                                            const XMLCh* typeName,
                                                            const XMLCh* value, 
                                                            const DynamicContext* context){
  return (const ATTimeOrDerived::Ptr)datatypeLookup_.getTimeFactory()->createInstance(typeURI, typeName, value, context);
}

/** create an xdt:untypedAtomic */
ATUntypedAtomic::Ptr XQFactoryImpl::createUntypedAtomic(const XMLCh* value, const DynamicContext* context) {
  return (const ATUntypedAtomic::Ptr)datatypeLookup_.getUntypedAtomicFactory()->createInstance(value, context);
}
