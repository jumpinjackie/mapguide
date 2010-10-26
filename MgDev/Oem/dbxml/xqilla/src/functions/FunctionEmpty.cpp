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
#include <xqilla/functions/FunctionEmpty.hpp>
#include <xqilla/ast/XQLiteral.hpp>
#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/context/ContextHelpers.hpp>

const XMLCh FunctionEmpty::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_y, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionEmpty::minArgs = 1;
const unsigned int FunctionEmpty::maxArgs = 1;

/*
 * fn:empty($arg as item()*) as xs:boolean
 */

FunctionEmpty::FunctionEmpty(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "item()*", args, memMgr)
{
}

ASTNode* FunctionEmpty::staticResolution(StaticContext *context) {
  AutoNodeSetOrderingReset orderReset(context);
  return resolveArguments(context);
}

ASTNode *FunctionEmpty::staticTypingImpl(StaticContext *context)
{
  _src.clear();
  _src.getStaticType() = StaticType::BOOLEAN_TYPE;

  ASTNode *result = calculateSRCForArguments(context);
  if(context && result == this) {
    const StaticAnalysis &sa = _args[0]->getStaticAnalysis();
    const StaticType &sType = sa.getStaticType();
    if((sType.getMin() > 0 || sType.getMax() == 0) && !sa.areDocsOrCollectionsUsed() && !sa.isNoFoldingForced()) {
      XPath2MemoryManager* mm = context->getMemoryManager();
      return XQLiteral::create(sType.getMax() == 0, mm, this);
    }
  }

  return result;
}

Sequence FunctionEmpty::createSequence(DynamicContext* context, int flags) const
{
  Result items = getParamNumber(1,context);
  return Sequence(context->getItemFactory()->createBoolean(items->next(context).isNull(), context),
                  context->getMemoryManager());
}
















