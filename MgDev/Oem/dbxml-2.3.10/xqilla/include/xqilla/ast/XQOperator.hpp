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
 * $Id: XQOperator.hpp,v 1.8 2006/11/01 16:37:11 jpcs Exp $
 */

#ifndef _XQOPERATOR_HPP
#define _XQOPERATOR_HPP

#include <xqilla/ast/ASTNodeImpl.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/items/Node.hpp>
#include <xercesc/util/XMLUniDefs.hpp>      // because every implementation will use these to define the function name

class Node;

/** Virtual interface class for operator */
class XQILLA_API XQOperator : public ASTNodeImpl
{
public:

  /// constructor, checks for the correct number of arguments.
  XQOperator(const XMLCh* opName, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

  /** used to manipulate arguments */
  void addArgument(ASTNode* arg);
  void removeArgument(unsigned int index);
  ASTNode* getArgument(unsigned int index) const;
  void setArgument(unsigned int index, ASTNode *arg);

  /** returns the number of parameters passed into the operator */
  unsigned int getNumArgs() const;

  virtual const XMLCh* getOperatorName() const;
  const VectorOfASTNodes &getArguments() const;
  
protected:
  VectorOfASTNodes _args; // The real store for arguments

  const XMLCh* _opName;
};

#endif // _XQOPERATOR_HPP
