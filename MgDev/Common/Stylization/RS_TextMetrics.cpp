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
#include "RS_TextMetrics.h"
#include "RichTextEngine.h"
#include "RS_Font.h"

using namespace RichText::ATOM;


//////////////////////////////////////////////////////////////////////////////
RS_TextMetrics::RS_TextMetrics()
    : font(NULL),
      font_height(0.0),
      text_width(0.0),
      text_height(0.0)
{
}


//////////////////////////////////////////////////////////////////////////////
RS_TextMetrics::~RS_TextMetrics()
{
    const RichText::ATOM::Particle* pParticle;
    const RichText::ATOM::Particle* pNext;
    size_t numLists = format_changes.size();
    for (size_t i=0; i<numLists; ++i)
    {
        pParticle = format_changes[i];
        while (pParticle)
        {
            pNext = pParticle->Next();
            delete pParticle;
            pParticle = pNext;
        }
    }
}



