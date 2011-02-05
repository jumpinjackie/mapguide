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

#ifndef _atom_element_location_h
#define _atom_element_location_h

class LocationElement: public ATOM::ILocation
{
public:
    LocationElement();
    ~LocationElement();

    // Sets one (or a bunch) of semantics at once.
    void SetSemantics(ATOM::ILocation::SemanticType eType);

    // Adds one semantic at a time.
    void AddSemantic(ATOM::ILocation::SemanticType eType)
    {
        m_eSemantics =  (ATOM::ILocation::SemanticType)((m_eSemantics & ~ATOM::ILocation::keNormal)|eType);
    }

    bool AddOperation(const ATOM::LocationParticle& oParticle);

    void Reset();

    // Push from some outer location context.
    void Push(LocationElement&);
    // Push from the environment (ie, initialize outermost context)
    void Push(ATOM::IEnvironment*);

    void Pop(LocationElement&);

public: // Implementing ATOM::ILocation interface

    // Describes the nature of the location change.
    ATOM::ILocation::SemanticType Semantics() const;

    // Zero or more operations to effect the location change.
    ATOM::LocationParticle* Operations() const;

protected:
    bool AddToList(ATOM::LocationParticle*& pList, const ATOM::LocationParticle& oParticle);
private:
    // What does this location description mean?
    ATOM::ILocation::SemanticType m_eSemantics;

    // What are the operations that go into making this location change.
    ATOM::LocationParticle* m_pOperations;
};

#endif//_atom_element_location_h
