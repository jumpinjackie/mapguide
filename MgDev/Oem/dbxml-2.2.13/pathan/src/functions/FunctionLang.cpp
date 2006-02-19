/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/functions/FunctionLang.hpp>

#include <pathan/XPath2Utils.hpp>
#include <pathan/DynamicContext.hpp>
#include <pathan/exceptions/FunctionException.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <pathan/ATBooleanOrDerived.hpp>
#include <pathan/ATStringOrDerived.hpp>
#include <pathan/Node.hpp>
#include <pathan/internal/factory/DatatypeFactory.hpp>
#include <pathan/dataItem/StaticResolutionContext.hpp>

const XMLCh FunctionLang::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionLang::minArgs = 1;
const unsigned int FunctionLang::maxArgs = 2;

/**
 * fn:lang($testlang as xs:string?) as xs:boolean
 * fn:lang($testlang as xs:string?, $node as node()) as xs:boolean
**/

FunctionLang::FunctionLang(const VectorOfDataItems &args, XPath2MemoryManager* memMgr)
  : DataItemFunction(name, minArgs, maxArgs, "string?,node()", args, memMgr)
{
}

DataItem* FunctionLang::staticResolution(StaticContext *context) {
  if(_args.size()==1)
    _src.contextItemUsed(true);
  return resolveDataItems(_args, context, _args.size()!=1);
}

Sequence FunctionLang::collapseTreeInternal(DynamicContext* context, int flags) const
{
    XPath2MemoryManager* memMgr = context->getMemoryManager();

    Sequence arg1=getParamNumber(1, context);
    const XMLCh *param1 = XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString;
    if(!arg1.isEmpty())
        param1=arg1.first()->asString(context);

    const XMLCh* xmlLangValue = XPath2Utils::toLower(param1, context->getMemoryManager());
    bool isSubLang = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::indexOf(xmlLangValue, XERCES_CPP_NAMESPACE_QUALIFIER chDash) > 0;

    Node::Ptr ctxNode;
    if(getNumArgs() == 2)
    {
        Sequence arg=getParamNumber(2,context);
        ctxNode=arg.first();
    }
    else
    {
        const Item::Ptr item = context->getContextItem();
        if(item==NULLRCP)
            DSLthrow(FunctionException, X("FunctionLang::collapseTreeInternal"),X("Undefined context item in fn:lang [err:FONC0001]"));
        if(!item->isNode())
            DSLthrow(FunctionException, X("FunctionLang::collapseTreeInternal"),X("The context item is not a node [err:XPTY0006]"));
        ctxNode=item;
    }

    static const XMLCh xmlLang[] = { XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a,
                                     XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g,
                                     XERCES_CPP_NAMESPACE_QUALIFIER chNull};

    static const XMLCh* xmlLangNamespace = XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgXMLURIName;

    Node::Ptr node = ctxNode;
    while(node.notNull())
    {
      Result attrs = node->dmAttributes(context);
      Node::Ptr att;
      while((att = (Node::Ptr)attrs.next(context)).notNull()) {
        ATQNameOrDerived::Ptr name = att->dmNodeName(context);
        if(name.notNull()) {
          const XMLCh *node_uri = ((const ATQNameOrDerived*)name.get())->getURI();
          const XMLCh *node_name = ((const ATQNameOrDerived*)name.get())->getName();

          if(XPath2Utils::equals(xmlLang, node_name) &&
             XPath2Utils::equals(xmlLangNamespace, node_uri)) {
            const XMLCh *value = att->dmStringValue(context);

            const XMLCh* asLower = XPath2Utils::toLower(value, context->getMemoryManager());
            int dashLocation = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::indexOf(asLower, XERCES_CPP_NAMESPACE_QUALIFIER chDash);

            if(dashLocation!=-1 && !isSubLang) {
              asLower = XPath2Utils::subString(asLower, 0, dashLocation, memMgr);
            }

            return Sequence(context->getPathanFactory()->createBoolean(XPath2Utils::equals(asLower, xmlLangValue), context), memMgr);
          }
        }
      }

      node = node->dmParent(context);
    }

    return Sequence(context->getPathanFactory()->createBoolean(false, context), memMgr);
}
