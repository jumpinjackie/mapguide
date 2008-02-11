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

#ifndef STYLIZATION_H_
#define STYLIZATION_H_

// OS specific DLL options
#include "StylizationAPI.h"

// basic definitions
#include "StylizationDefs.h"

// MDF headers
#include "MdfModel.h"
#include "MdfOwnerCollection.h"

#include "MapDefinition.h"

#include "DrawingLayerDefinition.h"
#include "GridLayerDefinition.h"
#include "VectorLayerDefinition.h"

#include "IFeatureTypeStyleVisitor.h"
#include "CompositeTypeStyle.h"
#include "AreaTypeStyle.h"
#include "LineTypeStyle.h"
#include "PointTypeStyle.h"
#include "AreaRule.h"
#include "LineRule.h"
#include "PointRule.h"

#include "CompositeSymbolization.h"
#include "AreaSymbolization2D.h"
#include "LineSymbolization2D.h"
#include "PointSymbolization2D.h"
#include "Fill.h"
#include "Stroke.h"
#include "ISymbolVisitor.h"
#include "BlockSymbol.h"
#include "FontSymbol.h"
#include "ImageSymbol.h"
#include "MarkSymbol.h"
#include "TextSymbol.h"
#include "W2DSymbol.h"

#include "SimpleSymbolDefinition.h"
#include "CompoundSymbolDefinition.h"
#include "ISymbolDefinitionVisitor.h"
#include "IGraphicElementVisitor.h"
#include "AreaUsage.h"
#include "LineUsage.h"
#include "PointUsage.h"
#include "Path.h"
#include "Image.h"
#include "Text.h"
#include "Override.h"

#include "Base64.h"

// FDO headers
#pragma warning(push)
#pragma warning(disable: 4201)
#include "Fdo.h"
#include "FdoExpressionEngine.h"
#include "FdoExpressionEngineFunctionCollection.h"
#include "FdoExpressionEngineINonAggregateFunction.h"
#pragma warning(pop)

// avoid linux warnings
#ifndef _WIN32
static const void* avoid_warning1 = (void*)FDO_ACTIVELONGTRANSACTION;
static const void* avoid_warning2 = (void*)FDO_ROOTLONGTRANSACTION;
#endif

#endif
