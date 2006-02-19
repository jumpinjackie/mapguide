/*
 * Copyright (c) 2003, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _XMLCHCOMPARE_HPP
#define _XMLCHCOMPARE_HPP

#include <pathan/Pathan.hpp>
#include <algorithm>
#include <xercesc/util/XercesDefs.hpp>

class PATHAN_EXPORT QNameSort {
public:
    bool operator() (const std::pair<XMLCh*, XMLCh *> lhs, const std::pair<XMLCh*, XMLCh *> rhs) const;
};

class PATHAN_EXPORT XMLChSort {
public:
    bool operator() (const XMLCh* lhs, const XMLCh* rhs) const;
};

#endif
