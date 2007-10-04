#include "stdafx.h"
#include "assert.h"

#include "atom.h"
#include "atom_element.h"

LocationElement::LocationElement()
: m_eSemantics(ATOM::ILocation::keNormal)
, m_pOperations(NULL)
{
}

LocationElement::~LocationElement()
{
    Reset();
}

void LocationElement::Reset()
{
    // Kill the operations.
    while(m_pOperations) {
        ATOM::LocationParticle* p = m_pOperations;
        m_pOperations = const_cast<ATOM::LocationParticle*>(p->Next());
        delete(p);
    }
    this->m_eSemantics = ATOM::ILocation::keNormal;
}

void LocationElement::Push(LocationElement& oOther)
{
    // Location isn't slurped.
    // We assume it's Normal, and that markup will
    // explicitly tell us otherwise.
}

// Push from the environment (ie, initialize outermost context)
void LocationElement::Push(ATOM::IEnvironment* pEnv)
{
    // Location isn't slurped (yet?)
}

void LocationElement::Pop(LocationElement& oOther)
{
}


void LocationElement::SetSemantics(ATOM::ILocation::SemanticType eType)
{
    this->m_eSemantics = eType;
}

bool LocationElement::AddOperation(const ATOM::LocationParticle& oParticle)
{
    return AddToList(this->m_pOperations,oParticle);
}


bool LocationElement::AddToList(ATOM::LocationParticle*& pList, const ATOM::LocationParticle& oParticle)
{
    ATOM::LocationParticle* pClone = oParticle.Clone();
    if(pClone == NULL)
        return false;

    if(pList != NULL)
        pList->Append(pClone);
    else // (or start a new list with a clone.)
        pList = pClone;

    return false;
}



// Describes the nature of the location change.
ATOM::ILocation::SemanticType LocationElement::Semantics() const
{
    return this->m_eSemantics;
}

// Zero or more operations to effect the location change.
ATOM::LocationParticle* LocationElement::Operations() const
{
    return this->m_pOperations;
}