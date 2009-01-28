//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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


/////////////////////////////////////////////////////////////////////
//
// class CGwsFeaturesPool
//
/////////////////////////////////////////////////////////////////////

CGwsFeaturesPool::CGwsFeaturesPool (IGWSExtendedFeatureDescription * pFeatDesc)
{
    m_pFeatDesc = pFeatDesc;
    pFeatDesc->AddRef ();
    m_used = 0;
}


CGwsFeaturesPool::~CGwsFeaturesPool    ()
{
    for (size_t i = 0; i < m_pool.size (); i ++) {
        CGwsPooledFeature * pfeature = m_pool [i];
        pfeature->Release ();
    }
}


int CGwsFeaturesPool::GetCount ()
{
    return m_used;
}


int CGwsFeaturesPool::GetSize ()
{
    return (int) m_pool.size ();
}


IGWSFeature * CGwsFeaturesPool::GetFeature (int i)
{
    CGwsPooledFeature * pfeature = NULL;
    if (i < m_used) {
        pfeature = m_pool[i];
        pfeature->AddRef ();
    }
    return pfeature;
}


void CGwsFeaturesPool::Reset ()
{
    m_used = 0;
}


void CGwsFeaturesPool::AddFeature (IGWSFeature * feat)
{
    CGwsPooledFeature * pfeature = NULL;
    if (m_used < (int) m_pool.size ()) {
        pfeature = m_pool[m_used];

    } else {
        pfeature = new CGwsPooledFeature (m_pFeatDesc);
        pfeature->AddRef ();
        m_pool.push_back (pfeature);
    }
    m_used ++;
    pfeature->Set (feat);
}
