//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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

#ifndef SE_POSITIONINGALGORITHMS_H_
#define SE_POSITIONINGALGORITHMS_H_

#include "SE_RenderProxies.h"

const std::wstring HIGWAY_SHIELD_SYMBOLS_LOCATION = L"C:/";
const std::wstring HIGWAY_SHIELD_SYMBOLS_RESOURCE = L"Library://Symbology/HighwaySymbols/MultipleShields.SymbolDefinition";
const std::wstring HIGWAY_SHIELD_SYMBOLS_PREFIX = L"shield_";

class SE_ApplyContext;
class RS_FeatureReader;


class SE_PositioningAlgorithms
{
public:
    static void Default(SE_ApplyContext* applyCtx,
                        SE_RenderStyle*  rstyle);

    static void EightSurrounding(SE_ApplyContext* applyCtx,
                                 SE_RenderStyle*  rstyle,
                                 double           mm2su);

    static void PathLabels(SE_ApplyContext* applyCtx,
                           SE_RenderStyle*  rstyle);

    static void MultipleHighwaysShields(SE_ApplyContext*  applyCtx,
                                        SE_RenderStyle*   rstyle,
                                        double            mm2su,
                                        RS_FeatureReader* featureReader,
                                        SE_SymbolManager* symbolManager);
};


class StringOfTokens
{
public:
    StringOfTokens(std::wstring tokenstring, std::wstring delimiter);
    int getTokenCount();
    std::wstring getFirstToken();
    std::wstring getNextToken();

private:
    std::wstring m_delimiter;
    std::wstring m_tokenstring;
    std::wstring::size_type m_currentPos;
};

#endif
