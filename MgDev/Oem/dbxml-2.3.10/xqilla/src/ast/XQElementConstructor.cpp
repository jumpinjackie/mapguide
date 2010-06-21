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
 * $Id: XQElementConstructor.cpp,v 1.1 2006/11/03 17:04:20 jpcs Exp $
 */

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/ast/XQElementConstructor.hpp>
#include <xqilla/ast/XQDebugHook.hpp>
#include <xqilla/ast/StaticResolutionContext.hpp>
#include <xqilla/ast/XQSequence.hpp>
#include <xqilla/ast/XQLiteral.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/context/XQScopedNamespace.hpp>
#include <xqilla/exceptions/ASTException.hpp>
#include <xqilla/exceptions/NamespaceLookupException.hpp>
#include <xqilla/exceptions/StaticErrorException.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/items/AnyAtomicTypeConstructor.hpp>
#include <xqilla/ast/XQAtomize.hpp>
#include <xqilla/parser/QName.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <xercesc/util/XMLChar.hpp>
#include <assert.h>
#include <set>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

static const XMLCh *definePrefix(const XMLCh *szPrefix, const XMLCh *szURI, const XQScopedNamespace &newNSScope,
                                 XQScopedNamespace &locallyDefinedNamespaces, std::vector<Node::Ptr> &attrList,
                                 DynamicContext *context);

XQElementConstructor::XQElementConstructor(ASTNode* name, VectorOfASTNodes* attrList, VectorOfASTNodes* children,
	XPath2MemoryManager* mm)
  : XQDOMConstructor(mm),
    m_name(name),
    m_attrList(attrList),
    m_children(children),
    m_namespaces(NULL)
{
  setType(ASTNode::DOM_CONSTRUCTOR);
}

static bool isTextNode(const Node::Ptr &node)
{
  return node->dmNodeKind() == Node::text_string;
}

