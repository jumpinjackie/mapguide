#include "stdafx.h"
#include "assert.h"

#include "atom.h"
#include "atom_element.h"


AbandonmentElement::AbandonmentElement(ATOM::Status o)
: m_oStatus(o)
{
}

void AbandonmentElement::SetMarkup(const ATOM::StRange& sEntireString)
{
    this->m_sMarkupString = sEntireString;
    this->m_sMarkupContext = sEntireString; // default to something; SetContext can refine.
}

void AbandonmentElement::SetContext(const ATOM::StRange& sCurrentLine)
{
    this->m_sMarkupContext = sCurrentLine;
}

void AbandonmentElement::SetPosition(const ATOM::StRange& sPosition)
{
    this->m_sAbandonedPosition = sPosition;
}


const ATOM::Status AbandonmentElement::Reason()
{
    return this->m_oStatus;
}


const ATOM::StRange& AbandonmentElement::Markup()
{
    return this->m_sMarkupString;
}

const ATOM::StRange& AbandonmentElement::Context()
{
    return this->m_sMarkupContext;
}

const ATOM::StRange& AbandonmentElement::Position()
{
    return this->m_sAbandonedPosition;
}


