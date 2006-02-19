#include "../config/pathan_config.h"
#include <assert.h>
#include <sstream>

#include <pathan/SequenceType.hpp>

#include <pathan/XPath2Utils.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/DocumentCache.hpp>
#include <pathan/XPath2MemoryManager.hpp>
#include <pathan/internal/parser/QName.hpp>
#include <pathan/exceptions/StaticErrorException.hpp>
#include <pathan/exceptions/XPath2ErrorException.hpp>
#include <pathan/exceptions/XPath2TypeMatchException.hpp>
#include <pathan/exceptions/XPath2TypeCastException.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/ATDoubleOrDerived.hpp>
#include <pathan/ATUntypedAtomic.hpp>
#include <pathan/Node.hpp>
#include <pathan/functions/FunctionConstructor.hpp>
#include <pathan/functions/FunctionString.hpp>
#include <pathan/functions/FunctionNumber.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/validators/schema/SchemaAttDef.hpp>
#include <xercesc/validators/schema/SchemaElementDecl.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

SequenceType::SequenceType(const XMLCh* typeURI,const XMLCh* typeName, SequenceType::OccurrenceIndicator occur /* = EXACTLY_ONE */)
{
  // Possible memory leak? - jpcs
	m_pItemType=new SequenceType::ItemType(ItemType::TEST_ATOMIC_TYPE,NULL,new QualifiedName(XMLUni::fgZeroLenString, typeName));
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

Result SequenceType::matches(const Result &toBeTested, DynamicContext* context) const
{
  return new MatchesResult(toBeTested, this, context);
}

void SequenceType::setItemType(SequenceType::ItemType* itemType)
{
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
    return m_pItemType->getNameURI(context);
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
    return m_pItemType->getTypeURI(context);
  }
  return 0;
}

QualifiedName *SequenceType::getConstrainingName(void) const {

  return m_pItemType->getName();
}

