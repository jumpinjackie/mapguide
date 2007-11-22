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
 * $Id: ModuleResolver.hpp,v 1.2 2006/11/01 16:37:12 jpcs Exp $
 */

#ifndef _MODULERESOLVER_HPP
#define _MODULERESOLVER_HPP

#include <xqilla/framework/XQillaExport.hpp>

class DynamicContext;

class XQILLA_API ModuleResolver {
  public:
    /* virtual destructor, does nothing */
    virtual ~ModuleResolver() {};      

    /* Fills the string vector with a list of locations tht, once assembled, build the module referenced by the given URI.
       If the URI is unknown, returns false, otherwise returns true. */
    virtual bool resolveModuleLocation(VectorOfStrings* result, const XMLCh* nsUri, const StaticContext* context) = 0;
};
#endif // _MODULERESOLVER_HPP
