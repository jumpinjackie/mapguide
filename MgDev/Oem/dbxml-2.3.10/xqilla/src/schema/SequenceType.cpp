/*
 * Copyright (c) 2001-2006
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2006
 *     Progress Software Corporation. All rights reserved.
 * Copyright (c) 2004-2006
 *     Oracle. All rights reserved.
 *
 * See the file LICENSE for redistribution information.
 *
 * $Id: SequenceType.cpp,v 1.26 2006/11/01 16:37:22 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <assert.h>
#include <sstream>

#include <xqilla/schema/SequenceType.hpp>

#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/schema/DocumentCache.hpp>
#include <xqilla/framework/XPath2MemoryManager.hpp>
#include <xqilla/parser/QName.hpp>
#include <xqilla/exceptions/StaticErrorException.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/exceptions/XPath2TypeMatchException.hpp>
#include <xqilla/exceptions/XPath2TypeCastException.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/items/ATDoubleOrDerived.hpp>
#include <xqilla/items/ATUntypedAtomic.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/functions/FunctionString.hpp>
#include <xqilla/functions/FunctionNumber.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/ast/XQAtomize.hpp>
#include <xqilla/ast/XQTreatAs.hpp>
#include <xqilla/ast/XPath1Compat.hpp>
#include <xqilla/ast/ConvertFunctionArg.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/validators/schema/SchemaAttDef.hpp>
#include <xercesc/validators/schema/SchemaElementDecl.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

SequenceType::SequenceType(const XMLCh* typeURI,const XMLCh* typeName, SequenceType::OccurrenceIndicator occur, XPath2MemoryManager *mm)
{
  m_pItemType=new (mm) SequenceType::ItemType(ItemType::TEST_ATOMIC_TYPE,NULL,new (mm) QualifiedName(XMLUni::fgZeroLenString, typeName));
  m_pItemType->setTypeURI(typeURI);
	m_nOccurrence=occur;
}

SequenceType::SequenceType(SequenceType::ItemType* test, OccurrenceIndicator occur /*= EXACTLY_ONE*/)
{
  m_pItemType=test;
  m_nOccurrence=occur;
}

SequenceType::SequenceType()
{
	m_pItemType=0;
	m_nOccurrence=EXACTLY_ONE;
}

SequenceType::~SequenceType()
{
	if(m_pItemType)
		delete m_pItemType;
}

Result SequenceType::occurrenceMatches(const Result &toBeTested, const LocationInfo *location) const
{
  return new OccurrenceMatchesResult(toBeTested, this, location);
}

Result SequenceType::typeMatches(const Result &toBeTested, const LocationInfo *location) const
{
  return new TypeMatchesResult(toBeTested, this, location);
}

Result SequenceType::matches(const Result &toBeTested, const LocationInfo *location) const
{
  return typeMatches(occurrenceMatches(toBeTested, location), location);
}

void SequenceType::setItemType(SequenceType::ItemType* itemType)
{
  if(m_pItemType)
    delete m_pItemType;
  m_pItemType=itemType;
}

SequenceType::ItemType::ItemTestType SequenceType::getItemTestType(void) const {

  return m_pItemType->getItemTestType();
}

SequenceType::OccurrenceIndicator SequenceType::getOccurrenceIndicator(void) const {

  return m_nOccurrence;
}

void SequenceType::setOccurrence(SequenceType::OccurrenceIndicator nOccurrence)
{
  m_nOccurrence=nOccurrence;
}

void SequenceType::setNameURI(const XMLCh* const nameURI) {

  if(m_pItemType!=NULL) {
    m_pItemType->setNameURI(nameURI);
  }
}

const XMLCh* SequenceType::getNameURI(const StaticContext* context) const {

  if(m_pItemType!=NULL) {
    return m_pItemType->getNameURI(context, this);
  }
  return 0;
}

void SequenceType::setTypeURI(const XMLCh* const typeURI) {

  //cerr << "SequnceType:: Setting typeURI: " << typeURI << endl;

  if(m_pItemType!=NULL) {
    m_pItemType->setTypeURI(typeURI);
  }
}

