/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
  VarStoreImpl - Implementation of the variable store class
*/

#ifndef _VARSTOREIMPL_HPP
#define _VARSTOREIMPL_HPP

#include <pathan/Pathan.hpp>

#include <pathan/VariableStore.hpp>
#include <pathan/simpleVariables/VariableStoreTemplate.hpp>
#include <pathan/simpleVariables/Scope.hpp>
#include <pathan/simpleVariables/VarHashEntry.hpp>
#include <pathan/Sequence.hpp>
#include <xercesc/util/StringPool.hpp>

class XPath2MemoryManager;

/** implementation of the variable store. */
class PATHAN_EXPORT VarStoreImpl : public VariableStore
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
              const StaticContext* context);
  void setVar(const XMLCh* namespaceURI,
              const XMLCh* name,
              const Sequence &value,
              const StaticContext* context); 
  /** Declares and/or sets a variable in the global scope. */
  void setGlobalVar(const XMLCh* ident,
                    const Sequence &value,
                    const StaticContext* context);
  void setGlobalVar(const XMLCh* namespaceURI,
                    const XMLCh* name,
                    const Sequence &value,
                    const StaticContext* context);


  ///Declares a var in the top level scope
  void declareVar(const XMLCh* ident,
                  const Sequence &value,
                  const StaticContext* context);
  void declareVar(const XMLCh* namespaceURI,
                  const XMLCh* name,
                  const Sequence &value,
                  const StaticContext* context);


  /** Looks up the value of a variable in the current scope.
      Returns a boolean (true if successful), and the Sequence
      value of the variable*/
  const std::pair<bool, Sequence> getVar(const XMLCh* ident,
                              const StaticContext* context) const;
  const std::pair<bool, Sequence> getVar(const XMLCh* namespaceURI,
                              const XMLCh* name,
                              const StaticContext* context) const;
  /** Looks up the value of a variable in the current scope.
      Returns the VariableStore::Entry for the variable, or null*/
  VariableStore::Entry* getReferenceVar(const XMLCh* ident,
                               const StaticContext* context) const;
  VariableStore::Entry* getReferenceVar(const XMLCh* namespaceURI,
                               const XMLCh* name,
                               const StaticContext* context) const;
  /** Looks up the value of a variable in the current scope.
      Returns a boolean (true if successful), and the Sequence
      value of the variable*/
  const std::pair<bool, Sequence> getGlobalVar(const XMLCh* ident,
                                    const StaticContext* context) const;
  const std::pair<bool, Sequence> getGlobalVar(const XMLCh* namespaceURI,
                                    const XMLCh* name,
                                    const StaticContext* context) const;


  /** Deletes a variable from the current scope. */
  void delVar(const XMLCh* ident,
              const StaticContext* context);
  void delVar(const XMLCh* namespaceURI,
              const XMLCh* name,
              const StaticContext* context);
  /** Deletes a variable from the global scope. */
  void delGlobalVar(const XMLCh* ident,
                    const StaticContext* context);
  void delGlobalVar(const XMLCh* namespaceURI,
                    const XMLCh* name,
                    const StaticContext* context); 

  /** Gives human readable representation of the variable store */
  XMLCh* print(XPath2MemoryManager* memMgr) const;

  /** Returns a vector with the names of the variable currently in scope */
  std::vector< std::pair<const XMLCh*, const XMLCh*> > getVars() const;

private:
  VariableStoreTemplate<Sequence> _store;
};

#endif // _VARSTOREIMPL_HPP



