//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQDOMConstructor.cpp,v 1.40 2005/11/30 17:08:55 jsnelson Exp $
//

//////////////////////////////////////////////////////////////////////
// XQDOMConstructor.cpp: implementation of the XQDOMConstructor class.
//////////////////////////////////////////////////////////////////////

#include "xquery/XQEngine.hpp"
#include "xquery/dataItem/XQDOMConstructor.hpp"
#include "xquery/XQContext.hpp"
#include <xquery/context/XQDynamicContextImpl.hpp>
#include <xquery/context/XQFactory.hpp>
#include "xquery/dataItem/XQDebugHook.hpp"
#include "xquery/dom-extensions/XQScopedNamespace.hpp"
#include <pathan/exceptions/DataItemException.hpp>
#include <pathan/exceptions/NamespaceLookupException.hpp>
#include <pathan/exceptions/StaticErrorException.hpp>
#include <pathan/SequenceType.hpp>
#include <pathan/XPath2Utils.hpp>
#include <pathan/XPath2NSUtils.hpp>
#include <pathan/Node.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>
#include <pathan/dataItem/DataItemSequence.hpp>
#include <pathan/dataItem/DataItemLiteral.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/internal/DOMutils/XMLChCompare.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLChar.hpp>
#include <assert.h>
#include <set>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

class AutoNsScopeReset
{
public:
    AutoNsScopeReset(StaticContext* context)
    {
        _context=context;
        _oldNSResolver=_context->getNSResolver();
    }

    ~AutoNsScopeReset()
    {
        _context->setNSResolver(_oldNSResolver);
    }

protected:
    StaticContext* _context;
    const DOMXPathNSResolver* _oldNSResolver;
};

static const XMLCh *definePrefix(const XMLCh *szPrefix, const XMLCh *szURI, const XQScopedNamespace &newNSScope,
                                 XQScopedNamespace &locallyDefinedNamespaces, std::vector<Node::Ptr> &attrList,
                                 const DynamicContext *context);


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XQDOMConstructor::XQDOMConstructor(const XMLCh* nodeType, DataItem* name, VectorOfDataItems* attrList, VectorOfDataItems* children, XPath2MemoryManager* expr) :
  DataItemImpl(expr),
  m_name(name),
  m_attrList(attrList),
  m_children(children)
{
  m_nodeType=nodeType;
  setType((DataItem::whichType)XQContext::DOM_CONSTRUCTOR);
}

bool XQDOMConstructor::isTextNode(const Node::Ptr &node) const
{
  return node->dmNodeKind() == Node::text_string;
}

