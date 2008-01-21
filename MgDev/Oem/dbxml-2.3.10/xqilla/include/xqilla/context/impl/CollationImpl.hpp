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
 * $Id: CollationImpl.hpp,v 1.8 2006/11/01 16:37:12 jpcs Exp $
 */

#if !defined(AFXQ_COLLATIONIMPL_H__6BA76C4A_0A5B_480B_9870_86A89A118100__INCLUDED_)
#define AFXQ_COLLATIONIMPL_H__6BA76C4A_0A5B_480B_9870_86A89A118100__INCLUDED_

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/context/Collation.hpp>
#include <xqilla/runtime/Sequence.hpp>

class Item;
class XPath2MemoryManager;
class DynamicContext;

class XQILLA_API CollationHelper
{
public:
    virtual ~CollationHelper() {};
  virtual const XMLCh* getCollationName() const = 0;
  virtual int compare(const XMLCh* string1, const XMLCh* string2) const = 0;
};

class XQILLA_API CollationImpl : public Collation 
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

#endif // !defined(AFXQ_COLLATIONIMPL_H__6BA76C4A_0A5B_480B_9870_86A89A118100__INCLUDED_)

