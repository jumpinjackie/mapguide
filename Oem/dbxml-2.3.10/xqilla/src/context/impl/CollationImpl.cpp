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
 * $Id: CollationImpl.cpp,v 1.10 2006/11/17 18:08:07 jpcs Exp $
 */

#include "../../config/xqilla_config.h"
#include <xqilla/context/impl/CollationImpl.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/context/ItemFactory.hpp>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CollationImpl::CollationImpl(XPath2MemoryManager* memMgr, CollationHelper* helper)
{
  _memMgr=memMgr;
    _helper=helper;
}

const XMLCh* CollationImpl::getCollationName() const
{
    return _helper->getCollationName();
}

int CollationImpl::compare(const XMLCh* const string1, const XMLCh* const string2) const
{
    return _helper->compare(string1,string2);
}
    
Sequence CollationImpl::sort(Sequence data, const DynamicContext* context) const
{
  // build a sequence made of strings
  Sequence stringSeq = Sequence(data.getLength(), context->getMemoryManager());
  for(Sequence::iterator it=data.begin(); it!=data.end(); ++it) {
    const XMLCh *str = (*it)->asString(context);
    stringSeq.addItem(context->getItemFactory()->createString(str, context).get());
  }

  stringSeq.sortWithCollation(this, context);

  return stringSeq;
}