const XMLCh* SequenceType::getTypeURI(const StaticContext* context) const {

  if(m_pItemType!=NULL) {
    return m_pItemType->getTypeURI(context, this);
  }
  return 0;
}

QualifiedName *SequenceType::getConstrainingName(void) const {

  return m_pItemType->getName();
}

QualifiedName *SequenceType::getConstrainingType(void) const {

  return m_pItemType->getType();
}

void SequenceType::staticResolution(StaticContext* context) const
{
  if(m_pItemType!=NULL && m_pItemType->getItemTestType()==ItemType::TEST_ATOMIC_TYPE && m_pItemType->getType()!=NULL)
  {
      // check if the type to be tested is defined and is really an atomic one
      const XMLCh* uriToCheck=m_pItemType->getTypeURI(context, this);
      const XMLCh* nameToCheck=m_pItemType->getType()->getName();
      if(!context->getDocumentCache()->isTypeDefined(uriToCheck, nameToCheck))
      {
        XMLBuffer buf;
        buf.set(X("Type {"));
        buf.append(uriToCheck);
        buf.append(X("}:"));
        buf.append(nameToCheck);
        buf.append(X(" is not defined [err:XPST0051]"));
        XQThrow(StaticErrorException, X("SequenceType::staticResolution"), buf.getRawBuffer());
      }
      if(!context->getDocumentCache()->isTypeOrDerivedFromType(uriToCheck, nameToCheck,
                                                               FunctionConstructor::XMLChXPath2DatatypesURI, AnyAtomicType::fgDT_ANYATOMICTYPE)) 
      {
        XMLBuffer buf;
        buf.set(X("Type {"));
        buf.append(uriToCheck);
        buf.append(X("}:"));
        buf.append(nameToCheck);
        buf.append(X(" is not an atomic type [err:XPST0051]"));
        XQThrow(StaticErrorException, X("SequenceType::staticResolution"), buf.getRawBuffer());
      }
  }
}

SequenceType::ItemType::ItemType(ItemTestType test,QualifiedName* name /*=NULL*/, QualifiedName* type /*=NULL*/)
{
  m_nTestType=test;
  m_pName=name;
  m_pType=type;
  m_TypeURI=m_NameURI=NULL;
  m_bAllowNil=false;
}

SequenceType::ItemType::~ItemType()
{
    delete m_pName;
    delete m_pType;
}

SequenceType::ItemType::ItemTestType SequenceType::ItemType::getItemTestType(void) const
{
  return m_nTestType;
}

void SequenceType::ItemType::setAllowNilled(bool value)
{
  m_bAllowNil=value;
}

bool SequenceType::ItemType::getAllowNilled() const
{
  return m_bAllowNil;
}

void SequenceType::ItemType::setName(QualifiedName * name)
{
    m_pName=name;
}

QualifiedName *SequenceType::ItemType::getName(void) const {

  return m_pName;
}

void SequenceType::ItemType::setType(QualifiedName * type)
{
    m_pType=type;
}

QualifiedName *SequenceType::ItemType::getType(void) const {

  return m_pType;
}

void SequenceType::ItemType::setNameURI(const XMLCh* const nameURI) {

  m_NameURI = nameURI;
}

void SequenceType::ItemType::setTypeURI(const XMLCh* const typeURI) {

  m_TypeURI = typeURI;
}

const XMLCh* SequenceType::ItemType::getTypeURI(const StaticContext* context, const LocationInfo *location) const
{
    if(m_TypeURI!=0 && *m_TypeURI!=0) {
        return m_TypeURI;
    }
    if(m_pType != 0)
    {
        const XMLCh* prefix=m_pType->getPrefix();
        // an empty prefix means the default element and type namespace
        if(prefix==0 || *prefix==0)
            return context->getDefaultElementAndTypeNS();
        return context->getUriBoundToPrefix(prefix, location);
    }
    return 0;
}

