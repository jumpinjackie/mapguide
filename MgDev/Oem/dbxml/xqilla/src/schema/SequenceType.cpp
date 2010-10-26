/*
 * Copyright (c) 2001-2008
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2008
 *     Oracle. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * $Id$
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
#include <xqilla/items/FunctionRef.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/functions/FunctionString.hpp>
#include <xqilla/functions/FunctionNumber.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/ast/XQAtomize.hpp>
#include <xqilla/ast/XQTreatAs.hpp>
#include <xqilla/ast/XPath1Compat.hpp>
#include <xqilla/ast/ConvertFunctionArg.hpp>

#include <xercesc/validators/schema/SchemaAttDef.hpp>
#include <xercesc/validators/schema/SchemaElementDecl.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

SequenceType::SequenceType(const XMLCh* typeURI,const XMLCh* typeName, SequenceType::OccurrenceIndicator occur, XPath2MemoryManager *mm)
  : m_pItemType(new (mm) SequenceType::ItemType(typeURI, typeName, mm)),
    m_nOccurrence(occur)
{
}

SequenceType::SequenceType(SequenceType::ItemType* test, OccurrenceIndicator occur)
  : m_pItemType(test),
    m_nOccurrence(occur)
{
}

SequenceType::SequenceType()
  : m_pItemType(0),
    m_nOccurrence(EXACTLY_ONE)
{
}

SequenceType::~SequenceType()
{
  if(m_pItemType)
    delete m_pItemType;
}

Result SequenceType::occurrenceMatches(const Result &toBeTested, const LocationInfo *location, const XMLCh *errorCode) const
{
  return new OccurrenceMatchesResult(toBeTested, this, location, errorCode);
}

Result SequenceType::typeMatches(const Result &toBeTested, const LocationInfo *location, const XMLCh *errorCode) const
{
  return new TypeMatchesResult(toBeTested, this, location, errorCode);
}

Result SequenceType::matches(const Result &toBeTested, const LocationInfo *location, const XMLCh *errorCode) const
{
  return typeMatches(occurrenceMatches(toBeTested, location, errorCode), location, errorCode);
}

void SequenceType::setItemType(SequenceType::ItemType* itemType)
{
  if(m_pItemType)
    delete m_pItemType;
  m_pItemType=itemType;
}

SequenceType::ItemType::ItemTestType SequenceType::getItemTestType() const {

  return m_pItemType->getItemTestType();
}

SequenceType::OccurrenceIndicator SequenceType::getOccurrenceIndicator() const {

  return m_nOccurrence;
}

void SequenceType::setOccurrence(SequenceType::OccurrenceIndicator nOccurrence)
{
  m_nOccurrence=nOccurrence;
}

const XMLCh* SequenceType::getNameURI() const {

  if(m_pItemType)
    return m_pItemType->getNameURI();
  return 0;
}

const XMLCh* SequenceType::getTypeURI() const {

  if(m_pItemType)
    return m_pItemType->getTypeURI();
  return 0;
}

QualifiedName *SequenceType::getConstrainingName() const {

  return m_pItemType->getName();
}

QualifiedName *SequenceType::getConstrainingType() const {

  return m_pItemType->getType();
}

void SequenceType::staticResolution(StaticContext* context)
{
  if(m_pItemType)
    m_pItemType->staticResolution(context, this);
}

void SequenceType::ItemType::staticResolution(StaticContext *context, const LocationInfo *location)
{
  // Prefix resolution should only happen once
  // (since SequenceType objects can be multiple times in the AST)
  if(!staticallyResolved_) {
    staticallyResolved_ = true;

    if(m_pType && m_TypeURI == 0) {
      const XMLCh *prefix = m_pType->getPrefix();
      // an empty prefix means the default element and type namespace
      if(prefix == 0 || *prefix == 0) {
        m_TypeURI = context->getDefaultElementAndTypeNS();
      }
      else {
        m_TypeURI = context->getUriBoundToPrefix(prefix, location);
      }
    }
  
    if(m_pName && m_NameURI == 0) {
      const XMLCh *prefix = m_pName->getPrefix();
      // if we are testing for an attribute, an empty prefix means empty namespace; if we are testing an element, it means 
      // the default element and type namespace
      if(prefix == 0 || *prefix == 0) {
        if(m_nTestType == TEST_ELEMENT || m_nTestType == TEST_SCHEMA_ELEMENT ||
           m_nTestType == TEST_DOCUMENT || m_nTestType == TEST_SCHEMA_DOCUMENT)
          m_NameURI = context->getDefaultElementAndTypeNS();
      } else {
        m_NameURI = context->getUriBoundToPrefix(prefix, location);
      }
    }
  }

  if(m_pType) {
    if(m_nTestType == ItemType::TEST_ATOMIC_TYPE) {
      // check if the type to be tested is defined and is really an atomic one
      if(!context->getDocumentCache()->isTypeDefined(m_TypeURI, m_pType->getName())) {
        XMLBuffer buf;
        buf.set(X("Type {"));
        buf.append(m_TypeURI);
        buf.append(X("}:"));
        buf.append(m_pType->getName());
        buf.append(X(" is not defined [err:XPST0051]"));
        XQThrow3(StaticErrorException, X("SequenceType::ItemType::staticResolution"), buf.getRawBuffer(), location);
      }
      if(!context->getDocumentCache()->isTypeOrDerivedFromType(m_TypeURI, m_pType->getName(),
                                                               FunctionConstructor::XMLChXPath2DatatypesURI,
                                                               AnyAtomicType::fgDT_ANYATOMICTYPE)) {
        XMLBuffer buf;
        buf.set(X("Type {"));
        buf.append(m_TypeURI);
        buf.append(X("}:"));
        buf.append(m_pType->getName());
        buf.append(X(" is not an atomic type [err:XPST0051]"));
        XQThrow3(StaticErrorException, X("SequenceType::ItemType::staticResolution"), buf.getRawBuffer(), location);
      }
    }
    else if(!context->getDocumentCache()->isTypeDefined(m_TypeURI, m_pType->getName())) {
      XMLBuffer msg;
      msg.set(X("Type {"));
      msg.append(m_TypeURI);
      msg.append(X("}"));
      msg.append(m_pType->getName());
      msg.append(X(" is not defined [err:XPTY0004]"));
      XQThrow3(XPath2ErrorException,X("SequenceType::ItemType::matchesNameType"), msg.getRawBuffer(), location);
    }
  }

  switch(m_nTestType) {
  case TEST_SCHEMA_DOCUMENT:
  case TEST_SCHEMA_ELEMENT: {
    // retrieve the type of the element name
    SchemaElementDecl *elemDecl = context->getDocumentCache()->getElementDecl(m_NameURI, m_pName->getName());
    if(elemDecl == NULL) {
      XMLBuffer msg(1023, context->getMemoryManager());
      msg.set(X("Element {"));
      msg.append(m_NameURI);
      msg.append(X("}"));
      msg.append(m_pName->getName());
      msg.append(X(" is not defined as a global element [err:XPST0081]"));
      XQThrow3(StaticErrorException,X("SequenceType::ItemType::staticResolution"), msg.getRawBuffer(), location);
    }
    break;
  }
  case TEST_SCHEMA_ATTRIBUTE: {
    // retrieve the type of the attribute name
    SchemaAttDef *attrDecl = context->getDocumentCache()->getAttributeDecl(m_NameURI, m_pName->getName());
    if(attrDecl == NULL) {
      XMLBuffer msg(1023, context->getMemoryManager());
      msg.set(X("Attribute {"));
      msg.append(m_NameURI);
      msg.append(X("}"));
      msg.append(m_pName->getName());
      msg.append(X(" is not defined as a global attribute [err:XPST0081]"));
      XQThrow3(StaticErrorException,X("SequenceType::ItemType::staticResolution"), msg.getRawBuffer(), location);
    }
    break;
  }
  default: break;
  }

  if(returnType_)
    returnType_->staticResolution(context);
}

SequenceType::ItemType::ItemType(ItemTestType test,QualifiedName* name, QualifiedName* type)
  : m_nTestType(test),
    m_pName(name),
    m_pType(type),
    m_NameURI(0),
    m_TypeURI(0),
    m_bAllowNil(false),
    argTypes_(0),
    returnType_(0),
    staticallyResolved_(false)
{
}

SequenceType::ItemType::ItemType(const XMLCh *typeURI,const XMLCh *typeName, XPath2MemoryManager *mm)
  : m_nTestType(TEST_ATOMIC_TYPE),
    m_pName(0),
    m_pType(new (mm) QualifiedName(typeName, mm)),
    m_NameURI(0),
    m_TypeURI(typeURI),
    m_bAllowNil(false),
    argTypes_(0),
    returnType_(0),
    staticallyResolved_(true)
{
}

SequenceType::ItemType::ItemType(VectorOfSequenceTypes *argTypes, SequenceType *returnType)
  : m_nTestType(TEST_FUNCTION),
    m_pName(0),
    m_pType(0),
    m_NameURI(0),
    m_TypeURI(0),
    m_bAllowNil(false),
    argTypes_(argTypes),
    returnType_(returnType),
    staticallyResolved_(true)
{
}

SequenceType::ItemType::~ItemType()
{
    delete m_pName;
    delete m_pType;
    delete returnType_;
}

SequenceType::ItemType::ItemTestType SequenceType::ItemType::getItemTestType() const
{
  return m_nTestType;
}

void SequenceType::ItemType::setItemTestType(ItemTestType t)
{
  m_nTestType = t;
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

QualifiedName *SequenceType::ItemType::getName() const {

  return m_pName;
}

void SequenceType::ItemType::setType(QualifiedName * type)
{
    m_pType=type;
}

QualifiedName *SequenceType::ItemType::getType() const {

  return m_pType;
}

const XMLCh* SequenceType::ItemType::getTypeURI() const
{
  return m_TypeURI;
}

const XMLCh* SequenceType::ItemType::getNameURI() const
{
  return m_NameURI;
}

void SequenceType::getStaticType(StaticType &st, const StaticContext *context,
                                 bool &isExact, const LocationInfo *location) const
{
  if(m_pItemType == 0) {
    st = StaticType();
    isExact = true;
  }
  else {
    m_pItemType->getStaticType(st, context, isExact, location);

    switch(m_nOccurrence) {
    case EXACTLY_ONE: break;
    case STAR: st.multiply(0, StaticType::UNLIMITED); break;
    case PLUS: st.multiply(1, StaticType::UNLIMITED); break;
    case QUESTION_MARK: st.multiply(0, 1); break;
    }
  }
}

void SequenceType::ItemType::getStaticType(StaticType &st, const StaticContext *context, bool &isExact, const LocationInfo *location) const
{
  if(this == NULL) {
    st = StaticType();
    isExact = true;
    return;
  }

  switch(m_nTestType) {
  case TEST_ANYTHING: {
    st = StaticType::ITEM_TYPE;
    isExact = true;
    break;
  }
  case TEST_ATOMIC_TYPE: {
    st = StaticType::create(m_TypeURI, m_pType->getName(), context, isExact);
    break;
  }
  case TEST_DOCUMENT: {
    st = StaticType::DOCUMENT_TYPE;
    if(m_pName == NULL && m_pType == NULL)
      isExact = true;
    else isExact = false;
    break;
  }
  case TEST_ELEMENT: {
    st = StaticType::ELEMENT_TYPE;
    if(m_pName == NULL && m_pType == NULL)
      isExact = true;
    else isExact = false;
    break;
  }
  case TEST_ATTRIBUTE: {
    st = StaticType::ATTRIBUTE_TYPE;
    if(m_pName == NULL && m_pType == NULL)
      isExact = true;
    else isExact = false;
    break;
  }
  case TEST_PI: {
    st = StaticType::PI_TYPE;
    if(m_pName == NULL && m_pType == NULL)
      isExact = true;
    else isExact = false;
    break;
  }
  case TEST_SCHEMA_ELEMENT: {
    st = StaticType::ELEMENT_TYPE;
    isExact = false;
    break;
  }
  case TEST_SCHEMA_ATTRIBUTE: {
    st = StaticType::ATTRIBUTE_TYPE;
    isExact = false;
    break;
  }
  case TEST_SCHEMA_DOCUMENT: {
    st = StaticType::DOCUMENT_TYPE;
    isExact = false;
    break;
  }
  case TEST_COMMENT: {
    st = StaticType::COMMENT_TYPE;
    isExact = true;
    break;
  }
  case TEST_TEXT: {
    st = StaticType::TEXT_TYPE;
    isExact = true;
    break;
  }
  case TEST_NODE: {
    st = StaticType::NODE_TYPE;
    isExact = true;
    break;
  }
  case TEST_FUNCTION: {
    if(returnType_ == 0) {
      st = StaticType::FUNCTION_TYPE;
      isExact = true;
    }
    else {
      StaticType ret;
      returnType_->getStaticType(ret, context, isExact, location);

      isExact = isExact && argTypes_->size() == 0;

      st = StaticType(context->getMemoryManager(), (unsigned int)argTypes_->size(), ret);
    }
    break;
  }
  }
}

void SequenceType::toBuffer(XMLBuffer &buffer) const
{
  if(m_pItemType == 0) {
    buffer.append(X("empty-sequence()"));
  }
  else {
    m_pItemType->toBuffer(buffer, m_nOccurrence != EXACTLY_ONE);

    switch(m_nOccurrence) {
    case EXACTLY_ONE: break;
    case STAR: buffer.append('*'); break;
    case PLUS: buffer.append('+'); break;
    case QUESTION_MARK: buffer.append('?'); break;
    }
  }
}

inline void outputPrefixOrURI(const XMLCh *prefix, const XMLCh *uri, XMLBuffer &buffer)
{
  if(prefix != 0) {
    buffer.append(prefix);
    buffer.append(':');
  }
  else if(XPath2Utils::equals(uri, SchemaSymbols::fgURI_SCHEMAFORSCHEMA)) {
    buffer.append(X("xs:"));
  }
  else if(uri != 0) {
    buffer.append('{');
    buffer.append(uri);
    buffer.append('}');
  }
}

void SequenceType::ItemType::toBuffer(XMLBuffer &buffer, bool addBrackets) const
{
  switch(m_nTestType) {
  case TEST_ANYTHING: {
    buffer.append(X("item()"));
    break;
  }
  case TEST_ATOMIC_TYPE: {
    outputPrefixOrURI(m_pType->getPrefix(), m_TypeURI, buffer);
    buffer.append(m_pType->getName());
    break;
  }
  case TEST_SCHEMA_DOCUMENT:
  case TEST_DOCUMENT: {
    buffer.append(X("document-node("));

    if(m_pName != NULL || m_pType != NULL) {

      if(m_nTestType == TEST_DOCUMENT)
        buffer.append(X("element("));
      else
        buffer.append(X("schema-element("));

      if(m_pName != NULL) {
        outputPrefixOrURI(m_pName->getPrefix(), m_NameURI, buffer);
        buffer.append(m_pName->getName());
      }

      if(m_pType != NULL) {
        if(m_pName == NULL) {
          buffer.append('*');
        }
        buffer.append(X(", "));
        outputPrefixOrURI(m_pType->getPrefix(), m_TypeURI, buffer);
        buffer.append(m_pType->getName());
      }

      buffer.append(')');
    }

    buffer.append(')');
    break;
  }
  case TEST_ELEMENT: {
    buffer.append(X("element("));

    if(m_pName != NULL) {
      outputPrefixOrURI(m_pName->getPrefix(), m_NameURI, buffer);
      buffer.append(m_pName->getName());
    }

    if(m_pType != NULL) {
      if(m_pName == NULL) {
        buffer.append('*');
      }
      buffer.append(X(", "));
      outputPrefixOrURI(m_pType->getPrefix(), m_TypeURI, buffer);
      buffer.append(m_pType->getName());
    }

    buffer.append(')');
    break;
  }
  case TEST_ATTRIBUTE: {
    buffer.append(X("attribute("));

    if(m_pName != NULL) {
      outputPrefixOrURI(m_pName->getPrefix(), m_NameURI, buffer);
      buffer.append(m_pName->getName());
    }

    if(m_pType != NULL) {
      if(m_pName == NULL) {
        buffer.append('*');
      }
      buffer.append(X(", "));
      outputPrefixOrURI(m_pType->getPrefix(), m_TypeURI, buffer);
      buffer.append(m_pType->getName());
    }

    buffer.append(')');
    break;
  }
  case TEST_PI: {
    buffer.append(X("processing-instruction("));

    if(m_pName != NULL) {
      outputPrefixOrURI(m_pName->getPrefix(), m_NameURI, buffer);
      buffer.append(m_pName->getName());
    }

    buffer.append(')');
    break;
  }
  case TEST_SCHEMA_ELEMENT: {
    buffer.append(X("schema-element("));

    if(m_pName != NULL) {
      outputPrefixOrURI(m_pName->getPrefix(), m_NameURI, buffer);
      buffer.append(m_pName->getName());
    }

    buffer.append(')');
    break;
  }
  case TEST_SCHEMA_ATTRIBUTE: {
    buffer.append(X("schema-attribute("));

    if(m_pName != NULL) {
      outputPrefixOrURI(m_pName->getPrefix(), m_NameURI, buffer);
      buffer.append(m_pName->getName());
    }

    buffer.append(')');
    break;
  }
  case TEST_COMMENT: {
    buffer.append(X("comment()"));
    break;
  }
  case TEST_TEXT: {
    buffer.append(X("text()"));
    break;
  }
  case TEST_NODE: {
    buffer.append(X("node()"));
    break;
  }
  case TEST_FUNCTION: {
    if(returnType_ == 0) {
      buffer.append(X("function()"));
    }
    else {
      addBrackets = addBrackets && returnType_->getOccurrenceIndicator() == EXACTLY_ONE;
      if(addBrackets) buffer.append('(');

      buffer.append(X("function("));

      bool doneOne = false;
      for(VectorOfSequenceTypes::const_iterator i = argTypes_->begin();
          i != argTypes_->end(); ++i) {
        (*i)->toBuffer(buffer);
        if(doneOne) buffer.append(',');
        doneOne = true;
      }
      buffer.append(X(") as "));
      returnType_->toBuffer(buffer);

      if(addBrackets) buffer.append(')');
    }
    break;
  }
  }
}

bool SequenceType::ItemType::matchesNameType(const Item::Ptr &toBeTested, const DynamicContext* context) const
{
  // Check name constraint
  if(m_pName) {
    if(toBeTested->isNode()) {
      ATQNameOrDerived::Ptr name = ((const Node*)(const Item*)toBeTested)->dmNodeName(context);
      if(name.isNull()) return false;

      // Match node name
      if(!(XPath2Utils::equals(m_pName->getName(), ((const ATQNameOrDerived*)name.get())->getName())))
        return false;

      // Match node uri
      if(!(XPath2Utils::equals(m_NameURI, ((const ATQNameOrDerived*)name.get())->getURI())))
        return false;
    }
    else return false;
  }

  //A named atomic type matches a value if the dynamic type of the
  //value is the same as the named atomic type or is derived from the
  //named atomic type by restriction. For example, the ItemType
  //xs:decimal matches the value 12.34 (a decimal literal); it also
  //matches a value whose dynamic type is shoesize, if shoesize is an
  //atomic type derived from xs:decimal.

  if(m_pType) {
    if(toBeTested->isAtomicValue()) {
      return ((AnyAtomicType*)toBeTested.get())->isInstanceOfType(m_TypeURI, m_pType->getName(), context);
    } else if (toBeTested->isNode()) {
      return ((Node*)toBeTested.get())->hasInstanceOfType(m_TypeURI, m_pType->getName(), context);
    }
    return false;
  }

  return true;
}

bool SequenceType::ItemType::matchesSchemaElement(const Node::Ptr &toBeTested, const DynamicContext* context) const
{
  // retrieve the type of the element name
  assert(m_pName!=NULL);
  const XMLCh* elementNS=m_NameURI;
  const XMLCh* elementName=m_pName->getName();
  SchemaElementDecl *elemDecl=context->getDocumentCache()->getElementDecl(elementNS, elementName);
  assert(elemDecl != NULL);

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
}

bool SequenceType::ItemType::matches(const Node::Ptr &toBeTested, DynamicContext* context) const
{
  switch(m_nTestType) {
    case TEST_ELEMENT:
    {
      if(toBeTested->dmNodeKind() != Node::element_string)
        return false;

      if(!matchesNameType(toBeTested, context))
        return false;

      // if the element has xsi:nil="true", m_bAllowNil MUST be true
      if(toBeTested->dmNilled(context)->isTrue() && !m_bAllowNil)
        return false;

      return true;
    }

    case TEST_ATTRIBUTE:
    {
      if(toBeTested->dmNodeKind() != Node::attribute_string)
        return false;
      if(!matchesNameType(toBeTested, context))
        return false;
      return true;
    }

    case TEST_SCHEMA_ELEMENT:
    {
      if(toBeTested->dmNodeKind() != Node::element_string)
        return false;

      return matchesSchemaElement(toBeTested, context);
    }

    case TEST_SCHEMA_ATTRIBUTE:
    {
      if(toBeTested->dmNodeKind() != Node::attribute_string)
        return false;

      // retrieve the type of the attribute name
      assert(m_pName!=NULL);
      const XMLCh* attributeNS=m_NameURI;
      const XMLCh* attributeName=m_pName->getName();
      SchemaAttDef* attrDecl=context->getDocumentCache()->getAttributeDecl(attributeNS, attributeName);
      assert(attrDecl != NULL);

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
    }

    case TEST_PI:
    {
      if(toBeTested->dmNodeKind() != Node::processing_instruction_string)
        return false;
      if(!matchesNameType(toBeTested, context))
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

    case TEST_SCHEMA_DOCUMENT:
    case TEST_DOCUMENT:
    {
        if(toBeTested->dmNodeKind() != Node::document_string)
          return false;

        if(m_pName == NULL && m_pType == NULL)
          return true;

        // if we have a constraint on name/type, they apply to the document element
        Result children = toBeTested->dmChildren(context, 0);
        Node::Ptr docElement;
        while((docElement = children->next(context)).notNull() &&
              docElement->dmNodeKind() != Node::element_string) {}

        if(docElement.isNull()) return false;

        if(m_nTestType == TEST_DOCUMENT)
          return matchesNameType(docElement, context);
        else
          return matchesSchemaElement(docElement, context);
    }

    case TEST_NODE:
    case TEST_ANYTHING:
    {
      return true;
    }

    case TEST_ATOMIC_TYPE:
    case TEST_FUNCTION:
    {
        return false;
    }

  }
  return true;
}

bool SequenceType::ItemType::matches(const Item::Ptr &toBeTested, DynamicContext* context) const
{
  if(toBeTested->isNode())
    return matches((const Node::Ptr)toBeTested, context);
    
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
    case TEST_SCHEMA_DOCUMENT:
    {
      return false;
    }
    
    case TEST_ANYTHING:
    {
      return true;
    }

    case TEST_ATOMIC_TYPE:
    {
      if(!toBeTested->isAtomicValue()) return false;
      return matchesNameType(toBeTested, context);
    }

    case TEST_FUNCTION:
    {
      if(!toBeTested->isFunction()) return false;
      if(returnType_ == 0) return true;

      FunctionRef *func = (FunctionRef*)toBeTested.get();
      if(func->getNumArgs() != argTypes_->size()) return false;

      return true;
    }

  }
  return true;
}

bool SequenceType::matches(const SequenceType *toBeTested, const StaticContext* context) const
{
  if(m_pItemType == 0)
    return toBeTested->m_pItemType == 0;
  if(toBeTested->m_pItemType == 0)
    return m_nOccurrence == QUESTION_MARK || m_nOccurrence == STAR;

  switch(m_nOccurrence) {
  case EXACTLY_ONE:
    if(toBeTested->m_nOccurrence != EXACTLY_ONE) return false;
    break;
  case PLUS:
    if(toBeTested->m_nOccurrence != EXACTLY_ONE &&
       toBeTested->m_nOccurrence != PLUS) return false;
    break;
  case QUESTION_MARK:
    if(toBeTested->m_nOccurrence == PLUS) return false;
    break;
  case STAR:
    break;
  }

  return m_pItemType->matches(toBeTested->m_pItemType, context);
}

bool SequenceType::ItemType::matchesNameType(const ItemType *toBeTested, const StaticContext* context) const
{
  if(m_pName) {
    if(toBeTested->m_pName == 0) return false;
    if(!XPath2Utils::equals(m_pName->getName(), toBeTested->m_pName->getName()) ||
       !XPath2Utils::equals(m_NameURI, toBeTested->m_NameURI))
      return false;
  }

  if(m_pType) {
    if(toBeTested->m_pType == 0) return false;
    if(!context->isTypeOrDerivedFromType(toBeTested->m_TypeURI, toBeTested->m_pType->getName(), m_TypeURI, m_pType->getName()))
      return false;
  }

  return true;
}

bool SequenceType::ItemType::matches(const ItemType *toBeTested, const StaticContext* context) const
{
  switch(m_nTestType) {
  case TEST_ANYTHING: {
    return true;
  }

  case TEST_NODE: {
    switch(toBeTested->m_nTestType) {
    case TEST_ELEMENT:
    case TEST_ATTRIBUTE:
    case TEST_SCHEMA_ELEMENT:
    case TEST_SCHEMA_ATTRIBUTE:
    case TEST_NODE:
    case TEST_PI:
    case TEST_COMMENT:
    case TEST_TEXT:
    case TEST_DOCUMENT:
    case TEST_SCHEMA_DOCUMENT:
      return true;
    default:
      return false;
    }
  }

  case TEST_ELEMENT: {
    if(toBeTested->m_nTestType != TEST_ELEMENT)
      return false;
    if(!matchesNameType(toBeTested, context))
      return false;
    if(!m_bAllowNil && toBeTested->m_bAllowNil) return false;
    return true;
  }

  case TEST_ATTRIBUTE: {
    if(toBeTested->m_nTestType != TEST_ATTRIBUTE)
      return false;
    if(!matchesNameType(toBeTested, context))
      return false;
    return true;
  }

  case TEST_SCHEMA_ELEMENT: {
    if(toBeTested->m_nTestType != TEST_SCHEMA_ELEMENT)
      return false;
    if(!matchesNameType(toBeTested, context))
      return false;
    return true;
  }

  case TEST_SCHEMA_ATTRIBUTE: {
    if(toBeTested->m_nTestType != TEST_SCHEMA_ATTRIBUTE)
      return false;
    if(!matchesNameType(toBeTested, context))
      return false;
    return true;
  }

  case TEST_PI: {
    if(toBeTested->m_nTestType != TEST_PI)
      return false;
    if(!matchesNameType(toBeTested, context))
      return false;
    return true;
  }

  case TEST_COMMENT: {
    if(toBeTested->m_nTestType != TEST_COMMENT)
      return false;
    return true;
  }

  case TEST_TEXT: {
    if(toBeTested->m_nTestType != TEST_TEXT)
      return false;
    return true;
  }

  case TEST_DOCUMENT: {
    if(toBeTested->m_nTestType != TEST_DOCUMENT)
      return false;
    if(!matchesNameType(toBeTested, context))
      return false;
    return true;
  }
    
  case TEST_SCHEMA_DOCUMENT: {
    if(toBeTested->m_nTestType != TEST_SCHEMA_DOCUMENT)
      return false;
    if(!matchesNameType(toBeTested, context))
      return false;
    return true;
  }
    
  case TEST_ATOMIC_TYPE: {
    if(toBeTested->m_nTestType != TEST_ATOMIC_TYPE)
      return false;
    if(!matchesNameType(toBeTested, context))
      return false;
    return true;
  }

  case TEST_FUNCTION: {
    if(toBeTested->m_nTestType != TEST_FUNCTION) return false;
    if(returnType_ == 0) return true;
    if(toBeTested->returnType_ == 0) return false;

    if(toBeTested->argTypes_->size() != argTypes_->size()) return false;

    VectorOfSequenceTypes::const_iterator i = toBeTested->argTypes_->begin();
    VectorOfSequenceTypes::const_iterator j = argTypes_->begin();
    for(; i != toBeTested->argTypes_->end() && j != argTypes_->end(); ++i, ++j) {
      if(!(*i)->matches(*j, context)) return false;
    }

    if(!returnType_->matches(toBeTested->returnType_, context))
      return false;
    return true;
  }
  }
  return true;
}

Result SequenceType::convertFunctionArg(const Result &input, DynamicContext *context, bool xpath1Compat,
                                        const LocationInfo *location, const XMLCh *errorCode)
{
  // From XPath2 Spec, Section 3.1.5 (http://www.w3.org/TR/xpath20/#id-function-calls)

  // The function conversion rules are used to convert an argument value to its expected type; that is, to the
  // declared type of the function parameter. The expected type is expressed as a SequenceType. The function
  // conversion rules are applied to a given value as follows:

  Result result(input);

  if(m_pItemType!=NULL)
  {
    SequenceType::ItemType::ItemTestType testType = getItemTestType();
    // FS says we atomize first if the sequence type is atomic, and I think that's sensible - jpcs
    if(testType == ItemType::TEST_ATOMIC_TYPE) {
      result = new AtomizeResult(location, result);
    }

    // If XPath 1.0 compatibility mode is true and an argument is not of the expected type, then the following
    // conversions are applied sequentially to the argument value V:
    if(xpath1Compat) {
      if(m_nOccurrence == SequenceType::EXACTLY_ONE || m_nOccurrence == SequenceType::QUESTION_MARK) {
        result = new XPath1CompatConvertFunctionArgResult(location, result, this);
      }
    }
    // If the expected type is a sequence of an atomic type (possibly with an occurrence indicator *, +, or ?),
    // the following conversions are applied:
    else if(testType == ItemType::TEST_ATOMIC_TYPE) {
      const XMLCh *uri = m_pItemType->getTypeURI();
      const XMLCh *name = m_pItemType->getType()->getName();

      bool isPrimitive = false;
      AnyAtomicType::AtomicObjectType typeIndex = context->getItemFactory()->getPrimitiveTypeIndex(uri, name, isPrimitive);

      if(!XPath2Utils::equals(name, AnyAtomicType::fgDT_ANYATOMICTYPE) ||
         !XPath2Utils::equals(uri, SchemaSymbols::fgURI_SCHEMAFORSCHEMA)) {
        result = new PromoteUntypedResult(location, result, isPrimitive, typeIndex, uri, name);
      }

      if(XPath2Utils::equals(uri, SchemaSymbols::fgURI_SCHEMAFORSCHEMA)) {
        if(XPath2Utils::equals(name, SchemaSymbols::fgDT_DOUBLE) ||
           XPath2Utils::equals(name, SchemaSymbols::fgDT_FLOAT)) {
          result = new PromoteNumericResult(location, result, typeIndex);
        }

        if(XPath2Utils::equals(name, SchemaSymbols::fgDT_STRING)) {
          result = new PromoteAnyURIResult(location, result);
        }
      }
    }
  }
  // If, after the above conversions, the resulting value does not match the expected type according to the
  // rules for SequenceType Matching, a type error is raised. [err:XPTY0004] Note that the rules for
  // SequenceType Matching permit a value of a derived type to be substituted for a value of its base type.
  return matches(result, location, errorCode);
}

ASTNode *SequenceType::convertFunctionArg(ASTNode *arg, StaticContext *context, bool numericFunction,
                                          const LocationInfo *location)
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
    if(testType == ItemType::TEST_ATOMIC_TYPE) {
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
      arg = new (mm) XQAtomize(arg, mm);
      arg->setLocationInfo(location);

      const XMLCh *uri = m_pItemType->getTypeURI();
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

SequenceType::OccurrenceMatchesResult::OccurrenceMatchesResult(const Result &parent, const SequenceType *seqType, const LocationInfo *location, const XMLCh *errorCode)
  : ResultImpl(location),
    _seqType(seqType),
    _parent(parent),
    _errorCode(errorCode),
    _toDo(true)
{
}

Item::Ptr SequenceType::OccurrenceMatchesResult::next(DynamicContext *context)
{
  Item::Ptr item = _parent->next(context);

  // "SequenceType matching between a given value and a given SequenceType is performed as follows:
  //  If the SequenceType is empty, the match succeeds only if the value is an empty sequence."
  if(_seqType->getItemType() == NULL && item.notNull()) {
    XMLBuffer buf;
    buf.set(X("Sequence does not match type "));
    _seqType->toBuffer(buf);
    buf.append(X(" - the sequence contains items ["));
    buf.append(X(" ["));
    buf.append(_errorCode);
    buf.append(X("]"));
    XQThrow(XPath2TypeMatchException, X("SequenceType::OccurrenceMatchesResult::next"), buf.getRawBuffer());
  }
  // "If the SequenceType contains an ItemType and an OccurrenceIndicator, the match succeeds only if 
  //  the number of items in the value matches the OccurrenceIndicator and each of these items matches the ItemType. "
  if(_seqType->getItemType() &&
     (_seqType->getOccurrenceIndicator() == PLUS || _seqType->getOccurrenceIndicator() == EXACTLY_ONE) &&
     item.isNull()) {
    XMLBuffer buf;
    buf.set(X("Sequence does not match type "));
    _seqType->toBuffer(buf);
    buf.append(X(" - the sequence does not contain items ["));
    buf.append(_errorCode);
    buf.append(X("]"));
    XQThrow(XPath2TypeMatchException, X("SequenceType::OccurrenceMatchesResult::next"), buf.getRawBuffer());
  }

  // "If the SequenceType is an ItemType with no OccurrenceIndicator, the match succeeds only if 
  //  the value contains precisely one item and that item matches the ItemType "
  if(_seqType->getItemType() &&
     (_seqType->getOccurrenceIndicator() == EXACTLY_ONE || _seqType->getOccurrenceIndicator() == QUESTION_MARK) &&
     item.notNull()) {
    // Do the tests on the number of items up front,
    // since often functions that cast to a single or
    // optional item only call next once. - jpcs

    Item::Ptr second = _parent->next(context);

    if(second.isNull()) {
      _parent = 0;
    }
    else {
      XMLBuffer buf;
      buf.set(X("Sequence does not match type "));
      _seqType->toBuffer(buf);
      buf.append(X(" - the sequence contains more than one item ["));
      buf.append(_errorCode);
      buf.append(X("]"));
      XQThrow(XPath2TypeMatchException, X("SequenceType::OccurrenceMatchesResult::next"), buf.getRawBuffer());
    }
  }

  if(item.isNull()) {
    *resultPointer_ = 0;
  }
  else {
    *resultPointer_ = _parent;
  }

  return item;
}

////////////////////////////////////////
// TypeMatchesResult
////////////////////////////////////////

SequenceType::TypeMatchesResult::TypeMatchesResult(const Result &parent, const SequenceType *seqType, const LocationInfo *location, const XMLCh *errorCode)
  : ResultImpl(location),
    _seqType(seqType),
    _parent(parent),
    _errorCode(errorCode)
{
}

Item::Ptr SequenceType::TypeMatchesResult::next(DynamicContext *context)
{
  Item::Ptr item = _parent->next(context);
  if(item.isNull()) {
    _parent = 0;
  }
  else if(!_seqType->getItemType()->matches(item, context)) {
    XMLBuffer buf;
    buf.set(X("Sequence does not match type "));
    _seqType->toBuffer(buf);
    buf.append(X(" - found item of type "));
    item->typeToBuffer(context, buf);
    buf.append(X(" ["));
    buf.append(_errorCode);
    buf.append(X("]"));
    XQThrow(XPath2TypeMatchException, X("SequenceType::MatchesResult::next"), buf.getRawBuffer());
  }

  return item;
}

