//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifndef MG_FEATURE_SERVICE_EXCEPTION_DEF_H
#define MG_FEATURE_SERVICE_EXCEPTION_DEF_H

#include "MapGuideCommon.h"
#include "Fdo.h"


#define MG_FEATURE_SERVICE_TRY()                                              \
    MG_TRY()                                                                  \

#define MG_FEATURE_SERVICE_CATCH(methodName)                                  \
    }                                                                         \
    catch (FdoException* e)                                                   \
    {                                                                         \
        STRING messageId;                                                     \
        MgStringCollection arguments;                                         \
        wchar_t* buf = (wchar_t*)e->GetExceptionMessage();                    \
                                                                              \
        if (NULL != buf)                                                      \
        {                                                                     \
            messageId = L"MgFormatInnerExceptionMessage";                     \
            arguments.Add(buf);                                               \
        }                                                                     \
                                                                              \
        FDO_SAFE_RELEASE(e);                                                  \
        mgException = new MgFdoException(methodName, __LINE__, __WFILE__, NULL, messageId, &arguments); \
                                                                              \
    MG_CATCH(methodName)                                                      \

#define MG_FEATURE_SERVICE_THROW()                                            \
    MG_THROW()                                                                \

#define MG_FEATURE_SERVICE_CATCH_AND_THROW(methodName)                        \
    MG_FEATURE_SERVICE_CATCH(methodName)                                      \
                                                                              \
    MG_FEATURE_SERVICE_THROW()                                                \

#endif
