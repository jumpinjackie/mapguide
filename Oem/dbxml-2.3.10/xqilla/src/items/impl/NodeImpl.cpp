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
 * $Id: NodeImpl.cpp,v 1.21 2006/11/01 16:37:21 jpcs Exp $
 */

#include "../../config/xqilla_config.h"
#include <xqilla/items/impl/NodeImpl.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/XQScopedNamespace.hpp>
#include <xqilla/framework/XPath2MemoryManager.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/runtime/Result.hpp>
#include <xqilla/exceptions/ItemException.hpp>
#include <xqilla/exceptions/XPath2TypeCastException.hpp>
#include <xqilla/items/ATAnyURIOrDerived.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/items/ATQNameOrDerived.hpp>
#include <xqilla/items/ATUntypedAtomic.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/functions/FunctionRoot.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/schema/DocumentCache.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/schema/DocumentCacheImpl.hpp>

#include <xqilla/axis/AncestorAxis.hpp>
#include <xqilla/axis/AncestorOrSelfAxis.hpp>
#include <xqilla/axis/AttributeAxis.hpp>
#include <xqilla/axis/ChildAxis.hpp>
#include <xqilla/axis/DescendantAxis.hpp>
#include <xqilla/axis/DescendantOrSelfAxis.hpp>
#include <xqilla/axis/FollowingAxis.hpp>
#include <xqilla/axis/FollowingSiblingAxis.hpp>
#include <xqilla/axis/NamespaceAxis.hpp>
#include <xqilla/axis/ParentAxis.hpp>
#include <xqilla/axis/PrecedingAxis.hpp>
#include <xqilla/axis/PrecedingSiblingAxis.hpp>
#include <xqilla/axis/SelfAxis.hpp>
#include <xqilla/axis/NodeTest.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMPSVITypeInfo.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/validators/schema/SchemaElementDecl.hpp>
#include <xercesc/validators/schema/ComplexTypeInfo.hpp>
#include <xercesc/validators/datatype/ListDatatypeValidator.hpp>
#include <xercesc/util/XMLUri.hpp>
#include <xercesc/util/XMLURL.hpp>
#include <assert.h>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

const XMLCh NodeImpl::ls_string[] =
{
    chLatin_L, chLatin_S,
    chNull
};

NodeImpl::NodeImpl(const DOMNode *node, const DynamicContext *context) : 
    Node(), 
    fSerializer(0), 
    fNode(node),
    context_(context)
{  
  assert(node!=0);
  context_->incrementDocumentRefCount(XPath2Utils::getOwnerDoc(fNode));
}

NodeImpl::NodeImpl(const DOMNode *node)
  : fSerializer(0),
    fNode(node),
    context_(0)
{
  assert(node!=0);
}

NodeImpl::~NodeImpl()
{
  if(context_ != 0) {
    context_->decrementDocumentRefCount(XPath2Utils::getOwnerDoc(fNode));
  }
  if(fSerializer != 0) {
	  fSerializer->release();
  }
}

void *NodeImpl::getInterface(const XMLCh *name) const
{
  if(XPath2Utils::equals(name,Item::gXQilla)) {
    return (void*)this;
  }
  else if(XPath2Utils::equals(name,Node::gXerces)) {
    return (void*)fNode;
  }
  return 0;
}

bool NodeImpl::isNode() const
{
    return true;
}

bool NodeImpl::isAtomicValue() const
{
    return false;
}

const XMLCh* NodeImpl::asString(const DynamicContext* context) const
{
  if(!fSerializer) {
    DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(ls_string);
    ((NodeImpl*)this)->fSerializer = ((DOMImplementationLS*)impl)->createDOMWriter(context->getMemoryManager());
    ((NodeImpl*)this)->fSerializer->setFeature(XMLUni::fgDOMXMLDeclaration, false);
  }
  return fSerializer->writeToString(*fNode);
}

/** check if the underlying type of this node is instance of a certain type */
bool NodeImpl::hasInstanceOfType(const XMLCh* typeURI, const XMLCh* typeName, const DynamicContext* context) const {
    const XMLCh* uri, *name;
    getTypeUriAndName(uri,name);
    return context->isTypeOrDerivedFromType(uri, name, typeURI, typeName);
}


/* Accessor functions */

Sequence NodeImpl::dmBaseURI(const DynamicContext* context) const {
  
    const XMLCh* baseURI=NULL;
	switch (fNode->getNodeType()) {
    case DOMNode::ATTRIBUTE_NODE : 
        {
            DOMElement* pNode=((DOMAttr*)fNode)->getOwnerElement();
            if(pNode)
                baseURI = pNode->getBaseURI();
        }
        break;
    case DOMNode::COMMENT_NODE : 
    case DOMNode::TEXT_NODE : 
    case DOMNode::ELEMENT_NODE : 
    case DOMNode::DOCUMENT_NODE :
    case DOMNode::PROCESSING_INSTRUCTION_NODE : 
        {
            baseURI = fNode->getBaseURI();
            break;
        }
	}
    const XMLCh* moduleBaseURI=context->getBaseURI();
    if(baseURI && baseURI[0]) 
    {
        ATAnyURIOrDerived::Ptr tempURI;
        if(moduleBaseURI && moduleBaseURI[0])
        {
            try
            {
                XMLUri temp(moduleBaseURI, context->getMemoryManager());
                XMLUri temp2(&temp, baseURI, context->getMemoryManager());
                tempURI = context->getItemFactory()->createAnyURI(temp2.getUriText(), context);
            }
            catch(MalformedURLException& e)
            {
                XQThrow2(ItemException, X("NodeImpl::dmBaseURI"), X("Base-URI is a malformed URL"));
            }
        }
        else 
            tempURI = context->getItemFactory()->createAnyURI(baseURI, context);
        return Sequence(tempURI, context->getMemoryManager());
    }
    else if(moduleBaseURI && moduleBaseURI[0])
    {
        const ATAnyURIOrDerived::Ptr tempURI = context->getItemFactory()->createAnyURI(moduleBaseURI, context);
        return Sequence(tempURI, context->getMemoryManager());
	}
    return Sequence(context->getMemoryManager());
}

