#ifndef _atom_element_abandonment_h
#define _atom_element_abandonment_h




class AbandonmentElement: public ATOM::IAbandonment {
public:
    AbandonmentElement(ATOM::Status);

    void SetMarkup(const ATOM::StRange& sEntireString);

    void SetContext(const ATOM::StRange& sCurrentLine);

    void SetPosition(const ATOM::StRange& sCurrentLine);

public: // from IAbandonment
    const ATOM::Status   Reason();

    const ATOM::StRange& Markup();

    const ATOM::StRange& Context();

    const ATOM::StRange& Position();

private:
    ATOM::Status         m_oStatus;

    ATOM::StRange   m_sMarkupString;      // generally, the entire string.
    ATOM::StRange   m_sMarkupContext;     // the context surrounding the abandonment
    ATOM::StRange   m_sAbandonedPosition; // The position at which abandonment occurred.
};


#endif//_atom_element_abandonment_h
