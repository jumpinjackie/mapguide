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
#include <xqilla/functions/FunctionConcat.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <xqilla/context/ItemFactory.hpp>

XERCES_CPP_NAMESPACE_USE

const XMLCh FunctionConcat::name[] = {
  chLatin_c, chLatin_o, chLatin_n, 
  chLatin_c, chLatin_a, chLatin_t, 
  chNull 
};
const unsigned int FunctionConcat::minArgs = 2;
const unsigned int FunctionConcat::maxArgs = (unsigned int) UNLIMITED;

/**
 * fn:concat($arg1 as xdt:anyAtomicType?, $arg2 as xdt:anyAtomicType?, ...) as xs:string
**/


FunctionConcat::FunctionConcat(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "anyAtomicType?", args, memMgr) // Between 2 and UNLIMITED arguments
{
  _src.getStaticType() = StaticType::STRING_TYPE;
}

Sequence FunctionConcat::createSequence(DynamicContext* context, int flags) const
{
  XMLBuffer result;
  for(unsigned int i = 1; i <= getNumArgs(); ++i) {
    Item::Ptr item = getParamNumber(i,context)->next(context);
    if(!item.isNull()) {
      result.append(item->asString(context));
    }
  }
  return Sequence(context->getItemFactory()->createString(result.getRawBuffer(), context), context->getMemoryManager());
}
