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
 * $Id: VarHashEntry.hpp,v 1.8 2006/11/01 16:37:12 jpcs Exp $
 */

#ifndef _VARHASHENTRY_HPP
#define _VARHASHENTRY_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xercesc/util/XMemory.hpp>

/** The class that stores the values of the variables. */
template<class TYPE>
class VarHashEntry : public XERCES_CPP_NAMESPACE_QUALIFIER XMemory
{
public:
  virtual ~VarHashEntry() {}

  /** Gets the value of the variable (overload in derived classes for
      special behaviour) */
  virtual const TYPE &getValue() const = 0;

  /** Sets the value of the variable (overload in derived classes for
      special behaviour) */
  virtual void setValue(const TYPE &value) = 0;
};

#endif

