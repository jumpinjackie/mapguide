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

#ifndef MG_MAPPING_SERVICE_DEFS_H
#define MG_MAPPING_SERVICE_DEFS_H

#include "ServiceManager.h"
#include "ServiceOperation.h"
#include "ServerManager.h"
#include "LogManager.h"
#include "IServiceHandler.h"

#include "CSysTransformer.h"

#include "ServerMappingService.h"
#include "MappingOperation.h"

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////
/// Mg Server Mapping Service try/catch/throw macros.
///
#define MG_SERVER_MAPPING_SERVICE_TRY()                                       \
    MG_TRY()                                                                  \

#define MG_SERVER_MAPPING_SERVICE_CATCH(methodName)                           \
    MG_CATCH(methodName)                                                      \

#define MG_SERVER_MAPPING_SERVICE_THROW()                                     \
    MG_THROW()                                                                \

#define MG_SERVER_MAPPING_SERVICE_CATCH_AND_THROW(methodName)                 \
    MG_SERVER_MAPPING_SERVICE_CATCH(methodName)                               \
                                                                              \
    MG_SERVER_MAPPING_SERVICE_THROW()                                         \

#endif
