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

#include "stdafx.h"
#include "atom.h"
#include "atom_element.h"
#include <assert.h>


void TextRunElement::Push()
{
    // If this is already the outermost context
    // let's not waste any time, because there's
    // nothing out there to draw from.
    if(m_pParent == NULL)
        return;

    // Is there a pending notification?
    // If this assertion fires, it's probably because
    // of a missing ITextRun call just beforehand;
    // theoretically, the markup that introduces a
    // recursive structure should also have caused
    // the previous TextRun to have notified the sink.
    assert(m_pParent->IsReset());
    // Contents aren't pushed.  Theoretically, there
    // shouldn't even be any.

    m_oLocation.Push(m_pParent->m_oLocation);
    m_oStyle.Push(m_pParent->m_oStyle);
    m_oTransform.Push(m_pParent->m_oTransform);
}

void TextRunElement::Push(ATOM::IEnvironment* pEnv)
{
    if(pEnv == NULL)
        return;

    // Contents aren't pushed.  Theoretically, there
    // shouldn't even be any.

    m_oLocation.Push(pEnv);
    m_oStyle.Push(pEnv);
    m_oTransform.Push(pEnv);
}



// This method identifies the differences between the inner (this)
// and outer (parent) context.  It "emphasizes" those differences
// by asserting them in the outer context's "delta" list.  Really,
// it's just self-reporting what's already out there, but allows
// that outer "delta" list to be present and correct.
void TextRunElement::Pop()
{
    // If this is already the outermost context
    // let's not waste any time reporting out
    // deltas.
    if(m_pParent == NULL)
        return;

    // Is there a pending notification?
    // If this assertion fires, it's probably because
    // of a missing ITextRun call just beforehand;
    // theoretically, the markup that introduces a
    // recursive structure should also have caused
    // the previous TextRun to have notified the sink.
    assert(IsReset());

    if(m_pParent) {
        m_oLocation.Pop(m_pParent->m_oLocation);
        m_oStyle.Pop(m_pParent->m_oStyle);
        m_oTransform.Pop(m_pParent->m_oTransform);
    }
}
