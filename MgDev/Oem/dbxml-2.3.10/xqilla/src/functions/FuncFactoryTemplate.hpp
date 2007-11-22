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
 * $Id: FuncFactoryTemplate.hpp,v 1.8.2.1 2007/01/12 18:04:46 gmfeinberg Exp $
 */

/*
  Factory template class
*/

#ifndef _FUNCFACTORYTEMPLATE_HPP
#define _FUNCFACTORYTEMPLATE_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/functions/FuncFactory.hpp>

///Macro used to facilitate the creation of functions
template<class TYPE>
class FuncFactoryTemplate : public FuncFactory
{
public:

  FuncFactoryTemplate(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr)
		: qname(1023, memMgr)
  {
    qname.set(getName());
    qname.append(getURI());
  }

  virtual ASTNode *createInstance(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr) const
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

  virtual const XMLCh *getQName() const
  {
    return qname.getRawBuffer();
  }
	
  virtual unsigned int getMinArgs() const
  {
    return TYPE::minArgs;
  }

  virtual unsigned int getMaxArgs() const
  {
    return TYPE::maxArgs;
  }

private:
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer qname;
};

#endif