Sequence XQDOMConstructor::collapseTreeInternal(DynamicContext *context, int flags) const 
{
    XQDynamicContext* xqcontext=CAST_TO_DYNAMIC(context);
    Node::Ptr result;
#if defined(_DEBUG) || defined(DEBUG)
    // in debug builds, I'd rather get the exception
    try
    {
#endif
      // DOCUMENT node
      if(m_nodeType == Node::document_string)
      {
        std::vector<Node::Ptr> childList;

        for (VectorOfDataItems::const_iterator itCont = m_children->begin(); itCont != m_children->end (); ++itCont) 
        {
          DataItem* childItem=(*itCont);
          Result oneChild = childItem->collapseTree(context);
          Item::Ptr child;
          while((child = oneChild.next(context)) != NULLRCP)
          {
            if(child->isNode())
            {
              Node::Ptr sourceNode=(Node::Ptr)child;
              // If the content sequence contains an attribute node following a node that is not an attribute node, a type error is raised [err:XQTY0024].
              if(sourceNode->dmNodeKind()==Node::attribute_string)
                DSLthrow(DataItemException,X("DOM Constructor"),X("An attribute node cannot be a child of a document [err:XQTY0024]"));
              // If the content sequence contains a document node, the document node is replaced in the content 
              // sequence by its children.
              else if(sourceNode->dmNodeKind()==Node::document_string) {
                Result children = sourceNode->dmChildren(context);
                Node::Ptr childNode;
                while((childNode = children.next(context)).notNull()) {
                  childList.push_back(childNode);
                }
              }
              // Adjacent text nodes in the content sequence are merged into a single text node by concatenating 
              // their contents, with no intervening blanks. After concatenation, any text node whose content is 
              // a zero-length string is deleted from the content sequence.
              else if(isTextNode(sourceNode) && !childList.empty() && isTextNode(childList.back())) {
                const XMLCh* buff=XPath2Utils::concatStrings(childList.back()->dmStringValue(context),
                                                             sourceNode->dmStringValue(context),
                                                             context->getMemoryManager());
                childList.pop_back();
                childList.push_back(((XQFactory*)context->getPathanFactory())->createTextNode(buff, context));
              }
              else {
                childList.push_back(sourceNode);
              }
            }
            else
            {
              if(!childList.empty() && isTextNode(childList.back())) {
                const XMLCh* buff=XPath2Utils::concatStrings(childList.back()->dmStringValue(context),child->asString(context),context->getMemoryManager());

                childList.pop_back();
                childList.push_back(((XQFactory*)context->getPathanFactory())->createTextNode(buff, context));
              }
              else {
                childList.push_back(((XQFactory*)context->getPathanFactory())->createTextNode(child->asString(context), context));
              }
            }
          }
        }
        result = ((XQFactory*)context->getPathanFactory())->createDocumentNode(childList, context);
      }

      // ELEMENT node
      else if(m_nodeType == Node::element_string)
      {
        Result resName=m_name->collapseTree(context).atomize(context);
        AnyAtomicType::Ptr itemName=resName.next(context);
        if(itemName==NULLRCP || resName.next(context)!=NULLRCP)
          DSLthrow(DataItemException,X("DOM Constructor"),X("The name for the element must be a single xs:QName, xs:string or xs:untypedAtomic item [err:XPTY0004]"));

        std::vector<Node::Ptr> attrList;
        std::vector<XQFactory::ElementChild> childList;

        AutoNsScopeReset jan(context);
        // Add a new scope for the namespace definitions, before we try to assign a URI to the name of the element
        XQScopedNamespace locallyDefinedNamespaces(context->getMemoryManager(), NULL);
        XQScopedNamespace newNSScope(context->getMemoryManager(), context->getNSResolver());
        context->setNSResolver(&newNSScope);

        if(m_attrList != 0)
        {
          for (VectorOfDataItems::const_iterator itAttr = m_attrList->begin(); itAttr != m_attrList->end (); ++itAttr) 
          {
            DataItem* attrItem=(*itAttr);
            static SequenceType nodeSequence(new SequenceType::ItemType(SequenceType::ItemType::TEST_NODE), SequenceType::STAR);
	    StaticType stype;
	    stype.flags = StaticType::NODE_TYPE;
            Result oneAttribute=attrItem->collapseTree(context).convertFunctionArg(&nodeSequence,stype,context);
            Item::Ptr attr;
            while((attr = oneAttribute.next(context)) != NULLRCP) 
            {
              if(attr->isNode()) 
              {
                Node::Ptr node=(Node::Ptr)attr;
                if(node->dmNodeKind() == Node::attribute_string) 
                {
                  ATQNameOrDerived::Ptr name = node->dmNodeName(context);
                  const XMLCh *node_prefix = ((const ATQNameOrDerived*)name.get())->getPrefix();
                  const XMLCh *node_name = ((const ATQNameOrDerived*)name.get())->getName();

                  // check for namespace nodes
                  if(XPath2Utils::equals(node_name, XMLUni::fgXMLNSString))
                  {
                    locallyDefinedNamespaces.addNamespaceBinding(XMLUni::fgZeroLenString, node->dmStringValue(context));
                    xqcontext->setNamespaceBinding(XMLUni::fgZeroLenString, node->dmStringValue(context));
                  }
                  else if(node_prefix && XPath2Utils::equals(node_prefix, XMLUni::fgXMLNSString))
                  {
                    locallyDefinedNamespaces.addNamespaceBinding(node_name, node->dmStringValue(context));
                    xqcontext->setNamespaceBinding(node_name, node->dmStringValue(context));
                  }

                  attrList.push_back(node);
                }
              }
            }
          }
        }

        const XMLCh* nodeUri=NULL, *nodePrefix=NULL, *nodeName=NULL;
        if(itemName->getPrimitiveTypeIndex()==AnyAtomicType::QNAME)
        {
          const ATQNameOrDerived* pQName=(const ATQNameOrDerived*)(const AnyAtomicType*)itemName;
          nodeUri=pQName->getURI();
          nodePrefix=pQName->getPrefix();
          nodeName=pQName->getName();
        }
        else if(itemName->getPrimitiveTypeIndex()==AnyAtomicType::STRING || itemName->getPrimitiveTypeIndex()==AnyAtomicType::UNTYPED_ATOMIC)
        {
          const XMLCh* pString=itemName->asString(context);
          nodePrefix=XPath2NSUtils::getPrefix(pString, context->getMemoryManager());
          nodeUri=context->getUriBoundToPrefix(nodePrefix);
          // if the prefix was empty and we didn't find a xmlns=".." declaration in the scope, use the default element/type ns
          if((nodePrefix==0 || *nodePrefix==0) && (nodeUri==0 || *nodeUri==0))
            nodeUri=context->getDefaultElementAndTypeNS();
          // keep the specified prefix in the node name
          nodeName=pString;
        }
        else
          DSLthrow(DataItemException,X("DOM Constructor"),X("The name for the element must be either a xs:QName, xs:string or xs:untypedAtomic [err:XPTY0004]"));

        // TODO: what error should we return if the string is empty?
        if(XMLString::stringLen(nodeName)==0)
          DSLthrow(DataItemException,X("DOM Constructor"),X("The name for the element is empty"));

        for (VectorOfDataItems::const_iterator itCont = m_children->begin(); itCont != m_children->end (); ++itCont)
        {
          DataItem* childItem=(*itCont);
          Result children=childItem->collapseTree(context);
          Item::Ptr child;
          bool lastWasAtomic = false;
          while((child = children.next(context)) != NULLRCP)
          {
            if(child->isNode())
            {
              lastWasAtomic = false;
              Node::Ptr sourceNode = (Node::Ptr)child;

              // DOCUMENT node
              if(sourceNode->dmNodeKind() == Node::document_string)
              {
                Result children = sourceNode->dmChildren(context);
                Node::Ptr childNode;
                while((childNode = children.next(context)).notNull()) {
                  childList.push_back(childNode);
                }
              }

              // ATTRIBUTE node
              else if(sourceNode->dmNodeKind() == Node::attribute_string)
              {
                if(!childList.empty())
                  DSLthrow(DataItemException,X("DOM Constructor"),X("Attribute nodes must be created before the other child nodes of an element [err:XQTY0024]"));

                // check if the attribute has a prefix that has been defined
                ATQNameOrDerived::Ptr name = sourceNode->dmNodeName(context);
                const XMLCh *szPrefix = ((const ATQNameOrDerived*)name.get())->getPrefix();
                const XMLCh *szURI = ((const ATQNameOrDerived*)name.get())->getURI();
                if(szPrefix!=NULL && *szPrefix!=0)
                {
                  const XMLCh *newPrefix = definePrefix(szPrefix, szURI, newNSScope, locallyDefinedNamespaces, attrList, context);
                  if(newPrefix != szPrefix)
                  {
                    sourceNode = ((XQFactory*)context->getPathanFactory())->
                      createAttributeNode(szURI, newPrefix, ((const ATQNameOrDerived*)name.get())->getName(), sourceNode->dmStringValue(context), context);
                  }
                }
                attrList.push_back(sourceNode);
              }

              else
              {
                if(isTextNode(sourceNode) && !childList.empty() && isTextNode(childList.back())) {
                  const XMLCh* buff=XPath2Utils::concatStrings(childList.back()->dmStringValue(context),
                                                               sourceNode->dmStringValue(context),
                                                               context->getMemoryManager());
                  childList.pop_back();
                  childList.push_back(XQFactory::ElementChild(((XQFactory*)context->getPathanFactory())->createTextNode(buff, context), /*clone*/false));
                }
                else {
                  DataItem* pChild=childItem;
                  if((unsigned int)pChild->getType()==XQContext::DEBUG_HOOK)
                    pChild=((XQDebugHook*)pChild)->m_impl;
                  // if it's a text node, ensure it's not empty
                  if(!isTextNode(sourceNode) || !XPath2Utils::equals(sourceNode->dmStringValue(context),0))
                    // if the node we should add as a child was generated by a DOM constructor, we can skip cloning it
                    childList.push_back(XQFactory::ElementChild(sourceNode, (unsigned int)pChild->getType()!=XQContext::DOM_CONSTRUCTOR));
                }
              }
            }
            else
            {
              const XMLCh* valueStr=child->asString(context);
              if(lastWasAtomic)
              {
                XMLCh space[]={ ' ', 0 };
                valueStr=XPath2Utils::concatStrings(space,valueStr,context->getMemoryManager());
              }
              lastWasAtomic = true;

              if(!childList.empty() && isTextNode(childList.back())) {
                const XMLCh* buff=XPath2Utils::concatStrings(childList.back()->dmStringValue(context),valueStr,context->getMemoryManager());
                        
                childList.pop_back();
                childList.push_back(((XQFactory*)context->getPathanFactory())->createTextNode(buff, context));
              }
              else {
                childList.push_back(((XQFactory*)context->getPathanFactory())->createTextNode(valueStr, context));
              }
            }
          }
        }
        result = ((XQFactory*)context->getPathanFactory())->createElementNode(nodeUri, nodePrefix, nodeName, attrList, childList, context);
      }

      // ATTRIBUTE node
      else if(m_nodeType == Node::attribute_string)
      {
        Result resName=m_name->collapseTree(context).atomize(context);
        AnyAtomicType::Ptr itemName=resName.next(context);
        if(itemName==NULLRCP || resName.next(context)!=NULLRCP)
          DSLthrow(DataItemException,X("DOM Constructor"),X("The name for the attribute must be a single xs:QName, xs:string or xs:untypedAtomic item [err:XPTY0004]"));

        const XMLCh* nodeUri=NULL, *nodePrefix=NULL, *nodeName=NULL;
        if(itemName->getPrimitiveTypeIndex()==AnyAtomicType::QNAME)
        {
          const ATQNameOrDerived* pQName=(const ATQNameOrDerived*)(const AnyAtomicType*)itemName;
          nodeUri=pQName->getURI();
          nodePrefix=pQName->getPrefix();
          nodeName=pQName->getName();
        }
        else if(itemName->getPrimitiveTypeIndex()==AnyAtomicType::STRING || itemName->getPrimitiveTypeIndex()==AnyAtomicType::UNTYPED_ATOMIC)
        {
          const XMLCh* pString=itemName->asString(context);
          nodePrefix=XPath2NSUtils::getPrefix(pString, context->getMemoryManager());
          // if the prefix was empty we are in no namespace
          if(nodePrefix==0 || *nodePrefix==0)
            nodeUri=NULL;
          else
            nodeUri=context->getUriBoundToPrefix(nodePrefix);
          // keep the specified prefix in the node name
          nodeName=pString;
        }
        else
          DSLthrow(DataItemException,X("DOM Constructor"),X("The name for the attribute must be either a xs:QName, xs:string or xs:untypedAtomic [err:XPTY0004]"));

        if(nodeUri==NULL && XPath2Utils::equals(nodeName, XMLUni::fgXMLNSString))
          nodeUri=XMLUni::fgXMLNSURIName;
        // TODO: add a flag to distinguish between direct attribute constructors and computed constructor: this test must
        // be done only on computed attribute constuctors
        //if(XPath2Utils::equals(nodeUri, XMLUni::fgXMLNSURIName))
        //    DSLthrow(DataItemException,X("DOM Constructor"),X("A computed attribute constructor cannot create a namespace declaration [err:XQ0044]"));

        // TODO: what error should we return if the string is empty?
        if(XMLString::stringLen(nodeName)==0)
          DSLthrow(DataItemException,X("DOM Constructor"),X("The name for the attribute is empty"));

        XMLBuffer value;
        getStringValue(value, context);
        XMLString::replaceWS(value.getRawBuffer(), context->getMemoryManager());

        result = ((XQFactory*)context->getPathanFactory())->createAttributeNode(nodeUri, nodePrefix, nodeName, value.getRawBuffer(), context);
      }

      // PROCESSING INSTRUCTION node
      else if(m_nodeType == Node::processing_instruction_string)
      {
        Result resName=m_name->collapseTree(context).atomize(context);
        AnyAtomicType::Ptr itemName=resName.next(context);
        if(itemName==NULLRCP || resName.next(context)!=NULLRCP)
          DSLthrow(DataItemException,X("DOM Constructor"),X("The target for the processing instruction must be a single xs:NCName, xs:string or xs:untypedAtomic item [err:XPTY0004]"));

        const XMLCh* nodeName=NULL;
        // the specs specify that a xs:NCName could be returned, but we create a xs:string in that case
        if(itemName->getPrimitiveTypeIndex()==AnyAtomicType::STRING || itemName->getPrimitiveTypeIndex()==AnyAtomicType::UNTYPED_ATOMIC)
        {
          nodeName=itemName->asString(context);
          if(!XMLChar1_0::isValidNCName(nodeName, XMLString::stringLen(nodeName)))
            DSLthrow(DataItemException,X("DOM Constructor"),X("The target for the processing instruction must be a valid xs:NCName [err:XQDY0041]"));
        }
        else
          DSLthrow(DataItemException,X("DOM Constructor"),X("The target for the processing instruction must be a single xs:NCName, xs:string or xs:untypedAtomic item [err:XPTY0004]"));

        if(XMLString::compareIString(nodeName, XMLUni::fgXMLString)==0)
          DSLthrow(DataItemException,X("DOM Constructor"),X("The target for the processing instruction must not be 'XML' [err:XQDY0064]"));
        int nIndex=XMLString::indexOf(nodeName, chQuestion);
        int nTargetLen=XMLString::stringLen(nodeName);
        while(nIndex!=-1)
        {
          if((nIndex+1)<nTargetLen && nodeName[nIndex+1]==chCloseAngle)
            DSLthrow(DataItemException,X("DOM Constructor"),X("The target for the processing instruction must not contain the string '?>' [err:XQDY0026]"));
          nIndex=XMLString::indexOf(nodeName, chQuestion, nIndex+1);
        }

        if(XMLString::stringLen(nodeName)==0)
          DSLthrow(DataItemException,X("DOM Constructor"),X("The name for the processing instruction is empty"));
        XMLBuffer value;
        getStringValue(value, context);

        // Check for the substring "?>"
        bool foundQ = false;
        const XMLCh *ptr = value.getRawBuffer();
        const XMLCh *end = ptr + value.getLen();
        while(ptr != end) {
          if(*ptr == chCloseAngle && foundQ)
            DSLthrow(DataItemException,X("DOM Constructor"),X("The content for the processing instruction must not contain the string '?>' [err:XQDY0026]"));
          else if(*ptr == chQuestion) foundQ = true;
          else foundQ = false;
          ++ptr;
        }

        result = ((XQFactory*)context->getPathanFactory())->createPINode(nodeName, value.getRawBuffer(), context);
      }

      // COMMENT node
      else if(m_nodeType == Node::comment_string)
      {
        XMLBuffer value;
        getStringValue(value, context);

        // Check for two dashes in a row, or a dash at the end
        bool foundDash = false;
        const XMLCh *ptr = value.getRawBuffer();
        const XMLCh *end = ptr + value.getLen();
        while(ptr != end) {
          if(*ptr == chDash) {
            if(foundDash) break;
            foundDash = true;
          }
          else foundDash = false;
          ++ptr;
        }
        if(foundDash)
          DSLthrow(DataItemException,X("DOM Constructor"),X("It is a dynamic error if the result of the content expression of a computed comment constructor contains two adjacent hyphens or ends with a hyphen. [err:XQDY0072]"));

        result = ((XQFactory*)context->getPathanFactory())->createCommentNode(value.getRawBuffer(), context);
      }

      // TEXT node
      else if(m_nodeType == Node::text_string)
      {
        XMLBuffer value;
        if(getStringValue(value, context))
          result = ((XQFactory*)context->getPathanFactory())->createTextNode(value.getRawBuffer(), context);
      }
      else
      {
        assert(false);
      }
#if defined(_DEBUG) || defined(DEBUG)
    }
    catch(DOMException& e) {
        DSLthrow(DataItemException,X("DOM Constructor"),e.getMessage());
    }
#endif
    if(result.notNull())
        return Sequence(result, context->getMemoryManager());
    return Sequence(context->getMemoryManager());
}