// The dm:node-kind accessor returns a string value representing the
// node's kind: either "document", "element", "attribute", "text",
// "namespace", "processing-instruction", or "comment".

const XMLCh* NodeImpl::dmNodeKind(void) const {
  
    switch(fNode->getNodeType()) {
        
    case DOMNode::DOCUMENT_NODE:
        return document_string;
        
    case DOMNode::ELEMENT_NODE:
        return element_string;
        
    case DOMNode::ATTRIBUTE_NODE:
        return attribute_string;

    case DOMNode::CDATA_SECTION_NODE:
    case DOMNode::TEXT_NODE:
        return text_string;
        
    case DOMNode::PROCESSING_INSTRUCTION_NODE:
        return processing_instruction_string;
        
    case DOMNode::COMMENT_NODE:
        return comment_string;
        
    case DOMXPathNamespace::XPATH_NAMESPACE_NODE:
        return namespace_string;
	}
    
  XQThrow2(ItemException, X("NodeImpl::dmNodeKind"), X("Unknown node type."));
}


ATQNameOrDerived::Ptr NodeImpl::dmNodeName(const DynamicContext* context) const {
  
	switch(fNode->getNodeType())
	{
	case DOMNode::ELEMENT_NODE:
        return context->getItemFactory()->createQName(fNode->getNamespaceURI(), fNode->getPrefix(), fNode->getLocalName(), context);

	case DOMNode::ATTRIBUTE_NODE:				
        return context->getItemFactory()->createQName(fNode->getNamespaceURI(), fNode->getPrefix(), fNode->getLocalName(), context);

	case DOMNode::PROCESSING_INSTRUCTION_NODE:	
        return context->getItemFactory()->createQName(XMLUni::fgZeroLenString, XMLUni::fgZeroLenString, fNode->getNodeName(), context);

	case DOMXPathNamespace::XPATH_NAMESPACE_NODE:  
        {
            const XMLCh* prefix = fNode->getPrefix();
            if(prefix)
                return context->getItemFactory()->createQName(XMLUni::fgZeroLenString, XMLUni::fgZeroLenString, prefix, context);
        }
	}
	return 0;
}


void NodeImpl::addStringValueToBuffer(const DOMNode* node, XMLBuffer& buffer) const
{
    short nodeType=node->getNodeType();
    if(nodeType==DOMNode::TEXT_NODE ||
       nodeType==DOMNode::CDATA_SECTION_NODE)
    {
        buffer.append(node->getNodeValue());
    }
    else
    {
        for(DOMNode* child=node->getFirstChild();child!=NULL;child=child->getNextSibling())
            addStringValueToBuffer(child,buffer);
    }
}

//The dm:string-value accessor returns the node's string
//representation. For some kinds of nodes, this is part of the node;
//for other kinds of nodes, it is computed from the dm:string-value of
//its descendant nodes.


