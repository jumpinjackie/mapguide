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
 * $Id: VariableTypeStore.hpp,v 1.8 2006/11/01 16:37:12 jpcs Exp $
 */

#ifndef _VARIABLETYPESTORE_HPP
#define _VARIABLETYPESTORE_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/util/XMemory.hpp>

class SequenceType;
class XPath2MemoryManager;
class StaticResolutionContext;

/** This is the wrapper class for the variable store, which implements the 
    lookup and scoping of simple variables. */
class XQILLA_API VariableTypeStore : public XERCES_CPP_NAMESPACE_QUALIFIER XMemory
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