DataItem* XQDOMConstructor::staticResolution(StaticContext *context)
{
  XQContext* xqcontext = CAST_TO_XQCONTEXT(context);

  if(m_name != 0) {
    m_name = m_name->staticResolution(context);
    _src.add(m_name->getStaticResolutionContext());
  }

  {
      AutoNsScopeReset jan(context);
      // Add a new scope for the namespace definitions
      XQScopedNamespace newNSScope(context->getMemoryManager(), context->getNSResolver());
      context->setNSResolver(&newNSScope);
      unsigned int i;
	  if(m_attrList != 0)
      {
        for (i=0;i<m_attrList->size();i++) {
          (*m_attrList)[i]=(*m_attrList)[i]->staticResolution(context);
          _src.add((*m_attrList)[i]->getStaticResolutionContext()); 
       }
        // after we have added all the namespace declaration, check for duplicate attributes
        std::set<const XMLCh*, XMLChSort> attrNames;
        AutoRelease<DynamicContext> dContext(context->createDynamicContext());
        dContext->setMemoryManager(context->getMemoryManager());
        for (i=0;i<m_attrList->size();i++) 
        {
          if((*m_attrList)[i]->getType()==(DataItem::whichType)XQContext::DOM_CONSTRUCTOR)
          {
            const DataItem* dItem=((XQDOMConstructor*)(*m_attrList)[i])->getName();
            if(dItem->getType()==DataItem::SEQUENCE)
            {
              const ItemConstructor::Vector &ics = ((DataItemSequence*)dItem)->getItemConstructors();
              if(ics.size() == 1) {
                Item::Ptr item = ics[0]->createItem(dContext);
                QualifiedName attrName(item->asString(dContext));
                XMLBuffer buff(200, dContext->getMemoryManager());
                if(attrName.getPrefix()!=0 && *attrName.getPrefix()!=0)
                {
                    buff.append(chOpenCurly);
                    buff.append(dContext->getUriBoundToPrefix(attrName.getPrefix()));
                    buff.append(chCloseCurly);
                }
                buff.append(attrName.getName());
                if(attrNames.find(buff.getRawBuffer())!=attrNames.end())
                    if(XPath2Utils::equals(attrName.getFullName(dContext->getMemoryManager()), XMLUni::fgXMLNSString) ||
                       XPath2Utils::equals(attrName.getPrefix(), XMLUni::fgXMLNSString))
                      DSLthrow(StaticErrorException,X("DOM Constructor"),X("Two namespace declaration attributes specified by a direct element constructor do not have distinct names. [err:XQST0071]"));
                    else
                      DSLthrow(StaticErrorException,X("DOM Constructor"),X("Two attributes specified by a direct element constructor do not have distinct expanded QNames. [err:XQST0040]"));
                attrNames.insert(XMLString::replicate(buff.getRawBuffer(), dContext->getMemoryManager()));
              }
            }
          }
        }
      }
    for (i=0;i<m_children->size();i++) {
      (*m_children)[i]=(*m_children)[i]->staticResolution(context);
      _src.add((*m_children)[i]->getStaticResolutionContext());

    }
  }

  if(m_nodeType==Node::attribute_string && m_name->isConstant()) {
    AutoRelease<DynamicContext> dContext(context->createDynamicContext());
    dContext->setMemoryManager(context->getMemoryManager());

    Item::Ptr item;
    if(m_name->getType() == DataItem::LITERAL) {
      item = ((DataItemLiteral*)m_name)->getItemConstructor()->createItem(dContext);
    }
    else if(m_name->getType() == DataItem::SEQUENCE) {
      const ItemConstructor::Vector &ics = ((DataItemSequence*)m_name)->getItemConstructors();
      if(ics.size() == 1) {
        item = ics[0]->createItem(dContext);
      }
    }

    if(item != NULLRCP) {
      const XMLCh* strName = item->asString(dContext);
      const XMLCh* XMLNSPrefix=NULL;
      if(XPath2Utils::equals(strName,XMLUni::fgXMLNSString)) {
        XMLNSPrefix=XMLUni::fgZeroLenString;
      }
      else if(XPath2Utils::equals(XPath2NSUtils::getPrefix(strName, context->getMemoryManager()),
                                                               XMLUni::fgXMLNSString)) {
        XMLNSPrefix=XPath2NSUtils::getLocalName(strName);
      }

      if(XMLNSPrefix!=NULL) {
        // we are a namespace attribute: check that we have a constant value
        if(m_children->size()==0) { // TODO: supporting Namespace 1.1 would mean unsetting the binding...
          xqcontext->setNamespaceBinding(XMLNSPrefix, XMLUni::fgZeroLenString);
        }
        else if(m_children->size()>1 || (*m_children)[0]->getType()!=DataItem::SEQUENCE) {
          DSLthrow(StaticErrorException,X("DOM Constructor"),X("The value of a namespace declaration attribute must be a literal string [err:XQST0022]"));
        }
        else {
          DataItem *child = (*m_children)[0];
          Item::Ptr nsUri;
          if(child->getType() == DataItem::LITERAL) {
            nsUri = ((DataItemLiteral*)child)->getItemConstructor()->createItem(dContext);
          }
          else if(child->getType() == DataItem::SEQUENCE) {
            const ItemConstructor::Vector &ics = ((DataItemSequence*)child)->getItemConstructors();
            if(ics.size() == 1) {
              nsUri = ics[0]->createItem(dContext);
            }
          }

          if(nsUri == NULLRCP)
            DSLthrow(StaticErrorException,X("DOM Constructor"),X("The value of a namespace declaration attribute must be a literal string [err:XQST0022]"));
          xqcontext->setNamespaceBinding(XMLNSPrefix, nsUri->asString(dContext));
        }
      }
    }
  }

  _src.getStaticType().flags = StaticType::NODE_TYPE;
  _src.forceNoFolding(true);
  return resolvePredicates(context); // Never constant fold
}

