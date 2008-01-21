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
 * $Id: VarStoreImpl.hpp,v 1.10 2006/11/01 16:37:18 jpcs Exp $
 */

/*
  VarStoreImpl - Implementation of the variable store class
*/

#ifndef _VARSTOREIMPL_HPP
#define _VARSTOREIMPL_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/context/VariableStore.hpp>
#include <xqilla/context/impl/VariableStoreTemplate.hpp>
#include <xqilla/context/Scope.hpp>
#include <xqilla/context/VarHashEntry.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/framework/StringPool.hpp>

class XPath2MemoryManager;

/** implementation of the variable store. */
class XQILLA_API VarStoreImpl : public VariableStore
{
public:
  /** constructor. automatically creates a global store for the
      global variables. */
  VarStoreImpl(XPath2MemoryManager* memMgr);
  /// destructor
  ~VarStoreImpl();

  /** Clears all variable values and added scopes from the store */
  void clear();

  /** Adds a new scope to the store. Add a LOCAL_SCOPE scope at the
      beginning of *procedural* methods, and a LOGICAL_BLOCK_SCOPE
      scope at the beginning of logical block methods. */
  void addLocalScope();
  void addLogicalBlockScope();
  /** Removes the top level scope from the store. To be called at
      the end of methods to implement scoping. */
  void removeScope();

  /// Returns an encapsulation of the state of the variable store.
  VariableStore::MyScope *getScopeState();
  /// Sets the variable store to a previous state.
  void setScopeState(VariableStore::MyScope *state);

  /** Declares and/or sets a variable in the top level scope. */
  void setVar(const XMLCh* ident,
              const Sequence &value,
              const StaticContext* context,
              const LocationInfo *location);
  void setVar(const XMLCh* namespaceURI,
              const XMLCh* name,
              const Sequence &value,
              const StaticContext* context); 
  /** Declares and/or sets a variable in the global scope. */
  void setGlobalVar(const XMLCh* ident,
                    const Sequence &value,
                    const StaticContext* context,
                    const LocationInfo *location);
  void setGlobalVar(const XMLCh* namespaceURI,
                    const XMLCh* name,
                    const Sequence &value,
                    const StaticContext* context);


  ///Declares a var in the top level scope
  void declareVar(const XMLCh* ident,
                  const Sequence &value,
                  const StaticContext* context,
                  const LocationInfo *location);
  void declareVar(const XMLCh* namespaceURI,
                  const XMLCh* name,
                  const Sequence &value,
                  const StaticContext* context);


  /** Looks up the value of a variable in the current scope.
      Returns a boolean (true if successful), and the Sequence
      value of the variable*/
  const std::pair<bool, Sequence> getVar(const XMLCh* ident,
                                         const StaticContext* context,
                                         const LocationInfo *location) const;
  const std::pair<bool, Sequence> getVar(const XMLCh* namespaceURI,
                              const XMLCh* name,
                              const StaticContext* context) const;
  /** Looks up the value of a variable in the current scope.
      Returns the VariableStore::Entry for the variable, or null*/
  VariableStore::Entry* getReferenceVar(const XMLCh* ident,
                                        const StaticContext* context,
                                        const LocationInfo *location) const;
  VariableStore::Entry* getReferenceVar(const XMLCh* namespaceURI,
                               const XMLCh* name,
                               const StaticContext* context) const;
  /** Looks up the value of a variable in the current scope.
      Returns a boolean (true if successful), and the Sequence
      value of the variable*/
  const std::pair<bool, Sequence> getGlobalVar(const XMLCh* ident,
                                               const StaticContext* context,
                                               const LocationInfo *location) const;
  const std::pair<bool, Sequence> getGlobalVar(const XMLCh* namespaceURI,
                                    const XMLCh* name,
                                    const StaticContext* context) const;


  /** Deletes a variable from the current scope. */
  void delVar(const XMLCh* ident,
              const StaticContext* context,
              const LocationInfo *location);
  void delVar(const XMLCh* namespaceURI,
              const XMLCh* name,
              const StaticContext* context);
  /** Deletes a variable from the global scope. */
  void delGlobalVar(const XMLCh* ident,
                    const StaticContext* context,
                    const LocationInfo *location);
  void delGlobalVar(const XMLCh* namespaceURI,
                    const XMLCh* name,
                    const StaticContext* context); 

  /** Gives human readable representation of the variable store */
  XMLCh* print(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr) const;

  /** Returns a vector with the names of the variable currently in scope */
  std::vector< std::pair<const XMLCh*, const XMLCh*> > getVars() const;

  const XMLCh* getVariableNsURI(const XMLCh* qName, const StaticContext* context, const LocationInfo *location) const;

private:
  VariableStoreTemplate<Sequence> _store;
};

#endif // _VARSTOREIMPL_HPP



