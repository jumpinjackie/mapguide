#ifndef _atom_element_universe_h
#define _atom_element_universe_h

ATOM::IUniverse* ATOM::BigBang();

#define MAX_PARSERS_IN_UNIVERSE 8

class UniverseElement /* would that be Hydrogen? ;-)*/ : public ATOM::IUniverse
{
public:
    UniverseElement();

public:
    // Registers a ParserGenerator, used by the parsing module
    // when introduced to the universe.
    ATOM::Status Register(ATOM::IParserGenerator*);

    // Unregisters a ParserGenerator.
    ATOM::Status Unregister(ATOM::IParserGenerator*);

    // How many parser/generators are registered?
    int RegisteredCount();

    // Gets a parser generator (by position in registration list)
    // to allow the application to begin a parsing operation.
    // 0 <= iIndex < RegisteredCount();
    //
    // Note: the iIndex is NOT a key, as registration may change
    // the order of Parser Generators... USE ONLY Name() to
    // get a persistent key for any specific Parser Generator.
    ATOM::IParserGenerator* GetGenerator(int iIndex);

    // Same as above, but indexed off the IParserGenerator::Name()
    // method.
    ATOM::IParserGenerator* GetGenerator(const ATOM::StRange& sName);

private:
    // Cheap implementation, just a hard array.
    // Growing "array list" might be called upon in the future
    // but fortunately, the specifics are hidden behind the interface. ;-)
    ATOM::IParserGenerator* m_pRegistrants[MAX_PARSERS_IN_UNIVERSE];
    int m_iCount;

    ATOM::IParserGenerator** Find(const ATOM::StRange& sName);
    ATOM::IParserGenerator** FindEmpty();
};



#endif//_atom_element_universe_h


