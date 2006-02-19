/*
 * Copyright (c) 2004, Parthenon Computing Limited, All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/ReferenceCounted.hpp>
#include <pathan/XPath2MemoryManager.hpp>

// ReferenceCounted::ReferenceCounted(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm)
//   : _ref_count(0),
//     _ref_mm(mm)
// {
//   // Do nothing
// }

// ReferenceCounted::~ReferenceCounted()
// {
//   // Do nothing
// }

// void ReferenceCounted::incrementRefCount() const
// {
//   ++const_cast<unsigned int&>(_ref_count);
// }

// void ReferenceCounted::decrementRefCount() const
// {
//   if(--const_cast<unsigned int&>(_ref_count) == 0) {
//     if(_ref_mm == 0) {
//       delete this;
//     }
//     else {
//       this->~ReferenceCounted();
//       operator delete((void*)this, _ref_mm);
//     }
//   }
// }