const XMLCh* NodeImpl::dmStringValue(const DynamicContext* context) const {

	switch(fNode->getNodeType())
	{
	case DOMNode::DOCUMENT_NODE:
		{
			// Returns the concatenation of the string-values of all its text node descendants in document order
			XMLBuffer str(1023, context->getMemoryManager());
			addStringValueToBuffer(fNode,str);
		    return context->getMemoryManager()->getPooledString(str.getRawBuffer());
		}
        break;

	case DOMNode::ELEMENT_NODE:
		{
			XMLBuffer str(1023, context->getMemoryManager());
            const XMLCh* typeUri, *typeName;
            getTypeUriAndName(typeUri,typeName);
		    //Returns the string value calculated as follows:
		    // - If the element has a type of xdt:untyped, a complex type with complex content, or a complex type with mixed content, 
		    //   returns the concatenation of the string-values of all its text node descendants in document order. 
		    //   It returns "" if the element has no text node descendants.
		    // - If the element has a complex type with empty content, returns "".
		    if(XPath2Utils::equals(typeName, DocumentCacheParser::g_szUntyped) && XPath2Utils::equals(typeUri, FunctionConstructor::XMLChXPath2DatatypesURI)
		       || context->getDocumentCache()->getComplexTypeInfo(typeUri, typeName)!=NULL)
			    addStringValueToBuffer(fNode,str);
		    else
		    {
			    // - If the element has a simple type or a complex type with simple content:
			    //     - If the element type is xs:string, or a type derived from xs:string, returns that string.
			    if(context->getDocumentCache()->isTypeOrDerivedFromType(typeUri,typeName,SchemaSymbols::fgURI_SCHEMAFORSCHEMA, SchemaSymbols::fgDT_STRING))
				    str.set(fNode->getTextContent());
			    //     - If the element type is xs:anyURI, returns the characters of the URI.
			    else if(XPath2Utils::equals(typeName, SchemaSymbols::fgDT_ANYURI) &&
					    XPath2Utils::equals(typeUri, SchemaSymbols::fgURI_SCHEMAFORSCHEMA))
				    str.set(fNode->getTextContent());
			    //     - If the element type is xs:QName returns the value calculated as follows:
			    //          - If the value has no namespace URI and the in-scope namespaces map the default namespace to any 
			    //            namespace URI, then an error is raised ("default namespace is defined").
			    //          - If the value has a namespace URI, then there must be at least one prefix mapped to that URI in 
			    //            the in-scope namespaces. If there is no such prefix, an error is raised ("no prefix defined for namespace"). 
			    //            If there is more than one such prefix, the one that is chosen is implementation dependent.
			    //          - If no error occurs, returns a string with the lexical form of a xs:QName using the prefix chosen 
			    //            as described above, and the local name of the value.
			    else if(XPath2Utils::equals(typeName, SchemaSymbols::fgDT_QNAME) &&
					    XPath2Utils::equals(typeUri, SchemaSymbols::fgURI_SCHEMAFORSCHEMA))
			    {
				    // REVISIT: the rules assume that a QName has been expanded upon loading, but we don't do that...
				    str.set(fNode->getTextContent());
			    }
			    //     - If the element type is xs:dateTime, xs:date, or xs:time, returns the original lexical representation of 
			    //       the typed value recovered as follows: if an explicit timezone was present, the normalized value is adjusted 
			    //       using the explicit timezone; if an explicit timezone was not present, the Z is dropped from the normalized value. 
			    //       The normalized value and the explicit timezone, if present, are converted separately to xs:string and concatenated 
			    //       to yield the string value.
			    else if(XPath2Utils::equals(typeUri, SchemaSymbols::fgURI_SCHEMAFORSCHEMA) &&
					    (XPath2Utils::equals(typeName, SchemaSymbols::fgDT_DATETIME) ||
					     XPath2Utils::equals(typeName, SchemaSymbols::fgDT_DATE) ||
					     XPath2Utils::equals(typeName, SchemaSymbols::fgDT_TIME)))
			    {
				    const AnyAtomicType::Ptr item = context->getItemFactory()->createDerivedFromAtomicType(typeUri, typeName, fNode->getTextContent(), context);
				    str.set(item->asString(context));
			    }
			    //     - In all other cases, returns the concatenation of the string-values of all its text node descendants in document order.
			    else
				    addStringValueToBuffer(fNode,str);
		    }
		    return context->getMemoryManager()->getPooledString(str.getRawBuffer());
		}
        break;

	case DOMNode::ATTRIBUTE_NODE:
        {
            const XMLCh* typeUri, *typeName;
            getTypeUriAndName(typeUri,typeName);
            // Returns the value calculated as follows:
            //  - If the attribute type is xdt:untypedAtomic, xs:string, or a type derived from xs:string, returns that string.
            if(XPath2Utils::equals(typeName, ATUntypedAtomic::fgDT_UNTYPEDATOMIC) &&
               XPath2Utils::equals(typeUri, FunctionConstructor::XMLChXPath2DatatypesURI) ||
               context->getDocumentCache()->isTypeOrDerivedFromType(typeUri,typeName,SchemaSymbols::fgURI_SCHEMAFORSCHEMA, SchemaSymbols::fgDT_STRING))
                return context->getMemoryManager()->getPooledString(fNode->getNodeValue());
            //  - If the attribute type is xs:anyURI, returns the characters of the URI.
            else if(XPath2Utils::equals(typeName, SchemaSymbols::fgDT_ANYURI) &&
                    XPath2Utils::equals(typeUri, SchemaSymbols::fgURI_SCHEMAFORSCHEMA))
                return context->getMemoryManager()->getPooledString(fNode->getNodeValue());
            //  - If the attribute type is xs:QName returns the value calculated as follows:
            //      - If the value has no namespace URI, then an error is raised ("default namespace is defined") if the in-scope 
            //        namespaces map the default namespace to any namespace URI.
            //      - If the value has a namespace URI, then there must be at least one prefix mapped to that URI in the in-scope 
            //        namespaces. If there is no such prefix, an error is raised ("no prefix defined for namespace"). 
            //        If there is more than one such prefix, the one that is chosen is implementation dependent.
            //      - If no error occurs, returns a string with the lexical form of a xs:QName using the prefix chosen as described 
            //        above, and the local name of the value.
            else if(XPath2Utils::equals(typeName, SchemaSymbols::fgDT_QNAME) &&
                    XPath2Utils::equals(typeUri, SchemaSymbols::fgURI_SCHEMAFORSCHEMA))
            {
                // REVISIT: the rules assume that a QName has been expanded upon loading, but we don't do that...
                return context->getMemoryManager()->getPooledString(fNode->getNodeValue());
            }
            //  - If the attribute type is xs:dateTime, xs:date, or xs:time, returns the original lexical representation recovered as follows: 
            //      - if an explicit timezone was present, the normalized value is adjusted using the explicit timezone; 
            //      - if an explicit timezone was not present, the Z is dropped from the normalized value. 
            //        The normalized value and the explicit timezone, if present, are converted separately to xs:string and concatenated to yield the string value.
            else if(XPath2Utils::equals(typeUri, SchemaSymbols::fgURI_SCHEMAFORSCHEMA) &&
                    (XPath2Utils::equals(typeName, SchemaSymbols::fgDT_DATETIME) ||
                     XPath2Utils::equals(typeName, SchemaSymbols::fgDT_DATE) ||
                     XPath2Utils::equals(typeName, SchemaSymbols::fgDT_TIME)))
            {
                const AnyAtomicType::Ptr item = context->getItemFactory()->createDerivedFromAtomicType(typeUri, typeName, fNode->getTextContent(), context);
                return item->asString(context);
            }
            else
                return context->getMemoryManager()->getPooledString(fNode->getNodeValue());
        }
        break;
	case DOMNode::PROCESSING_INSTRUCTION_NODE:
	case DOMNode::COMMENT_NODE:
	case DOMNode::TEXT_NODE:
	case DOMNode::CDATA_SECTION_NODE:
        // Returns the value of the content property.
        return context->getMemoryManager()->getPooledString(fNode->getNodeValue());
        break;

	case DOMXPathNamespace::XPATH_NAMESPACE_NODE:
        // Returns the value of the uri property.
        return context->getMemoryManager()->getPooledString(fNode->getTextContent());
        break;

	}
	return XMLUni::fgZeroLenString;
}

