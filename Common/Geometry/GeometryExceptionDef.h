//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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


#ifndef MG_GEOMETRY_EXCEPTION_DEF_H
#define MG_GEOMETRY_EXCEPTION_DEF_H

#define MG_GEOMETRY_TRY()                                                     \
    MG_TRY()                                                                  \

#define MG_GEOMETRY_CATCH(methodName)                                         \
    }                                                                         \
    catch (GEOSException* e)                                                  \
    {                                                                         \
        MgStringCollection arguments;                                         \
        arguments.Add(MgUtil::MultiByteToWideChar(e->toString()));            \
        mgException = NULL; \
        delete e;                                                             \
                                                                              \
    MG_CATCH(methodName)                                                      \

#define MG_GEOMETRY_THROW()                                                   \
    MG_THROW()                                                                \

#define MG_GEOMETRY_CATCH_AND_THROW(methodName)                               \
    MG_GEOMETRY_CATCH(methodName)                                             \
                                                                              \
    MG_GEOMETRY_THROW()                                                       \

#endif


/*
        mgException = new MgGeometryException(methodName, __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments); \
*/
