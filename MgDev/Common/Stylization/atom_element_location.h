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


