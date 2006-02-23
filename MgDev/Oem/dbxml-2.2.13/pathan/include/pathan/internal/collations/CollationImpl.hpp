// CollationImpl.h: interface for the CollationImpl interface.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLLATIONIMPL_H__6BA76C4A_0A5B_480B_9870_86A89A118100__INCLUDED_)
#define AFX_COLLATIONIMPL_H__6BA76C4A_0A5B_480B_9870_86A89A118100__INCLUDED_

#include <pathan/Pathan.hpp>

#include <pathan/Collation.hpp>
#include <pathan/Sequence.hpp>

class Item;
class XPath2MemoryManager;
class DynamicContext;

class PATHAN_EXPORT CollationHelper
{
public:
    virtual ~CollationHelper() {};
  virtual const XMLCh* getCollationName() const = 0;
  virtual int compare(const XMLCh* string1, const XMLCh* string2) const = 0;
};

class PATHAN_EXPORT CollationImpl : public Collation 
{
public:
  CollationImpl(XPath2MemoryManager* memMgr, CollationHelper* helper);

  virtual const XMLCh* getCollationName() const;
  virtual Sequence sort(Sequence data, const DynamicContext* context) const;
  virtual int compare(const XMLCh* const string1, const XMLCh* const string2) const;

protected:
    XPath2MemoryManager* _memMgr;
    CollationHelper* _helper;
};

#endif // !defined(AFX_COLLATIONIMPL_H__6BA76C4A_0A5B_480B_9870_86A89A118100__INCLUDED_)

