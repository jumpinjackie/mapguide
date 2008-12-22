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

/////////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GwsQueryEngineImp.h"


wchar_t * GwsQueryXml::xmlGwsQuery                      = L"Query";
wchar_t * GwsQueryXml::xmlGwsQualifiedClassName         = L"Qualified_Class_Name";
wchar_t * GwsQueryXml::xmlGwsClassFeatureSource         = L"Feature_Source";
wchar_t * GwsQueryXml::xmlGwsClassSchema                = L"Schema";
wchar_t * GwsQueryXml::xmlGwsClassName                  = L"Class_Name";
wchar_t * GwsQueryXml::xmlGwsFilter                     = L"Filter";
wchar_t * GwsQueryXml::xmlGwsFilterStringValue          = L"Value";

wchar_t * GwsQueryXml::xmlGwsQueryTypeAttribute         = L"Type";
wchar_t * GwsQueryXml::xmlGwsQueryTypeFeature           = L"Feature";
wchar_t * GwsQueryXml::xmlGwsQueryTypeLeftOuterJoin     = L"Left Outer Join";
wchar_t * GwsQueryXml::xmlGwsQueryTypeEqualJoin         = L"Equal Join";
wchar_t * GwsQueryXml::xmlGwsLeftJoinAttributes         = L"LeftJoinAttributes";
wchar_t * GwsQueryXml::xmlGwsRightJoinAttributes        = L"RightJoinAttributes";
wchar_t * GwsQueryXml::xmlGwsJoinAttibuteNames          = L"Names";
wchar_t * GwsQueryXml::xmlGwsQuerySelectList            = L"Select_List";
wchar_t * GwsQueryXml::xmlGwsQuerySelectExpressionList  = L"Select_Expression_List";
wchar_t * GwsQueryXml::xmlGwsQuerySelectExpression      = L"Select_Expression";
wchar_t * GwsQueryXml::xmlGwsQuerySelectExpressionName  = L"Select_Expression_Name";
