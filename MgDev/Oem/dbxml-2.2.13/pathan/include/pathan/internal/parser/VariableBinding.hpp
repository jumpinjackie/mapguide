/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
  Utility class used by FOR and SOME/EVERY
*/

#ifndef _VARIABLEBINDING_HPP
#define _VARIABLEBINDING_HPP

#include <pathan/Pathan.hpp>
#include <xercesc/util/XercesDefs.hpp>

class DataItem;

class PATHAN_EXPORT VariableBinding 
{
public:
  VariableBinding(const XMLCh* const variable, DataItem* allValues);

  const XMLCh *getVariable() const;
  const DataItem *getValue() const;
  DataItem *getValue();

  void setValue(DataItem *item);

private:
  const XMLCh* _variable;
  DataItem* _allValues;
};

#endif    
