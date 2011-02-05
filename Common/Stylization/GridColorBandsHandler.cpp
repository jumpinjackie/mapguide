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
#include "GridColorBandsHandler.h"
#include "GridData.h"
#include "Band.h"
#include "Color.h"

//
// GridColorBandsHandler
//

GridColorBandsHandler::GridColorBandsHandler()
{
    Clear();
}

GridColorBandsHandler::~GridColorBandsHandler()
{
    Clear();
}

bool GridColorBandsHandler::Initialize(const MdfModel::RuleCollection *pRules, const GridData *pGrid)
{
    Clear();
    bool bInit = false;
    if (pRules->GetCount() == 1)
    {
        MdfModel::GridColorRule *pRule = dynamic_cast<MdfModel::GridColorRule*>(pRules->GetAt(0));
        if (pRule->GetFilter().empty())
        {
            MdfModel::GridColorBands *pColor = dynamic_cast<MdfModel::GridColorBands*>(pRule->GetGridColor());
            if (pColor)
            {
                m_redChannel = pColor->GetRedBand();
                m_greenChannel = pColor->GetGreenBand();
                m_blueChannel = pColor->GetBlueBand();

                if (0 == wcscmp(m_redChannel.GetBand().c_str(), m_greenChannel.GetBand().c_str())
                    && 0 == wcscmp(m_redChannel.GetBand().c_str(), m_blueChannel.GetBand().c_str()))
                    m_bGray = true;

                m_pRedBand = pGrid->GetBand(m_redChannel.GetBand());
                m_pGreenBand = pGrid->GetBand(m_greenChannel.GetBand());
                m_pBlueBand = pGrid->GetBand(m_blueChannel.GetBand());
                bInit = (m_pRedBand != NULL) && (m_pGreenBand != NULL) && (m_pBlueBand != NULL);
            }
        }
    }
    if (!bInit)
        Clear();
    return bInit;
}

bool GridColorBandsHandler::GetColor(Color &color, unsigned int x, unsigned int y)
{
    bool bGet = false;
    double dRed(0.0), dGreen(0.0), dBlue(0.0);
    if (m_pRedBand->GetValueAsDouble(x, y, dRed)
        && m_pGreenBand->GetValueAsDouble(x, y, dGreen)
        && m_pBlueBand->GetValueAsDouble(x, y, dBlue))
    {
        unsigned char red(0), green(0), blue(0);
        if (m_redChannel.GetChannelValue(red, dRed)
            && m_greenChannel.GetChannelValue(green, dGreen)
            && m_blueChannel.GetChannelValue(blue, dBlue))
        {
            color.SetARGB(Color::kChannelMax, red, green, blue);
            bGet = true;
        }
    }
    return bGet;
}

void GridColorBandsHandler::Clear()
{
    m_redChannel.Clear();
    m_greenChannel.Clear();
    m_blueChannel.Clear();
    m_pRedBand = NULL;
    m_pGreenBand = NULL;
    m_pBlueBand = NULL;
    m_bGray = false;
}

Band* GridColorBandsHandler::GetRedBand()
{
    return m_pRedBand;
}

Band* GridColorBandsHandler::GetGreenBand()
{
    return m_pGreenBand;
}

Band* GridColorBandsHandler::GetBlueBand()
{
    return m_pBlueBand;
}

bool GridColorBandsHandler::IsGray()
{
    return m_bGray;
}
