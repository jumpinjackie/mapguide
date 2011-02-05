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


ATOM::IUniverse* ATOM::BigBang()
{
    // Q: What do you name the instance of our (ATOM) Universe?
    static UniverseElement fortyTwo;    // Should be obvious, no? ;-)

    return &fortyTwo;
}


UniverseElement::UniverseElement()
: m_iCount(0)
{
    for(int i=0; i<MAX_PARSERS_IN_UNIVERSE; ++i)
        this->m_pRegistrants[i] = NULL;
}

ATOM::IGenerator** UniverseElement::Find(const ATOM::StRange& sName)
{
    for(int i=0; i<MAX_PARSERS_IN_UNIVERSE; ++i)
    {
        if(m_pRegistrants[i] && m_pRegistrants[i]->Name() == sName)
            return &m_pRegistrants[i];
    }

    return NULL;
}

ATOM::IGenerator** UniverseElement::FindEmpty()
{
    for(int i=0; i<MAX_PARSERS_IN_UNIVERSE; ++i)
    {
        if(m_pRegistrants[i] == NULL)
            return &m_pRegistrants[i];
    }

    return NULL;
}


// Registers a Generator, used by the parsing module
// when introduced to the universe.
ATOM::Status UniverseElement::Register(ATOM::IGenerator* pNew)
{
    const ATOM::StRange sName = pNew->Name();
    if(Find(sName))
        return ATOM::Status::keAlreadyPresent;

    // TODO: This strategy is now bogus!
    ATOM::IGenerator** pp = FindEmpty();
    if(!pp)
        return ATOM::Status::keNoResource;

    *pp = pNew;
    m_iCount++;

    return ATOM::Status::keOk;
}

// Unregisters a Generator.
ATOM::Status UniverseElement::Unregister(ATOM::IGenerator* pDead)
{
    const ATOM::StRange sName = pDead->Name();
    ATOM::IGenerator** pp = Find(sName);
    if(!pp)
        return ATOM::Status::keNotPresent;

    *pp = NULL;
    m_iCount--;
    // TODO: finish this: move the deleted items down.

    return ATOM::Status::keOk;
}

// How many parser/generators are registered?
int UniverseElement::RegisteredCount()
{
    return m_iCount;
}

// Gets a parser generator (by position in registration list)
// to allow the application to begin a parsing operation.
// 0 <= iIndex < RegisteredCount();
ATOM::IGenerator* UniverseElement::GetGenerator(int iIndex)
{
    if(iIndex < 0 || iIndex >= m_iCount)
        return NULL;

    return this->m_pRegistrants[iIndex];
}

// Same as above, but indexed off the IGenerator::Name()
// method.
ATOM::IGenerator* UniverseElement::GetGenerator(const ATOM::StRange& sName)
{
    ATOM::IGenerator** pp = Find(sName);
    if(pp)
        return *pp;
    else
        return NULL;
}
