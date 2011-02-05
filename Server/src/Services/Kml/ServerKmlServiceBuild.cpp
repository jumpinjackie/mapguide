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

// ignore warnings about exception definitions
#pragma warning(disable: 4290)

#ifdef _WIN32
#include "Dll.cpp"
#endif
#include "OpGetMapKml.cpp"
#include "OpGetLayerKml.cpp"
#include "OpGetFeaturesKml.cpp"
#include "KmlOperation.cpp"
#include "KmlOperationFactory.cpp"
#include "KmlServiceHandler.cpp"
#include "ServerKmlService.cpp"