const XMLCh* SequenceType::ItemType::getNameURI(const StaticContext* context, const LocationInfo *location) const
{
    if(m_NameURI!=0 && *m_NameURI!=0)
        return m_NameURI;
    if(m_pName != 0)
    {
        const XMLCh* prefix=m_pName->getPrefix();
        // if we are testing for an attribute, an empty prefix means empty namespace; if we are testing an element, it means 
        // the default element and type namespace
        if(prefix==0 || *prefix==0)
            if(m_nTestType==TEST_ATTRIBUTE || m_nTestType==TEST_SCHEMA_ATTRIBUTE)
                return 0;
            else if(m_nTestType==TEST_ELEMENT || m_nTestType==TEST_SCHEMA_ELEMENT)
                return context->getDefaultElementAndTypeNS();

        return context->getUriBoundToPrefix(prefix, location);
    }
    return 0;
}

void SequenceType::ItemType::getStaticType(StaticType &st, const StaticContext *context, bool &isExact, const LocationInfo *location) const
{
  if(this==NULL)
  {
    st.flags = 0;
    isExact = true;
    return;
  }

  switch(m_nTestType) {
  case TEST_ANYTHING: {
    st.flags = StaticType::ITEM_TYPE;
    isExact = true;
    break;
  }
  case TEST_ATOMIC_TYPE: {
    st.flags = StaticType::getFlagsFor(getTypeURI(context, location), getType()->getName(), context, isExact);
    break;
  }
  case TEST_DOCUMENT: {
    st.flags = StaticType::DOCUMENT_TYPE;
    if(m_pName == NULL && m_pType == NULL)
	    isExact = true;
    else isExact = false;
    break;
  }
  case TEST_ELEMENT: {
    st.flags = StaticType::ELEMENT_TYPE;
    if(m_pName == NULL && m_pType == NULL)
	    isExact = true;
    else isExact = false;
    break;
  }
  case TEST_ATTRIBUTE: {
    st.flags = StaticType::ATTRIBUTE_TYPE;
    if(m_pName == NULL && m_pType == NULL)
	    isExact = true;
    else isExact = false;
    break;
  }
  case TEST_PI: {
    st.flags = StaticType::PI_TYPE;
    if(m_pName == NULL && m_pType == NULL)
	    isExact = true;
    else isExact = false;
    break;
  }
  case TEST_SCHEMA_ELEMENT: {
    st.flags = StaticType::ELEMENT_TYPE;
    isExact = false;
    break;
  }
  case TEST_SCHEMA_ATTRIBUTE: {
    st.flags = StaticType::ATTRIBUTE_TYPE;
    isExact = false;
    break;
  }
  case TEST_COMMENT: {
    st.flags = StaticType::COMMENT_TYPE;
    isExact = true;
    break;
  }
  case TEST_TEXT: {
    st.flags = StaticType::TEXT_TYPE;
    isExact = true;
    break;
  }
  case TEST_NODE: {
    st.flags = StaticType::NODE_TYPE;
    isExact = true;
    break;
  }
  }
}

