/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
  Scope
*/

#ifndef _SCOPE_HPP
#define _SCOPE_HPP

#include <pathan/Pathan.hpp>
#include <vector>
#include <xercesc/util/RefHash2KeysTableOf.hpp>

template<class TYPE> class VarHashEntry;

/** used inside VariableStore to implement variable scoping */
template<class TYPE>
class Scope
{
public:
  /** enum for classifying type of scope */
  typedef enum {
    GLOBAL_SCOPE,
    LOCAL_SCOPE,
    LOGICAL_BLOCK_SCOPE
  } Type;

  typedef XERCES_CPP_NAMESPACE_QUALIFIER RefHash2KeysTableOf< VarHashEntry<TYPE> > VarHash;

  /** constructor. */
  Scope(XPath2MemoryManager* memMgr, Type type);
  virtual ~Scope() {};

  virtual void clear();

  virtual Type getType() const;
  virtual VarHashEntry<TYPE>* get(unsigned int nsID, const XMLCh* name);
  virtual void put(unsigned int nsID, const XMLCh* name, VarHashEntry<TYPE>* value);
  virtual void remove(unsigned int nsID, const XMLCh* name);
  virtual std::vector< std::pair<unsigned int, const XMLCh*> > getVars() const;
  virtual void release();

  virtual Scope* getNext();
  virtual void setNext(Scope* next);
  
private:
  typename Scope<TYPE>::Type _type;
  VarHash _map;
  XPath2MemoryManager* _memMgr;
  Scope<TYPE>* _next;
};

template<class TYPE>
Scope<TYPE>::Scope(XPath2MemoryManager* memMgr, typename Scope<TYPE>::Type type) :
    _map(17, true, memMgr)
{
  _memMgr=memMgr;
  _type = type;
  _next = NULL;
}

template<class TYPE>
void Scope<TYPE>::clear()
{
  _map.removeAll();
}

template<class TYPE>
typename Scope<TYPE>::Type Scope<TYPE>::getType() const
{
  return _type;
}

template<class TYPE>
VarHashEntry<TYPE>* Scope<TYPE>::get(unsigned int nsID, const XMLCh* name)
{
	return _map.get(name,nsID);
}

template<class TYPE>
void Scope<TYPE>::put(unsigned int nsID, const XMLCh* name, VarHashEntry<TYPE>* value)
{
	_map.put((void*)_memMgr->getPooledString(name),nsID,value);
}

template<class TYPE>
void Scope<TYPE>::remove(unsigned int nsID, const XMLCh* name)
{
	_map.removeKey(name,nsID);
}

template<class TYPE>
std::vector< std::pair<unsigned int, const XMLCh*> > Scope<TYPE>::getVars() const
{
    std::vector< std::pair<unsigned int, const XMLCh*> > result;
    XERCES_CPP_NAMESPACE_QUALIFIER RefHash2KeysTableOfEnumerator< VarHashEntry<TYPE> > iterator(const_cast<VarHash*>(&_map));
    while(iterator.hasMoreElements())
	{
		XMLCh* name;
		int nsID;
		iterator.nextElementKey((void*&)name, nsID);
        result.push_back(std::pair<unsigned int, const XMLCh*>(nsID,name));
	}
	return result;
}

template<class TYPE>
void Scope<TYPE>::release()
{
  _map.removeAll();
  _memMgr->deallocate(this);
}

template<class TYPE>
Scope<TYPE>* Scope<TYPE>::getNext()
{
    return _next;
}

template<class TYPE>
void Scope<TYPE>::setNext(Scope<TYPE>* next)
{
    _next=next;
}

#endif // _SCOPE_HPP
