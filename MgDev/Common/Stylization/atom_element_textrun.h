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

#ifndef _atom_element_textrun_h
#define _atom_element_textrun_h

class TextRunElement: public ATOM::ITextRun
{
public:
    // Standard constructor, ignoring environment.
    TextRunElement()
    : m_iDepth(0)
    , m_pParent(NULL)
    {
        // Nothing to Push; this *is* the outermost context...
    }

    // Standard constructor, accessing evironment
    // for initial settings...
    // Use this for synchronizing with the ambient
    // style in the environment.
    // (Alternative: use default ctor above and
    // InitFrom(pEnv) below.)
    TextRunElement(ATOM::IEnvironment* pEnv)
    : m_iDepth(0)
    , m_pParent(NULL)
    {
        Push(pEnv);
    }

    // Recursion constructor: makes a snapshot of
    // the current (parent) state.  Parser should
    // resort to using this constructor when recursive
    // markup occurs (such as RTF's {...} or SVG's
    // nested elements.
    // (Alternative: use default ctor above and
    // InitFrom(pParent) below.)
    TextRunElement(TextRunElement* pParent)
    : m_iDepth(0)
    , m_pParent(NULL)
    {
        InitFrom(pParent);
    }

    // Clean up.
    ~TextRunElement()
    {
        Pop();
    }

    // Needed if you use the default constructor
    // and you're processing a nested context
    // of a nested markup language.
    void InitFrom(TextRunElement* pParent)
    {
        m_pParent = pParent;
        m_oStructure.SetOuter(&(m_pParent->m_oStructure));
        //m_iDepth = m_pParent->m_iDepth+1;

        Push();
    }

    // Needed if you use the default constructor
    // and you're processing the outermost context.
    void InitFrom(ATOM::IEnvironment* pEnv)
    {
        m_pParent = NULL;
        m_iDepth = 0;

        Push(pEnv);
    }


    // Writable (non-const) access to Location
    LocationElement& Location()
    {
        return m_oLocation;
    }

    // Writable (non-const) access to Contents
    ATOM::StRange& Contents()
    {
        return m_oContents;
    }

    // Writable (non-const) access to Style
    StyleChangeElement& Style()
    {
        return m_oStyle;
    }

    // Writable (non-const) access to Transform
    TransformElement& Transform()
    {
        return m_oTransform;
    }


public:
    // Implementations of the ITextRun interface
    const ATOM::IStructure* Structure() const
    {
        return &m_oStructure;
    }

    const ATOM::IStyleChange* Style() const
    {
        return &m_oStyle;
    }

    const ATOM::ITransformChange* Transform() const
    {
        return &m_oTransform;
    }

    const ATOM::StRange Contents() const
    {
        return m_oContents;
    }

    const ATOM::ILocation* Location() const
    {
        return &m_oLocation;
    }


public:
    // Cleanup needed between TextRun notifications.
    // Parser should call this after the notification
    // is complete in order to be ready for the next
    // text run.
    void Reset()
    {
        m_oLocation.Reset();
        m_oContents.Reset();

        m_oStyle.Reset();
        m_oTransform.Reset();
    }

    bool IsReset()
    {
        return m_oContents.Length() == 0;
    }

private:
    void Push(); // ... from nested context
    void Push(ATOM::IEnvironment*); // ... from outermost context (accessing Env's AmbientStyle)
    void Pop();

    StructureElement    m_oStructure;
    StyleChangeElement  m_oStyle;
    TransformElement    m_oTransform;
    LocationElement     m_oLocation;
    ATOM::StRange       m_oContents;

    int                 m_iDepth;

    TextRunElement*     m_pParent;
};

#endif//_atom_element_textrun_h
