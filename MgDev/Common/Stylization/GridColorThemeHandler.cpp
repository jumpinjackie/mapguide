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

#include "stdafx.h"
#include "GridColorThemeHandler.h"
#include "Color.h"
#include "GridTheme.h"
#include "GridThemeParser.h"
#include "Band.h"
#include "GridData.h"

//
// GridColorThemeHandler
//

GridColorThemeHandler::GridColorThemeHandler()
{
    Clear();
}

GridColorThemeHandler::~GridColorThemeHandler()
{
    Clear();
}

bool GridColorThemeHandler::Initialize(const MdfModel::RuleCollection *pRules, const GridData *pGrid)
{
    Clear();
    bool bInit = false;
    GridThemeParser parser;
    m_spTheme.reset(parser.ParseThemeColorRules(pRules));
    if (NULL != m_spTheme.get())
    {
        m_spHashTable.reset(new GridThemeHashTable());
        if (!m_spHashTable->Initialize(m_spTheme.get()))
            m_spHashTable.reset();
        Band *pBand = pGrid->GetBand(m_spTheme->GetBandName());
        if (m_spTheme->GetType() == GridThemeParser::sm_AspectType)
            m_pThemeBand = pBand->GetAspectBand();
        else if (m_spTheme->GetType() == GridThemeParser::sm_SlopeType)
            m_pThemeBand = pBand->GetSlopeBand();
        else if (m_spTheme->GetType() == GridThemeParser::sm_HeightType)
            m_pThemeBand = pBand;
        bInit = (m_pThemeBand != NULL);
    }
    if (!bInit)
        Clear();
    return bInit;
}

void GridColorThemeHandler::Clear()
{
    m_spHashTable.reset();
    m_spTheme.reset();
    m_pThemeBand = NULL;
}

bool GridColorThemeHandler::GetColor(Color &color, unsigned int x, unsigned int y)
{
    bool bGet = false;

    //this will return false if value is not valid
    double geometryvalue;
    bool ret = m_pThemeBand->GetValueAsDouble(x, y, geometryvalue);

    if (ret)
    {
        if (NULL != m_spHashTable.get())
            bGet = m_spHashTable->SearchColorByValue(color, geometryvalue);
        else if (NULL != m_spTheme.get())
            bGet = m_spTheme->SearchColorByValue(color, geometryvalue);
    }

    return bGet;
}
