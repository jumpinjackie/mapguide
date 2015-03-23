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
#include "atom.h"
#include "atom_element.h"
#include <assert.h>


AbandonmentElement::AbandonmentElement(ATOM::Status o)
: m_oStatus(o)
{
}

void AbandonmentElement::SetMarkup(const ATOM::StRange& sEntireString)
{
    this->m_sMarkupString = sEntireString;
    this->m_sMarkupContext = sEntireString; // default to something; SetContext can refine.
}

void AbandonmentElement::SetContext(const ATOM::StRange& sCurrentLine)
{
    this->m_sMarkupContext = sCurrentLine;
}

void AbandonmentElement::SetPosition(const ATOM::StRange& sPosition)
{
    this->m_sAbandonedPosition = sPosition;
}


const ATOM::Status AbandonmentElement::Reason()
{
    return this->m_oStatus;
}


const ATOM::StRange& AbandonmentElement::Markup()
{
    return this->m_sMarkupString;
}

const ATOM::StRange& AbandonmentElement::Context()
{
    return this->m_sMarkupContext;
}

const ATOM::StRange& AbandonmentElement::Position()
{
    return this->m_sAbandonedPosition;
}