Sequence NodeImpl::getListTypeTypedValue(DatatypeValidator *dtv, const DynamicContext* context) const {

    const XMLCh *stringVal = dmStringValue(context);
    BaseRefVectorOf<XMLCh>* tokenVector = XMLString::tokenizeString(stringVal);
    Sequence s(tokenVector->size(), context->getMemoryManager());

    //the actual type we want
    DatatypeValidator* theItemTypeDTV = ((ListDatatypeValidator*)dtv)->getItemTypeDTV();
    if (theItemTypeDTV->getType() == DatatypeValidator::Union)
    {
        RefVectorOf<DatatypeValidator>* membersDV = ((UnionDatatypeValidator*)theItemTypeDTV)->getMemberTypeValidators();
        unsigned int size = membersDV->size();
        // find the first datatype in the union that validates the piece
        for ( unsigned int j = 0; j < tokenVector->size(); j++ )
        {
            const XMLCh* szPiece=tokenVector->elementAt(j);
            bool bFound=false;
            for (unsigned int i=0; i<size; i++)
            {
                DatatypeValidator* pDV=membersDV->elementAt(i);
                try
                {
                    pDV->validate(szPiece, NULL, context->getMemoryManager());
                    const XMLCh* itemTypeDTVName = pDV->getTypeLocalName();
                    const XMLCh* itemTypeDTVUri = pDV->getTypeUri();
                    s.addItem(context->getItemFactory()->createDerivedFromAtomicType(itemTypeDTVUri, itemTypeDTVName, szPiece, context));
                    bFound=true;
                    break;
                }
                catch (XMLException&)
                {
                    //absorbed
                }
            }
            if(!bFound)
                XQThrow2(ItemException, X("NodeImpl::getListTypeTypedValue"), X("Value in list doesn't validate with any of the componenets of the union type"));
        }
    } 
    else
    {
        const XMLCh* itemTypeDTVName = theItemTypeDTV->getTypeLocalName();
        const XMLCh* itemTypeDTVUri = theItemTypeDTV->getTypeUri();

        for ( unsigned int j = 0; j < tokenVector->size(); j++ )
            s.addItem(context->getItemFactory()->createDerivedFromAtomicType(itemTypeDTVUri, itemTypeDTVName, tokenVector->elementAt(j), context));
    }
    return s;
 
}


