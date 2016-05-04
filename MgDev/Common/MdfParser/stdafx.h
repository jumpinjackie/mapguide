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

#ifndef _STDAFX_H
#define _STDAFX_H

#ifdef _WIN32

// Exclude rarely-used stuff from Windows headers.
#define WIN32_LEAN_AND_MEAN

// Windows Header Files:
#include <windows.h>

// for memory leak detection
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#else

#define _ASSERT(x)
#define _wcsnicmp wcsncasecmp
#define _wcsicmp wcscasecmp

#include <memory>

#endif //_WIN32

// Xerces includes
#include "xercesc/util/XMLString.hpp"

// Misc Includes
#include <iostream>
#include <fstream>

// MdfModel includes
#include "MdfModel.h"
#include "Version.h"

// MdfParser includes
#include "MdfParser.h"
#include "IOUtil.h"

#define EMIT_BOOL_PROPERTY(stream, object, prop, optional, defaultVal, tabObj)  \
            bool emit##prop = true;                                             \
            const MdfString& str##prop = object->Get##prop();                   \
            if (optional)                                                       \
            {                                                                   \
                if (str##prop.size() == 0)                                      \
                    emit##prop = false;                                         \
                else                                                            \
                {                                                               \
                    bool val;                                                   \
                    if (wstrToBool(str##prop, val) && val == defaultVal)        \
                        emit##prop = false;                                     \
                }                                                               \
            }                                                                   \
            if (emit##prop)                                                     \
            {                                                                   \
                stream << tabObj.tab() << "<" #prop ">";                        \
                stream << EncodeString(str##prop);                              \
                stream << "</" #prop ">" << std::endl;                          \
            }

#define EMIT_DOUBLE_PROPERTY(stream, object, prop, optional, defaultVal, tabObj)\
            bool emit##prop = true;                                             \
            const MdfString& str##prop = object->Get##prop();                   \
            if (optional)                                                       \
            {                                                                   \
                if (str##prop.size() == 0)                                      \
                    emit##prop = false;                                         \
                else                                                            \
                {                                                               \
                    double val;                                                 \
                    if (wstrToDouble(str##prop, val) && val == defaultVal)      \
                        emit##prop = false;                                     \
                }                                                               \
            }                                                                   \
            if (emit##prop)                                                     \
            {                                                                   \
                stream << tabObj.tab() << "<" #prop ">";                        \
                stream << EncodeString(str##prop);                              \
                stream << "</" #prop ">" << std::endl;                          \
            }

#define EMIT_INTEGER_PROPERTY(stream, object, prop, optional, defaultVal, tabObj)\
            bool emit##prop = true;                                             \
            const MdfString& str##prop = object->Get##prop();                   \
            if (optional)                                                       \
            {                                                                   \
                if (str##prop.size() == 0)                                      \
                    emit##prop = false;                                         \
                else                                                            \
                {                                                               \
                    int val;                                                    \
                    if (wstrToInt(str##prop, val) && val == defaultVal)         \
                        emit##prop = false;                                     \
                }                                                               \
            }                                                                   \
            if (emit##prop)                                                     \
            {                                                                   \
                stream << tabObj.tab() << "<" #prop ">";                        \
                stream << EncodeString(str##prop);                              \
                stream << "</" #prop ">" << std::endl;                          \
            }

#define EMIT_STRING_PROPERTY(stream, object, prop, optional, defaultVal, tabObj)\
            bool emit##prop = true;                                             \
            const MdfString& str##prop = object->Get##prop();                   \
            if (optional)                                                       \
            {                                                                   \
                if (str##prop.size() == 0)                                      \
                    emit##prop = false;                                         \
                else if (_wcsicmp(str##prop.c_str(), defaultVal) == 0)          \
                    emit##prop = false;                                         \
            }                                                                   \
            if (emit##prop)                                                     \
            {                                                                   \
                stream << tabObj.tab() << "<" #prop ">";                        \
                stream << EncodeString(str##prop);                              \
                stream << "</" #prop ">" << std::endl;                          \
            }

#define EMIT_ENUM_2(stream, object, enumtype, prop, e1, e2, optIdx, tabObj)     \
            enumtype::prop propVal##prop = object->Get##prop();                 \
            bool emit##prop = true;                                             \
            switch (optIdx)                                                     \
            {                                                                   \
            case 1:                                                             \
                emit##prop = (propVal##prop != enumtype::e1);                   \
                break;                                                          \
            case 2:                                                             \
                emit##prop = (propVal##prop != enumtype::e2);                   \
                break;                                                          \
            }                                                                   \
            if (emit##prop)                                                     \
            {                                                                   \
                stream << tabObj.tab() << "<" #prop ">";                        \
                switch (propVal##prop)                                          \
                {                                                               \
                case enumtype::e1:                                              \
                    stream << #e1;                                              \
                    break;                                                      \
                case enumtype::e2:                                              \
                    stream << #e2;                                              \
                    break;                                                      \
                }                                                               \
                stream << "</" #prop ">" << std::endl;                          \
            }

#define EMIT_ENUM_3(stream, object, enumtype, prop, e1, e2, e3, optIdx, tabObj) \
            enumtype::prop propVal##prop = object->Get##prop();                 \
            bool emit##prop = true;                                             \
            switch (optIdx)                                                     \
            {                                                                   \
            case 1:                                                             \
                emit##prop = (propVal##prop != enumtype::e1);                   \
                break;                                                          \
            case 2:                                                             \
                emit##prop = (propVal##prop != enumtype::e2);                   \
                break;                                                          \
            case 3:                                                             \
                emit##prop = (propVal##prop != enumtype::e3);                   \
                break;                                                          \
            }                                                                   \
            if (emit##prop)                                                     \
            {                                                                   \
                stream << tabObj.tab() << "<" #prop ">";                        \
                switch (propVal##prop)                                          \
                {                                                               \
                case enumtype::e1:                                              \
                    stream << #e1;                                              \
                    break;                                                      \
                case enumtype::e2:                                              \
                    stream << #e2;                                              \
                    break;                                                      \
                case enumtype::e3:                                              \
                    stream << #e3;                                              \
                    break;                                                      \
                }                                                               \
                stream << "</" #prop ">" << std::endl;                          \
            }

#define EMIT_ENUM_4(stream, object, enumtype, prop, e1, e2, e3, e4, optIdx, tabObj)\
            enumtype::prop propVal##prop = object->Get##prop();                 \
            bool emit##prop = true;                                             \
            switch (optIdx)                                                     \
            {                                                                   \
            case 1:                                                             \
                emit##prop = (propVal##prop != enumtype::e1);                   \
                break;                                                          \
            case 2:                                                             \
                emit##prop = (propVal##prop != enumtype::e2);                   \
                break;                                                          \
            case 3:                                                             \
                emit##prop = (propVal##prop != enumtype::e3);                   \
                break;                                                          \
            case 4:                                                             \
                emit##prop = (propVal##prop != enumtype::e4);                   \
                break;                                                          \
            }                                                                   \
            if (emit##prop)                                                     \
            {                                                                   \
                stream << tabObj.tab() << "<" #prop ">";                        \
                switch (propVal##prop)                                          \
                {                                                               \
                case enumtype::e1:                                              \
                    stream << #e1;                                              \
                    break;                                                      \
                case enumtype::e2:                                              \
                    stream << #e2;                                              \
                    break;                                                      \
                case enumtype::e3:                                              \
                    stream << #e3;                                              \
                    break;                                                      \
                case enumtype::e4:                                              \
                    stream << #e4;                                              \
                    break;                                                      \
                }                                                               \
                stream << "</" #prop ">" << std::endl;                          \
            }

#define EMIT_ENUM_5(stream, object, enumtype, prop, e1, e2, e3, e4, e5, optIdx, tabObj)\
            enumtype::prop propVal##prop = object->Get##prop();                 \
            bool emit##prop = true;                                             \
            switch (optIdx)                                                     \
            {                                                                   \
            case 1:                                                             \
                emit##prop = (propVal##prop != enumtype::e1);                   \
                break;                                                          \
            case 2:                                                             \
                emit##prop = (propVal##prop != enumtype::e2);                   \
                break;                                                          \
            case 3:                                                             \
                emit##prop = (propVal##prop != enumtype::e3);                   \
                break;                                                          \
            case 4:                                                             \
                emit##prop = (propVal##prop != enumtype::e4);                   \
                break;                                                          \
            case 5:                                                             \
                emit##prop = (propVal##prop != enumtype::e5);                   \
                break;                                                          \
            }                                                                   \
            if (emit##prop)                                                     \
            {                                                                   \
                stream << tabObj.tab() << "<" #prop ">";                        \
                switch (propVal##prop)                                          \
                {                                                               \
                case enumtype::e1:                                              \
                    stream << #e1;                                              \
                    break;                                                      \
                case enumtype::e2:                                              \
                    stream << #e2;                                              \
                    break;                                                      \
                case enumtype::e3:                                              \
                    stream << #e3;                                              \
                    break;                                                      \
                case enumtype::e4:                                              \
                    stream << #e4;                                              \
                    break;                                                      \
                case enumtype::e5:                                              \
                    stream << #e5;                                              \
                    break;                                                      \
                }                                                               \
                stream << "</" #prop ">" << std::endl;                          \
            }

#define IF_STRING_PROPERTY(curprop, object, prop, value)                        \
            if (curprop == L###prop)                                            \
                object->Set##prop(value);

#define IF_DOUBLE_PROPERTY(curprop, object, prop, value)                        \
            if (curprop == L###prop)                                            \
                object->Set##prop(wstrToDouble(value));

#define IF_INTEGER_PROPERTY(curprop, object, prop, value)                       \
            if (curprop == L###prop)                                            \
                object->Set##prop(wstrToInt(value));

#define IF_ENUM_2(curprop, object, enumtype, prop, value, e1, e2)               \
            if (curprop == L###prop)                                            \
            {                                                                   \
                if      (::wcscmp(value, L###e1) == 0)                          \
                    object->Set##prop(enumtype::e1);                            \
                else if (::wcscmp(value, L###e2) == 0)                          \
                    object->Set##prop(enumtype::e2);                            \
            }

#define IF_ENUM_3(curprop, object, enumtype, prop, value, e1, e2, e3)           \
            if (curprop == L###prop)                                            \
            {                                                                   \
                if      (::wcscmp(value, L###e1) == 0)                          \
                    object->Set##prop(enumtype::e1);                            \
                else if (::wcscmp(value, L###e2) == 0)                          \
                    object->Set##prop(enumtype::e2);                            \
                else if (::wcscmp(value, L###e3) == 0)                          \
                    object->Set##prop(enumtype::e3);                            \
            }

#define IF_ENUM_4(curprop, object, enumtype, prop, value, e1, e2, e3, e4)       \
            if (curprop == L###prop)                                            \
            {                                                                   \
                if      (::wcscmp(value, L###e1) == 0)                          \
                    object->Set##prop(enumtype::e1);                            \
                else if (::wcscmp(value, L###e2) == 0)                          \
                    object->Set##prop(enumtype::e2);                            \
                else if (::wcscmp(value, L###e3) == 0)                          \
                    object->Set##prop(enumtype::e3);                            \
                else if (::wcscmp(value, L###e4) == 0)                          \
                    object->Set##prop(enumtype::e4);                            \
            }

#define IF_ENUM_5(curprop, object, enumtype, prop, value, e1, e2, e3, e4, e5)   \
            if (curprop == L###prop)                                            \
            {                                                                   \
                if      (::wcscmp(value, L###e1) == 0)                          \
                    object->Set##prop(enumtype::e1);                            \
                else if (::wcscmp(value, L###e2) == 0)                          \
                    object->Set##prop(enumtype::e2);                            \
                else if (::wcscmp(value, L###e3) == 0)                          \
                    object->Set##prop(enumtype::e3);                            \
                else if (::wcscmp(value, L###e4) == 0)                          \
                    object->Set##prop(enumtype::e4);                            \
                else if (::wcscmp(value, L###e5) == 0)                          \
                    object->Set##prop(enumtype::e5);                            \
            }

#endif
