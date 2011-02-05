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

#ifndef _atom_element_universe_h
#define _atom_element_universe_h

ATOM::IUniverse* ATOM::BigBang();

#define MAX_PARSERS_IN_UNIVERSE 8

class UniverseElement /* would that be Hydrogen? ;-)*/ : public ATOM::IUniverse
{
public:
    UniverseElement();

public:
    // Registers a Parser's Generator, used by the parsing module
    // when introduced to the universe.
    ATOM::Status Register(ATOM::IGenerator*);

    // Unregisters a Parser's Generator.
    ATOM::Status Unregister(ATOM::IGenerator*);

    // How many parser/generators are registered?
    int RegisteredCount();

    // Gets a parser generator (by position in registration list)
    // to allow the application to begin a parsing operation.
    // 0 <= iIndex < RegisteredCount();
    //
    // Note: the iIndex is NOT a key, as registration may change
    // the order of Parser Generators... USE ONLY Name() to
    // get a persistent key for any specific Parser Generator.
    ATOM::IGenerator* GetGenerator(int iIndex);

    // Same as above, but indexed off the IGenerator::Name()
    // method.
    ATOM::IGenerator* GetGenerator(const ATOM::StRange& sName);

private:
    // Cheap implementation, just a hard array.
    // Growing "array list" might be called upon in the future
    // but fortunately, the specifics are hidden behind the interface. ;-)
    ATOM::IGenerator* m_pRegistrants[MAX_PARSERS_IN_UNIVERSE];
    int m_iCount;

    ATOM::IGenerator** Find(const ATOM::StRange& sName);
    ATOM::IGenerator** FindEmpty();
};

#endif//_atom_element_universe_h