bool SequenceType::ItemType::matchesNameType(const Item::Ptr &toBeTested, DynamicContext* context,
                                             const LocationInfo *location) const
{
  // Check name constraint
  if(m_pName!=NULL) {
    if(toBeTested->isNode()) {

      ATQNameOrDerived::Ptr name = ((const Node*)(const Item*)toBeTested)->dmNodeName(context);
      if(name.isNull()) return false;

      // Match node name
      const XMLCh* nodeName = m_pName->getName();

      if(!(XPath2Utils::equals(nodeName, ((const ATQNameOrDerived*)name.get())->getName())))
        return false;

      // Match node uri
      if(!(XPath2Utils::equals(getNameURI(context, location), ((const ATQNameOrDerived*)name.get())->getURI())))
        return false;
    }
  }

  //A named atomic type matches a value if the dynamic type of the
  //value is the same as the named atomic type or is derived from the
  //named atomic type by restriction. For example, the ItemType
  //xs:decimal matches the value 12.34 (a decimal literal); it also
  //matches a value whose dynamic type is shoesize, if shoesize is an
  //atomic type derived from xs:decimal.

  if(m_pType) {
    if(!context->getDocumentCache()->isTypeDefined(getTypeURI(context, location), m_pType->getName()))
    {
      XMLBuffer msg(1023, context->getMemoryManager());
      msg.set(X("Type {"));
      msg.append(getTypeURI(context, location));
      msg.append(X("}"));
      msg.append(m_pType->getName());
      msg.append(X(" is not defined [err:XPTY0004]"));
      XQThrow3(XPath2ErrorException,X("SequenceType::ItemType::matchesNameType"), msg.getRawBuffer(), location);
    }
    bool result;
    if(toBeTested->isAtomicValue()) {
      result = ((AnyAtomicType*)(const Item*)toBeTested)->isInstanceOfType(getTypeURI(context, location), m_pType->getName(), context);
    } else if (toBeTested->isNode()) {
      result = ((Node*)(const Item*)toBeTested)->hasInstanceOfType(getTypeURI(context, location), m_pType->getName(), context);
    } else {
      assert(false); // should never get here
    }
    //cerr << "was toBeTested ("<<XMLString::transcode(((AnyAtomicType*)toBeTested)->getTypeURI()) << ":"
    //     << XMLString::transcode(((AnyAtomicType*)toBeTested)->getTypeName())
    //     << ") instance of "<< XMLString::transcode(getTypeURI(context)) << ":" 
    //     << XMLString::transcode(m_pType->getName()) << "? "<< result<<endl;
    
    return result;
  }

  return true;
}

