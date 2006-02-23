/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _DATAITEMOPERATOR_HPP
#define _DATAITEMOPERATOR_HPP

#include <pathan/dataItem/DataItemImpl.hpp>
#include <pathan/Sequence.hpp>
#include <pathan/Node.hpp>
#include <xercesc/util/XMLUniDefs.hpp>      // because every implementation will use these to define the function name

class Node;

/** Virtual interface class for operator */
class PATHAN_EXPORT DataItemOperator : public DataItemImpl
{
public:

  /// constructor, checks for the correct number of arguments.
  DataItemOperator(const XMLCh* opName, const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  virtual DataItem* staticResolution(StaticContext *context);

  /** used to manipulate arguments */
  void addArgument(DataItem* arg);
  void removeArgument(unsigned int index);
  DataItem* getArgument(unsigned int index) const;
  void setArgument(unsigned int index, DataItem *arg);

  /** returns the number of parameters passed into the operator */
  unsigned int getNumArgs() const;

  virtual const XMLCh* getOperatorName() const;
  const VectorOfDataItems &getArguments() const;
  
  /** Helper function for node operators */
  Node::Ptr getNodeParam(unsigned int index,DynamicContext* context) const;

protected:
  /** Helper function for operators on sequences of nodes */
  static bool checkSequenceIsNodes(const Sequence &s);

  VectorOfDataItems _args; // The real store for arguments

  const XMLCh* _opName;
};

#endif // _DATAITEMOPERATOR_HPP
