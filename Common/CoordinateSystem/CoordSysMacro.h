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

#ifndef _MG_COORDSYSMACRO_H_
#define _MG_COORDSYSMACRO_H_

//make sure to have CS_MAP_DEF_VARIABLE defined before including this header; this is used by VERIFY_INITIALIZED(x)

#define MAKE_L_STRING(x) L## #x

#define MAKE_QUALIFIED_MEMBER_STRING(className,member) MAKE_L_STRING(className.member)

#ifdef CS_MAP_DEF_VARIABLE
    #define VERIFY_INITIALIZED(x)   if (NULL == CS_MAP_DEF_VARIABLE) \
        throw new MgInvalidOperationException(x, __LINE__, __WFILE__, NULL, L"", NULL)
#else
    #define VERIFY_INITIALIZED(x)
#endif

#define VERIFY_NOT_PROTECTED(x) if (this->IsProtected()) \
    throw new MgCoordinateSystemInitializationFailedException(x, __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL);

#define ENSURE_NOT_NULL(param,method) \
    if (NULL == param) \
        throw new MgNullArgumentException(method, __LINE__, __WFILE__, NULL, L"", NULL)

#define DEFINE_GET_STRING(x,y) STRING x::Get##y()
#define DEFINE_SET_STRING(x,y) void x::Set##y(CREFSTRING propertyValue)

#define DEFINE_GET_SET_STRING(className,propertyName,charBuffer) \
    DEFINE_GET_STRING(className,propertyName) \
    { \
        VERIFY_INITIALIZED(MAKE_QUALIFIED_MEMBER_STRING(className,Get##propertyName)); \
        return MentorReadString(charBuffer); \
    } \
    DEFINE_SET_STRING(className,propertyName) \
    { \
        VERIFY_INITIALIZED(MAKE_QUALIFIED_MEMBER_STRING(className,Set##propertyName)); \
        if (this->IsProtected()) \
            throw new MgCoordinateSystemInitializationFailedException(MAKE_QUALIFIED_MEMBER_STRING(className,Set##propertyName), __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL); \
        \
        MentorSetString(propertyValue, charBuffer /* target buffer */, sizeof(charBuffer)); \
    }

#define DEFINE_GET_NUMERIC(x,y,targetType) targetType x::Get##y()
#define DEFINE_SET_NUMERIC(x,y,targetType) void x::Set##y(targetType propertyValue)

#define DEFINE_GET_NUMERIC_IDX(x,y,targetType) targetType x::Get##y(INT32 index)
#define DEFINE_SET_NUMERIC_IDX(x,y,targetType) void x::Set##y(INT32 index, targetType propertyValue)

#define DEFINE_GET_SET_NUMERIC(className, propertyName,targetType,target) \
    DEFINE_GET_NUMERIC(className,propertyName,targetType) \
    { \
        VERIFY_INITIALIZED(MAKE_QUALIFIED_MEMBER_STRING(className,Get##propertyName)); \
        return target; \
    } \
    DEFINE_SET_NUMERIC(className,propertyName,targetType) \
    { \
        VERIFY_INITIALIZED(MAKE_QUALIFIED_MEMBER_STRING(className,Set##propertyName)); \
        if (this->IsProtected()) \
            throw new MgCoordinateSystemInitializationFailedException(MAKE_QUALIFIED_MEMBER_STRING(className,Set##propertyName), __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL); \
        \
        target = propertyValue;\
    }

#define DEFINE_GET_SET_NUMERIC_IDX(className, propertyName,targetType,target,maxIdx) \
    DEFINE_GET_NUMERIC_IDX(className,propertyName,targetType) \
    { \
        VERIFY_INITIALIZED(MAKE_QUALIFIED_MEMBER_STRING(className,Get##propertyName)); \
        if (index < 0 || index > maxIdx) \
            throw new MgArgumentOutOfRangeException(MAKE_QUALIFIED_MEMBER_STRING(className,Get##propertyName), __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL); \
        \
        return target[index]; \
    } \
    DEFINE_SET_NUMERIC_IDX(className,propertyName,targetType) \
    { \
        VERIFY_INITIALIZED(MAKE_QUALIFIED_MEMBER_STRING(className,Set##propertyName)); \
        if (this->IsProtected()) \
            throw new MgCoordinateSystemInitializationFailedException(MAKE_QUALIFIED_MEMBER_STRING(className,Set##propertyName), __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL); \
        \
        if (index < 0 || index > maxIdx) \
            throw new MgArgumentOutOfRangeException(MAKE_QUALIFIED_MEMBER_STRING(className,Get##propertyName), __LINE__, __WFILE__, NULL, L"MgCoordinateSystemProtectedException", NULL); \
        \
        target[index] = propertyValue;\
    }

#endif //_MG_COORDSYSMACRO_H_
