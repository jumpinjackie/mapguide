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
#include "GridColorNullHandler.h"
#include "Color.h"

GridColorNullHandler::GridColorNullHandler()
{
}

GridColorNullHandler::~GridColorNullHandler()
{
}

bool GridColorNullHandler::Initialize(const MdfModel::RuleCollection *pRules, const GridData *pGrid)
{
    pGrid;
    return (NULL == pRules || pRules->GetCount() == 0);
}

bool GridColorNullHandler::GetColor(Color &color, unsigned int x, unsigned int y)
{
    x;y;
    // Set the color to white.
    color.SetARGB(Color::kChannelMax, Color::kChannelMax, Color::kChannelMax, Color::kChannelMax);
    return true;
}

void GridColorNullHandler::Clear()
{
}