Sequence NodeImpl::dmTypedValue(DynamicContext* context) const {
    /*
    cerr<<"NodeImpl::dmTypedValue getTypeName(): "<<XMLString::transcode(getTypeName())<<endl;
    cerr<<"NodeImpl::dmTypedValue getTypeURI():  "<<XMLString::transcode(getTypeURI())<<endl;

    cerr<<"NodeImpl::dmTypedValue nodenaem:  "<<XMLString::transcode(fNode->getNodeName())<<endl;


    cerr<<"NodeImpl::dmTypedValue uri:  "<<XMLString::transcode(fNode->getNamespaceURI())<<endl;

    cerr << "\n\n" << endl;
    */
    XQScopedNamespace newNSScope(context->getMemoryManager(), context->getNSResolver());
    newNSScope.setNodeContext(fNode);
    AutoNsScopeReset jan(context,&newNSScope);

    switch(fNode->getNodeType())
    {
    case DOMNode::ATTRIBUTE_NODE:
        {
            // Data Model, § 7.3.3 and 7.3.4
            const XMLCh* typeUri, *typeName;
            getMemberTypeUriAndName(typeUri,typeName);
            Item::Ptr item = 0;

            //cerr<<"NodeImpl::dmTypedValue getTypeName(): "<<XMLString::transcode(typeName)<<endl;
            //cerr<<"NodeImpl::dmTypedValue getTypeURI():  "<<XMLString::transcode(typeUri)<<endl;

            // If the attribute is of type xdt:untypedAtomic: its typed-value is its dm:string-value as an xdt:untypedAtomic
            if(XPath2Utils::equals(typeName, ATUntypedAtomic::fgDT_UNTYPEDATOMIC) &&
               XPath2Utils::equals(typeUri, FunctionConstructor::XMLChXPath2DatatypesURI))
            {
                item = (const Item::Ptr)context->getItemFactory()->createUntypedAtomic(dmStringValue(context), context);
                return Sequence(item, context->getMemoryManager()); 
            } 
            else 
            {
                // Otherwise: its typed-value is a sequence of zero or more atomic values derived from the string-value of 
                // the node and its type in a way that is consistent with XML Schema validation. The type of each atomic value 
                // is assigned as described in 3.3.1.2 Atomic Value Type Names
                DatatypeValidator *dtv = context->getDocumentCache()->getDatatypeValidator(typeUri, typeName);

                if(!dtv) 
                {
                    assert(false);
                    XMLBuffer msg(1023, context->getMemoryManager());
                    msg.set(X("The datatype {"));
                    msg.append(typeUri);
                    msg.append(X("}"));
                    msg.append(typeName);
                    msg.append(X(" is unknown"));
                    XQThrow2(XPath2TypeCastException,X("NodeImpl::dmTypedValue"), msg.getRawBuffer());
                } 

                if(dtv->getType() == DatatypeValidator::List)
                    return getListTypeTypedValue(dtv, context);
                else 
                {
                    item = (const Item::Ptr)context->getItemFactory()->createDerivedFromAtomicType(typeUri, typeName, dmStringValue(context), context);
                    return Sequence(item, context->getMemoryManager()); 
                }
            }
        }
        break;
    case DOMNode::ELEMENT_NODE:
        {
            // Data Model, § 7.2.3 and 7.2.4
            // If the nilled property is true, its typed-value is ().
            if(((const ATBooleanOrDerived*)dmNilled(context).get())->isTrue())
                return Sequence(context->getMemoryManager());

            const XMLCh* typeUri, *typeName;
            getMemberTypeUriAndName(typeUri,typeName);

            // If the element is of type xdt:untyped or xs:anyType, its typed-value is its dm:string-value as an xdt:untypedAtomic.
            if((XPath2Utils::equals(typeName, DocumentCacheParser::g_szUntyped) && XPath2Utils::equals(typeUri, FunctionConstructor::XMLChXPath2DatatypesURI)) ||
               (XPath2Utils::equals(typeName, SchemaSymbols::fgATTVAL_ANYTYPE) && XPath2Utils::equals(typeUri, SchemaSymbols::fgURI_SCHEMAFORSCHEMA))
              )
            {
                const AnyAtomicType::Ptr item = context->getItemFactory()->createUntypedAtomic(dmStringValue(context), context);
                return Sequence(item, context->getMemoryManager());
            }

            // If the element has a simple type or a complex type with simple content: it’s typed value is compute 
            // as described in 3.3.1.2 Atomic Value Type Names. The result is a sequence of zero or more atomic values 
            // derived from the string-value of the node and its type in a way that is consistent with XML Schema validation.
            DatatypeValidator *dtv = context->getDocumentCache()->getDatatypeValidator(typeUri, typeName);
            if(dtv) 
            {
                if(dtv->getType() == DatatypeValidator::List)
                    return getListTypeTypedValue(dtv, context);
                else 
                {
                    const AnyAtomicType::Ptr item = context->getItemFactory()->createDerivedFromAtomicType(typeUri, typeName, dmStringValue(context), context);
                    return Sequence(item, context->getMemoryManager()); 
                }
            }
            ComplexTypeInfo *cti = context->getDocumentCache()->getComplexTypeInfo(typeUri, typeName);
            if(cti) 
            {
                if(cti->getContentType() == SchemaElementDecl::Simple) 
                {
                    DatatypeValidator *dtv = cti->getDatatypeValidator();
                    assert(dtv != 0);

                    if(dtv->getType() == DatatypeValidator::List)
                        return getListTypeTypedValue(dtv, context);
                    else 
                    {
                        const AnyAtomicType::Ptr item = context->getItemFactory()->createDerivedFromAtomicType(dtv->getTypeUri(), dtv->getTypeLocalName(), dmStringValue(context), context);
                        return Sequence(item, context->getMemoryManager());
                    }
                } 

                // If the element is empty: its typed-value is the empty sequence, ().
                if(cti->getContentType() == SchemaElementDecl::Empty)
                    return Sequence(context->getMemoryManager());
        
                // If the element has a complex type with mixed content, its typed-value is its dm:string-value as an xdt:untypedAtomic.
                if(cti->getContentType() == SchemaElementDecl::Mixed_Simple || 
                   cti->getContentType() == SchemaElementDecl::Mixed_Complex)
                {
                    const AnyAtomicType::Ptr item = context->getItemFactory()->createUntypedAtomic(dmStringValue(context), context);
                    return Sequence(item, context->getMemoryManager());
                }

                // Otherwise, the element must be a complex type with element-only content. 
                // The typed-value of such an element is undefined. Attempting to access this property with the dm:typed-value 
                // accessor always raises an error.
                if(cti->getContentType() == SchemaElementDecl::Children) 
                    XQThrow2(ItemException, X("NodeImpl::dmTypedValue"), X("Attempt to get typed value from a complex type with non-mixed complex content [err:FOTY0012]"));
            }
            else 
            {
                //throw error here?? Should not happen now, but if we stop giving anytype to invalid in DOMTypeInfo
                //then we will get here if its null.
                assert(false);
            }
        }
        break;
    case DOMNode::DOCUMENT_NODE:
    case DOMNode::TEXT_NODE:
    case DOMNode::CDATA_SECTION_NODE:
        {
            const XMLCh *stringValue = dmStringValue(context);
            const AnyAtomicType::Ptr untypedAtomic = context->getItemFactory()->createUntypedAtomic(stringValue, context);
            return Sequence(untypedAtomic, context->getMemoryManager()); 
        }
        break;
    case DOMXPathNamespace::XPATH_NAMESPACE_NODE:
    case DOMNode::COMMENT_NODE:
    case DOMNode::PROCESSING_INSTRUCTION_NODE:
        {
            const XMLCh *stringValue = dmStringValue(context);
            const AnyAtomicType::Ptr untypedAtomic = context->getItemFactory()->createString(stringValue, context);
            return Sequence(untypedAtomic, context->getMemoryManager());
        }
        break;
    }
    return Sequence(context->getMemoryManager());
}

