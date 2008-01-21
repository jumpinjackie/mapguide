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
 * $Id: FunctionLookup.hpp,v 1.10 2006/11/01 16:37:13 jpcs Exp $
 */

#ifndef _FLOOKUP_HPP
#define _FLOOKUP_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <vector>
#include <xercesc/util/XercesDefs.hpp>
#include <xqilla/ast/ASTNode.hpp>

class FuncFactory;
class XPath2MemoryManager;

class XQILLA_API FunctionLookup
{
public:
  virtual ~FunctionLookup() {}

  ///adds a function to the custom function table
  virtual void insertFunction(FuncFactory *func) = 0;
  /// replaces the implementation of an existing function
  virtual void replaceFunction(FuncFactory *func) = 0;
  ///returns the approriate Function object
  virtual ASTNode* lookUpFunction(const XMLCh* URI, const XMLCh* fname, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr) const = 0;

  /// returns all the defined functions
  virtual std::vector< std::pair<const XMLCh*,const XMLCh*> > getFunctions() const = 0;
  virtual std::vector< FuncFactory* > getFunctionFactories() const = 0;
};

#endif 
