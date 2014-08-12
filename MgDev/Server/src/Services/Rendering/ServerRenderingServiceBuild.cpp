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

#ifdef _WIN32
#include "Dll.cpp"
#endif
#include "FeatureInfoRenderer.cpp"
#include "FeaturePropRenderer.cpp"
#include "OpRenderDynamicOverlay.cpp"
#include "OpRenderMap.cpp"
#include "OpRenderMapLegend.cpp"
#include "OpRenderTile.cpp"
#include "OpRenderTileXYZ.cpp"
#include "OpQueryFeatures.cpp"
#include "OpQueryFeatureProperties.cpp"
#include "RenderingOperation.cpp"
#include "RenderingOperationFactory.cpp"
#include "RenderingServiceHandler.cpp"
#include "ServerRenderingService.cpp"
