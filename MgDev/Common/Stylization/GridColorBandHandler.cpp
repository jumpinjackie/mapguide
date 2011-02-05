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
#include "GridColorBandHandler.h"
#include "Band.h"
#include "GridData.h"
#include "Color.h"
#include "MathHelper.h"
// MdfModel
#include "GridColorRule.h"

using namespace MDFMODEL_NAMESPACE;

//
// GridColorBandHandler
//

GridColorBandHandler::GridColorBandHandler()
{
    Clear();
}

GridColorBandHandler::~GridColorBandHandler()
{
    Clear();
}

bool GridColorBandHandler::Initialize(const MdfModel::RuleCollection *pRules, const GridData *pGrid)
{
    Clear();
    bool bInit = false;
    if (pRules->GetCount() == 1)
    {
        MdfModel::GridColorRule *pRule = dynamic_cast<MdfModel::GridColorRule*>(pRules->GetAt(0));
        if (pRule->GetFilter().empty())
        {
            MdfModel::GridColorBand *pColor = dynamic_cast<MdfModel::GridColorBand*>(pRule->GetGridColor());
            if (pColor)
            {
                m_sBandName = pColor->GetBand();
                m_pColorBand = pGrid->GetBand(m_sBandName);
                bInit = (m_pColorBand != NULL);
            }
        }
    }
    if (!bInit)
        Clear();
    return bInit;
}

bool GridColorBandHandler::GetColor(Color &color, unsigned int x, unsigned int y)
{
    bool bGet = false;
    double value = 0.0;

    // Currently, data type is always 32 bits in most cases.
    if (Band::Int32 == m_pColorBand->GetDataType() ||
        Band::UnsignedInt32 == m_pColorBand->GetDataType ())
    {
        color.SetARGB( m_pColorBand->GetColorValue(x,y) );
        color.SetA(Color::kChannelMax); // To Make it not transparent.
        bGet = true;
    }
    else
    {
        if (m_pColorBand->GetValueAsDouble(x, y, value))
        {
            INT64 nResult = Double2Int64(value);
            if (nResult >= 0 && nResult <= UINT_MAX)
            {
                color.SetARGB(static_cast<unsigned int>(nResult));
                color.SetA(Color::kChannelMax); // To Make it not transparent.
                bGet = true;
            }
        }
    }
    return bGet;
}

void GridColorBandHandler::Clear()
{
    m_sBandName.clear();
    m_pColorBand = NULL;
}
