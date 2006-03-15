//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "OgcFramework.h"
#include "WfsFeatures.h"

MgWfsFeatures::MgWfsFeatures(CPSZ inputXml):
    m_bOk(true),
    m_bDone(false)
{
    m_responseString = inputXml;
}

MgWfsFeatures::~MgWfsFeatures()
{
}

bool MgWfsFeatures::Next()
{
    if(m_bOk)
    {
        if(m_bDone)
        {
            m_bOk = false;
        }
    }
    return m_bOk;
}

void MgWfsFeatures::GenerateDefinitions(MgUtilDictionary& Dictionary)
{
    if(m_bOk)
    {
        Dictionary.AddDefinition(L"GetFeature.FeatureCollection", m_responseString.c_str());
        m_bDone = true;
    }
}

