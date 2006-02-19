/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
  Factory template class
*/

#ifndef _FUNCFACTORYTEMPLATE_HPP
#define _FUNCFACTORYTEMPLATE_HPP

#include <pathan/Pathan.hpp>

#include <pathan/internal/factory/FuncFactory.hpp>

///Macro used to facilitate the creation of functions
template<class TYPE>
class FuncFactoryTemplate : public FuncFactory
{
public:

  FuncFactoryTemplate()
  {
  }

  virtual DataItem *createInstance(const VectorOfDataItems &args, XPath2MemoryManager* memMgr) const
  {
	  return new (memMgr) TYPE(args, memMgr);
  }

  virtual const XMLCh *getName() const
  {
    return TYPE::name;
  }

  virtual const XMLCh *getURI() const
  {
    return TYPE::XMLChFunctionURI;
  }

  virtual unsigned int getMinArgs() const
  {
    return TYPE::minArgs;
  }

  virtual unsigned int getMaxArgs() const
  {
    return TYPE::maxArgs;
  }
};

#endif
