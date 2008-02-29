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
 * $Id: VariableStore.hpp,v 1.10 2006/11/01 16:37:12 jpcs Exp $
 */

#ifndef _VARIABLESTORE_HPP
#define _VARIABLESTORE_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <vector>
#include <xercesc/util/XMemory.hpp>

class Sequence;
template<class TYPE> class VarHashEntry;
template<class TYPE> class Scope;
class StaticContext;
class XPath2MemoryManager;
class LocationInfo;

/** This is the wrapper class for the variable store, which implements the 
    lookup and scoping of simple variables. */
class XQILLA_API VariableStore : public XERCES_CPP_NAMESPACE_QUALIFIER XMemory
{
public:
  typedef Scope<Sequence> MyScope;
  typedef VarHashEntry<Sequence> Entry;

  /** default destructor */
  virtual ~VariableStore() {};

  /** Clears all variable values and added scopes from the store */
  virtual void clear() = 0;

  /** Adds a new local scope to the store. */
  virtual void addLocalScope() = 0;
  /** Adds a new logical block scope to the store. */
  virtual void addLogicalBlockScope() = 0;
  /** Removes the top level scope from the store. To be called at the end of methods to
      implement scoping. */
  virtual void removeScope() = 0;

  /// Returns an encapsulation of the state of the variable store.
  virtual MyScope *getScopeState() = 0;
  /// Sets the variable store to a previous state.
  virtual void setScopeState(MyScope *state) = 0;

  /** Declares and/or sets a variable in the global scope. */
  virtual void setGlobalVar(const XMLCh* ident, const Sequence &value,
                            const StaticContext* context,
                            const LocationInfo *location) = 0;
  virtual void setGlobalVar(const XMLCh* namespaceURI, const XMLCh* name, const Sequence &value,
		    const StaticContext* context) = 0;

  /** Declares and/or sets a variable in the top level scope. */
  virtual void setVar(const XMLCh* ident, const Sequence &value,
                      const StaticContext* context,
                      const LocationInfo *location) = 0;
  virtual void setVar(const XMLCh* namespaceURI, const XMLCh* name, const Sequence &value,
	      const StaticContext* context) = 0;

  /** Declare a var in the top level scope (A full set of
      these namespaceURI/name pair methods should be made) */
  virtual void declareVar(const XMLCh* ident, 
                          const Sequence &value,
                          const StaticContext* context,
                          const LocationInfo *location) = 0;
  virtual void declareVar(const XMLCh* namespaceURI,
                  const XMLCh* name,
                  const Sequence &value,
                  const StaticContext* context) = 0;

  /** Looks up the value of a variable in the current scope, using ident as an
      qname. Returns a boolean (true if successful), and the Sequence value
      of the variable*/
  virtual const std::pair<bool, Sequence> getVar(const XMLCh* ident,
                                                 const StaticContext* context,
                                                 const LocationInfo *location) const = 0;
  virtual const std::pair<bool, Sequence> getVar(const XMLCh* namespaceURI,
                                    const XMLCh* name,
                                    const StaticContext* context) const = 0;

  /** Looks up the value of a variable in the current scope, using ident as an
      qname. Returns the VariableStore::Entry for the variable, or null if it doesn't
      exist*/
  virtual Entry* getReferenceVar(const XMLCh* ident,
                                 const StaticContext* context,
                                 const LocationInfo *location) const = 0;
  virtual Entry* getReferenceVar(const XMLCh* namespaceURI,
                                    const XMLCh* name,
                                    const StaticContext* context) const = 0;

  /** Gets a variable from the global scope */
  virtual const std::pair<bool, Sequence> getGlobalVar(const XMLCh* ident,
                                                       const StaticContext* context,
                                                       const LocationInfo *location) const = 0;
  virtual const std::pair<bool, Sequence> getGlobalVar(const XMLCh* namespaceURI,
                                    const XMLCh* name,
                                    const StaticContext* context) const = 0;

  /** Deletes a variable from the current scope. */
  virtual void delVar(const XMLCh* ident, const StaticContext* context,
                      const LocationInfo *location) = 0;
  virtual void delVar(const XMLCh* namespaceURI, const XMLCh* name, const StaticContext* context) = 0;

  /** Deletes a variable from the global scope. */
  virtual void delGlobalVar(const XMLCh* ident, const StaticContext* context,
                            const LocationInfo *location) = 0;

  /** Gives human readable representation of the variable store */
  virtual XMLCh* print(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr) const = 0;

  /** Returns a vector with the names of the variable currently in scope */
  virtual std::vector< std::pair<const XMLCh*, const XMLCh*> > getVars() const = 0;
};

#endif
