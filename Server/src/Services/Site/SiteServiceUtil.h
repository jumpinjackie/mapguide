//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef MGSITESERVICEUTIL_H_
#define MGSITESERVICEUTIL_H_

#define MG_SITE_SERVICE_TRY()                                                 \
    MG_TRY()                                                                  \

#define MG_SITE_SERVICE_CATCH(methodName)                                     \
    MG_CATCH(methodName)                                                      \

#define MG_SITE_SERVICE_THROW()                                               \
    MG_THROW()                                                                \

#define MG_SITE_SERVICE_CATCH_AND_THROW(methodName)                           \
    MG_CATCH_AND_THROW(methodName)                                            \

#endif // MGSITESERVICEUTIL_H_