Sequence XQElementConstructor::collapseTreeInternal(DynamicContext *context, int flags) const 
{
  Node::Ptr result;
  try
  {
    // Add a new scope for the namespace definitions, before we try to assign a URI to the name of the element
    XQScopedNamespace locallyDefinedNamespaces(context->getMemoryManager(), NULL);
    XQScopedNamespace newNSScope(context->getMemoryManager(), context->getNSResolver());
    AutoNsScopeReset jan(context, &newNSScope);

    std::vector<Node::Ptr> attrList;
    if(m_namespaces != 0) {
      RefHashTableOfEnumerator<XMLCh> nsEnumVal(m_namespaces, false, context->getMemoryManager());
      RefHashTableOfEnumerator<XMLCh> nsEnumKey(m_namespaces, false, context->getMemoryManager());
      while(nsEnumVal.hasMoreElements())
      {
        XMLCh* uri=&nsEnumVal.nextElement();
        XMLCh* prefix=(XMLCh*)nsEnumKey.nextElementKey();
        locallyDefinedNamespaces.addNamespaceBinding(prefix, uri);
        Node::Ptr node;
        if(XPath2Utils::equals(prefix, XMLUni::fgZeroLenString)) {
          context->setDefaultElementAndTypeNS(uri);
          node = context->getItemFactory()->createAttributeNode(XMLUni::fgXMLNSURIName, XMLUni::fgZeroLenString,
                                                                XMLUni::fgXMLNSString, uri, context);
        }
        else {
          context->setNamespaceBinding(prefix, uri);
          node = context->getItemFactory()->createAttributeNode(XMLUni::fgXMLNSURIName, XMLUni::fgXMLNSString, prefix, uri,
                                                                context);
        }

        attrList.push_back(node);
      }
    }
    if(m_attrList != 0) {
      for(VectorOfASTNodes::const_iterator itAttr = m_attrList->begin(); itAttr != m_attrList->end (); ++itAttr) {
        Result oneAttribute = (*itAttr)->collapseTree(context);
        Item::Ptr attr;
        while((attr = oneAttribute->next(context)) != NULLRCP) {
          assert(attr->isNode());
          Node::Ptr node=(Node::Ptr)attr;

          assert(node->dmNodeKind() == Node::attribute_string);
          attrList.push_back(node);
        }
      }
    }

    // Now that we have converted our namespace attributes into namespace bindings, resolve the name
    Result resName=m_name->collapseTree(context);
    AnyAtomicType::Ptr itemName=resName->next(context);
    if(itemName==NULLRCP || resName->next(context)!=NULLRCP)
      XQThrow(ASTException,X("DOM Constructor"),
              X("The name for the element must be a single xs:QName, xs:string or xs:untypedAtomic item [err:XPTY0004]"));

    const XMLCh* nodeUri=NULL, *nodePrefix=NULL, *nodeName=NULL;
    if(itemName->getPrimitiveTypeIndex()==AnyAtomicType::QNAME)
    {
      const ATQNameOrDerived* pQName=(const ATQNameOrDerived*)(const AnyAtomicType*)itemName;
      nodeUri=pQName->getURI();
      nodePrefix=pQName->getPrefix();
      nodeName=pQName->getName();
    }
    else if(itemName->getPrimitiveTypeIndex()==AnyAtomicType::STRING ||
            itemName->getPrimitiveTypeIndex()==AnyAtomicType::UNTYPED_ATOMIC)
    {
      const XMLCh* pString=itemName->asString(context);
      if(!XMLChar1_0::isValidQName(pString, XMLString::stringLen(pString)))
        XQThrow(ASTException,X("DOM Constructor"),
                X("The name for the element cannot be converted to a xs:QName [err:XQDY0074]"));

      nodePrefix=XPath2NSUtils::getPrefix(pString, context->getMemoryManager());
      try
      {
        nodeUri=context->getUriBoundToPrefix(nodePrefix, this);
      }
      catch(NamespaceLookupException&)
      {
        XQThrow(ASTException,X("DOM Constructor"),X("The name for the element cannot be converted to a "
                                                    "xs:QName because the prefix is undefined [err:XQDY0074]"));
      }
      // if the prefix was empty and we didn't find a xmlns=".." declaration in the scope, use the default element/type ns
      if((nodePrefix==0 || *nodePrefix==0) && (nodeUri==0 || *nodeUri==0))
        nodeUri=context->getDefaultElementAndTypeNS();
      // keep the specified prefix in the node name
      nodeName=pString;
    }
    else
      XQThrow(ASTException,X("DOM Constructor"),X("The name for the element must be either a xs:QName, "
                                                  "xs:string or xs:untypedAtomic [err:XPTY0004]"));

    // TODO: what error should we return if the string is empty?
    if(nodeName==NULL || *nodeName==0)
      XQThrow(ASTException,X("DOM Constructor"),X("The name for the element is empty"));

    std::vector<ItemFactory::ElementChild> childList;
    for (VectorOfASTNodes::const_iterator itCont = m_children->begin(); itCont != m_children->end (); ++itCont)
    {
      ASTNode* childItem=(*itCont);
      Result children=childItem->collapseTree(context);
      Item::Ptr child;
      bool lastWasAtomic = false;
      while((child = children->next(context)) != NULLRCP)
      {
        if(child->isNode())
        {
          lastWasAtomic = false;
          Node::Ptr sourceNode = (Node::Ptr)child;

          // DOCUMENT node
          if(sourceNode->dmNodeKind() == Node::document_string)
          {
            Result children = sourceNode->dmChildren(context, this);
            Node::Ptr childNode;
            while((childNode = children->next(context)).notNull()) {
              // merge consecutive text nodes
              if(isTextNode(childNode) && !childList.empty() && isTextNode(childList.back())) {
                const XMLCh* buff=XPath2Utils::concatStrings(childList.back()->dmStringValue(context),
                                                             childNode->dmStringValue(context),
                                                             context->getMemoryManager());
                childList.pop_back();
                childList.push_back(context->getItemFactory()->createTextNode(buff, context));
              }
              else
                childList.push_back(childNode);
            }
          }

          // ATTRIBUTE node
          else if(sourceNode->dmNodeKind() == Node::attribute_string)
          {
            if(!childList.empty())
              XQThrow(ASTException,X("DOM Constructor"),X("Attribute nodes must be created before the "
                                                          "other child nodes of an element [err:XQTY0024]"));

            // check if the attribute has a prefix that has been defined
            ATQNameOrDerived::Ptr name = sourceNode->dmNodeName(context);
            const XMLCh *szPrefix = ((const ATQNameOrDerived*)name.get())->getPrefix();
            const XMLCh *szURI = ((const ATQNameOrDerived*)name.get())->getURI();
            if(szPrefix!=NULL && *szPrefix!=0)
            {
              const XMLCh *newPrefix = definePrefix(szPrefix, szURI, newNSScope, locallyDefinedNamespaces, attrList,
                                                    context);
              if(newPrefix != szPrefix)
              {
                sourceNode = context->getItemFactory()->
                  createAttributeNode(szURI, newPrefix, ((const ATQNameOrDerived*)name.get())->getName(),
                                      sourceNode->dmStringValue(context), context);
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
              childList.push_back(ItemFactory::ElementChild(context->getItemFactory()->createTextNode(buff, context),
                                                            /*clone*/false));
            }
            else {
              ASTNode* pChild=childItem;
              if((unsigned int)pChild->getType()==ASTNode::DEBUG_HOOK)
                pChild=((XQDebugHook*)pChild)->m_impl;
              // if it's a text node, ensure it's not empty
              if(!isTextNode(sourceNode) || !XPath2Utils::equals(sourceNode->dmStringValue(context),0))
                // if the node we should add as a child was generated by a DOM constructor, we can skip cloning it
                childList.push_back(ItemFactory::ElementChild(sourceNode, (unsigned int)pChild->getType()!=
                                                              ASTNode::DOM_CONSTRUCTOR));
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
          // empty strings are stripped
          if(valueStr && *valueStr)
          {
            if(!childList.empty() && isTextNode(childList.back())) {
              const XMLCh* buff=XPath2Utils::concatStrings(childList.back()->dmStringValue(context),valueStr,
                                                           context->getMemoryManager());
                        
              childList.pop_back();
              childList.push_back(context->getItemFactory()->createTextNode(buff, context));
            }
            else {
              childList.push_back(context->getItemFactory()->createTextNode(valueStr, context));
            }
          }
        }
      }
    }
    result = context->getItemFactory()->createElementNode(nodeUri, nodePrefix, nodeName, attrList, childList, context);
  }
  catch(DOMException& e) {
    XQThrow(ASTException,X("ElementConstructor"),e.getMessage());
  }
  if(result.notNull())
    return Sequence(result, context->getMemoryManager());
  return Sequence(context->getMemoryManager());
}

ASTNode* XQElementConstructor::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  // Add a new scope for the namespace definitions
  XQScopedNamespace newNSScope(context->getMemoryManager(), context->getNSResolver());
  AutoNsScopeReset jan(context, &newNSScope);
  unsigned int i;

  if(m_attrList != 0) {
    AutoDelete<DynamicContext> dContext(context->createDynamicContext());
    dContext->setMemoryManager(context->getMemoryManager());
    // process the namespace attributes (they are all at the beginning of the list)
    for (VectorOfASTNodes::iterator it=m_attrList->begin();it!=m_attrList->end();) 
    {
      ASTNode* astNode=*it;
      if(astNode->getType()==ASTNode::DEBUG_HOOK)
          astNode=((XQDebugHook*)astNode)->m_impl;
      assert(astNode->getType()==ASTNode::DOM_CONSTRUCTOR && 
             ((XQDOMConstructor*)astNode)->getNodeType()==Node::attribute_string);
      XQDOMConstructor* attrConstructor=(XQDOMConstructor*)astNode;
      const ASTNode* dItem=attrConstructor->getName();
      assert(dItem!=NULL && dItem->getType() == ASTNode::LITERAL);
      const ItemConstructor* itemConstr=((XQLiteral*)dItem)->getItemConstructor();
      assert(XPath2Utils::equals(itemConstr->getTypeURI(),
                                 SchemaSymbols::fgURI_SCHEMAFORSCHEMA) &&
             XPath2Utils::equals(itemConstr->getTypeName(),
                                 SchemaSymbols::fgDT_QNAME));

      // createItem will throw an exception if the prefix is undefined
      AnyAtomicType::Ptr atomName;
      try
      {
        atomName = itemConstr->createItem(dContext);
      }
      catch(XQException& e)
      {
        // this is a normal attribute, there are no more namespace declarations
        break;
      }

      const ATQNameOrDerived* pQName=(const ATQNameOrDerived*)atomName.get();
      const XMLCh* XMLNSPrefix=NULL;
      if((pQName->getPrefix()==NULL || *pQName->getPrefix()==0) &&
         XPath2Utils::equals(pQName->getName(),XMLUni::fgXMLNSString)) {
        XMLNSPrefix=XMLUni::fgZeroLenString;
      }
      else if(XPath2Utils::equals(pQName->getPrefix(), XMLUni::fgXMLNSString)) {
        XMLNSPrefix=pQName->getName();
      }
      else
        // no more namespace declaration
        break;
      // we are a namespace attribute: check that we have a constant value
      const VectorOfASTNodes *children=attrConstructor->getChildren();
      const XMLCh* uri=NULL;
      if(children->size()==0) { // supporting Namespace 1.1 means unsetting the binding...
        uri=XMLUni::fgZeroLenString;
      }
      else if(children->size()>1 || ((*children)[0]->getType()!=ASTNode::LITERAL &&
                                     (*children)[0]->getType()!=ASTNode::SEQUENCE)) {
        XQThrow(StaticErrorException,X("DOM Constructor"),X("The value of a namespace declaration attribute must "
                                                            "be a literal string [err:XQST0022]"));
      }
      else {
        Item::Ptr nsUri = ((XQLiteral*)(*children)[0])->getItemConstructor()->createItem(dContext);
        if(nsUri == NULLRCP)
          XQThrow(StaticErrorException,X("DOM Constructor"),X("The value of a namespace declaration attribute must "
                                                              "be a literal string [err:XQST0022]"));
        uri=nsUri->asString(dContext);
      }
      if(XMLNSPrefix != XMLUni::fgZeroLenString) {
        context->setNamespaceBinding(XMLNSPrefix, uri);
      }
      else {
        context->setDefaultElementAndTypeNS(uri);
      }
      if(m_namespaces==NULL)
        m_namespaces = new (mm) RefHashTableOf< XMLCh >(5, false, mm);
      
      if(m_namespaces->containsKey(XMLNSPrefix))
        XQThrow(StaticErrorException,X("DOM Constructor"),X("Two namespace declaration attributes specified by a direct ele"
                                                            "ment constructor do not have distinct names. [err:XQST0071]"));
      m_namespaces->put((void*)mm->getPooledString(XMLNSPrefix), (XMLCh*)mm->getPooledString(uri));
      it=m_attrList->erase(it);
    }

    // now run static resolution on the real attributes
    for (i=0;i<m_attrList->size();i++) {
      (*m_attrList)[i] = (*m_attrList)[i]->staticResolution(context);
    }
    // now that we have added the local namespace declaration, check for duplicate attribute names
    std::set<const XMLCh*, XMLChSort> attrNames;
    for (i=0;i<m_attrList->size();i++) 
    {
      ASTNode* astNode=(*m_attrList)[i];
      if(astNode->getType()==ASTNode::DEBUG_HOOK)
          astNode=((XQDebugHook*)astNode)->m_impl;
      assert(astNode->getType()==ASTNode::DOM_CONSTRUCTOR && 
             ((XQDOMConstructor*)astNode)->getNodeType()==Node::attribute_string);
      XQDOMConstructor* attrConstructor=(XQDOMConstructor*)astNode;
      const ASTNode* dItem=attrConstructor->getName();
      if(dItem->getType() == ASTNode::ATOMIZE) {
	      dItem = ((XQAtomize*)dItem)->getExpression();
      }
      assert(dItem->getType()==ASTNode::SEQUENCE); 
      const ItemConstructor::Vector &ics = ((XQSequence*)dItem)->getItemConstructors();
      assert(ics.size() == 1);
      Item::Ptr item = ics[0]->createItem(dContext);
      QualifiedName attrName(item->asString(dContext));
      XMLBuffer buff(200, dContext->getMemoryManager());
      if(attrName.getPrefix()!=0 && *attrName.getPrefix()!=0)
      {
        buff.append(chOpenCurly);
        buff.append(dContext->getUriBoundToPrefix(attrName.getPrefix(), this));
        buff.append(chCloseCurly);
      }
      buff.append(attrName.getName());
      if(attrNames.find(buff.getRawBuffer())!=attrNames.end())
        XQThrow(StaticErrorException,X("DOM Constructor"),X("Two attributes specified by a direct element constructor do not have distinct expanded QNames. [err:XQST0040]"));
      attrNames.insert(XMLString::replicate(buff.getRawBuffer(), dContext->getMemoryManager()));
    }
  }
  if(m_name)
  {
    // verify the namespace prefix used by the node name
    if(m_name->getType() == ASTNode::LITERAL) {
      const ItemConstructor* itemConstr=((XQLiteral*)m_name)->getItemConstructor();
      // if the type is xs:QName, it was a named constructor, and it must be checked
      if(XPath2Utils::equals(itemConstr->getTypeURI(),SchemaSymbols::fgURI_SCHEMAFORSCHEMA) &&
         XPath2Utils::equals(itemConstr->getTypeName(),SchemaSymbols::fgDT_QNAME)) 
      {
        AutoDelete<DynamicContext> dContext(context->createDynamicContext());
        dContext->setMemoryManager(context->getMemoryManager());
        // createItem will throw an exception if the prefix is undefined
        AnyAtomicType::Ptr atomName;
        try
        {
          atomName = itemConstr->createItem(dContext);
        }
        catch(XQException& e)
        {
          XMLBuffer buff(1023, context->getMemoryManager());
          buff.set(e.getError());
          int index=XMLString::indexOf(buff.getRawBuffer(),chOpenSquare);
          if(index!=-1)
            XMLString::copyString(buff.getRawBuffer()+index, X("[err:XPST0081]"));
          else
            buff.append(X("[err:XPST0081]"));
          XQThrow(StaticErrorException, X("XQElementConstructor::staticResolution"), buff.getRawBuffer());
        }
      }
    }
    // and run static resolution
    m_name = new (mm) XQAtomize(m_name, mm);
    m_name->setLocationInfo(this);
    m_name = m_name->staticResolution(context);
  }


  for (i=0;i<m_children->size();i++) {
    // normalize whitespace and expand entities in string literals
    if((*m_children)[i]->getType()==LITERAL)
    {
      AutoDelete<DynamicContext> dContext(context->createDynamicContext());
      dContext->setMemoryManager(mm);
      XQLiteral *lit = (XQLiteral*)(*m_children)[i];
      Item::Ptr item = lit->getItemConstructor()->createItem(dContext);
      if(((AnyAtomicType*)(const Item*)item)->getPrimitiveTypeIndex()==AnyAtomicType::STRING)
      {
        const XMLCh* str=item->asString(dContext);
        XMLBuffer buff(XMLString::stringLen(str)+1, mm);
        buff.append(str);
        unescapeEntities(buff);
        AnyAtomicTypeConstructor *newIC = new (mm)
                        AnyAtomicTypeConstructor(
                            SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                            SchemaSymbols::fgDT_STRING,
                mm->getPooledString(buff.getRawBuffer()), AnyAtomicType::STRING);
        lit->setItemConstructor(newIC);
      }
    }
    (*m_children)[i] = (*m_children)[i]->staticResolution(context);
  }

  return this;
}

ASTNode* XQElementConstructor::staticTyping(StaticContext *context)
{
  _src.clear();

  // Add a new scope for the namespace definitions
  XQScopedNamespace newNSScope(context->getMemoryManager(), context->getNSResolver());
  AutoNsScopeReset jan(context, &newNSScope);

  if(m_namespaces != 0) {
    RefHashTableOfEnumerator<XMLCh> nsEnumVal(m_namespaces, false, context->getMemoryManager());
    RefHashTableOfEnumerator<XMLCh> nsEnumKey(m_namespaces, false, context->getMemoryManager());
    while(nsEnumVal.hasMoreElements()) {
      XMLCh *prefix = (XMLCh*)nsEnumKey.nextElementKey();
      XMLCh *uri = &nsEnumVal.nextElement();
      if(XPath2Utils::equals(prefix, XMLUni::fgZeroLenString)) {
        context->setDefaultElementAndTypeNS(uri);
      }
      else {
        context->setNamespaceBinding(prefix, uri);
      }
    }
  }

  unsigned int i;
  if(m_attrList != 0) {
    for(i = 0; i < m_attrList->size(); ++i) {
      (*m_attrList)[i] = (*m_attrList)[i]->staticTyping(context);
      _src.add((*m_attrList)[i]->getStaticResolutionContext()); 
    }
  }

  if(m_name) {
    m_name = m_name->staticTyping(context);
    _src.add(m_name->getStaticResolutionContext());
  }

  for(i = 0; i < m_children->size(); ++i) {
    (*m_children)[i] = (*m_children)[i]->staticTyping(context);
    _src.add((*m_children)[i]->getStaticResolutionContext());
  }

  _src.getStaticType().flags = StaticType::ELEMENT_TYPE;
  _src.forceNoFolding(true);
  _src.creative(true);
  return this; // Never constant fold
}

const XMLCh *definePrefix(const XMLCh *szPrefix, const XMLCh *szURI, const XQScopedNamespace &newNSScope,
                          XQScopedNamespace &locallyDefinedNamespaces, std::vector<Node::Ptr> &attrList,
                          DynamicContext *context)
{
  // check if the attribute has a prefix that has been defined
  const XMLCh* associatedURI=locallyDefinedNamespaces.lookupNamespaceURI(szPrefix);
  if(associatedURI == NULL)
    associatedURI=newNSScope.lookupNamespaceURI(szPrefix);

  if(associatedURI==NULL) // prefix is not defined
  {
    locallyDefinedNamespaces.addNamespaceBinding(szPrefix,szURI);
    attrList.push_back(context->getItemFactory()->
      createAttributeNode(XMLUni::fgXMLNSURIName, XMLUni::fgXMLNSString, szPrefix, szURI, context));
  }
  else if(!XPath2Utils::equals(szURI, associatedURI))  // prefix is defined, but it is associated to another URI
  {
    if(locallyDefinedNamespaces.lookupNamespaceURI(szPrefix)==NULL)    // prefix is inherited, override it
    {
      locallyDefinedNamespaces.addNamespaceBinding(szPrefix,szURI);
      attrList.push_back(context->getItemFactory()->
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
      attrList.push_back(context->getItemFactory()->
        createAttributeNode(XMLUni::fgXMLNSURIName, XMLUni::fgXMLNSString, szPrefix, szURI, context));
    }
  }

  return szPrefix;
}

const XMLCh* XQElementConstructor::getNodeType() const
{
  return Node::element_string;
}

const ASTNode *XQElementConstructor::getName() const
{
  return m_name;
}

const VectorOfASTNodes *XQElementConstructor::getAttributes() const
{
  return m_attrList;
}

const VectorOfASTNodes *XQElementConstructor::getChildren() const
{
  return m_children;
}

void XQElementConstructor::setName(ASTNode *name)
{
  m_name = name;
}
