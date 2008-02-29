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
 * $Id: FunctionLookupImpl.hpp,v 1.9.2.1 2007/01/12 18:04:46 gmfeinberg Exp $
 */

#ifndef _FLOOKUPIMPL_HPP
#define _FLOOKUPIMPL_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/functions/FunctionLookup.hpp>
#include <xercesc/util/RefHash2KeysTableOf.hpp>
#include <xercesc/util/StringPool.hpp>

class XPath2MemoryManagerImpl;

/** class implementing a lookup table for functions */
class XQILLA_API FunctionLookupImpl : public FunctionLookup
{
public:
  FunctionLookupImpl(XPath2MemoryManager* memMgr, bool addGlobals = false);
  virtual ~FunctionLookupImpl();

  ///insert a new function factory
  virtual void insertFunction(FuncFactory *func);

  /// replaces the implementation of an existing function
  virtual void replaceFunction(FuncFactory *func);

  ///returns the approriate Function object
  virtual ASTNode* lookUpFunction(const XMLCh* URI, const XMLCh* fname, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr) const;

  /// returns all the defined functions
  virtual std::vector< std::pair<const XMLCh*,const XMLCh*> > getFunctions() const;
  virtual std::vector< FuncFactory* > getFunctionFactories() const;

public:
  static void initialize();
  static void terminate();

protected:
  ///function to create the table
  void createTable(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr);

private:  
  static FunctionLookupImpl *g_globalFunctionTable;
  static XPath2MemoryManagerImpl *g_memMgr;

private:
  XERCES_CPP_NAMESPACE_QUALIFIER RefHash2KeysTableOf< FuncFactory > _funcTable;
};




#endif 
