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
 * $Id: VarTypeStoreImpl.hpp,v 1.7 2006/11/01 16:37:18 jpcs Exp $
 */

/*
  VarStoreImpl - Implementation of the variable store class
*/

#ifndef _VARTYPESTOREIMPL_HPP
#define _VARTYPESTOREIMPL_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/context/VariableTypeStore.hpp>
#include <xqilla/context/impl/VariableStoreTemplate.hpp>
#include <xqilla/context/Scope.hpp>
#include <xqilla/context/VarHashEntry.hpp>
#include <xqilla/framework/StringPool.hpp>

class XPath2MemoryManager;

/** implementation of the variable type store. */
class XQILLA_API VarTypeStoreImpl : public VariableTypeStore
{
public:
  /** constructor. automatically creates a global store for the
      global variables. */
  VarTypeStoreImpl(XPath2MemoryManager* memMgr);
  /// destructor
  ~VarTypeStoreImpl();

  /** Clears all variable values and added scopes from the store */
  virtual void clear();

  /** Adds a new scope to the store. Add a LOCAL_SCOPE scope at the
      beginning of *procedural* methods, and a LOGICAL_BLOCK_SCOPE
      scope at the beginning of logical block methods. */
  virtual void addLocalScope();
  virtual void addLogicalBlockScope();
  /** Removes the top level scope from the store. To be called at
      the end of methods to implement scoping. */
  virtual void removeScope();

  /** Declares and/or sets a variable in the global scope. */
  virtual void declareGlobalVar(const XMLCh* namespaceURI,
                                const XMLCh* name,
                                const StaticResolutionContext &src);

  /** Looks up the value of a variable in the current scope.
      Returns a boolean (true if successful), and the SequenceType
      value of the variable*/
  virtual const StaticResolutionContext* getGlobalVar(const XMLCh* namespaceURI,
                                                      const XMLCh* name) const;

  ///Declares a var in the top level scope
  virtual void declareVar(const XMLCh* namespaceURI,
                          const XMLCh* name,
                          const StaticResolutionContext &src);

  /** Looks up the value of a variable in the current scope.
      Returns a boolean (true if successful), and the SequenceType
      value of the variable*/
  virtual const StaticResolutionContext* getVar(const XMLCh* namespaceURI,
                                                const XMLCh* name) const;

private:
  VariableStoreTemplate<const StaticResolutionContext*> _store;
};

#endif
