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

#ifndef _atom_element_structure_h
#define _atom_element_structure_h

class StructureElement: public ATOM::IStructure
{
public:
    StructureElement();

    void SetOuter(StructureElement*);

    void SetShape(ATOM::Shape::Type eShape);

    void SetContinuous(bool);

public: // implementations of the IStructure interface;

    // Current depth within the markup.
    int         Depth() const
    {
        return m_iDepth;
    }

    // Pointer to an outer IStructure (with Depth()-1)
    ATOM::IStructure* Outer() const
    {
        return m_pOuter;
    }

    // What is the "shape" of the run?  Does it flow and wrap, or... ?
    ATOM::Shape::Type GetShape() const
    {
        return m_eShape;
    }

    // Is selection considered continuous with previous run?
    bool        Continuous() const
    {
        return m_bContinuous;
    }

private:
    StructureElement* m_pOuter;
    int               m_iDepth;
    ATOM::Shape::Type m_eShape;
    bool              m_bContinuous;
};

#endif//_atom_element_structure.h
