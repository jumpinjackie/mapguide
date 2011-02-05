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


StyleDescriptionElement::StyleDescriptionElement()
: m_pDescription(0)
{}

// List-copy constructor
StyleDescriptionElement::StyleDescriptionElement(const ATOM::StyleParticle* pOtherList)
: m_pDescription(0)
{
    const ATOM::StyleParticle* pStyle = pOtherList;
    while(pStyle != NULL) {
        AddToDescription(*pStyle);
        pStyle = pStyle->Next();
    }
}



StyleDescriptionElement::~StyleDescriptionElement()
{
    // Kill the description.
    while(m_pDescription) {
        ATOM::StyleParticle* p = m_pDescription;
        m_pDescription = const_cast<ATOM::StyleParticle*>(p->Next());
        delete(p);
    }
}


const ATOM::StyleParticle* StyleDescriptionElement::Description() const
{
    return m_pDescription;
}


const ATOM::StyleParticle* StyleDescriptionElement::DescriptionParticle(ATOM::StyleParticle::StyleParticleType eType) const
{
    return GetDescriptionParticle(eType);
}



ATOM::Status StyleDescriptionElement::AddToDescription(const ATOM::StyleParticle& oParticle)
{
    return AddToSet(m_pDescription,oParticle);
}


ATOM::Status StyleDescriptionElement::RemoveFromDescription(const ATOM::StyleParticle::StyleParticleType eType)
{
    ATOM::StyleParticle* pGone = RemoveFromList(m_pDescription,eType);
    if(pGone != NULL)
    {
        delete(pGone);
        return ATOM::Status::keOk;
    }

    // Nothing to remove?  Say so.
    return ATOM::Status::keUnchanged;
}

// Gets the first particle of the type indicated.
ATOM::StyleParticle* StyleDescriptionElement::GetDescriptionParticle(ATOM::StyleParticle::StyleParticleType eType) const
{
    return GetParticle(eType,this->m_pDescription);
}



/* Style Particle List Tools */

// Finds (the first) particle in a list (or set)
// and returns a pointer to it, or NULL if none found.
ATOM::StyleParticle* StyleDescriptionElement::GetParticle(ATOM::StyleParticle::StyleParticleType eType,const ATOM::StyleParticle* pList)
{
    while(pList != NULL) {
        if(pList->Type() == eType)
            return const_cast<ATOM::StyleParticle*>(pList);
        else
            pList = const_cast<ATOM::StyleParticle*>(pList->Next());
    }
    return NULL;
}

// Imparts "set" semantics to a list of Particles
// Only one particle of any given type can exist
// within such a set.  Adding a duplicate replaces
// the original with the newer particle's value.
// In all cases, oParticle's ownership remains outside
// of this list (that is, the list contains only copies.)
ATOM::Status StyleDescriptionElement::AddToSet(ATOM::StyleParticle*& pSet, const ATOM::StyleParticle& oParticle)
{
    ATOM::StyleParticle* pInList = GetParticle(oParticle.Type(),pSet);

    // if it's already in the list/set, we copy the
    // contents of the particle over the corresponding
    // value already in the list.
    if(pInList) {
        *pInList = oParticle;
        return ATOM::Status::keReplaced;
    }
    else { // Otherwise, we append a clone to the list
        if(pSet != NULL)
            pSet->Append(oParticle.Clone());
        else // (or start a new list with a clone.)
            pSet = oParticle.Clone();
        return ATOM::Status::keOk;
    }
}

ATOM::StyleParticle* StyleDescriptionElement::RemoveFromList(ATOM::StyleParticle*& pList,
                                                             const ATOM::StyleParticle::StyleParticleType eType)
{
    // Empty List, nothing to delete.
    if(pList == NULL)
        return NULL;

    ATOM::StyleParticle* pParticle = pList;
    // Need to remove the head of the list?
    if(pParticle->Type() == eType) {
        pList = const_cast<ATOM::StyleParticle*>(pParticle->Next());
        pParticle->SetNext(NULL);
        return pParticle;
    }
    else { // Let's traverse into the list.
        ATOM::StyleParticle* pPrev = pParticle;
        pParticle = const_cast<ATOM::StyleParticle*>(pParticle->Next());
        while(pParticle != NULL) {
            if(pParticle->Type() == eType) {
                pPrev->SetNext(const_cast<ATOM::StyleParticle*>(pParticle->Next())); // jump over this particle
                pParticle->SetNext(NULL);          // and unlink it.
                return pParticle;
            }
            // Advance our two pointers.
            pPrev = pParticle;
            pParticle = const_cast<ATOM::StyleParticle*>(pParticle->Next());
        }

        // Not found.  Nothing to remove.  Let's say so.
        return NULL;
    }
}





