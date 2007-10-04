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
        m_iDepth = m_pParent->m_iDepth+1;

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

    int Depth() const
    {
        return m_iDepth;
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

    StyleChangeElement  m_oStyle;
    TransformElement    m_oTransform;
    LocationElement     m_oLocation;
    ATOM::StRange       m_oContents;

    int                 m_iDepth;

    TextRunElement*     m_pParent;
};


#endif//_atom_element_textrun_h