bool SequenceType::ItemType::matches(const Node::Ptr &toBeTested, DynamicContext* context,
                                     const LocationInfo *location) const
{
  switch(m_nTestType) {
    case TEST_ELEMENT:
    {
      if(toBeTested->dmNodeKind() != Node::element_string)
        return false;

      if(!matchesNameType(toBeTested, context, location))
        return false;

      // if the element has xsi:nil="true", m_bAllowNil MUST be true
      if(toBeTested->dmNilled(context)->isTrue() && !m_bAllowNil)
        return false;

      return true;
    }//case

    case TEST_ATTRIBUTE:
    {
      if(toBeTested->dmNodeKind() != Node::attribute_string)
        return false;
      if(!matchesNameType(toBeTested, context, location))
        return false;
      return true;
    }//case

    case TEST_SCHEMA_ELEMENT:
    {
      if(toBeTested->dmNodeKind() != Node::element_string)
        return false;

      // retrieve the type of the element name
      assert(m_pName!=NULL);
      const XMLCh* elementNS=getNameURI(context, location);
      const XMLCh* elementName=m_pName->getName();
      SchemaElementDecl* elemDecl=context->getDocumentCache()->getElementDecl(elementNS, elementName);
      if(elemDecl==NULL) {
        XMLBuffer msg(1023, context->getMemoryManager());
        msg.set(X("Element {"));
        msg.append(elementNS);
        msg.append(X("}"));
        msg.append(elementName);
        msg.append(X(" is not defined as a global element [err:XPST0081]"));
        XQThrow3(StaticErrorException,X("SequenceType::ItemType::matches"), msg.getRawBuffer(), location);
      }
      // 1. The name of the candidate node matches the specified ElementName or matches the name of an element in a 
      //    substitution group headed by an element named ElementName.
      ATQNameOrDerived::Ptr name = toBeTested->dmNodeName(context);
      if(name.isNull()) return false;
      const XMLCh *node_uri = ((const ATQNameOrDerived*)name.get())->getURI();
      const XMLCh *node_name = ((const ATQNameOrDerived*)name.get())->getName();

      if(!(XPath2Utils::equals(elementName, node_name)) ||
         !(XPath2Utils::equals(elementNS, node_uri)))
      {
        // the node doesn't match the ElementName; check if it is in its substitution group
        SchemaElementDecl* thisElemDecl=context->getDocumentCache()->getElementDecl(node_uri, node_name);
        if(thisElemDecl==NULL) // the node to be tested has no type info
            return false;

        SchemaElementDecl* rootElemDecl=thisElemDecl->getSubstitutionGroupElem();
        bool foundIt=false;
        while (rootElemDecl)
        {
          if (XPath2Utils::equals(rootElemDecl->getBaseName(), elementName) &&
              XPath2Utils::equals(context->getDocumentCache()->getSchemaUri(rootElemDecl->getURI()), elementNS))
          {
            foundIt = true;
            break;
          }

          rootElemDecl = rootElemDecl->getSubstitutionGroupElem();
        }
        if(!foundIt)
          return false;
      }

      // 2. derives-from(AT, ET) is true, where AT is the type of the candidate node and ET is the type declared for 
      //    element ElementName in the in-scope element declarations.
      ComplexTypeInfo* pTypeInfo=elemDecl->getComplexTypeInfo();
      if(pTypeInfo && !toBeTested->hasInstanceOfType(pTypeInfo->getTypeUri(), pTypeInfo->getTypeLocalName(), context))
        return false;

      // 3. Either the nilled property of the candidate node is false, or the element declaration for ElementName in 
      //    the in-scope element declarations is nillable.
      if(toBeTested->dmNilled(context).get()->isTrue() &&
         !(elemDecl->getMiscFlags() & SchemaSymbols::XSD_NILLABLE))
        return false;
      
      return true;
    }//case

    case TEST_SCHEMA_ATTRIBUTE:
    {
      if(toBeTested->dmNodeKind() != Node::attribute_string)
        return false;

      // retrieve the type of the attribute name
      assert(m_pName!=NULL);
      const XMLCh* attributeNS=getNameURI(context, location);
      const XMLCh* attributeName=m_pName->getName();
      SchemaAttDef* attrDecl=context->getDocumentCache()->getAttributeDecl(attributeNS, attributeName);
      if(attrDecl==NULL) {
        XMLBuffer msg(1023, context->getMemoryManager());
        msg.set(X("Attribute {"));
        msg.append(attributeNS);
        msg.append(X("}"));
        msg.append(attributeName);
        msg.append(X(" is not defined as a global attribute [err:XPST0081]"));
        XQThrow3(StaticErrorException,X("SequenceType::ItemType::matches"), msg.getRawBuffer(), location);
      }
      // 1. The name of the candidate node matches the specified AttributeName
      ATQNameOrDerived::Ptr name = toBeTested->dmNodeName(context);
      if(name.isNull()) return false;
      const XMLCh *node_uri = ((const ATQNameOrDerived*)name.get())->getURI();
      const XMLCh *node_name = ((const ATQNameOrDerived*)name.get())->getName();

      if(!(XPath2Utils::equals(attributeName, node_name)) ||
         !(XPath2Utils::equals(attributeNS, node_uri)))
        return false;

      // 2. derives-from(AT, ET) is true, where AT is the type of the candidate node and ET is the type declared 
      //    for attribute AttributeName in the in-scope attribute declarations.
      DatatypeValidator* pDV=attrDecl->getDatatypeValidator();
      if(pDV && !toBeTested->hasInstanceOfType(pDV->getTypeUri(), pDV->getTypeLocalName(), context))
        return false;

      return true;
    }//case

    case TEST_NODE:
    {
      return true;
    }

    case TEST_PI:
    {
      if(toBeTested->dmNodeKind() != Node::processing_instruction_string)
        return false;
      if(!matchesNameType(toBeTested, context, location))
        return false;
      return true;
    }

    case TEST_COMMENT:
    {
      return (toBeTested->dmNodeKind() == Node::comment_string);
    }

    case TEST_TEXT:
    {
      return (toBeTested->dmNodeKind() == Node::text_string);
    }

    case TEST_DOCUMENT:
    {
        if(toBeTested->dmNodeKind() != Node::document_string)
          return false;
        if(m_pName!=NULL || m_pType!=NULL) {

          // if we have a constraint on name/type, they apply to the document element
          Result children = toBeTested->dmChildren(context,0);
          Node::Ptr docElement;
          while((docElement = children->next(context)).notNull() &&
                docElement->dmNodeKind() != Node::element_string) {}

          if(docElement.isNull()) return false;

          if(!matchesNameType(docElement, context, location))
            return false;            
        }
        return true;
    }

    case TEST_ANYTHING:
    {
      return true;
    }

    case TEST_ATOMIC_TYPE:
    {
        return false;
    }//case
  }//switch
  return true;
}

