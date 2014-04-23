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
