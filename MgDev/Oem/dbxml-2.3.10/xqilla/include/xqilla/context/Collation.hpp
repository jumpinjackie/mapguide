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
 * $Id: Collation.hpp,v 1.8 2006/11/01 16:37:11 jpcs Exp $
 */

#if !defined(AFXQ_COLLATION_H__6BA76C4A_0A5B_480B_9870_86A89A118100__INCLUDED_)
#define AFXQ_COLLATION_H__6BA76C4A_0A5B_480B_9870_86A89A118100__INCLUDED_

#include <xqilla/framework/XQillaExport.hpp>
#include <xercesc/util/XercesDefs.hpp>

class Sequence;
class DynamicContext;

class XQILLA_API Collation
{
public:
  virtual ~Collation() {};

  virtual const XMLCh* getCollationName() const = 0;
  virtual Sequence sort(Sequence data, const DynamicContext* context) const = 0;
  virtual int compare(const XMLCh* const string1, const XMLCh* const string2) const = 0;

protected:
  Collation() {}
};

#endif // !defined(AFXQ_COLLATION_H__6BA76C4A_0A5B_480B_9870_86A89A118100__INCLUDED_)
