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


// Implement a dummy resolver, since most of the time,
// parsers won't need one.
ATOM::Status PassthroughReferenceResolverElement::Initialize()
{
    return ATOM::Status::keOk;
}

// Requests the resolver to resolve a reference.
ATOM::Status PassthroughReferenceResolverElement::Resolve(const ATOM::StRange /*sParserName*/,
                                                          const ATOM::StRange sReference,
                                                                ATOM::StRange& sResult,
                                                                ATOM::IEnvironment* /*pEnv*/)
{
    sResult = sReference;
    return ATOM::Status::keUnchanged;
}

// Allows the resolver to clean up.
ATOM::Status PassthroughReferenceResolverElement::Terminate()
{
    return ATOM::Status::keOk;
}

// This is just used for defaulting the environment.
PassthroughReferenceResolverElement  gDummyResolver;



EnvironmentElement::EnvironmentElement(ATOM::ISink* pSink,ATOM::IStyleTable* pStyleTable,ATOM::Color rgbaCanvas)
: m_pSink(pSink)
, m_pStyleTable(pStyleTable)
, m_pResolver(&gDummyResolver)
, m_rgbaCanvas(rgbaCanvas)
{
    assert(m_pSink);
    assert(m_pStyleTable);
}

ATOM::Status EnvironmentElement::SetSink(ATOM::ISink* pSink)
{
    // if this->m_pSink->SinkState() == keWaiting && pSink->SinkState() == keWaiting ...
    m_pSink = pSink;
    return ATOM::Status::keOk;
}


ATOM::Status EnvironmentElement::SetResolver(ATOM::IReferenceResolver* pResolver)
{
    m_pResolver = pResolver? pResolver : &gDummyResolver;
    return ATOM::Status::keOk;
}

ATOM::Status EnvironmentElement::UpdateAmbientStyle(const ATOM::StyleParticle& oParticle)
{
    return m_AmbientStyle.AddToDescription(oParticle);
}

ATOM::Status EnvironmentElement::UpdateAmbientTransform(const ATOM::TransformParticle& oParticle)
{
    return m_AmbientTransform.AddTransform(oParticle);
}

const ATOM::IStyleDescription* EnvironmentElement::AmbientStyle() const
{
    return &m_AmbientStyle;
}
const ATOM::ITransform* EnvironmentElement::AmbientTransform() const
{
    return &m_AmbientTransform;
}

const ATOM::IStyleTable*       EnvironmentElement::StyleDictionary() const
{
    return m_pStyleTable;
}


ATOM::ISink* EnvironmentElement::Sink() const
{
    return m_pSink;
}


ATOM::IReferenceResolver* EnvironmentElement::References() const
{
    return m_pResolver;
}

ATOM::Color EnvironmentElement::CanvasColor() const
{
    return m_rgbaCanvas;
}
