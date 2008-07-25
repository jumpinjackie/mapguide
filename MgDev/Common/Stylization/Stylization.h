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

#ifndef STYLIZATION_H_
#define STYLIZATION_H_

//OS specific DLL options
#ifdef WIN32
#ifdef STYLIZATION_EXPORTS
#define STYLIZATION_API __declspec(dllexport)
#else
#define STYLIZATION_API __declspec(dllimport)
#endif
#else
#define STYLIZATION_API
#endif

//std headers
#include "wchar.h"

//MDF headers
#include "MdfModel.h"
#include "MdfOwnerCollection.h"
#include "LengthConverter.h"

#include "MapDefinition.h"
#include "MapLayer.h"
#include "MapLayerGroup.h"

#include "DrawingLayerDefinition.h"
#include "VectorLayerDefinition.h"
#include "GridLayerDefinition.h"

#include "VectorScaleRange.h"
#include "IFeatureTypeStyleVisitor.h"
#include "FeatureTypeStyle.h"
#include "LineTypeStyle.h"
#include "AreaTypeStyle.h"
#include "PointTypeStyle.h"
#include "CompositeTypeStyle.h"

#include "AreaRule.h"
#include "LineRule.h"
#include "PointRule.h"

#include "LineSymbolization2D.h"
#include "AreaSymbolization2D.h"
#include "PointSymbolization2D.h"
#include "Fill.h"
#include "Stroke.h"
#include "Symbol.h"
#include "W2DSymbol.h"
#include "TextSymbol.h"
#include "MarkSymbol.h"
#include "FontSymbol.h"
#include "ImageSymbol.h"
#include "SimpleSymbolDefinition.h"
#include "CompoundSymbolDefinition.h"
#include "ISymbolVisitor.h"
#include "ISymbolDefinitionVisitor.h"
#include "IGraphicElementVisitor.h"
#include "CompositeSymbolization.h"
#include "PointUsage.h"
#include "LineUsage.h"
#include "AreaUsage.h"
#include "Path.h"
#include "Text.h"
#include "Image.h"
#include "GraphicElement.h"
#include "Base64.h"

#ifdef _WIN32
// disable warning C4201: nonstandard extension used : nameless struct/union
#pragma warning(disable : 4201)
#endif

//FDO headers
#include "Fdo.h"


#ifndef __WFILE__
// Wide character version of __FILE__ macro
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)
#endif

//Non-fatal FDO exception logging mechanism
//Implementation is defined in Stylizer.cpp
typedef void (*StylizerExceptionCallback)(FdoException* exception, int line, wchar_t* file);
void ProcessStylizerException(FdoException* exception);
STYLIZATION_API void SetStylizerExceptionCallback(StylizerExceptionCallback callbackFunction);

//avoid linux warnings
static const void* avoid_warning1 = (void*)FDO_ACTIVELONGTRANSACTION;
static const void* avoid_warning2 = (void*)FDO_ROOTLONGTRANSACTION;

#define MAP_PRODUCT_AUTHOR  L"Autodesk"
#define MAP_PRODUCT_NAME    L"MapGuide Open Source"
#define MAP_PRODUCT_VERSION L"1.0.0"

#ifdef _WIN32
#define RESTRICT __restrict
#else
#define RESTRICT __restrict__
#endif

#define SE_INLINE inline

#endif
