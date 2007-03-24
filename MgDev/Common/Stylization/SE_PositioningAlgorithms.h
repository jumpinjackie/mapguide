//
//  Copyright (C) 2007 by Autodesk, Inc.
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

#ifndef SE_POSITIONINGALGORITHMS_H
#define SE_POSITIONINGALGORITHMS_H

#include "SE_RenderProxies.h"

const std::wstring HIGWAY_SHIELD_SYMBOLS_LOCATION = L"C:/";
const std::wstring HIGWAY_SHIELD_SYMBOLS_RESOURCE = L"Library://Symbology/HighwaySymbols/MultipleShields.SymbolDefinition";
const std::wstring HIGWAY_SHIELD_SYMBOLS_PREFIX = L"shield_";

class LineBuffer;
class SE_Renderer;
class SE_SymbolManager;
class RS_FeatureReader;
struct SE_Matrix;
struct SE_Style;
struct SE_RenderStyle;


class SE_PositioningAlgorithms
{
public:
    static void EightSurrounding(SE_Renderer*    renderer,
                          LineBuffer*     geometry,
                          SE_Matrix&      xform,
                          SE_Style*       style,
                          SE_RenderStyle* rstyle,
                          double          mm2px
                          );

    static void PathLabels(SE_Renderer*    renderer,
                          LineBuffer*     geometry,
                          SE_Matrix&      xform,
                          SE_Style*       style,
                          SE_RenderStyle* rstyle,
                          double          mm2px
                          );

    static void MultipleHighwaysShields(SE_Renderer*    renderer,
                                        LineBuffer*     geometry,
                                        SE_Matrix&      xform,
                                        SE_Style*       style,
                                        SE_RenderStyle* rstyle,
                                        double          mm2px,
                                        RS_FeatureReader* featureReader,
                                        SE_SymbolManager* symbolManager
                                        );
};

class StringOfTokens
{

public:


    StringOfTokens(std::wstring tokenstring, std::wstring delimiter) :
        m_tokenstring(tokenstring),
        m_delimiter(delimiter)
    {
        m_currentPos = 0;
    }


    int getTokenCount();
    std::wstring getFirstToken();
    std::wstring getNextToken();

private:

    std::wstring  m_delimiter;
    std::wstring  m_tokenstring;
    std::wstring::size_type m_currentPos;


};

#endif // SE_POSITIONINGALGORITHMS_H
