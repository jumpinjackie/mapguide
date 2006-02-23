/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _VARHASHENTRY_HPP
#define _VARHASHENTRY_HPP

#include <pathan/Pathan.hpp>

/** The class that stores the values of the variables. */
template<class TYPE>
class VarHashEntry
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

