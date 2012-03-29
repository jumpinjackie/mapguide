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


TransformElement::TransformElement()
: m_pDescription(NULL)
, m_pDeltas(NULL)
, m_oMatrix(m_nMatrixElements)
, m_eMatrixComposition(ATOM::TransformParticle::keNone)
, m_bMatrixSynced(false)
{
}

TransformElement::~TransformElement()
{
    // Kill the description.
    while(m_pDescription) {
        ATOM::TransformParticle* p = m_pDescription;
        m_pDescription = const_cast<ATOM::TransformParticle*>(p->Next());
        delete(p);
    }

    Reset();
}

void TransformElement::Reset()
{
    // Only kill the deltas, the full description
    // needs to persist between runs.
    while(m_pDeltas) {
        ATOM::TransformParticle* p = m_pDeltas;
        m_pDeltas = const_cast<ATOM::TransformParticle*>(p->Next());
        delete(p);
    }
}

void TransformElement::Push(TransformElement& /*from*/ oOther)
{
    // Transform needs to be slurped.
    const ATOM::TransformParticle* pTransform = oOther.Description();
    while(pTransform != NULL) {
        AddTransform(*pTransform);
        pTransform = pTransform->Next();
    }
}

// Push from the environment (ie, initialize outermost context)
void TransformElement::Push(ATOM::IEnvironment* /*pEnv*/)
{
    // Transform isn't slurped (yet?) ..
}

void TransformElement::Pop(TransformElement& /*to*/ /*oOther*/)
{
}



ATOM::Status TransformElement::AddTransform(const ATOM::TransformParticle& oParticle)
{
    ATOM::Status eRet;
    eRet = AddToList(m_pDescription,oParticle);
    if(!eRet.Succeeded())
        return eRet;

    eRet = AddToList(m_pDeltas,oParticle);
    if(!eRet.Succeeded())
        return eRet;

    this->m_bMatrixSynced = false;

    return ATOM::Status::keOk;
}

ATOM::Status TransformElement::RemoveIdenticalTransform(const ATOM::TransformParticle& oParticle)
{
    ATOM::TransformParticle* pGone = RemoveFromList(m_pDescription,oParticle);
    if(pGone != NULL) {
        this->m_bMatrixSynced = false;
        delete(pGone);
    }

    return AddToList(m_pDeltas,oParticle);
}

ATOM::Status TransformElement::RemoveSameTypeTransform(const ATOM::TransformParticle& oParticle)
{
    ATOM::TransformParticle* pGone = RemoveFromList(m_pDescription,oParticle.Type());
    if(pGone != NULL) {
        this->m_bMatrixSynced = false;
        delete(pGone);
    }

    return AddToList(m_pDeltas,oParticle);
}


ATOM::Status TransformElement::ReplaceTransform(const ATOM::TransformParticle& oParticle)
{
    if(ReplaceInList(m_pDescription,oParticle) != NULL)
        this->m_bMatrixSynced = false;

    return AddToList(m_pDeltas,oParticle);
}




ATOM::TransformParticle* TransformElement::GetParticle(ATOM::TransformParticle::TransformParticleType eType,
                                                       ATOM::TransformParticle* pList)
{
    while(pList != NULL) {
        if(pList->Type() == eType)
            return pList;
        else
            pList = const_cast<ATOM::TransformParticle*>(pList->Next());
    }
    return NULL;
}


ATOM::Status TransformElement::AddToList(ATOM::TransformParticle*& pList, const ATOM::TransformParticle& oParticle)
{
    if(pList != NULL)
        pList->Append(oParticle.Clone());
    else // (or start a new list with a clone.)
        pList = oParticle.Clone();

    return ATOM::Status::keOk;
}

