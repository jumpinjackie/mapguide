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
#include <xqilla/functions/FunctionId.hpp>

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/ast/StaticAnalysis.hpp>
#include <xqilla/items/DatatypeFactory.hpp>

const XMLCh FunctionId::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionId::minArgs = 1;
const unsigned int FunctionId::maxArgs = 2;

/**
 * fn:id($arg as xs:string*) as element()*
 * fn:id($arg as xs:string*, $node as node()) as element()*
**/

FunctionId::FunctionId(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "string*, node()", args, memMgr)
{
}

ASTNode* FunctionId::staticResolution(StaticContext *context) {
  if(_args.size()==2 && _args.back()->getType()==ASTNode::CONTEXT_ITEM)
      _args.pop_back();
  return resolveArguments(context);
}

ASTNode *FunctionId::staticTypingImpl(StaticContext *context)
{
  _src.clear();

  _src.getStaticType() = StaticType(StaticType::ELEMENT_TYPE, 0, StaticType::UNLIMITED);
  if(_args.size()==1)
    _src.contextItemUsed(true);
  return calculateSRCForArguments(context);
}

Sequence FunctionId::createSequence(DynamicContext* context, int flags) const
{
  Node::Ptr ctxNode;
  if(getNumArgs() == 2)
  {
    Sequence arg=getParamNumber(2,context)->toSequence(context);
    ctxNode=arg.first();
  }
  else
  {
    const Item::Ptr item = context->getContextItem();
    if(item==NULLRCP)
      XQThrow(FunctionException, X("FunctionId::createSequence"),X("Undefined context item in fn:id [err:XPDY0002]"));
    if(!item->isNode())
      XQThrow(FunctionException, X("FunctionId::createSequence"),X("The context item is not a node [err:XPTY0004]"));
    ctxNode=item;
  }

  Node::Ptr root = ctxNode->root(context);
  if(root->dmNodeKind() != Node::document_string) {
    XQThrow(FunctionException,X("FunctionId::createSequence"), X("Current context doesn't belong to a document [err:FODC0001]"));
  }
    
  Sequence strings = getParamNumber(1, context)->toSequence(context);
  if(strings.isEmpty())
    return Sequence(context->getMemoryManager());
  
  std::vector<const XMLCh*> values;
  
  //get the list of id values we're looking for by iterating over each string in the sequence
  for (Sequence::iterator stringIt = strings.begin(); stringIt != strings.end(); ++stringIt) {
    const XMLCh *str = (*stringIt)->asString(context);
    std::vector<const XMLCh*> idList = XPath2Utils::getVal(str, context->getMemoryManager());

    //for each list obtained from a string check that each id is unique to the full list and if so add it
    for (std::vector<const XMLCh*>::iterator listIt=idList.begin(); listIt!=idList.end(); ++listIt) {
      if (!XPath2Utils::containsString(values, *listIt))
        values.push_back(*listIt);
    }
  }

  Sequence result(context->getMemoryManager());
  std::vector<const XMLCh*> returnedVals;

  std::vector<Result> resultStack;
  resultStack.push_back(root->dmChildren(context, this));
  Node::Ptr child = resultStack.back()->next(context);
  while(child.notNull()) {
    if(child->dmNodeKind() == Node::element_string) {
      bool added = false;
      if(child->dmIsId(context)->isTrue()) {
        // child is of type xs:ID
        const XMLCh* id = child->dmStringValue(context);
        if(XPath2Utils::containsString(values, id) &&
           !XPath2Utils::containsString(returnedVals, id)) {
          returnedVals.push_back(id);
          result.addItem(child);
          added = true;
        }
      }

      if(!added) {
        Result attrs = child->dmAttributes(context, this);
        Node::Ptr att;
        while((att = (Node::Ptr)attrs->next(context)).notNull()) {
          if(att->dmIsId(context)->isTrue()) {
            // att is of type xs:ID
            const XMLCh* id = att->dmStringValue(context);
            if(XPath2Utils::containsString(values, id) &&
               !XPath2Utils::containsString(returnedVals, id)) {
              returnedVals.push_back(id);
              result.addItem(child);
              break;
            }
          }
        }
      }
    }

    resultStack.push_back(child->dmChildren(context, this));
    while(!resultStack.empty() && (child = resultStack.back()->next(context)).isNull()) {
      resultStack.pop_back();
    }
  }

  return result;
}

