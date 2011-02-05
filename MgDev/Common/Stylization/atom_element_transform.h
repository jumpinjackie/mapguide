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

#ifndef _atom_element_transform_h
#define _atom_element_transform_h


class TransformElement: public ATOM::ITransformChange
{
public:
    TransformElement();

    virtual ~TransformElement();

    // Indiscriminately adds a transform to the (end of the) list
    ATOM::Status AddTransform(const ATOM::TransformParticle& oParticle);

    // Removes first found transform exactly == to the one given.  Adds oParticle into delta list
    ATOM::Status RemoveIdenticalTransform(const ATOM::TransformParticle& oParticle);
    // Removes first-found transform with same type. Adds oParticle into delta list.
    ATOM::Status RemoveSameTypeTransform(const ATOM::TransformParticle& oParticle);

    // Removes first found transform of the type indicated.
    //void RemoveTransform(ATOM::TransformParticle::TransformParticleType eType);

    // Replaces first found transform of the type of the given particle with the particle.
    // (or adds it to the list if no match exists yet.)
    // Adds oParticle into delta list.
    ATOM::Status ReplaceTransform(const ATOM::TransformParticle& oParticle);

    void Reset();

    void Push(TransformElement&);
    // Push from the environment (ie, initialize outermost context)
    void Push(ATOM::IEnvironment*);

    void Pop(TransformElement&);

protected:
    //
    // Utility functions
    //

    // Gets a particle in the list that has the type indicated.
    static ATOM::TransformParticle* GetParticle(ATOM::TransformParticle::TransformParticleType eType,
                                                ATOM::TransformParticle* pList);
    // Gets a particle in the list that is equal to the one indicated.
    //static ATOM::TransformParticle* GetParticle(const ATOM::TransformParticle& oParticle,
    //                                            ATOM::TransformParticle* pList);

    //  Adds the particle to the list.
    static ATOM::Status AddToList(ATOM::TransformParticle*& pList, const ATOM::TransformParticle& oParticle);

    // Removes the particle from the list that is equal to the particle given.
    static ATOM::TransformParticle* RemoveFromList(ATOM::TransformParticle*& pList, const ATOM::TransformParticle& oParticle);

    // Removes the particle from the list that has the same type as given.
    static ATOM::TransformParticle* RemoveFromList(ATOM::TransformParticle*& pList, const ATOM::TransformParticle::TransformParticleType eType);

    // Replaces the particle in the list with one that is of the same type as the particle given;
    // if one is not found, the particle is added.
    static ATOM::TransformParticle* ReplaceInList(ATOM::TransformParticle*& pList, const ATOM::TransformParticle& oParticle);


public:
    // Implementing the ITransform interface
    //

    // Gets the aggregate transformation matrix.
    ATOM::TransformParticle::TransformParticleType       AsMatrix(ATOM::Matrix* ) const; // describes only the sum of these TransformationElements

    // Gets the list of constituent transformations that go into the matrix.
    const ATOM::TransformParticle*                       Description()           const;

    // Gets the list of changes that that were reported.
    const ATOM::TransformParticle*                       Deltas()                const;

private:
    ATOM::TransformParticle* m_pDescription;
    ATOM::TransformParticle* m_pDeltas;

    // Caching support for the Matrix method.
    ATOM::NUMBER m_nMatrixElements[9];
    ATOM::Matrix m_oMatrix;
    ATOM::TransformParticle::TransformParticleType m_eMatrixComposition;
    bool m_bMatrixSynced;
};

#endif//_atom_element_transform_h
