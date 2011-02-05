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

#ifndef MG_EXCEPTION_DEFS_H_
#define MG_EXCEPTION_DEFS_H_


#define DECLARE_EXCEPTION_ABSTRACT(className) \
    virtual INT32 GetClassId(); \
    virtual void Raise() = 0; \
    protected: \
    className() throw();

#define IMPLEMENT_EXCEPTION_ABSTRACT(className) \
    className::className() throw() {} \
    INT32 className::GetClassId() { return m_cls_id; }

#define DECLARE_EXCEPTION_DEFAULTS(className) \
    static MgObject* CreateObject(); \
    virtual INT32 GetClassId(); \
    virtual void Raise(); \
    protected: \
    className() throw();

#define IMPLEMENT_EXCEPTION_DEFAULTS(className, parentClassName) \
    className::className() throw() {} \
    MgObject* className::CreateObject() { return new className(); } \
    INT32 className::GetClassId() { return m_cls_id; } \
    void className::Raise() { throw this; } \


////////////////////////////////////////////////////////////////////////////////////////////
/// MapGuide try/catch/throw macros.
///
#define MG_TRY()                                                              \
    Ptr<MgException> mgException;                                             \
    STRING mgStackParams;                                                     \
                                                                              \
    try                                                                       \
    {                                                                         \

#define MG_CATCH(methodName)                                                  \
    }                                                                         \
    catch (MgException* e)                                                    \
    {                                                                         \
        mgException = e;                                                      \
        mgException->AddStackTraceInfo(methodName, mgStackParams, __LINE__, __WFILE__);      \
    }                                                                         \
    catch (exception& e)                                                      \
    {                                                                         \
        mgException = MgSystemException::Create(e, methodName, __LINE__, __WFILE__); \
    }                                                                         \
    catch (...)                                                               \
    {                                                                         \
        mgException = new MgUnclassifiedException(methodName, __LINE__, __WFILE__, NULL, L"", NULL); \
    }                                                                         \

#define MG_THROW()                                                            \
    if (mgException != NULL)                                                  \
    {                                                                         \
        (*mgException).AddRef();                                              \
        mgException->Raise();                                                 \
    }                                                                         \

#define MG_CATCH_AND_THROW(methodName)                                        \
    MG_CATCH(methodName)                                                      \
                                                                              \
    MG_THROW()                                                                \

#define MG_CATCH_AND_RELEASE()                                                \
    }                                                                         \
    catch (MgException* e)                                                    \
    {                                                                         \
        mgException = e;                                                      \
    }                                                                         \
    catch (...)                                                               \
    {                                                                         \
    }                                                                         \

#endif
