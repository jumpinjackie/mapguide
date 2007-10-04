#include "stdafx.h"
#include "assert.h"

#include "atom.h"
#include "atom_element.h"


// Q: What do you name the instance of our (ATOM) Universe?
UniverseElement* pFortyTwo = NULL; // Should be obvious, no? ;-)

ATOM::IUniverse* ATOM::BigBang()
{
	if ( pFortyTwo == NULL )
		pFortyTwo = new UniverseElement();
    return pFortyTwo;
}






UniverseElement::UniverseElement()
: m_iCount(0)
{
    for(int i=0; i<MAX_PARSERS_IN_UNIVERSE; i++)
        this->m_pRegistrants[i] = NULL;
}

ATOM::IParserGenerator** UniverseElement::Find(const ATOM::StRange& sName)
{
    for(int i=0; i<MAX_PARSERS_IN_UNIVERSE; i++) {
        if(m_pRegistrants[i] && m_pRegistrants[i]->Name() == sName)
            return &m_pRegistrants[i];
    }

    return NULL;
}

ATOM::IParserGenerator** UniverseElement::FindEmpty()
{
    for(int i=0; i<MAX_PARSERS_IN_UNIVERSE; i++) {
        if(m_pRegistrants[i] == NULL)
            return &m_pRegistrants[i];
    }

    return NULL;
}


// Registers a ParserGenerator, used by the parsing module
// when introduced to the universe.
ATOM::Status UniverseElement::Register(ATOM::IParserGenerator* pNew)
{
    const ATOM::StRange sName = pNew->Name();
    if(Find(sName))
        return ATOM::Status::keAlreadyPresent;

    // TODO: This strategy is now bogus!
    ATOM::IParserGenerator** pp = FindEmpty();
    if(!pp)
        return ATOM::Status::keNoResource;
    
    *pp = pNew;
    m_iCount++;

    return ATOM::Status::keOk;
}

// Unregisters a ParserGenerator.
ATOM::Status UniverseElement::Unregister(ATOM::IParserGenerator* pDead)
{
    const ATOM::StRange sName = pDead->Name();
    ATOM::IParserGenerator** pp = Find(sName);
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
ATOM::IParserGenerator* UniverseElement::GetGenerator(int iIndex)
{
    if(iIndex < 0 || iIndex >= m_iCount)
        return NULL;

    return this->m_pRegistrants[iIndex];
}

// Same as above, but indexed off the IParserGenerator::Name()
// method.
ATOM::IParserGenerator* UniverseElement::GetGenerator(const ATOM::StRange& sName)
{
    ATOM::IParserGenerator** pp = Find(sName);
    if(pp)
        return *pp;
    else
        return NULL;
}
