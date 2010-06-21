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
 * $Id: UTF8Str.hpp,v 1.7 2006/11/01 16:37:17 jpcs Exp $
 */

#ifndef _UTF8STR_HPP
#define _UTF8STR_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xercesc/util/XercesDefs.hpp>

#include <xercesc/util/XMLUTF8Transcoder.hpp>

class XQILLA_API UTF8Str
{
public :
    UTF8Str(const XMLCh* const toTranscode);
    ~UTF8Str()
    {
        delete [] fUTF8Form;
    }

    const char* UTF8Form() const
    {
        return (char*)fUTF8Form;
    }

private :
    XMLByte *fUTF8Form;
};

#define UTF8(str) UTF8Str(str).UTF8Form()

#endif