StyleChangeElement::StyleChangeElement()
: m_pDeltas(0)
{
}

StyleChangeElement::~StyleChangeElement()
{
    Reset();
}

void StyleChangeElement::Reset()
{
    // Kill the deltas.
    while(m_pDeltas)
    {
        ATOM::StyleParticle* p = m_pDeltas;
        m_pDeltas = const_cast<ATOM::StyleParticle*>(p->Next());
        delete(p);
    }
}

void StyleChangeElement::Push(StyleChangeElement& /*from*/ oOther)
{
    // Is there a pending notification?
    // Did somebody forget to send out an ITextRun notification
    // before entering the nested context?
    assert(oOther.Deltas() == NULL);

    const ATOM::StyleParticle* pStyle = oOther.Description();
    while(pStyle != NULL) {
        AddToDescription(*pStyle);
        pStyle = pStyle->Next();
    }
}

// Push from the environment (ie, initialize outermost context)
void StyleChangeElement::Push(ATOM::IEnvironment* pEnv)
{
    const ATOM::StyleParticle* pStyle = pEnv->AmbientStyle()->Description();
    while(pStyle != NULL) {
        AddToDescription(*pStyle);
        pStyle = pStyle->Next();
    }
}


void StyleChangeElement::Pop(StyleChangeElement& /*to*/ oOuter)
{
    // Is there a pending notification?
    // Did somebody forget to send out an ITextRun notification
    // before leaving the nested context?

    // TODO: evaluate this.  It's possible to emerge from two
    // consecutive contexts, the inner one pushing some deltas
    // into the list, and the outer running afoul of this check.
    //assert(Deltas() == NULL);

    const ATOM::StyleParticle* pStyle = Description();

    while(pStyle != NULL) {
        const ATOM::StyleParticle* pOuterStyle = oOuter.GetDescriptionParticle(pStyle->Type());
        // If the outer style contains this particle, but we've changed it
        // let's assert the delta back to that outer value.
        //
        // This doesn't detect the condition where we have
        // particle X but the outer context doesn't (it was first used
        // in our context.)  The outer Description() will still be
        // correct, but the Delta() just won't pick it up.  It's a matter
        // of the environment containing an AmbientStyle that is complete,
        // such that X always exists.  The problem is, we just don't know
        // what value (some default) to assign to that outer particle.
        if(pOuterStyle != NULL && !(*pStyle == *pOuterStyle)) {
            oOuter.AddDelta(*pOuterStyle);
        }

        pStyle = pStyle->Next();
    }
}



const ATOM::StyleParticle* StyleChangeElement::Deltas() const
{
    return m_pDeltas;
}


// TODO: see if there's something that can be done to consolidate this imp.
// (maybe inline?)
const ATOM::StyleParticle* StyleChangeElement::Description() const
{
    return StyleDescriptionElement::Description();
}

const ATOM::StyleParticle* StyleChangeElement::DescriptionParticle(ATOM::StyleParticle::StyleParticleType eType) const
{
    return StyleDescriptionElement::DescriptionParticle(eType);
}


ATOM::Status StyleChangeElement::AddDelta(const ATOM::StyleParticle& oParticle)
{
    if(m_pDeltas == NULL)
        m_pDeltas = oParticle.Clone();
    else
        m_pDeltas->Append(oParticle.Clone());

    return AddToDescription(oParticle);
}

// Gets the nth particle of the type indicated, from the Delta list.
ATOM::StyleParticle* StyleChangeElement::GetDeltaParticle(ATOM::StyleParticle::StyleParticleType eType,int n)
{
    ATOM::StyleParticle* pRet = this->m_pDeltas;
    for ( ; ; )
    {
        pRet = StyleChangeElement::GetParticle(eType,pRet);
        // Are we done?
        if(--n <= 0)
            break;

        // Can we continue?
        if(pRet)
            pRet = const_cast<ATOM::StyleParticle*>(pRet->Next());
    }

    return pRet;
}


//
EmptyStyleTable::EmptyStyleTable()
{
}


const ATOM::IStyleDescription* EmptyStyleTable::operator[] (const ATOM::StRange& /*sName*/) const
{
    // TO DO: implement the addition of styles
    return NULL;
}

ATOM::Status EmptyStyleTable::AddStyle(const ATOM::StRange& /*sName*/, const ATOM::IStyleDescription* /*pStyle*/)
{
    // TO DO: implement the addition of styles.
    return ATOM::Status::keNotImplemented;
}
