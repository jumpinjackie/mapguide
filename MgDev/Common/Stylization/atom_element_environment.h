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

#ifndef _atom_element_environment_h
#define _atom_element_environment_h


/*
    This is an implementation of the ATOM IEnvironment
    interface.  It's to be used by a parser in support of the
    parsing operation.
*/
class EnvironmentElement: public ATOM::IEnvironment {
public:

    EnvironmentElement(ATOM::ISink* pSink,ATOM::IStyleTable* pStyleTable,ATOM::Color rgbaCanvas);

    ATOM::Status SetSink(ATOM::ISink* pSink);

    ATOM::Status SetResolver(ATOM::IReferenceResolver*);

    ATOM::Status UpdateAmbientStyle(const ATOM::StyleParticle& oParticle);

    ATOM::Status UpdateAmbientTransform(const ATOM::TransformParticle& oParticle);


public: // interface implementation
    const ATOM::IStyleDescription*  AmbientStyle()    const;

    const ATOM::ITransform*         AmbientTransform()const;

    const ATOM::IStyleTable*        StyleDictionary() const;

          ATOM::ISink*              Sink()            const;

          ATOM::IReferenceResolver* References()      const;

          ATOM::Color               CanvasColor()     const;

private:
    StyleDescriptionElement     m_AmbientStyle;
    TransformElement            m_AmbientTransform;
    ATOM::IStyleTable*          m_pStyleTable;
    ATOM::ISink*                m_pSink;
    ATOM::IReferenceResolver*   m_pResolver;
    ATOM::Color                 m_rgbaCanvas;
};


// A dummy resolver, just gives back what you passed in.
class PassthroughReferenceResolverElement: public ATOM::IReferenceResolver
{
public:
    ATOM::Status Initialize();

    // Requests the resolver to resolve a reference.
    ATOM::Status Resolve(const ATOM::StRange sParserName,
                         const ATOM::StRange sReference,
                               ATOM::StRange& sResult,
                               ATOM::IEnvironment* pEnv);

    // Allows the resolver to clean up.
    ATOM::Status Terminate();
};

#endif//_atom_element_environment_h
