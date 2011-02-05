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

#ifndef _atom_element_style_h
#define _atom_element_style_h


/*
    This is an implementation of the ATOM IStyleDescription
    interface.  It's to be used by a parser in support of the
    parsing operation.
*/
class StyleDescriptionElement: public ATOM::IStyleDescription
{
public:
    StyleDescriptionElement();

    // List-copy constructor
    StyleDescriptionElement(const ATOM::StyleParticle* pOtherList);

    virtual ~StyleDescriptionElement();

    // Adds to the full Description of the style.  Employs "set" semantics,
    // meaning that only one particle of any given type resides in the set;
    // Any existing element of the same type is replaced.
    // The caller retains ownership of oParticle; the description will contain
    // its own clone.
    ATOM::Status AddToDescription(const ATOM::StyleParticle& oParticle);
    ATOM::Status RemoveFromDescription(const ATOM::StyleParticle::StyleParticleType eType);

    // Gets the first particle of the type indicated.
    ATOM::StyleParticle* GetDescriptionParticle(ATOM::StyleParticle::StyleParticleType eType) const;


public: // from ATOM::IStyleDescription
    const ATOM::StyleParticle* Description() const;

    const ATOM::StyleParticle* DescriptionParticle(ATOM::StyleParticle::StyleParticleType eType) const;


protected:
    // Utility functions.

    // Gets the first particle of the type indicated, but from the indicated list.
    static ATOM::StyleParticle* GetParticle(ATOM::StyleParticle::StyleParticleType eType,
                                            const ATOM::StyleParticle* pList);

    // Implements the set semantics, ensuring "uniqueness" of particle types in the set.
    static ATOM::Status AddToSet(ATOM::StyleParticle*& pSet, const ATOM::StyleParticle& oParticle);

    static ATOM::StyleParticle* RemoveFromList(ATOM::StyleParticle*& pList,
                                               const ATOM::StyleParticle::StyleParticleType eType);

private:
    ATOM::StyleParticle* m_pDescription; // current state
};





/*
    This is an implementation of the ATOM IStyleChange
    interface.  It's to be used by a parser in support of the
    parsing operation.
*/
class StyleChangeElement: public StyleDescriptionElement,
                          public ATOM::IStyleChange
{
public:
    StyleChangeElement();
    ~StyleChangeElement();

    // Adds to the Delta list, (as well as keeping the overall Description in sync.)
    // Particles are added to the end of the list (that is, the order is preserved)
    // The delta list does not maintain set semantics, so several (possibly redundant)
    // particles may co-exist here (but their manifestation in the Description remains
    // unique, with last-in prevailing.)
    ATOM::Status AddDelta(const ATOM::StyleParticle& oParticle);

    // Gets the nth particle of the type indicated, from the Delta list.
    ATOM::StyleParticle* GetDeltaParticle(ATOM::StyleParticle::StyleParticleType eType,int n=1);

    void Reset();

    // Push from some outer context
    void Push(StyleChangeElement&);
    // Push from the environment (ie, initialize outermost context)
    void Push(ATOM::IEnvironment*);

    // Pop style changes off, update outermost delta list.
    void Pop(StyleChangeElement&);



public: // from ATOM::IStyleChange (and IStyleDescription)
    const ATOM::StyleParticle* Description() const; // technically, duplicated...
    const ATOM::StyleParticle* DescriptionParticle(ATOM::StyleParticle::StyleParticleType eType) const;

    const ATOM::StyleParticle* Deltas() const;

private:
    ATOM::StyleParticle* m_pDeltas;
};



// Simple implementation:
// TO-DO: create add support.
class EmptyStyleTable: public ATOM::IStyleTable
{
public:
    EmptyStyleTable();

    const ATOM::IStyleDescription* operator[] (const ATOM::StRange& sName) const;

    ATOM::Status AddStyle(const ATOM::StRange& sName, const ATOM::IStyleDescription* pStyle);
};

#endif//_atom_element_style_h