bool SequenceType::ItemType::matches(const Item::Ptr &toBeTested, DynamicContext* context,
                                     const LocationInfo *location) const
{
  if(toBeTested->isNode())
    return matches((const Node::Ptr)toBeTested, context, location);
    
  switch(m_nTestType) {
    case TEST_ELEMENT:
    case TEST_ATTRIBUTE:
    case TEST_SCHEMA_ELEMENT:
    case TEST_SCHEMA_ATTRIBUTE:
    case TEST_NODE:
    case TEST_PI:
    case TEST_COMMENT:
    case TEST_TEXT:
    case TEST_DOCUMENT:
    {
      return false;
    }
    
    case TEST_ANYTHING:
    {
      return true;
    }

    case TEST_ATOMIC_TYPE:
    {
        return matchesNameType(toBeTested, context, location);
    }//case

  }//switch
  return true;
}

ASTNode *SequenceType::convertFunctionArg(ASTNode *arg, StaticContext *context, bool numericFunction,
                                          const LocationInfo *location) const
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  // From XPath2 Spec, Section 3.1.5 (http://www.w3.org/TR/xpath20/#id-function-calls)

  // The function conversion rules are used to convert an argument value to its expected type; that is, to the
  // declared type of the function parameter. The expected type is expressed as a SequenceType. The function
  // conversion rules are applied to a given value as follows:

  if(m_pItemType!=NULL)
  {
    SequenceType::ItemType::ItemTestType testType = getItemTestType();
    // FS says we atomize first if the sequence type is atomic, and I think that's sensible - jpcs
    if( testType == ItemType::TEST_ATOMIC_TYPE) {
      arg = new (mm) XQAtomize(arg, mm);
      arg->setLocationInfo(location);
    }

    // If XPath 1.0 compatibility mode is true and an argument is not of the expected type, then the following
    // conversions are applied sequentially to the argument value V:
    if(context->getXPath1CompatibilityMode()) {
      if(m_nOccurrence == SequenceType::EXACTLY_ONE || m_nOccurrence == SequenceType::QUESTION_MARK) {
          arg = new (mm) XPath1CompatConvertFunctionArg(arg, this, mm);
          arg->setLocationInfo(location);
      }
    }
    // If the expected type is a sequence of an atomic type (possibly with an occurrence indicator *, +, or ?),
    // the following conversions are applied:
    else if(testType == ItemType::TEST_ATOMIC_TYPE) {
      const XMLCh *uri = m_pItemType->getTypeURI(context, this);
      const XMLCh *name = m_pItemType->getType()->getName();

      if(numericFunction &&
         XPath2Utils::equals(uri, SchemaSymbols::fgURI_SCHEMAFORSCHEMA) &&
         XPath2Utils::equals(name, AnyAtomicType::fgDT_ANYATOMICTYPE)) {
        arg = new (mm) XQPromoteUntyped(arg, 
                                        SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                        SchemaSymbols::fgDT_DOUBLE, 
                                        mm);
        arg->setLocationInfo(location);
      }
      else {
        arg = new (mm) XQPromoteUntyped(arg, uri, name, mm);
        arg->setLocationInfo(location);
      }

      arg = new (mm) XQPromoteNumeric(arg, uri, name, mm);
      arg->setLocationInfo(location);
      arg = new (mm) XQPromoteAnyURI(arg, uri, name, mm);
      arg->setLocationInfo(location);
    }
  }
  // If, after the above conversions, the resulting value does not match the expected type according to the
  // rules for SequenceType Matching, a type error is raised. [err:XPTY0004] Note that the rules for
  // SequenceType Matching permit a value of a derived type to be substituted for a value of its base type.
  arg = new (mm) XQTreatAs(arg, this, mm);
  arg->setLocationInfo(location);

  return arg;
}