ATOM::TransformParticle* TransformElement::RemoveFromList(ATOM::TransformParticle*& pList, const ATOM::TransformParticle& oParticle)
{
    // Empty List, nothing to delete.
    if(pList == NULL)
        return NULL;

    ATOM::TransformParticle* pParticle = pList;
    // Need to remove the head of the list?
    if(*pParticle == oParticle) {
        pList = const_cast<ATOM::TransformParticle*>(pParticle->Next());
        pParticle->SetNext(NULL);
        return pParticle;
    }
    else { // Let's traverse into the list.
        ATOM::TransformParticle* pPrev = pParticle;
        pParticle = const_cast<ATOM::TransformParticle*>(pParticle->Next());
        while(pParticle != NULL) {
            if(*pParticle == oParticle) {
                pPrev->SetNext(const_cast<ATOM::TransformParticle*>(pParticle->Next())); // jump over this particle
                pParticle->SetNext(NULL);          // and unlink it.
                return pParticle;
            }
            // Advance our two pointers.
            pPrev = pParticle;
            pParticle = const_cast<ATOM::TransformParticle*>(pParticle->Next());
        }

        // Not found.  Nothing to remove.  Let's say so.
        return NULL;
    }
}

ATOM::TransformParticle* TransformElement::RemoveFromList(ATOM::TransformParticle*& pList,
                                                          const ATOM::TransformParticle::TransformParticleType eType)
{
    // Empty List, nothing to delete.
    if(pList == NULL)
        return NULL;

    ATOM::TransformParticle* pParticle = pList;
    // Need to remove the head of the list?
    if(pParticle->Type() == eType) {
        pList = const_cast<ATOM::TransformParticle*>(pParticle->Next());
        pParticle->SetNext(NULL);
        return pParticle;
    }
    else { // Let's traverse into the list.
        ATOM::TransformParticle* pPrev = pParticle;
        pParticle = const_cast<ATOM::TransformParticle*>(pParticle->Next());
        while(pParticle != NULL) {
            if(pParticle->Type() == eType) {
                pPrev->SetNext(const_cast<ATOM::TransformParticle*>(pParticle->Next())); // jump over this particle
                pParticle->SetNext(NULL);          // and unlink it.
                return pParticle;
            }
            // Advance our two pointers.
            pPrev = pParticle;
            pParticle = const_cast<ATOM::TransformParticle*>(pParticle->Next());
        }

        // Not found.  Nothing to remove.  Let's say so.
        return NULL;
    }
}


// Replaces the particle in the list with one that is of the same type as the particle given;
// if one is not found, the particle is added.
ATOM::TransformParticle* TransformElement::ReplaceInList(ATOM::TransformParticle*& pList, const ATOM::TransformParticle& oParticle)
{
    ATOM::TransformParticle* pInList = GetParticle(oParticle.Type(),pList);
    if(pInList != NULL)
        *pInList = oParticle;
    else
        AddToList(pList,oParticle);

    return pInList;
}


// Builds the matrix represented as the product of all TransformParticles, taken in order.
// This implementation caches the matrix for repeated queries (as the transform may be in
// effect over several runs, each of which would theoretically want to query the matrix.)
ATOM::TransformParticle::TransformParticleType TransformElement::AsMatrix(ATOM::Matrix* pMat)  const
{

    if(!this->m_bMatrixSynced) {
        // Defy our "const" status for this, to update the cache.
        TransformElement* pThis = const_cast<TransformElement*>(this);
        pThis->m_eMatrixComposition = ATOM::TransformParticle::keNone;
        pThis->m_oMatrix.SetIdentity();

        const ATOM::TransformParticle* pParticle = m_pDescription;
        while(pParticle != NULL) {
            // Save ourselves a bunch of multiplies against
            // an identity matrix.
            if(!pParticle->IsIdentity()) {
                ATOM::NUMBER n[9];
                ATOM::Matrix m(n);
                m.SetIdentity();
                    pParticle->SetMatrix(m);

                pThis->m_oMatrix *= m;
                pThis->m_eMatrixComposition = (ATOM::TransformParticle::TransformParticleType)
                    ((int) pThis->m_eMatrixComposition | (int)pParticle->Type());
            }

            pParticle = pParticle->Next();
            pThis->m_bMatrixSynced = true;
        }
    }
    if(pMat != NULL)
        *pMat = this->m_oMatrix;

    return this->m_eMatrixComposition;
}

const ATOM::TransformParticle* TransformElement::Description() const
{
    return this->m_pDescription;
}

const ATOM::TransformParticle* TransformElement::Deltas() const
{
    return this->m_pDeltas;
}