Sequence NodeImpl::dmDocumentURI(const DynamicContext* context) const {
  if(fNode->getNodeType()!=DOMNode::DOCUMENT_NODE) 
    return Sequence(context->getMemoryManager());
    
  const DOMDocument* doc=(const DOMDocument *)fNode;
  const XMLCh* docURI = doc->getDocumentURI();
  if( docURI==NULL || *docURI==0)
    return Sequence(context->getMemoryManager());
  return Sequence(context->getItemFactory()->createAnyURI(docURI, context), context->getMemoryManager());
}

ATQNameOrDerived::Ptr NodeImpl::dmTypeName(const DynamicContext* context) const {
    short nodeType=fNode->getNodeType();
    if(nodeType != DOMNode::ELEMENT_NODE &&
       nodeType != DOMNode::ATTRIBUTE_NODE &&
       nodeType != DOMNode::TEXT_NODE &&
       nodeType != DOMNode::CDATA_SECTION_NODE) {
        return 0;
    }
 

  //otherwise return the Qname for the node 
  const XMLCh* typeUri, *typeName;
  getTypeUriAndName(typeUri,typeName);
  return context->getItemFactory()->createQName(typeUri, XMLUni::fgZeroLenString, typeName, context);
}

ATBooleanOrDerived::Ptr NodeImpl::dmNilled(const DynamicContext* context) const
{
    if(fNode->getNodeType() != DOMNode::ELEMENT_NODE) {
        return 0;
    }

    // If the [validity] property exists on an Element Node and is "valid" then if the [nil] property exists and is true, 
    // then nilled property is "true". In all other cases, including all cases where schema validity assessment was not 
    // attempted or did not succeed, the nilled property is "false".
    try
    {
        DOMPSVITypeInfo* psviType=(DOMPSVITypeInfo*)const_cast<DOMNode*>(fNode)->getInterface(XMLUni::fgXercescInterfacePSVITypeInfo);
        if(psviType && psviType->getNumericProperty(DOMPSVITypeInfo::PSVI_Validity)==PSVIItem::VALIDITY_VALID)
        {
            bool isNil=(psviType->getNumericProperty(DOMPSVITypeInfo::PSVI_Nil)!=0);
            return context->getItemFactory()->createBoolean(isNil, context);
        }
    } catch(DOMException&) {
        // ignore it; the implementation of getInterface for Xerces < 2.6 will throw it
    }
    return context->getItemFactory()->createBoolean(false, context);
}

bool NodeImpl::lessThan(const Node::Ptr &other, const DynamicContext *context) const
{
  const NodeImpl *otherImpl = (const NodeImpl*)other->getInterface(Item::gXQilla);
  if(otherImpl == 0) {
    // It's not a xqilla implementation Node, so it can't
    // be from the same tree as us - jpcs

    // Order them according to the address of their roots
    return FunctionRoot::root(this, context).get() < FunctionRoot::root(other, context).get();
  }

  const DOMNode* otherNode = otherImpl->getDOMNode();
  short pos = const_cast<DOMNode *>(fNode)->compareTreePosition(const_cast<DOMNode *>(otherNode));
  // compareTreePosition returns the position of the other node, compared to my position; so, if it sets the bit
  // TREE_POSITION_FOLLOWING, it means that we come before it
  if(pos & DOMNode::TREE_POSITION_FOLLOWING)
    return true;
  if(pos & DOMNode::TREE_POSITION_PRECEDING)
    return false;
  if(pos & DOMNode::TREE_POSITION_SAME_NODE)
	  return false;
  // if the two nodes are attributes or namespaces, we get that they are equivalent; we need a stable ordering, so
  // we resort to comparing their pointers (we could even decide to sort them on their names...)
  if(pos & DOMNode::TREE_POSITION_EQUIVALENT)
    return fNode < otherNode;
  // if we get they are disconnected, it could be they belong to different documents; in this case, order them according
  // to the pointer of their documents (all the nodes in document A must come before or after all the nodes in document B,
  // regardless of the chosen ordering criteria)
  // If they belong to the same document, they are floating, or maybe just one of them is floating; let's state we consider
  // the one connected coming before the disconnected one, and, if both are disconnected, we compare the two roots
  if(pos == DOMNode::TREE_POSITION_DISCONNECTED)
  {
    if(fNode->getOwnerDocument()!=otherNode->getOwnerDocument())
      return fNode->getOwnerDocument()<otherNode->getOwnerDocument();
    else
    {
      const DOMNode* myParent=fNode;
      const DOMNode* tmpParent = XPath2NSUtils::getParent(fNode);
      while(tmpParent != 0) {
        myParent = tmpParent;
        tmpParent = tmpParent->getParentNode();
      }
      const DOMNode* otherParent=otherNode;
      tmpParent = XPath2NSUtils::getParent(otherNode);
      while(tmpParent != 0) {
        otherParent = tmpParent;
        tmpParent = tmpParent->getParentNode();
      }

      return myParent < otherParent;
    }
  }
  assert(false);
  return true;
}

bool NodeImpl::equals(const Node::Ptr &other) const
{
  const NodeImpl *otherImpl = (const NodeImpl*)other->getInterface(Item::gXQilla);
  if(otherImpl == 0) return false;
  return fNode->isSameNode(otherImpl->getDOMNode());
}

bool NodeImpl::uniqueLessThan(const Node::Ptr &other, const DynamicContext *context) const
{
  const NodeImpl *otherImpl = (const NodeImpl*)other->getInterface(Item::gXQilla);
  if(otherImpl == 0) {
    // It's not a xqilla implementation Node, so it can't
    // be from the same tree as us - jpcs

    // Order them according to the address of their roots
    return FunctionRoot::root(this, context).get() < FunctionRoot::root(other, context).get();
  }
  return fNode < otherImpl->getDOMNode();
}