const SequenceType::ItemType *SequenceType::getItemType() const {
  return m_pItemType;
}

////////////////////////////////////////
// OccurrenceMatchesResult
////////////////////////////////////////

SequenceType::OccurrenceMatchesResult::OccurrenceMatchesResult(const Result &parent, const SequenceType *seqType, const LocationInfo *location)
  : ResultImpl(location),
    _seqType(seqType),
    _parent(parent),
    _toDo(true)
{
}

Item::Ptr SequenceType::OccurrenceMatchesResult::next(DynamicContext *context)
{
  Item::Ptr item = _parent->next(context);
  if(_toDo) {
    _toDo = false;

    // "SequenceType matching between a given value and a given SequenceType is performed as follows:
    //  If the SequenceType is empty, the match succeeds only if the value is an empty sequence."
    if(_seqType->getItemType() == NULL && item != NULLRCP) {
      XQThrow(XPath2TypeMatchException, X("SequenceType::MatchesResult::next"),
               X("SequenceType matching failed: the sequence is not empty [err:XPTY0004]"));
    }
    // "If the SequenceType contains an ItemType and an OccurrenceIndicator, the match succeeds only if 
    //  the number of items in the value matches the OccurrenceIndicator and each of these items matches the ItemType. "
    if(_seqType->getItemType() &&
       (_seqType->getOccurrenceIndicator() == PLUS || _seqType->getOccurrenceIndicator() == EXACTLY_ONE) &&
       item == NULLRCP) {
      XQThrow(XPath2TypeMatchException, X("SequenceType::MatchesResult::next"),
               X("SequenceType matching failed: the sequence does not contain items [err:XPTY0004]"));
    }

    // "If the SequenceType is an ItemType with no OccurrenceIndicator, the match succeeds only if 
    //  the value contains precisely one item and that item matches the ItemType "
    if(_seqType->getItemType() &&
       (_seqType->getOccurrenceIndicator() == EXACTLY_ONE || _seqType->getOccurrenceIndicator() == QUESTION_MARK) &&
       item != NULLRCP) {
      // Do the tests on the number of items up front,
      // since often functions that cast to a single or
      // optional item only call next once. - jpcs

      Item::Ptr second = _parent->next(context);

      if(second == NULLRCP) {
        _parent = 0;
      }
      else {
        XQThrow(XPath2TypeMatchException, X("SequenceType::MatchesResult::next"),
                 X("SequenceType matching failed: the sequence contains more than one item [err:XPTY0004]"));
      }
    }
  }

  if(item == NULLRCP) {
    _parent = 0;
  }

  return item;
}

std::string SequenceType::OccurrenceMatchesResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<occurrence_matches>" << std::endl;
  oss << _parent->asString(context, indent + 1);
  oss << in << "</occurrence_matches>" << std::endl;

  return oss.str();
}

////////////////////////////////////////
// TypeMatchesResult
////////////////////////////////////////

SequenceType::TypeMatchesResult::TypeMatchesResult(const Result &parent, const SequenceType *seqType, const LocationInfo *location)
  : ResultImpl(location),
    _seqType(seqType),
    _parent(parent)
{
}

Item::Ptr SequenceType::TypeMatchesResult::next(DynamicContext *context)
{
  Item::Ptr item = _parent->next(context);
  if(item == NULLRCP) {
    _parent = 0;
  }
  else if(!_seqType->getItemType()->matches(item, context, this)) {
    XQThrow(XPath2TypeMatchException, X("SequenceType::MatchesResult::next"),
            X("ItemType matching failed [err:XPTY0004]"));
  }

  return item;
}

std::string SequenceType::TypeMatchesResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<type_matches>" << std::endl;
  oss << _parent->asString(context, indent + 1);
  oss << in << "</type_matches>" << std::endl;

  return oss.str();
}