QualifiedName *SequenceType::getConstrainingType(void) const {

  return m_pItemType->getType();
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

const XMLCh* SequenceType::ItemType::getTypeURI(const StaticContext* context) const
{
	if(m_TypeURI!=0 && *m_TypeURI!=0) {
		//      cerr << "NameTypeConstrain: Returning m_TypeURI: " << XMLString::transcode(m_TypeURI) << endl;
		return m_TypeURI;
	}
	//    cerr << "NameTypeConstrain: Returning getUriBoundToPrefix: " << " prefix is: " << XMLString::transcode(m_pType->getPrefix()) << " result is: " << XMLString::transcode(context->getUriBoundToPrefix(m_pType->getPrefix())) <<  " default uri is " << XMLString::transcode(context->getDefaultFuncNS()) << endl;
  if(m_pType != 0)
    return context->getUriBoundToPrefix(m_pType->getPrefix());
  return 0;
}

const XMLCh* SequenceType::ItemType::getNameURI(const StaticContext* context) const
{
	if(m_NameURI!=0 && *m_NameURI!=0)
		return m_NameURI;
  if(m_pName != 0)
    {
        const XMLCh* prefix=m_pName->getPrefix();
        // if we are testing for an attribute, an empty prefix means empty namespace
        if((prefix==0 || *prefix==0) && (m_nTestType==TEST_ATTRIBUTE || m_nTestType==TEST_SCHEMA_ATTRIBUTE))
            return 0;
        return context->getUriBoundToPrefix(prefix);
    }
  return 0;
}

void SequenceType::ItemType::getStaticType(StaticType &st, const StaticContext *context) const
{
  switch(m_nTestType) {
      case SequenceType::ItemType::TEST_ANYTHING: {
	      st.flags = StaticType::NODE_TYPE | StaticType::NUMERIC_TYPE | StaticType::OTHER_TYPE;
	      break;
      }
      case SequenceType::ItemType::TEST_ATOMIC_TYPE: {
	      switch(context->getPathanFactory()->getPrimitiveTypeIndex(getTypeURI(context), getType()->getName())) {
        case AnyAtomicType::DECIMAL:
        case AnyAtomicType::DOUBLE:
        case AnyAtomicType::FLOAT: {
          st.flags = StaticType::NUMERIC_TYPE;
          break;
        }
        default: {
          st.flags = StaticType::OTHER_TYPE;
          break;
        }
        }     
        break;
      }
      default: {
	      st.flags = StaticType::NODE_TYPE;
	      break;
      }
      }
}

bool SequenceType::ItemType::matchesNameType(const Item::Ptr &toBeTested, DynamicContext* context) const
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
      if(!(XPath2Utils::equals(getNameURI(context), ((const ATQNameOrDerived*)name.get())->getURI())))
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
    bool result;
    if(toBeTested->isAtomicValue()) {
      result = ((AnyAtomicType*)(const Item*)toBeTested)->isInstanceOfType(getTypeURI(context), m_pType->getName(), context);
    } else if (toBeTested->isNode()) {
      result = ((Node*)(const Item*)toBeTested)->hasInstanceOfType(getTypeURI(context), m_pType->getName(), context);
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
    }//case

    case TEST_ATTRIBUTE:
    {
      if(toBeTested->dmNodeKind() != Node::attribute_string)
        return false;
      if(!matchesNameType(toBeTested, context))
        return false;
      return true;
    }//case

    case TEST_SCHEMA_ELEMENT:
    {
      if(toBeTested->dmNodeKind() != Node::element_string)
        return false;

      // retrieve the type of the element name
      assert(m_pName!=NULL);
      const XMLCh* elementNS=getNameURI(context);
      const XMLCh* elementName=m_pName->getName();
      SchemaElementDecl* elemDecl=context->getDocumentCache()->getElementDecl(elementNS, elementName);
      if(elemDecl==NULL) {
        XMLBuffer msg(1023, context->getMemoryManager());
        msg.set(X("Element {"));
        msg.append(elementNS);
        msg.append(X("}"));
        msg.append(elementName);
        msg.append(X(" is not defined as a global element [err:XPST0008]"));
        DSLthrow(StaticErrorException,X("SequenceType::ItemType::matches"), msg.getRawBuffer());
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
      const XMLCh* attributeNS=getNameURI(context);
      const XMLCh* attributeName=m_pName->getName();
      SchemaAttDef* attrDecl=context->getDocumentCache()->getAttributeDecl(attributeNS, attributeName);
      if(attrDecl==NULL) {
        XMLBuffer msg(1023, context->getMemoryManager());
        msg.set(X("Attribute {"));
        msg.append(attributeNS);
        msg.append(X("}"));
        msg.append(attributeName);
        msg.append(X(" is not defined as a global attribute [err:XPST0008]"));
        DSLthrow(StaticErrorException,X("SequenceType::ItemType::matches"), msg.getRawBuffer());
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

    case TEST_DOCUMENT:
    {
        if(toBeTested->dmNodeKind() != Node::document_string)
          return false;
        if(m_pName!=NULL || m_pType!=NULL) {

          // if we have a constraint on name/type, they apply to the document element
          Result children = toBeTested->dmChildren(context);
          Node::Ptr docElement;
          while((docElement = children.next(context)).notNull() &&
                docElement->dmNodeKind() != Node::element_string) {}

          if(docElement.isNull()) return false;

          if(!matchesNameType(docElement, context))
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
    {
      return false;
    }
    
    case TEST_ANYTHING:
    {
      return true;
    }

    ///this is wrong said nfg and gareth -- we have non atomic types now.  crioux thinks this is correct (we no longer have non-atomic types)

    // "A named atomic type matches a value if the dynamic type of the value is the same as the named atomic type or is derived 
    //  from the named atomic type by restriction. For example, the ItemType xs:decimal matches the value 12.34 (a decimal literal); 
    //  it also matches a value whose dynamic type is shoesize, if shoesize is a user-defined atomic type derived from xs:decimal. "
    case TEST_ATOMIC_TYPE:
    {
        // the constraining type could be one of the special XPath 2 types
        QualifiedName *constrainingType = getType();

        if(constrainingType != NULL)
        {
            const XMLCh *uri = getTypeURI(context);
            if(XPath2Utils::equals(uri,FunctionConstructor::XMLChXPath2DatatypesURI))
            {
                if(XPath2Utils::equals(constrainingType->getName(),AnyAtomicType::fgDT_ANYATOMICTYPE))
                    return true;
                else if(XPath2Utils::equals(constrainingType->getName(),ATUntypedAtomic::fgDT_UNTYPEDATOMIC))
                    return true;
            }
        }
        if(!matchesNameType(toBeTested, context))
            return false;

        return true;
    }//case

  }//switch
  return true;
}

Result SequenceType::convertFunctionArg(const Result &param, const StaticType &stype, DynamicContext* context) const {
  // From XPath2 Spec, Section 3.1.5 (http://www.w3.org/TR/xpath20/#id-function-calls)

  // The function conversion rules are used to convert an argument value to its expected type; that is, to the
  // declared type of the function parameter. The expected type is expressed as a SequenceType. The function
  // conversion rules are applied to a given value as follows:

  Result ret = param;

  // FS says we atomize first if the sequence type is atomic, and I think that's sensible - jpcs
  if((stype.flags & StaticType::NODE_TYPE) && getItemTestType() == ItemType::TEST_ATOMIC_TYPE) {
    ret = ret.atomize(context);
  }

  // If XPath 1.0 compatibility mode is true and an argument is not of the expected type, then the following
  // conversions are applied sequentially to the argument value V:
  if(context->getXPath1CompatibilityMode()) {
    ret = new XPath1CompatConvertFunctionArgResult(ret, this, context);
  }

  // If the expected type is a sequence of an atomic type (possibly with an occurrence indicator *, +, or ?),
  // the following conversions are applied:
  else if(getItemTestType() == ItemType::TEST_ATOMIC_TYPE) {
    ret = new AtomicTypeConvertFunctionArgResult(ret, this, context);
  }

  // If, after the above conversions, the resulting value does not match the expected type according to the
  // rules for SequenceType Matching, a type error is raised. [err:XPTY0004] Note that the rules for
  // SequenceType Matching permit a value of a derived type to be substituted for a value of its base type.
  ret = ret.matches(this, context);

  return ret;
}

const SequenceType::ItemType *SequenceType::getItemType() const {
  return m_pItemType;
}

////////////////////////////////////////
// AtomicTypeConvertFunctionArgResult
////////////////////////////////////////

SequenceType::AtomicTypeConvertFunctionArgResult::AtomicTypeConvertFunctionArgResult(const Result &parent, const SequenceType *seqType, DynamicContext *context)
  : ResultImpl(context),
    _seqType(seqType),
    _parent(parent)
{
}

Item::Ptr SequenceType::AtomicTypeConvertFunctionArgResult::next(DynamicContext *context)
{
  // If the expected type is a sequence of an atomic type (possibly with an occurrence indicator *, +, or ?),
  // the following conversions are applied:
  // 1. Atomization is applied to the given value, resulting in a sequence of atomic values.

  AnyAtomicType::Ptr item = (const AnyAtomicType::Ptr )_parent.next(context);

  if(item != NULLRCP) {
    const XMLCh* typeURI = _seqType->getTypeURI(context);
    const XMLCh* typeName = _seqType->getConstrainingType()->getName();

    // 2. Each item in the atomic sequence that is of type xdt:untypedAtomic is cast to the expected atomic
    //    type. For built-in functions where the expected type is specified as numeric, arguments of type
    //    xdt:untypedAtomic are cast to xs:double.
    // crioux thinks this should also add: unless the target type is anyAtomicType!
    if(item->getPrimitiveTypeIndex() == AnyAtomicType::UNTYPED_ATOMIC &&
       !(XPath2Utils::equals(typeName, AnyAtomicType::fgDT_ANYATOMICTYPE) && 
         XPath2Utils::equals(typeURI, FunctionConstructor::XMLChXPath2DatatypesURI))) {
      try {
        item = item->castAs(typeURI, typeName, context);
      }
      catch (XPath2TypeCastException &e) {
        DSLthrow(XPath2ErrorException, X("SequenceType::AtomicTypeConvertFunctionArgResult::next"),
                 X("Casting from xdt:untypedAtomic to required type failed."));
      } catch (const XMLException& e) {
        DSLthrow(XPath2ErrorException, X("SequenceType::AtomicTypeConvertFunctionArgResult::next"),
                 X("Casting from xdt:untypedAtomic to required type failed."));
      }
    }

    // 3. For each numeric item in the atomic sequence that can be promoted to the expected atomic type using
    //    the promotion rules in B.1 Type Promotion, the promotion is done.
    else if(item->isNumericValue()) {
      try {
        const Numeric::Ptr promotedType = ((const Numeric*)item.get())->promoteTypeIfApplicable(typeURI, typeName, context);
        if(promotedType != NULLRCP) {
          item = (const AnyAtomicType::Ptr)promotedType;
        }
      } catch (XPath2TypeCastException &e) {
        DSLthrow(XPath2ErrorException, X("SequenceType::AtomicTypeConvertFunctionArgResult::next"),
                 X("Numeric type promotion failed (for promotable type)"));
      } catch (const XMLException& e) {
        DSLthrow(XPath2ErrorException, X("SequenceType::AtomicTypeConvertFunctionArgResult::next"),
                 X("Numeric type promotion failed (for promotable type)"));
      }
    }

    // 4. For each item of type xs:anyURI in the atomic sequence that can be promoted to the expected atomic
    //    type using URI promotion as described in B.1 Type Promotion, the promotion is done.
    else if(item->getPrimitiveTypeIndex() == AnyAtomicType::ANY_URI &&
            XPath2Utils::equals(typeName, SchemaSymbols::fgDT_STRING) && 
            XPath2Utils::equals(typeURI, SchemaSymbols::fgURI_SCHEMAFORSCHEMA)) {
      try {
        item = (const AnyAtomicType::Ptr)item->castAs(typeURI, typeName, context);
      } catch (XPath2TypeCastException &e) {
        DSLthrow(XPath2ErrorException, X("SequenceType::AtomicTypeConvertFunctionArgResult::next"),
                 X("AnyURI type promotion failed (for promotable type)"));
      } catch (const XMLException& e) {
        DSLthrow(XPath2ErrorException, X("SequenceType::AtomicTypeConvertFunctionArgResult::next"),
                 X("AnyURI type promotion failed (for promotable type)"));
      }
    }
  }

  return (const Item::Ptr)item;
}

std::string SequenceType::AtomicTypeConvertFunctionArgResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<atomicconvertfunctionarg>" << std::endl;
  oss << _parent.asString(context, indent + 1);
  oss << in << "</atomicconvertfunctionarg>" << std::endl;

  return oss.str();
}

////////////////////////////////////////
// XPath1CompatConvertFunctionArgResult
////////////////////////////////////////

SequenceType::XPath1CompatConvertFunctionArgResult::XPath1CompatConvertFunctionArgResult(const Result &parent, const SequenceType *seqType, DynamicContext *context)
  : ResultImpl(context),
    _seqType(seqType),
    _parent(parent),
    _oneDone(false)
{
}

Item::Ptr SequenceType::XPath1CompatConvertFunctionArgResult::next(DynamicContext *context)
{
  // If XPath 1.0 compatibility mode is true and an argument is not of the expected type, then the following
  // conversions are applied sequentially to the argument value V:

  if(_seqType->getOccurrenceIndicator() == SequenceType::EXACTLY_ONE || _seqType->getOccurrenceIndicator() == SequenceType::QUESTION_MARK) {
    // 1. If the expected type calls for a single item or optional single item (examples: xs:string,
    //    xs:string?, xdt:untypedAtomic, xdt:untypedAtomic?, node(), node()?, item(), item()?), then the
    //    value V is effectively replaced by V[1].
    if(_oneDone) {
      return 0;
    }

    Item::Ptr item = _parent.next(context);

    if(_seqType->getItemTestType() == ItemType::TEST_ATOMIC_TYPE) {
      const XMLCh* typeURI = _seqType->getTypeURI(context);
      const XMLCh* typeName = _seqType->getConstrainingType()->getName();

      // 2. If the expected type is xs:string or xs:string?, then the value V is effectively replaced by
      //    fn:string(V).
      if(context->isTypeOrDerivedFromType(typeURI, typeName,
                                          SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                          SchemaSymbols::fgDT_STRING)) {
        item = FunctionString::string(item, context);
      }

      // 3. If the expected type is a (possibly optional) numeric type, then the value V is effectively
      //    replaced by fn:number(V).
      if(context->isTypeOrDerivedFromType(typeURI, typeName,
                                          SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                          SchemaSymbols::fgDT_DOUBLE)) {
        assert(item->isAtomicValue()); // Since atomization should have happened
        item = FunctionNumber::number((const AnyAtomicType::Ptr )item, context);
      }
    }

    return item;
  }

  return _parent.next(context);
}

std::string SequenceType::XPath1CompatConvertFunctionArgResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<xpath1convertfunctionarg>" << std::endl;
  oss << _parent.asString(context, indent + 1);
  oss << in << "</xpath1convertfunctionarg>" << std::endl;

  return oss.str();
}

////////////////////////////////////////
// MatchesResult
////////////////////////////////////////

SequenceType::MatchesResult::MatchesResult(const Result &parent, const SequenceType *seqType, DynamicContext *context)
  : ResultImpl(context),
    _seqType(seqType),
    _parent(parent),
    _toDo(true)
{
}

Item::Ptr SequenceType::MatchesResult::next(DynamicContext *context)
{
  Item::Ptr item;
  if(_toDo) {
    _toDo = false;

    item = _parent.next(context);

    // "SequenceType matching between a given value and a given SequenceType is performed as follows:
    //  If the SequenceType is empty, the match succeeds only if the value is an empty sequence."
    if(_seqType->getItemType() == NULL && item != NULLRCP) {
      DSLthrow(XPath2TypeMatchException, X("SequenceType::MatchesResult::next"),
               X("SequenceType matching failed: the sequence is not empty [err:XPTY0004]"));
    }

    // "If the SequenceType contains an ItemType and an OccurrenceIndicator, the match succeeds only if 
    //  the number of items in the value matches the OccurrenceIndicator and each of these items matches the ItemType. "
    if((_seqType->getOccurrenceIndicator() == PLUS || _seqType->getOccurrenceIndicator() == EXACTLY_ONE) &&
       item == NULLRCP) {
      DSLthrow(XPath2TypeMatchException, X("SequenceType::MatchesResult::next"),
               X("SequenceType matching failed: the sequence does not contain items [err:XPTY0004]"));
    }

    // "If the SequenceType is an ItemType with no OccurrenceIndicator, the match succeeds only if 
    //  the value contains precisely one item and that item matches the ItemType "
    if((_seqType->getOccurrenceIndicator() == EXACTLY_ONE || _seqType->getOccurrenceIndicator() == QUESTION_MARK) &&
       item != NULLRCP) {
      // Do the tests on the number of items up front,
      // since often functions that cast to a single or
      // optional item only call next once. - jpcs

      Item::Ptr second = _parent.next(context);

      if(second == NULLRCP) {
        _parent = 0;
      }
      else {
        DSLthrow(XPath2TypeMatchException, X("SequenceType::MatchesResult::next"),
                 X("SequenceType matching failed: the sequence contains more than one item [err:XPTY0004]"));
      }
    }
  }
  else {
    item = _parent.next(context);
  }

  // Now test that each item matches the ItemType
  if(item == NULLRCP) {
    _parent = 0;
  }
  else if(!_seqType->getItemType()->matches(item, context)) {
    DSLthrow(XPath2TypeMatchException, X("SequenceType::MatchesResult::next"),
             X("ItemType matching failed [err:XPTY0004]"));
  }

  return item;
}

std::string SequenceType::MatchesResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<matches>" << std::endl;
  oss << _parent.asString(context, indent + 1);
  oss << in << "</matches>" << std::endl;

  return oss.str();
}
