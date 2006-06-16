/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _FLOOKUPIMPL_HPP
#define _FLOOKUPIMPL_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/FunctionLookup.hpp>
#include <xercesc/util/RefHash2KeysTableOf.hpp>
#include <xercesc/util/StringPool.hpp>

/** class implementing a lookup table for functions */
class PATHAN_EXPORT FunctionLookupImpl : public FunctionLookup
{
public:
  FunctionLookupImpl(XPath2MemoryManager* memMgr);
  virtual ~FunctionLookupImpl();

  ///insert a new function factory
  virtual void insertFunction(FuncFactory *func);

  ///returns the approriate Function object
  virtual DataItem* lookUpFunction(const XMLCh* URI, const XMLCh* fname, const VectorOfDataItems &args, XPath2MemoryManager* memMgr) const;

  /// returns all the defined functions
  virtual std::vector< std::pair<const XMLCh*,const XMLCh*> > getFunctions() const;
  virtual std::vector< FuncFactory* > getFunctionFactories() const;

protected:
  ///function to create the table
  void createTable();

private:
  XERCES_CPP_NAMESPACE_QUALIFIER XMLStringPool _uriPool;
  XERCES_CPP_NAMESPACE_QUALIFIER RefHash2KeysTableOf< FuncFactory > _funcTable;
  XPath2MemoryManager* _memMgr;
};




#endif 