const DOMNode* NodeImpl::getDOMNode() const {
  return fNode;
}

Node::Ptr NodeImpl::dmParent(const DynamicContext* context) const
{
  DOMNode *parent = XPath2NSUtils::getParent(fNode);

  // Skip out of entity reference nodes
  while(parent != 0 && parent->getNodeType() == DOMNode::ENTITY_REFERENCE_NODE) {
    parent = parent->getParentNode();
  }

  if(parent == 0) return 0;

  return new NodeImpl(parent, context);
}

Result NodeImpl::dmAttributes(const DynamicContext* context, const LocationInfo *info) const
{
  if(fNode->getNodeType() == DOMNode::ELEMENT_NODE) {
    return new AttributeAxis(info, fNode, this, NULL);
  }
  return 0;
}

Result NodeImpl::dmNamespaceNodes(const DynamicContext* context, const LocationInfo *info) const
{
  if(fNode->getNodeType() == DOMNode::ELEMENT_NODE) {
    return new NamespaceAxis(info, fNode, this, NULL);
  }
  return 0;
}

Result NodeImpl::dmChildren(const DynamicContext *context, const LocationInfo *info) const
{
  if(fNode->getNodeType() == DOMNode::ELEMENT_NODE || fNode->getNodeType() == DOMNode::DOCUMENT_NODE) {
    return new ChildAxis(info, fNode, this, NULL);
  }
  return 0;
}

Result NodeImpl::getAxisResult(XQStep::Axis axis, const NodeTest *nodeTest, const DynamicContext *context, const LocationInfo *info) const
{
  switch(axis) {
  case XQStep::ANCESTOR: {
    return new AncestorAxis(info, fNode, this, nodeTest);
  }
  case XQStep::ANCESTOR_OR_SELF: {
    return new AncestorOrSelfAxis(info, fNode, this, nodeTest);
  }
  case XQStep::ATTRIBUTE: {
    if(fNode->getNodeType() == DOMNode::ELEMENT_NODE) {
      return new AttributeAxis(info, fNode, this, nodeTest);
    }
    break;
  }
  case XQStep::CHILD: {
    if(fNode->getNodeType() == DOMNode::ELEMENT_NODE || fNode->getNodeType() == DOMNode::DOCUMENT_NODE) {
      return new ChildAxis(info, fNode, this, nodeTest);
    }
    break;
  }
  case XQStep::DESCENDANT: {
    if(fNode->getNodeType() == DOMNode::ELEMENT_NODE || fNode->getNodeType() == DOMNode::DOCUMENT_NODE) {
      return new DescendantAxis(info, fNode, this, nodeTest);
    }
    break;
  }
  case XQStep::DESCENDANT_OR_SELF: {
    return new DescendantOrSelfAxis(info, fNode, this, nodeTest);
    break;
  }
  case XQStep::FOLLOWING: {
    return new FollowingAxis(info, fNode, this, nodeTest);
  }
  case XQStep::FOLLOWING_SIBLING: {
    return new FollowingSiblingAxis(info, fNode, this, nodeTest);
  }
  case XQStep::NAMESPACE: {
    if(fNode->getNodeType() == DOMNode::ELEMENT_NODE) {
      return new NamespaceAxis(info, fNode, this, nodeTest);
    }
    break;
  }
  case XQStep::PARENT: {
    return new ParentAxis(info, fNode, this, nodeTest);
  }
  case XQStep::PRECEDING: {
    return new PrecedingAxis(info, fNode, this, nodeTest);
  }
  case XQStep::PRECEDING_SIBLING: {
    return new PrecedingSiblingAxis(info, fNode, this, nodeTest);
  }
  case XQStep::SELF: {
    return nodeTest->filterResult(new SelfAxis(info, this), info);
  }
  }

  return 0;
}

ATBooleanOrDerived::Ptr NodeImpl::dmIsId(const DynamicContext* context) const
{
  const DOMTypeInfo *typeInfo = 0;

  if(fNode->getNodeType() == DOMNode::ELEMENT_NODE) {
    typeInfo = ((const DOMElement*)fNode)->getTypeInfo();
  }
  else if(fNode->getNodeType() == DOMNode::ATTRIBUTE_NODE) {
    typeInfo = ((const DOMAttr*)fNode)->getTypeInfo();
  }

  if(typeInfo != 0 &&
     XPath2Utils::equals(typeInfo->getName(), XMLUni::fgIDString) &&
     (XPath2Utils::equals(typeInfo->getNamespace(), SchemaSymbols::fgURI_SCHEMAFORSCHEMA) ||
      XPath2Utils::equals(typeInfo->getNamespace(), XMLUni::fgInfosetURIName)))
  {
    return context->getItemFactory()->createBoolean(true, context);
  }

  return context->getItemFactory()->createBoolean(false, context);
}

ATBooleanOrDerived::Ptr NodeImpl::dmIsIdRefs(const DynamicContext* context) const
{
  const DOMTypeInfo *typeInfo = 0;

  if(fNode->getNodeType() == DOMNode::ELEMENT_NODE) {
    typeInfo = ((const DOMElement*)fNode)->getTypeInfo();
  }
  else if(fNode->getNodeType() == DOMNode::ATTRIBUTE_NODE) {
    typeInfo = ((const DOMAttr*)fNode)->getTypeInfo();
  }

  if(typeInfo != 0 &&
     (XPath2Utils::equals(typeInfo->getName(), XMLUni::fgIDRefString) ||
      XPath2Utils::equals(typeInfo->getName(), XMLUni::fgIDRefsString)) &&
     (XPath2Utils::equals(typeInfo->getNamespace(), SchemaSymbols::fgURI_SCHEMAFORSCHEMA) ||
      XPath2Utils::equals(typeInfo->getNamespace(), XMLUni::fgInfosetURIName)))
  {
    return context->getItemFactory()->createBoolean(true, context);
  }

  return context->getItemFactory()->createBoolean(false, context);
}

