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
#include <xqilla/functions/FunctionCeiling.hpp>
#include <xqilla/items/Numeric.hpp>
#include <xqilla/context/DynamicContext.hpp>

const XMLCh FunctionCeiling::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionCeiling::minArgs = 1;
const unsigned int FunctionCeiling::maxArgs = 1;

/*
 * fn:ceiling($arg as numeric?) as numeric?
 */

FunctionCeiling::FunctionCeiling(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : NumericFunction(name, minArgs, maxArgs, "anyAtomicType?", args, memMgr)
{
  _src.getStaticType() = StaticType(StaticType::NUMERIC_TYPE, 0, 1);
}

Sequence FunctionCeiling::createSequence(DynamicContext* context, int flags) const
{
  Numeric::Ptr numericArg = getNumericParam(1, context);
    
  //If the argument is the empty sequence, the empty sequence is returned.
  if(numericArg.isNull()) {
    return Sequence(context->getMemoryManager());
  }

  if(numericArg->isNaN() || numericArg->isInfinite())
    return Sequence(numericArg, context->getMemoryManager());
  return Sequence(numericArg->ceiling(context), context->getMemoryManager());
}
