/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _VARIABLETYPESTORE_HPP
#define _VARIABLETYPESTORE_HPP

#include <pathan/Pathan.hpp>
#include <xercesc/util/XercesDefs.hpp>

class SequenceType;
class XPath2MemoryManager;
class StaticResolutionContext;

/** This is the wrapper class for the variable store, which implements the 
    lookup and scoping of simple variables. */
class PATHAN_EXPORT VariableTypeStore
{
public:
  /** default destructor */
  virtual ~VariableTypeStore() {};

  /** Clears all variable values and added scopes from the store */
  virtual void clear() = 0;

  /** Adds a new local scope to the store. */
  virtual void addLocalScope() = 0;
  /** Adds a new logical block scope to the store. */
  virtual void addLogicalBlockScope() = 0;
  /** Removes the top level scope from the store. To be called at the end of methods to
      implement scoping. */
  virtual void removeScope() = 0;

  /** Declares and/or sets a variable in the global scope. */
  virtual void declareGlobalVar(const XMLCh* namespaceURI, const XMLCh* name,
                                const StaticResolutionContext &src) = 0;

  /** Gets a variable from the global scope */
  virtual const StaticResolutionContext* getGlobalVar(const XMLCh* namespaceURI,
                                                      const XMLCh* name) const = 0;

  /** Declare a var in the top level scope (A full set of
      these namespaceURI/name pair methods should be made) */
  virtual void declareVar(const XMLCh* namespaceURI, const XMLCh* name,
                          const StaticResolutionContext &src) = 0;

  /** Looks up the value of a variable in the current scope, using ident as an
      qname. Returns a boolean (true if successful), and the SequenceType value
      of the variable*/
  virtual const StaticResolutionContext *getVar(const XMLCh* namespaceURI,
                                                const XMLCh* name) const = 0;
};

#endif
