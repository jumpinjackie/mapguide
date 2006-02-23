/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/internal/parser/VariableBinding.hpp>
#include <xercesc/util/XMLString.hpp>

VariableBinding::VariableBinding(const XMLCh* const variable, DataItem* allValues)
: _variable(variable),
  _allValues(allValues)
{
}

const XMLCh *VariableBinding::getVariable() const {
  return _variable;
}

const DataItem *VariableBinding::getValue() const {
  return _allValues;
}

DataItem *VariableBinding::getValue()
{
  return _allValues;
}

void VariableBinding::setValue(DataItem *item) {
  _allValues = item;
}