void NodeImpl::getMemberTypeUriAndName(const XMLCh*& uri, const XMLCh*& name) const
{
    short nodeType=fNode->getNodeType();
    try
    {
        if (nodeType == DOMNode::ELEMENT_NODE ||
            nodeType == DOMNode::ATTRIBUTE_NODE) 
        {
            // check if we have PSVI info
            DOMPSVITypeInfo* psviType=(DOMPSVITypeInfo*)const_cast<DOMNode*>(fNode)->getInterface(XMLUni::fgXercescInterfacePSVITypeInfo);
            if(psviType)
            {
                // check if we have PSVI info for the content of a union-validated type
                const XMLCh* memberName=psviType->getStringProperty(DOMPSVITypeInfo::PSVI_Member_Type_Definition_Name);
                if(memberName)
                {
                    uri=psviType->getStringProperty(DOMPSVITypeInfo::PSVI_Member_Type_Definition_Namespace);
                    name=memberName;
                    return;
                }
            }
        }
    } catch(DOMException&) {
        // ignore it; the implementation of getInterface for Xerces < 2.6 will throw it
    }
    // we are not a union, return the type of the node
    getTypeUriAndName(uri, name);
}

void NodeImpl::getTypeUriAndName(const XMLCh*& uri, const XMLCh*& name) const
{
    short nodeType=fNode->getNodeType();
    if (nodeType == DOMNode::ELEMENT_NODE) {
        // check if we have PSVI info
        try
        {
            DOMPSVITypeInfo* psviType=(DOMPSVITypeInfo*)const_cast<DOMNode*>(fNode)->getInterface(XMLUni::fgXercescInterfacePSVITypeInfo);
            if(psviType && psviType->getNumericProperty(DOMPSVITypeInfo::PSVI_Validity)==PSVIItem::VALIDITY_VALID)
            {
                uri=psviType->getStringProperty(DOMPSVITypeInfo::PSVI_Type_Definition_Namespace);
                name=psviType->getStringProperty(DOMPSVITypeInfo::PSVI_Type_Definition_Name);
                return;
            }
        } catch(DOMException&) {
            // ignore it; the implementation of getInterface for Xerces < 2.6 will throw it
        }
        // we are xdt:untyped
        uri=FunctionConstructor::XMLChXPath2DatatypesURI;
        name=DocumentCacheParser::g_szUntyped;
        return;
    }
    else if (nodeType == DOMNode::ATTRIBUTE_NODE) {
        // check if we have PSVI info
        try
        {
            DOMPSVITypeInfo* psviType=(DOMPSVITypeInfo*)const_cast<DOMNode*>(fNode)->getInterface(XMLUni::fgXercescInterfacePSVITypeInfo);
            if(psviType && psviType->getNumericProperty(DOMPSVITypeInfo::PSVI_Validity)==PSVIItem::VALIDITY_VALID)
            {
                uri=psviType->getStringProperty(DOMPSVITypeInfo::PSVI_Type_Definition_Namespace);
                name=psviType->getStringProperty(DOMPSVITypeInfo::PSVI_Type_Definition_Name);
                return;
            }
        } catch(DOMException&) {
            // ignore it; the implementation of getInterface for Xerces < 2.6 will throw it
        }
        // check if we have type informations coming from a DTD
        const DOMTypeInfo* pTypeInfo=((DOMAttr*)fNode)->getTypeInfo();
        const XMLCh* szUri=pTypeInfo->getNamespace();
        if(szUri==0 || szUri[0]==0)
        {
            // in these cases, we are xs:*
            const XMLCh* szName=pTypeInfo->getName();
            if(XPath2Utils::equals(szName,XMLUni::fgIDString) ||
               XPath2Utils::equals(szName,XMLUni::fgIDRefString) ||
               XPath2Utils::equals(szName,XMLUni::fgIDRefsString) ||
               XPath2Utils::equals(szName,XMLUni::fgEntityString) ||
               XPath2Utils::equals(szName,XMLUni::fgEntitiesString) ||
               XPath2Utils::equals(szName,XMLUni::fgNmTokenString) ||
               XPath2Utils::equals(szName,XMLUni::fgNmTokensString))
            {
                uri=SchemaSymbols::fgURI_SCHEMAFORSCHEMA;
                name=szName;
                return;
            }
        }
        // we are xdt:untypedAtomic
        uri=FunctionConstructor::XMLChXPath2DatatypesURI;
        name=ATUntypedAtomic::fgDT_UNTYPEDATOMIC;
        return;
    }
    else if(nodeType == DOMNode::TEXT_NODE || 
            nodeType == DOMNode::CDATA_SECTION_NODE) {
        uri=FunctionConstructor::XMLChXPath2DatatypesURI;
        name=ATUntypedAtomic::fgDT_UNTYPEDATOMIC;
        return;
    }
    XQThrow2(ItemException, X("NodeImpl::getTypeUriAndName"), X("Tried to get type informations on Node other than DOMElement, DOMAttribute or DOMText"));
}

const XMLCh* NodeImpl::getTypeName() const {
    const XMLCh* uri, *name;
    getTypeUriAndName(uri,name);
    return name;
}

const XMLCh* NodeImpl::getTypeURI() const {
    const XMLCh* uri, *name;
    getTypeUriAndName(uri,name);
    return uri;
}