bool XQDOMConstructor::getStringValue(XMLBuffer &value, DynamicContext *context) const
{
  bool bSomethingFound=false;
  for(VectorOfDataItems::const_iterator itCont = m_children->begin();
      itCont != m_children->end (); ++itCont) {
    Result childList = (*itCont)->collapseTree(context).atomize(context);
    Item::Ptr child;
    bool addSpace = false;
    while((child = childList.next(context)) != NULLRCP) {
      if(addSpace) value.append(' ');
      else addSpace = true;
      value.append(child->asString(context));
      bSomethingFound=true;
    }
  }
  return bSomethingFound;
}

const XMLCh *definePrefix(const XMLCh *szPrefix, const XMLCh *szURI, const XQScopedNamespace &newNSScope,
                          XQScopedNamespace &locallyDefinedNamespaces, std::vector<Node::Ptr> &attrList,
                          const DynamicContext *context)
{
  // check if the attribute has a prefix that has been defined
  const XMLCh* associatedURI=newNSScope.lookupNamespaceURI(szPrefix);
  if(associatedURI==NULL) // prefix is not defined
  {
    locallyDefinedNamespaces.addNamespaceBinding(szPrefix,szURI);
    attrList.push_back(((XQFactory*)context->getPathanFactory())->
      createAttributeNode(XMLUni::fgXMLNSURIName, XMLUni::fgXMLNSString, szPrefix, szURI, context));
  }
  else if(!XPath2Utils::equals(szURI, associatedURI))  // prefix is defined, but it is associated to another URI
  {
    if(locallyDefinedNamespaces.lookupNamespaceURI(szPrefix)==NULL)    // prefix is inherited, override it
    {
      locallyDefinedNamespaces.addNamespaceBinding(szPrefix,szURI);
      attrList.push_back(((XQFactory*)context->getPathanFactory())->
        createAttributeNode(XMLUni::fgXMLNSURIName, XMLUni::fgXMLNSString, szPrefix, szURI, context));
    }
    else // prefix is defined here, rename it
    {
      XMLCh szNumBuff[20];
      const XMLCh *szInitialPrefix = szPrefix;
      long index = 1;
      while(locallyDefinedNamespaces.lookupNamespaceURI(szPrefix) != NULL) {
        static XMLCh szUnderScore[] = { chUnderscore, chNull };
        XMLString::binToText(index++, szNumBuff, 19, 10);
        szPrefix = XPath2Utils::concatStrings(szInitialPrefix, szUnderScore, szNumBuff, context->getMemoryManager());
      }

      locallyDefinedNamespaces.addNamespaceBinding(szPrefix,szURI);
      attrList.push_back(((XQFactory*)context->getPathanFactory())->
        createAttributeNode(XMLUni::fgXMLNSURIName, XMLUni::fgXMLNSString, szPrefix, szURI, context));
    }
  }

  return szPrefix;
}

const XMLCh* XQDOMConstructor::getNodeType() const
{
  return m_nodeType;
}

const DataItem *XQDOMConstructor::getName() const
{
  return m_name;
}

const VectorOfDataItems *XQDOMConstructor::getAttributes() const
{
  return m_attrList;
}

const VectorOfDataItems *XQDOMConstructor::getChildren() const
{
  return m_children;
}

void XQDOMConstructor::setName(DataItem *name)
{
  m_name = name;
}
