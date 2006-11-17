/*
 * Copyright (c) 2003, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#include "../config/pathan_config.h"
#include <pathan/internal/DOMutils/XMLChCompare.hpp>
#include <pathan/XPath2Utils.hpp>
#include <xercesc/util/XMLString.hpp>

bool QNameSort::operator() (const std::pair<XMLCh*, XMLCh *> lhs, const std::pair<XMLCh*, XMLCh *> rhs) const {
    return XPath2Utils::equals(lhs.first, rhs.first) ? XERCES_CPP_NAMESPACE_QUALIFIER XMLString::compareString(lhs.second, rhs.second) < 0: XERCES_CPP_NAMESPACE_QUALIFIER XMLString::compareString(lhs.first, rhs.first) < 0;
}

bool XMLChSort::operator() (const XMLCh* lhs, const XMLCh* rhs) const {
    return XERCES_CPP_NAMESPACE_QUALIFIER XMLString::compareString(lhs, rhs) < 0;
}
