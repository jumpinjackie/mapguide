//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

#ifndef MG_HTTP_UTIL_H
#define MG_HTTP_UTIL_H

// Helper Macros

#define MG_HTTP_HANDLER_TRY()                                                 \
    MG_TRY()                                                                  \

#define MG_HTTP_HANDLER_CATCH(methodName)                                     \
    MG_CATCH(methodName)                                                      \
    MgHttpUtil::LogException(mgException);                                    \

#define MG_HTTP_HANDLER_THROW()                                               \
    MgHttpUtil::LogException(mgException);                                    \
    MG_THROW()                                                                \

#define MG_HTTP_HANDLER_CATCH_AND_THROW(methodName)                           \
    MG_CATCH(methodName)                                                      \
    MgHttpUtil::LogException(mgException);                                    \
    MG_THROW()                                                                \

#define MG_HTTP_HANDLER_CATCH_AND_THROW_EX(methodName)                        \
    MG_CATCH(methodName)                                                      \
    MgHttpUtil::LogException(mgException);                                    \
                                                                              \
    if (mgException != NULL)                                                  \
    {                                                                         \
        if (hResult != NULL)                                                  \
        {                                                                     \
            hResult->SetErrorInfo(m_hRequest, mgException);                   \
        }                                                                     \
                                                                              \
        (*mgException).AddRef();                                              \
        mgException->Raise();                                                 \
    }                                                                         \

#define MG_HTTP_HANDLER_CATCH_AND_THROW_EX2(methodName, hResult, hRequest)    \
    MG_CATCH(methodName)                                                      \
    MgHttpUtil::LogException(mgException);                                    \
                                                                              \
    if (mgException != NULL)                                                  \
    {                                                                         \
        if (hResult != NULL)                                                  \
        {                                                                     \
            hResult->SetErrorInfo(hRequest, mgException);                     \
        }                                                                     \
                                                                              \
        (*mgException).AddRef();                                              \
        mgException->Raise();                                                 \
    }                                                                         \

class MgHttpUtil
{
public:
    MgHttpUtil();
    ~MgHttpUtil();

    static void LogException(MgException* exception);
};

#endif
