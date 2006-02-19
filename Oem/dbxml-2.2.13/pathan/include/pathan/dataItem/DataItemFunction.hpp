/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _DATAITEMFUNCTION_HPP
#define _DATAITEMFUNCTION_HPP

#include "../Pathan.hpp"
#include "DataItemImpl.hpp"
#include "../Sequence.hpp"
#include "../ATStringOrDerived.hpp"
#include "../PathanFactory.hpp"

#include <xercesc/util/XMLUniDefs.hpp>      // because every implementation will use these to define the function name
#include <vector>

class SequenceType;

class PATHAN_EXPORT DataItemFunction : public DataItemImpl
{

public:
  DataItemFunction(const XMLCh* name, unsigned int argsFrom, unsigned int argsTo, const char* paramDecl, const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  virtual ~DataItemFunction();

  const XMLCh* getFunctionURI()const;
  const XMLCh* getFunctionName()const;
  const XMLCh* getFunctionSignature() const;
  const VectorOfDataItems &getArguments() const;
  
  static const XMLCh XMLChFunctionURI[];

  /** casts the expression given as a parameter into the appropriate type and returns the guaranteed correctly typed objects in a sequence  */
  virtual Result getParamNumber(unsigned int number, DynamicContext* context, int flags=0) const;

  /** returns the number of parameters passed into the function */
  unsigned int getNumArgs() const;

protected:
  
  /** casts the expression given as a parameter into the specified type and returns the guaranteed correctly typed objects in a sequence  */
  virtual Result getParamNumber(unsigned int paramNumber, unsigned int signatureNumber, DynamicContext* context, int flags=0) const;

  /** Helper method, produces a single ATStringOrDerived from the output of getParamNumber(). If getParamNumber() returns anything other than a single string, an exception is thrown. */
  ATStringOrDerived::Ptr getStringParam(unsigned int number, DynamicContext* context, int flags=0) const;

  static const unsigned int UNLIMITED;

  //parse the supplied string of comma separated arguments into vector of SequenceTypes
  static std::vector< SequenceType* >* parseParamDecl(const char* paramString);

  const XMLCh *_fName, *_fURI, *_signature;
  const unsigned int _nArgsFrom, _nArgsTo;

  const std::vector<SequenceType*>* _paramDecl;

  VectorOfDataItems _args; // The real store for arguments 

};

#endif
