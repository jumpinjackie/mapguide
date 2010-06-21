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
 * $Id: XQFunction.hpp,v 1.10.2.1 2007/01/04 17:45:34 jpcs Exp $
 */

#ifndef _XQFUNCTION_HPP
#define _XQFUNCTION_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/ast/ASTNodeImpl.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/context/ItemFactory.hpp>

#include <xercesc/util/XMLUniDefs.hpp>      // because every implementation will use these to define the function name
#include <vector>

class SequenceType;

class XQILLA_API XQFunction : public ASTNodeImpl
{

public:
    XQFunction(const XMLCh* name, size_t argsFrom, size_t argsTo, const char* paramDecl,const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  virtual ~XQFunction();

  const XMLCh* getFunctionURI()const;
  const XMLCh* getFunctionName()const;
  const XMLCh* getFunctionSignature() const;
  const VectorOfASTNodes &getArguments() const;
  
  static const XMLCh XMLChFunctionURI[];

  /** Wraps the arguments in the appropriate conversion functions,
      calls static resolution on them, and constant folds if
      this function's StaticResolutionContext is not used */
  ASTNode *resolveArguments(StaticContext *context, bool checkTimezone = false, bool numericFunction = false);
  ASTNode *calculateSRCForArguments(StaticContext *context, bool checkTimezone = false, bool numericFunction = false);

  /** casts the expression given as a parameter into the appropriate type and returns the guaranteed correctly typed objects in a sequence  */
  virtual Result getParamNumber(size_t number, DynamicContext* context, int flags=0) const;

  /** returns the number of parameters passed into the function */
  size_t getNumArgs() const;

protected:

  /** Helper method, produces a single ATStringOrDerived from the output of getParamNumber(). If getParamNumber() returns anything other than a single string, an exception is thrown. */
  static const size_t UNLIMITED;

  //parse the supplied string of comma separated arguments into vector of SequenceTypes
  void parseParamDecl(const char* paramString, XPath2MemoryManager *mm);

  const XMLCh *_fName, *_fURI, *_signature;
  const size_t _nArgsFrom, _nArgsTo;

  std::vector<SequenceType*, XQillaAllocator<SequenceType*> > _paramDecl;

  VectorOfASTNodes _args; // The real store for arguments 

};

#endif
