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

#ifndef _ATOM_H
#define _ATOM_H

#include <math.h>
#include <wchar.h>
#include <string.h>

namespace RichText { namespace ATOM
{

/**********************************************************************
 *
 *  BASIC TYPES
 *
 **********************************************************************/

#define LENCALL wcslen
typedef wchar_t CHARTYPE;

typedef float           NUMBER;  // a raw, scalar number.


/**********************************************************************
 *
 *  SUBSTRING RANGE REFERENCES
 *
 **********************************************************************/

/*
Chances are that parsing is happening inside of a buffer somewhere.
Rather than slicing and dicing new buffers, it's often practical to
just point to a substring within that buffer with pointer+length
semantics, which is what this class does.

NOTE: These objects are not assumed to own the referenced string.
That ownership must be managed separately (and the lifetime of that
string needs to exceed the lifetime of this reference to it.)
This is not usually a problem for string literals (lifetime==application
address space) or the buffer being worked on (lifetime >= parse.)

Furthermore, these objects make no assumptions about encoding, so
users of these types do need to concern themselves with "complex"
characters (such as multi-octet UTF-8 sequences, or UTF-16 Surrogate
Pairs) making sure that character is fully contained within the
string range, and doesn't straddle the start or end boundary.

Finally, while the StRange doesn't explicitly codify or enforce it,
the ATOM interface assumes all strings, regardless of encoding, represent
UNICODE codepoints.  It becomes the parser's responsibility to convert
from another character set into Unicode, and it's the sink's responsibility
to convert to another codepoint.
*/
class StRange // STRing RANGEs...
{
public:
    // Default constructor: a null string.
    StRange()
    : m_pszStart(0)
    , m_iLength(0)
    {
    }

    // Copy constructor
    StRange(const StRange& oOther)
    : m_pszStart(oOther.m_pszStart)
    , m_iLength(oOther.m_iLength)
    {
    }

    // Typically used to coerce string "literals"
    StRange(const CHARTYPE* pszLiteral)
    : m_pszStart(pszLiteral)
    , m_iLength(0)
    {
        if(pszLiteral)
            m_iLength = (int)LENCALL(pszLiteral);
    }

    StRange(const CHARTYPE* pszOther,int iLen)
    : m_pszStart(pszOther)
    , m_iLength(iLen >= 0? iLen : 0)
    {
    }
    StRange(const CHARTYPE* pszStart,const CHARTYPE* pszEnd)
    : m_pszStart(pszStart)
    , m_iLength(pszEnd >= pszStart? (int)(pszEnd - pszStart + 1) : 0)
    {
    }

    //
    //  ACCESSORS
    //

    // Access the start of the string range
    const CHARTYPE* Start() const
    {
        return m_pszStart;
    }

    // Access the end of the string range
    // (the last character in the range indicated.)
    // Only valid if the string isn't empty.
    //
    // Note: End is a completely synthetic
    // characteristic of the StRange; it's
    // just a manifestation of start+len,
    // hiding the invariable +1/-1isms.
    const CHARTYPE* End() const
    {
        if(m_iLength > 0)
            return m_pszStart + m_iLength - 1;
        else
            return NULL;
    }

    // Access the length of the string
    int Length() const
    {
        return m_iLength;
    }

    // Access characters within the range.
    // From this perspective, anything out of
    // bounds is non-existant: char 0 is
    // returned.
    CHARTYPE operator[] (int iIndex) const
    {
        if(iIndex >=0 && iIndex < m_iLength)
            return m_pszStart[iIndex];
        else
            return 0;
    }

    // The nth character beyond the end of
    // the StRange.
    const CHARTYPE* Beyond(int i = 0) const
    {
        return m_pszStart + m_iLength + i;
    }

    // Last() - returns the Last character
    // Last(2) - returns the second-to-last character, etc.
    // Like reverse [] notation, except 1-based.
    CHARTYPE Last(int iIndex = 1) const
    {
        return operator[](Length()-iIndex);
    }

    // Takes a "part" (ie, substring) of
    // the indicated string range.
    StRange Part(int iStart,int iLen) const
    {
        if(iStart < 0 || iStart > m_iLength)
            return StRange();

        if(iStart + iLen > m_iLength)
            iLen = m_iLength - iStart;

        return StRange(m_pszStart+iStart,iLen);
    }
    // Takes a "part" (ie substring) to end
    StRange Part(int iStart) const
    {
        if(iStart < 0 || iStart > m_iLength)
            return StRange();

        return StRange(m_pszStart+iStart,m_iLength-iStart);
    }

    // Finds the first occurance of chFind in the range
    // Returns a pointer to that character, or NULL if
    // not found.
    const CHARTYPE* Find(const CHARTYPE chFind) const
    {
        for(int i=0; i<m_iLength; ++i)
        {
            if(m_pszStart[i] == chFind)
                return m_pszStart+i;
        }

        return NULL;
    }

    // Finds the first occurance of sFind in the range
    // Returns a pointer to the start of that string,
    // or NULL if not found.
    const CHARTYPE* Find(const StRange sFind) const
    {
        int iMax = m_iLength - sFind.Length();
        if(iMax >= 0) {
            StRange sPart = Part(0,sFind.Length());
            for(int i=0; i<= iMax; ++i, sPart.Move(1)) {
                if(sPart == sFind)
                    return sPart.Start();
            }
        }
        return NULL;
    }

    //
    // MUTATORS
    //

    // Resets the string to an "uninitialized"
    // state (ie, not pointing to any buffer.)
    void Reset()
    {
        m_pszStart = 0;
        m_iLength = 0;
    }

    // General-purpose mutator: sets the start
    // and length.
    void Set(const CHARTYPE* pszStart,int iLen)
    {
        m_pszStart = pszStart;
        m_iLength  = iLen >= 0? iLen : 0;
    }

    void Set(const CHARTYPE* pszStart,const CHARTYPE* pszEnd)
    {
        m_pszStart = pszStart;
        m_iLength  = pszEnd >= pszStart? (int)(pszEnd - pszStart + 1) : 0;
    }

    // Sets the start; length remains unchanged
    void SetStart(const CHARTYPE* pszStart)
    {
        m_pszStart = pszStart;
    }

    // Sets the length based on the intended
    // "end" character.  "Backwards" strings
    // are not allowed.
    void SetEnd(const CHARTYPE* pszEnd)
    {
        if(pszEnd >= m_pszStart)
            m_iLength = (int)(pszEnd - m_pszStart + 1);
        else
            m_iLength = 0;
    }

    // Sets the start to a new string buffer,
    // same as Set(pszStart,LENCALL(pszStart))
    void SetString(const CHARTYPE* pszStart)
    {
        m_pszStart = pszStart;
        m_iLength = (pszStart)?(int)(LENCALL(pszStart)) : 0;
    }

    // Moves the entire string range iChars.
    void Move(int iChars)
    {
        m_pszStart += iChars;
    }

    // Moves only the start, end remains set.
    void MoveStart(int iChars)
    {
        m_pszStart += iChars;
        m_iLength  -= iChars;
        if(m_iLength < 0)
            m_iLength = 0;
    }

    // Sets only the length of the range.
    void SetLength(int iLen)
    {
        m_iLength = iLen >= 0? iLen : 0;
    }

    // Adds or removes from the length of
    // the range.
    void AddLength(int iLen)
    {
        m_iLength = (m_iLength + iLen >= 0)? m_iLength + iLen : 0;
    }


    //  Assignment/copy operator.
    StRange& operator= (const StRange& oOther)
    {
        this->m_pszStart = oOther.m_pszStart;
        this->m_iLength  = oOther.m_iLength;
        return *this;
    }

    // Equality operator.  Note that, with the
    // StRange(const CHARTYPE* pszLiteral), this
    // also allows operations against string literals,
    // so myStRange == L"Foo" works.
    bool operator== (const StRange& oOther) const
    {
        if(this->m_pszStart == oOther.m_pszStart
            && this->m_iLength == oOther.m_iLength)
            return true;
        else if(this->m_iLength != oOther.m_iLength)
            return false;
        else
        {
            for(int i=0; i<this->m_iLength; ++i)
            {
                if(this->m_pszStart[i] != oOther.m_pszStart[i])
                    return false;
            }
            return true;
        }
    }

    // Syntactic sugar: avoids (!(str == other))
    bool operator!= (const StRange& oOther) const
    {
        return !(operator==)(oOther);
    }

    //
    // Parsing Utilities
    //

    // Split a string along a character separator
    // Returns a range pointing to the String Range
    // occurring before the separator, and moves
    // the start of this string past the separator.
    StRange Split(const CHARTYPE chSep)
    {
        ATOM::StRange sRet;
        const CHARTYPE* p = Find(chSep);

        if(p != NULL) {
            sRet.Set(m_pszStart,p-1);
            MoveStart(sRet.Length()+1);
        }
        else {
            sRet = *this;
            MoveStart(sRet.Length());
        }
        return sRet;
    }

    // Split a string along a string range separator
    // Returns a range pointing to the String Range
    // occurring before the separator, and moves
    // the start of this string past the separator.
    StRange Split(const StRange sSep)
    {
        ATOM::StRange sRet;
        const CHARTYPE* p = Find(sSep);

        if(p != NULL) {
            sRet.Set(m_pszStart,p-1);
            MoveStart(sRet.Length()+sSep.Length());
        }
        else {
            sRet = *this;
            MoveStart(sRet.Length());
        }
        return sRet;
    }


private:
    const wchar_t* m_pszStart;
    int            m_iLength;
};


/**********************************************************************
 *
 *  SIMPLE MATRIX CLASS
 *
 **********************************************************************/

// This is a 3x3 matrix
#define elem_00 0
#define elem_01 1
#define elem_02 2
#define elem_10 3
#define elem_11 4
#define elem_11 4
#define elem_12 5
#define elem_20 6
#define elem_21 7
#define elem_22 8
// Essentially just wraps a 9-element array of NUMBERS;
// provides basic operator access.  Note: Matrix never
// "owns" the array it refers to.
class Matrix
{
public:
    // Standard constructor: gimme a 9-element
    // array, and I'll give you a matrix.
    Matrix(NUMBER* pElements)
    : m_pElements(pElements)
    {
    }

    // Allows derivations to take ownership
    // of their own arrays, if desired.
    virtual ~Matrix() {}

    // Treat the Matrix as an array of numbers
    operator NUMBER* () const
    {
        return m_pElements;
    }

    // Copy/assignment operator.
    Matrix& operator= (const Matrix& oOther)
    {
        for(int i=0; i<9; ++i)
            this->m_pElements[i] = oOther.m_pElements[i];

        return *this;
    }

    // Assignment operator, assign from an array.
    Matrix& operator= (const NUMBER* m)
    {
        for(int i=0; i<9; ++i)
            this->m_pElements[i] = m[i];

        return *this;
    }

    // Standard matrix multiply: M = M * N
    // right-hand-side is an array of numbers.
    void operator*= (const NUMBER* o);

    // Standard matrix multiply: M = M * N
    // right-hand-side is another matrix.
    void operator*= (const Matrix& oOther)
    {
        // Borrow the array overload
        (this->operator*=)((NUMBER*)(oOther));
    }

    // Access a row, as an array.  Allows
    // myMatrix[iRow][iCol] access.
    NUMBER* operator[] (int iRow) const
    {
        return m_pElements + iRow*3;
    }

    // Populates the matrix with all zeros.
    void SetZero();

    // Populates the matrix with the identity matrix.
    // (1's along row==col axis, 0's elsewhere.)
    void SetIdentity();

private:
    NUMBER* m_pElements;
};



/**********************************************************************
 *
 *  MEASURES
 *
 **********************************************************************/
// A Measure is basically a numeric quantity and a unit measure.
// There's no one canonical coordinate space amongst the markup grammars,
// so this type endeavors to describe more than just the numerical quantity
// associated with a measure.  The Sink is in a better position to reconcile
// model-space coordinates with device-space coordinates, or do proportional
// computations.
//
// To put it another way: is a font size of "12" really twelve points, twelve
// pixels, twelve twips, or ...?
//
// There's also an option to point to the original string representation in
// the markup.
class Measure
{
public:
    enum UnitType {
        keUnitless, // Unitless; just a number

        keModel,    // Units of the surrounding model space

        keDevice,   // Physical device units
        kePoints,   // 1/72 of an inch; Twips = Points * 20, Pica = 12 Points
                    //                                             qv http://en.wikipedia.org/wiki/Point_%28typography%29

        keEm,       // an "em" (1.0 times height of current font.) qv http://en.wikipedia.org/wiki/Em_%28typography%29
        keEx,       // The x-height (height of lower-case letter)  qv http://en.wikipedia.org/wiki/X-height

        keProportion,// Scale, 1 = normal (100%), 2 = twice normal, etc.
    };

    // Default constructor: a unitless zero.
    Measure()
    : m_nNumber(0)
    , m_eUnits(keUnitless)
    {}

    // Standard constructor: number, unit, and the source reference (which can be NULL)
    Measure(NUMBER nNum, UnitType eUnit, const StRange* pRef)
    : m_nNumber(nNum)
    , m_eUnits(eUnit)
    {
       if(pRef)
            m_oRef = *pRef;
    }


    NUMBER         Number()     const // The numeric part of the measurement.
    { return m_nNumber; }

    UnitType       Units()      const // The units the number is expressed in.
    { return m_eUnits; }

    const StRange& Reference()  const // The parser's string reference
    { return m_oRef; }

    Measure& operator= (const Measure& oOther)
    {
        m_nNumber = oOther.m_nNumber;
        m_eUnits  = oOther.m_eUnits;

        return *this;
    }

    bool operator== (const Measure& oOther) const
    {
        return m_nNumber == oOther.m_nNumber
            && m_eUnits  == oOther.m_eUnits;  // m_oRef isn't comparable.
    }

private:
    StRange  m_oRef;
    NUMBER   m_nNumber;
    UnitType m_eUnits;
};


/**********************************************************************
 *
 *  RADIAL MEASURES
 *
 **********************************************************************/
// This encapsulates the computation from "human" units (degrees, gon)
// to the more typical units employed in computation (radians.)  The design
// is to keep the "type" of units in an angular measure in the developer's
// mind, without having to remember the math.
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif//M_PI

// These three light-weight classes (no virtual methods, no complex
// operations) are defined inline for best performance.
// The ATOM interface is defined in terms of this RadialMeasure, but
// two derivations can be used that mask the conversion to/from Degrees
// or Gon (aka Grad, Gradient, ...)
class RadialMeasure
{
public:
    RadialMeasure(NUMBER nRadians)
    : m_nRadians(nRadians)
    {}

    RadialMeasure(const RadialMeasure& oOther)
    : m_nRadians(oOther.m_nRadians)
    {
    }

    NUMBER Radians() const
    {
        return m_nRadians;
    }

    RadialMeasure& operator= (const RadialMeasure& oOther)
    {
        m_nRadians = oOther.m_nRadians;
        return *this;
    }

    bool operator==(const RadialMeasure& o) const
    {
        return m_nRadians == o.m_nRadians;
    }
    // For other relops, use Radians() or deriv::Angle().

protected:
    NUMBER m_nRadians;
};

// If you prefer to deal with degrees, you can provide
// one of these in lieu of a plain ol' radial measure.
// usage:
//  SomethingTakingRadialMeasure(DegreeRadialMeasure(45));
// and:
//  DegreeRadialMeasure deg = SomethingReturningRadialMeasure();
//  NUMBER degrees = deg.Angle()
class DegreeRadialMeasure: public RadialMeasure
{
public:
    DegreeRadialMeasure(NUMBER nDegrees)
    : RadialMeasure(ToRadians(nDegrees))
    {}

    DegreeRadialMeasure(const RadialMeasure& oOther)
    : RadialMeasure(oOther)
    {
    }

    DegreeRadialMeasure& operator= (NUMBER nDegrees)
    {
        m_nRadians = ToRadians(nDegrees);
    }

    bool operator == (NUMBER nDegrees)
    {
        return m_nRadians == ToRadians(nDegrees);
    }

    operator NUMBER ()
    {
        return Angle();
    }

    NUMBER Angle()
    {
        return (NUMBER)(m_nRadians * 180 / M_PI);
    }

private:
    NUMBER ToRadians(NUMBER nDegrees)
    {
        return (NUMBER)(nDegrees * M_PI / 180.0);
    }
};

// Fundamentally the same as DegreeRadialMeasure,
// but based on the Gon (1/400th of a circle) as
// opposed to Degree (1/360th of a circle.)
class GonRadialMeasure: public RadialMeasure
{
    GonRadialMeasure(NUMBER nDegrees)
    : RadialMeasure(ToRadians(nDegrees))
    {}

    GonRadialMeasure(const RadialMeasure& oOther)
    : RadialMeasure(oOther)
    {
    }

    operator NUMBER ()
    {
        return Angle();
    }

    NUMBER Angle()
    {
        return (NUMBER)(m_nRadians * 200 / M_PI);
    }

    NUMBER ToRadians(NUMBER nDegrees)
    {
        return (NUMBER)(nDegrees * M_PI / 200.0);
    }
};



/**********************************************************************
 *
 *  COLOR
 *
 **********************************************************************/

#define ATOM_COLOR_B_BITS 0x000000FF
#define ATOM_COLOR_G_BITS 0x0000FF00
#define ATOM_COLOR_R_BITS 0x00FF0000
#define ATOM_COLOR_A_BITS 0xFF000000

#define ATOM_COLOR_B_SHIFT  0
#define ATOM_COLOR_G_SHIFT  8
#define ATOM_COLOR_R_SHIFT 16
#define ATOM_COLOR_A_SHIFT 24

class Color
{
public:
    Color()
    : m_argb(0)
    {}

    Color(int r,int g, int b, int a=0xff)
    : m_argb(ToLongARGB(r,g,b,a))
    {
    }

    Color(long argb)
    : m_argb(argb)
    {
    }

    bool IsNullColor() { return m_argb == 0; }

    int R() const { return (m_argb & ATOM_COLOR_R_BITS) >> ATOM_COLOR_R_SHIFT; }
    int G() const { return (m_argb & ATOM_COLOR_G_BITS) >> ATOM_COLOR_G_SHIFT; }
    int B() const { return (m_argb & ATOM_COLOR_B_BITS) >> ATOM_COLOR_B_SHIFT; }
    int A() const { return (m_argb & ATOM_COLOR_A_BITS) >> ATOM_COLOR_A_SHIFT; }

    long RGBA() const // deprecated; use LongARGB instead.
    {
        return m_argb;
    }

    long LongARGB() const
    {
        return m_argb;
    }

    long LongABGR() const
    {
        return ToLongARGB(B(),G(),R(),A());
    }

    void SetR(int r)
    {
        m_argb &= ~ATOM_COLOR_R_BITS;
        m_argb |= (r << ATOM_COLOR_R_SHIFT) & ATOM_COLOR_R_BITS;
    }

    void SetG(int g)
    {
        m_argb &= ~ATOM_COLOR_G_BITS;
        m_argb |= (g << ATOM_COLOR_G_SHIFT) & ATOM_COLOR_G_BITS;
    }

    void SetB(int b)
    {
        m_argb &= ~ATOM_COLOR_B_BITS;
        m_argb |= (b << ATOM_COLOR_B_SHIFT) & ATOM_COLOR_B_BITS;
    }

    void SetA(int a)
    {
        m_argb &= ~ATOM_COLOR_A_BITS;
        m_argb |= (a << ATOM_COLOR_A_SHIFT) & ATOM_COLOR_A_BITS;
    }

    void SetNull()
    {
        m_argb = 0;
    }

    void SetRGBA(long argb) // deprecated -- soon to be deleted.
    {
        m_argb = argb;
    }

    void SetLongARGB(long argb)
    {
        m_argb = argb;
    }

    void SetLongABGR(long abgr)
    {
        // This basically swaps blue and red, which are occupying each others' bits.

        // Preserve the components, moving them to their intended bit positions.
        long lBlue = ((abgr & ATOM_COLOR_R_BITS) >> ATOM_COLOR_R_SHIFT) << ATOM_COLOR_B_SHIFT;
        long lRed  = ((abgr & ATOM_COLOR_B_BITS) >> ATOM_COLOR_B_SHIFT) << ATOM_COLOR_R_SHIFT;
        // Keep what's already in place: Alpha and Green.
        abgr &= ATOM_COLOR_A_BITS | ATOM_COLOR_G_BITS;
        // Reincorporate what's been swapped around.
        abgr |= lBlue | lRed;

        SetLongARGB(abgr);
    }

    void Set(int r, int g, int b, int a = 0xff)
    {
        SetLongARGB(ToLongARGB(r,g,b,a));
    }

    void SetRGBA(int r, int g, int b, int a = 0xff) // deprecated, use Set.
    {
        return Set(r,g,b,a);
    }

    void SetBGRA(int b, int g, int r, int a = 0xff)
    {
        SetLongARGB(ToLongARGB(b,g,r,a)); // just swap the Red and Blue.
    }

    Color& operator=(const Color& o)
    {
        m_argb = o.m_argb;
        return *this;
    }

    bool operator==(const Color& o) const
    {
        return m_argb == o.m_argb;
    }

private:
    long ToLongARGB(int r,int g, int b, int a) const
    {
        return (r << ATOM_COLOR_R_SHIFT) & ATOM_COLOR_R_BITS
              |(g << ATOM_COLOR_G_SHIFT) & ATOM_COLOR_G_BITS
              |(b << ATOM_COLOR_B_SHIFT) & ATOM_COLOR_B_BITS
              |(a << ATOM_COLOR_A_SHIFT) & ATOM_COLOR_A_BITS;
    }

    long m_argb;
#pragma deprecated(RGBA)    // use LongARGB() instead of RGBA().
#pragma deprecated(SetRGBA) // use Set instead SetRGBA().
};


/**********************************************************************
 *
 *  STATUS OBJECT
 *
 **********************************************************************/

struct Status
{
    enum StatusType {
        // ----- Successful results; Status::Succeeded() returns true -----
        keOk                         = 0,          // Normal successful status
        keContinue                   = 1,          // Okay, but continuing
        keDone                       = 2,          // Okay, but operation completed
        keIgnoredUnsupported         = 3,          // Okay, but ignoring unsupported feature.
        keUnchanged                  = 4,          // Operation was innocuous, no action taken.
        keReplaced                   = 5,          // Operation replaced an existing item.

        // ----- Unsuccessful results -----
        keNotReady                   = 0x80000001, // Callee not in the proper state to receive call
        keInvalidArg                 = 0x80000002, // Invalid argument.
        keNoMemory                   = 0x80000003, // Out of memory
        keNoResource                 = 0x80000004, // Out of some other resource
        keAlreadyPresent             = 0x80000005, // Item is already present, and cannot be replaced.
        keNotPresent                 = 0x80000006, // Item is not present, cannot be found.
        keIncompleteString           = 0x80000007, // Incomplete string: premature EOS
        keUnmatchedConstruct         = 0x80000008, // Incomplete string: unmatched character.
        keUnexpectedCharacter        = 0x80000009, // Illegal character sequence.
        keUnknownMarkup              = 0x8000000a, // Grammatically correct, but unknown option.
        keUnknownVersion             = 0x8000000b, // Version markings indicate unknown version.
        keMissingRequired            = 0x8000000c, // Missing a required part.

        keInternalError              = 0x8000fffd, // Something rotten in the state of callee.
        keNotSupported               = 0x8000fffe, // Something outside of Capabilities.
        keNotImplemented             = 0x8000ffff, // Item not implemented.

        keAbandoned                  = 0x80010000, // -----------------------------------------
        keAbandonByUserRequest       = 0x80020000, // User requested abandonment of operation.


        keUninitialized              = 0xffffffff
    };

    Status() : m_eResult(keUninitialized) {}
    Status(StatusType e) : m_eResult(e) {}

    Status& operator= (const Status& oOther)
    {
        m_eResult = oOther.m_eResult;

        return *this;
    }

    bool operator== (StatusType e)
    {
        return m_eResult == e;
    }

    StatusType Result() const { return m_eResult; }

    bool Succeeded() const
    {
        return (m_eResult & 0x80000000) == 0;
    }

private:
    StatusType m_eResult;
    // StatusParticle* m_pStatus; // Future...
};


/**********************************************************************
 *
 *  BASIC PARTICLES
 *
 **********************************************************************/
// Just a common ancestor.  Offers basic particle characteristics:
// 1. Cloning
// 2. Simple "linked list" aggregation.
class Particle
{
public:
    Particle() : m_pNext(NULL) {}
    virtual ~Particle() {}

    virtual Particle* Clone() const = 0;

// protected:
    // This is the implementation, and derivations can
    // "brand" (that is, expose, with stricter type requirements)
    // these methods.

    const Particle* Next()  const
    { return m_pNext; }

    void SetNext(Particle* pNext)
    { m_pNext = pNext; }

    void Append(const Particle* pEnd);

    static Status AddToList(Particle*& pList, const Particle& oParticle);

private:
    Particle* m_pNext;
};


#define ATOM_PARTICLE_DECL(_part,_name)                                 \
    _part ## ParticleType Type() const;                                 \
                                                                        \
    _part ## Particle* Clone() const;                                   \
                                                                        \
    _part ## Particle& operator= (const _part ## Particle& o);          \
                                                                        \
    bool           operator==(const _part ## Particle& o) const;

#define ATOM_PARTICLE_IMPL(_part,_name)                                 \
                                                                        \
    ATOM::_part ## Particle::_part ## ParticleType _name ## _part ## Particle::Type() const     \
    { return _part ## Particle::ke ## _name;}


// Exposes a declaration for a default constructor and a destructor
#define ATOM_PARTICLE_CDTOR(_class)                                     \
    _class(): m_oVal(NULL){}                                            \
    ~_class();

// Exposes the AddToList operation in a specific type; inline, for perf.
#define ATOM_PARTICLE_ADDTOLIST(_typ,_head)                             \
    Status AddToList(const _typ& oEnd); // { Particle::AddToList((_typ*)_head,oEnd); }


// A "general" particle is one that represents a single value of a specific
// type.  StyleParticles, for example, fall into this category.
#define ATOM_GENERAL_PARTICLE_DECL(_name, _cat, _type, _extra)          \
class _name ## _cat ## Particle: public _cat ## Particle                \
{                                                                       \
public:                                                                 \
    _name ## _cat ## Particle(_type oVal);                              \
    _extra                                                              \
    _cat ## ParticleType Type()  const;                                 \
    _type                Value() const;                                 \
    _cat ## Particle*    Clone() const;                                 \
    _cat ## Particle&    operator= (const _cat ## Particle& o);         \
    bool                 operator==(const _cat ## Particle& o) const;   \
private:                                                                \
    _type m_oVal;                                                       \
};

// A "simple" particle is one that contains its value completely
// within itself (ie, not a list.)
#define ATOM_SIMPLE_PARTICLE_DECL(_name,_cat,_type) ATOM_GENERAL_PARTICLE_DECL(_name,_cat,_type,)
// A "list" particle is one that contains the head of a list of (sub)particles, and so
// needs special list management semantics: def-ctor, dtor, and access to append.
#define ATOM_LIST_PARTICLE_DECL(  _name,_cat,_type)                     \
    ATOM_GENERAL_PARTICLE_DECL(_name,_cat,_type*,                       \
                          ATOM_PARTICLE_CDTOR(_name ## _cat ## Particle)\
                          ATOM_PARTICLE_ADDTOLIST(_type,m_oVal))


// Default implementation for Clone.  Used by simple particles
#define ATOM_SIMPLE_CLONE_IMPL(_name,_cat)                              \
    _cat ## Particle* _name ## _cat ## Particle::Clone() const          \
    { return new _name ## _cat ## Particle(m_oVal); }                   \

// General implementation for particles.
#define ATOM_GENERAL_PARTICLE_IMPL(_name, _cat, _type,_extra)           \
    _name ## _cat ## Particle :: _name ## _cat ## Particle(_type oVal)  \
    : m_oVal(oVal)                                                      \
    {}                                                                  \
                                                                        \
    _extra                                                              \
                                                                        \
    ATOM::_cat ## Particle::_cat ## ParticleType _name ## _cat ## Particle::Type() const \
    { return _cat ## Particle::ke ## _name;}                            \
                                                                        \
    _type _name ## _cat ## Particle::Value() const { return m_oVal; }   \
                                                                        \
    _cat ## Particle& _name ## _cat ## Particle::operator= (const _cat ## Particle& o) \
    {                                                                   \
      if(Type() == o.Type())                                            \
        this->m_oVal = ((const _name ## _cat ## Particle&)o).m_oVal;    \
      return *this;                                                     \
    }                                                                   \
                                                                        \
    bool _name ## _cat ## Particle::operator== (const _cat ## Particle& o) const\
    {                                                                   \
      if(Type() != o.Type())                                            \
        return false;                                                   \
      else                                                              \
        return this->m_oVal == ((_name ## _cat ## Particle&)o).m_oVal;  \
    }

#define ATOM_SIMPLE_PARTICLE_IMPL(_name,_cat,_type)                     \
    ATOM_GENERAL_PARTICLE_IMPL(_name,_cat,_type,ATOM_SIMPLE_CLONE_IMPL(_name,_cat))

#define ATOM_LIST_PARTICLE_IMPL(_name,_cat,_type)                       \
    ATOM_GENERAL_PARTICLE_IMPL(_name,_cat,_type,)






#define ATOM_TRANSFORM_PARTICLE_BASE  0x0000 // zero
#define ATOM_STYLE_PARTICLE_BASE      0x1000
#define ATOM_CAPABILITY_PARTICLE_BASE 0x2000
#define ATOM_LOCATION_PARTICLE_BASE   0x3000


/**********************************************************************
 *
 *  STYLE PARTICLES
 *
 **********************************************************************/

// This is the base (abstract) class.
class StyleParticle: public Particle
{
public:
    enum StyleParticleType {
        keOther = ATOM_STYLE_PARTICLE_BASE,// Reserved for future use.

        keTypeface,           // Typeface used, eg "Times New Roman"
        kePitchFamily,        //
        keCharacterSet,       //
        keAltTypefaces,       //
        keSize,               // Typographical (cell-height) size of font
        keCapSize,            // Size of font expressed as Cap-Height.
        keItalic,             // Italic variant
        keUnderline,          // Underlined
        keOverline,           // Overlined
        keStrikethrough,      // Struck-through.
        keCaseShift,          // Uppercase, lowercase, small-caps
        keFontWeight,         // Normal, Bold, ...     See enum.
        keFillColor,          // RGB + Alpha
        keStrokeWeight,       // (Outline) stroke line weight.  Omitted if font is solid
        keStrokeColor,        // (Outline) stroke line color.  (ditto)
        keStrokeBehind,       // Boolean property: stroke rendered behind (before) fill?
        keTrackingAugment,    // Inter-character spacing: Amount of space added to character advance.
        keVerticalAlignment,  // Describes vertical relationship of text to insertion point.
        keHorizontalAlignment,// Describes horizontal relationship of text to insertion point.
        keAdvanceAlignment,   // Vertical relationship of text to other runs on same line.
        keJustification,      // *Multi-line* treatment of text: flush left ... fully justified.
        keLineHeight,         // *Multi-line* treatment: distance from one baseline to the next.
        keBeforePara,         // *Multi-line* treatment: extra distance to appear before paragraph.
        keAfterPara,          // *Multi-line* treatment: extra distance to appear after paragraph.
        keReferenceExpansion, // Identifies a resolved or unresolved Reference, or something to be treated as such.

        keBackgroundColor,    //

        keTopInnerPadding,
        keTopBorderLine,
        keTopBorderWidth,
        keTopBorderColor,
        keTopOuterPadding,

        keBottomInnerPadding,
        keBottomBorderLine,
        keBottomBorderWidth,
        keBottomBorderColor,
        keBottomOuterPadding,

        keLeftInnerPadding,
        keLeftBorderLine,
        keLeftBorderWidth,
        keLeftBorderColor,
        keLeftOuterPadding,

        keRightInnerPadding,
        keRightBorderLine,
        keRightBorderWidth,
        keRightBorderColor,
        keRightOuterPadding,
    };

    virtual StyleParticleType Type()                             const = 0;
    virtual StyleParticle*    Clone()                            const = 0;
    virtual StyleParticle&    operator= (const StyleParticle& o)       = 0;
    virtual bool              operator==(const StyleParticle& o) const = 0;


    const StyleParticle* Next() const
    {
        return (StyleParticle*)Particle::Next();
    }

    void SetNext(StyleParticle* pNext)
    {
        Particle::SetNext(pNext);
    }

    void Append(const StyleParticle* pEnd)
    {
        Particle::Append(pEnd);
    }

protected:
    StyleParticle() {}
};

/*
  For consistency, each Style Particle is modeled after the following
  macro.  For example

  STYLE_PARTICLE_DECL(Font,ASTRING)

  will produce a class declaration:
  1. a class FontStyleParticle (derived from StyleParticle) that
  2. has a Type() method that returns keFont, and
  3. a Value() method that returns a value of type ASTRING.
  4. Clone method that creates a clone of itself
  5. Assignment operator that receives an assignment of a
     like-typed particle.

  STYLE_PARTICLE_IMPL will produce the implemented methods.

  Generics are an alternative, but depending on the compiler,
  that can be bloaty.

  Also, RTTI is an alternative to the Type() method, but that
  would require a cascading collection of if(type-determination)
  statements; using an enum allows a switch statement.
*/
#define ATOM_STYLE_PARTICLE_DECL(     _name, _type)     ATOM_SIMPLE_PARTICLE_DECL(       _name, Style, _type)

#define ATOM_STYLE_PARTICLE_IMPL(     _name, _type)     ATOM_SIMPLE_PARTICLE_IMPL(       _name, Style, _type)

#define ATOM_STYLE_PARTICLE_TBLR_DECL(_name, _type)       \
    ATOM_STYLE_PARTICLE_DECL( Top ##  _name, _type)       \
    ATOM_STYLE_PARTICLE_DECL( Bottom##_name, _type)       \
    ATOM_STYLE_PARTICLE_DECL( Left ## _name, _type)       \
    ATOM_STYLE_PARTICLE_DECL( Right ##_name, _type)

#define ATOM_STYLE_PARTICLE_TBLR_IMPL( _name, _type)      \
    ATOM_STYLE_PARTICLE_IMPL( Top ##  _name, _type)       \
    ATOM_STYLE_PARTICLE_IMPL( Bottom##_name, _type)       \
    ATOM_STYLE_PARTICLE_IMPL( Left ## _name, _type)       \
    ATOM_STYLE_PARTICLE_IMPL( Right ##_name, _type)

// Specific StyleParticle types (Second arg in STYLE_PARTICLE_xxx macros)
class FontWeight
{
public:
    enum Type {
      keDontCare   = 0,
      keThin       = 100,

      keUltraLight = 200,
      keExtraLight = 200,

      keLight      = 300,

      keNormal     = 400,
      keRegular    = 400,

      keMedium     = 500,

      keSemiBold   = 600,
      keDemiBold   = 600,

      keBold       = 700,

      keUltraBold  = 800,
      keExtraBold  = 800,

      keBlack      = 900,
      keHeavy      = 900
    };
};

// One strategy for identifying a likely replacement
// for an unknown font.  Based on Win32 LOGFONT
class PitchFamily
{
public:
    enum Type {
        keFixedPitch    = 1,      // Fixed width font, like Courier
        keVariablePitch = 2,      // Variable width font, like Times Roman, etc.

        // --- above can be |'d with below ---

        keRoman      = (1<<4),    // Roman-like font, with serifs
        keSerif      = (1<<4),

        keSwiss      = (2<<4),    // Sans-serif font, like Helvetica
        keSansSerif  = (2<<4),

        keModern     = (3<<4),    // Uniform stroke thickness, typically sans-serif

        keScript     = (4<<4),    // Handwriting-like (cursive styles, etc.)

        keDecorative = (5<<4),    // More fanciful fonts (Wingdings, etc.)
    };
};

// This is a multi-line "flow" characteristic, distinct from alignment.
class Justification
{
public:
    enum Type {
        keLeft,         // aka "ragged-right"
        keCentered,
        keRight,        // aka "ragged-left"
        keJustified
    };
};

class VerticalAlignment
{
public:
    enum Type {
      keAscender,     // Top of ascent
      keXHeight,      // Top of small lowercase letters
      keMid,          // Arithmetic midpoint between base and Ascent.
      keBaseline,     // Font baseline
      keDescender     // Bottom of descent
    };
};

class HorizontalAlignment
{
public:
    enum Type {
      keLeft,
      keMiddle,
      keRight
    };
};

class TextLine
{
public:
    enum Type { // For underline, overline, and strike-through.
        keNone,
        keSingle,
        keDouble,
        keDotted,
    };
};

class CaseShift
{
public:
    enum Type {
        keNoShift,     //
        keUppercase,   // Lowercase letters are replaced with full-size capitals
        keLowercase,   // Uppercase letters are replaced with lowercase.
        keSmallCaps,   // Lowercase letters are replaced with smaller capitals.
    };
};

// Many markup languages have a means of referencing external text;
// MTEXT has %< .. >%, XML has &entity; notation, etc.  The parser may
// not have access to this information, so it relies on an external
// agent to substitute (expand) the original reference with the
// intended text.
class ReferenceExpansion
{
public:
    enum Type {
        keNotReference               = 0x00000,  // Normal text, not a reference.
        keSource                     = 0x00001,  // Represents the original reference text
        keExpanded                   = 0x00002,  // Represents the expanded text, the result of the
  //    keReferenceSourceMarkup11    = 0x01011,  // Represents original source, first/last chars are markup
  //    keReferenceSourceMarkup22    = 0x02021,  // Represents original source, first/last 2 chars are markup
  //    keReferenceSourceMarkup33    = 0x03031,  // Represents original source, first/last 3 chars are markup

  //    keReferenceSourceMarkupFirstBit  = 0x01000,
  //    keReferenceSourceMarkupLastBit   = 0x00010,
  //    keReferenceSourceMarkupFirstBits = 0xff000,
  //    keReferenceSourceMarkupLastBits  = 0x00ff0,
    };
};




class BorderLine
{
public:
    enum Type
    {
        keNoBorder,
        keSolid,
        keDot,
        keDash,
        keDouble,
        keGroove,
        keRidge,
        keSunken,
        keRaised
    };
};

// Note that these enum values are set to match the WIN32 charset values because it is expected that the
// CharacterSet value will most likely be used on a WIN32 platform to select a font.
class CharacterSet
{
public:
    enum Type {
          keANSI          = 0,
          keDefault       = 1,
          keSymbol        = 2,

          keMac           = 77,

          keShiftJIS      = 128,

          keHangul        = 129,
          keJohab         = 130,

          keGB2312        = 134,

          keBig5          = 136,

          keGreek         = 161,
          keTurkish       = 162,
          keVietnamese    = 163,

          keHebrew        = 177,
          keArabic        = 178,

          keBaltic        = 186,

          keRussian       = 204,

          keThai          = 222,

          keEastEurope    = 238,

          keOEM           = 255
    };
};


// These are the defined style particles.  All sinks should
// accept at least a subset of these, and gracefully ignore
// any particle it doesn't support, though in some cases it
// may choose to abandon the parse (rare!)
ATOM_STYLE_PARTICLE_DECL(Typeface,            StRange)           // Something like "Times new Roman" or "Sans Serif"
ATOM_STYLE_PARTICLE_DECL(PitchFamily,         PitchFamily::Type)   // Font-matching heuristics if font isn't known.
ATOM_STYLE_PARTICLE_DECL(CharacterSet,        CharacterSet::Type)  // Font-matching heuristics for which character set.
ATOM_STYLE_PARTICLE_DECL(AltTypefaces,        StRange)           // Alternate typefaces, to be tried if Typeface isn't found.
ATOM_STYLE_PARTICLE_DECL(Size,                Measure)           // Typographical definition: cell height |   Mutually
ATOM_STYLE_PARTICLE_DECL(CapSize,             Measure)           // Alternate definition: Cap height Size |   Exclusive
ATOM_STYLE_PARTICLE_DECL(FontWeight,          FontWeight::Type) // Weight of the font (400 = normal, 700 = bold)
ATOM_STYLE_PARTICLE_DECL(Italic,              bool)              // Italic variation of font?
ATOM_STYLE_PARTICLE_DECL(Underline,           TextLine::Type)      // None/Single/Double/Dotted.  Sink may support only
ATOM_STYLE_PARTICLE_DECL(Overline,            TextLine::Type)      // ..                          a limited subset
ATOM_STYLE_PARTICLE_DECL(Strikethrough,       TextLine::Type)      // ..                          and modify rendering.
ATOM_STYLE_PARTICLE_DECL(CaseShift,           CaseShift::Type)     // Small Caps, etc.
ATOM_STYLE_PARTICLE_DECL(FillColor,           Color)             // Color (with Alpha) of the fill
ATOM_STYLE_PARTICLE_DECL(StrokeWeight,        Measure)           // Line weight of the stroke.
ATOM_STYLE_PARTICLE_DECL(StrokeColor,         Color)             // Color (with Alpha) of the stroke.
ATOM_STYLE_PARTICLE_DECL(StrokeBehind,        bool)              // Does the stroke get rendered behind fill? (eg SFA vs SVG)
ATOM_STYLE_PARTICLE_DECL(TrackingAugment,     Measure)           // Inter-character space, to apply between each character.
ATOM_STYLE_PARTICLE_DECL(VerticalAlignment,   VerticalAlignment::Type)   // Vertical relationship of text to insertion point/path
ATOM_STYLE_PARTICLE_DECL(HorizontalAlignment, HorizontalAlignment::Type) // Horizontal relationship of text to insertion point/path
ATOM_STYLE_PARTICLE_DECL(AdvanceAlignment,    Measure)           // Vertical relationship of text runs with respect to other text on line
ATOM_STYLE_PARTICLE_DECL(Justification,       Justification::Type) // Multi-line support:
ATOM_STYLE_PARTICLE_DECL(LineHeight,          Measure)           // Multi-line support: distance from base-line to base-line
ATOM_STYLE_PARTICLE_DECL(BeforePara,          Measure)           // Multi-line support: extra space before a "paragraph" unit
ATOM_STYLE_PARTICLE_DECL(AfterPara,           Measure)           // Multi-line support: extra space after a "paragraph" unit.
ATOM_STYLE_PARTICLE_DECL(ReferenceExpansion,  ReferenceExpansion::Type)// Reference Expansion
ATOM_STYLE_PARTICLE_DECL(BackgroundColor,     Color)             // Color (with Alpha) of the background.
ATOM_STYLE_PARTICLE_TBLR_DECL(InnerPadding,   Measure)           // Top/Bottom/Left/Right InnerPadding: space between border and text
ATOM_STYLE_PARTICLE_TBLR_DECL(BorderLine,     BorderLine::Type)  // Top/Bottom/Left/Right BorderStyle: kind of border-line along edge
ATOM_STYLE_PARTICLE_TBLR_DECL(BorderWidth,    Measure)           // ... BorderWidth: linewidth of the border
ATOM_STYLE_PARTICLE_TBLR_DECL(BorderColor,    Color)             // ... BorderColor: color of the border
ATOM_STYLE_PARTICLE_TBLR_DECL(OuterPadding,   Measure)           // ... OuterPadding: space between border and adjacent text.











/**********************************************************************
 *
 *  STYLE DESCRIPTION
 *
 **********************************************************************/

// This interface merely describes a "style", a set of particles.
class IStyleDescription
{
public:
    virtual const StyleParticle* Description() const = 0;

    virtual const StyleParticle* DescriptionParticle(StyleParticle::StyleParticleType eType) const = 0;
};


// This interface describes a *change* in style, as well as the
// final state (inherited)
class IStyleChange: public IStyleDescription
{
public:
     // Contains the changes in style.
     virtual const StyleParticle* Deltas() const = 0;
};


// This interface describes a table of styles, a la a "style sheet".
// Each style is named.  Note that the implementation can late-bind
// the resolution of names to styles rather than requiring all styles
// be defined prior to parsing.
class IStyleTable
{
public:
    // Does a lookup of the style requested.  Returns NULL if no style has
    // the name given by sName.
    virtual const IStyleDescription* operator[] (const StRange& sName) const = 0;

    // Permits (late?) addition of styles to the style table.
    // (Used by parser to augment styles that may be defined in markup.)
    // Should supercede any style with the same that already exists.
    virtual Status AddStyle(const StRange& sName, const IStyleDescription* pStyle) = 0;
};



/**********************************************************************
 *
 *  TRANSFORMS
 *
 **********************************************************************/

// A transform particle describes how the space the text is being
// rendered into should be transformed.
class TransformParticle: public Particle {
public:
    enum TransformParticleType {
      keNone        = 0,    // No transform (the identity)
      keScale       = 0x01,
      keSkew        = 0x02,
      keTranslation = 0x04,
      keRotation    = 0x08,
      keArbitrary   = 0x10,
    };

    virtual TransformParticleType  Type()                                const = 0;
    virtual TransformParticle*     Clone()                               const = 0;
    virtual TransformParticle&     operator= (const TransformParticle& o)      = 0;
    virtual bool                   operator==(const TransformParticle& o)const = 0;

    // When requested, the particle will populate the 3x3 matrix with its
    // transformation.  It can assume that the matrix is pre-initialized to
    // the Identity state.
    virtual void                   SetMatrix(Matrix& m)                  const = 0;
    virtual bool                   IsIdentity()                          const = 0;

    const TransformParticle* Next() const
    {
        return (TransformParticle*) Particle::Next();
    }

    void    Append(const TransformParticle* pEnd)
    {
        Particle::Append(pEnd);
    }
    void    SetNext(TransformParticle* pNext)
    {
        Particle::SetNext(pNext);
    }

protected:
    TransformParticle() {}

private:
    TransformParticle* m_pNext;
};

#define TRANSFORM_PARTICLE_XY_DECL(_name,_type)                   \
class _name ## TransformParticle: public TransformParticle        \
{                                                                 \
public:                                                           \
    _name ## TransformParticle(_type x,_type y);                  \
    TransformParticleType Type() const;                           \
                                                                  \
    _type _name ## X() const;                                     \
    _type _name ## Y() const;                                     \
                                                                  \
    TransformParticle* Clone() const;                             \
    TransformParticle& operator= (const TransformParticle& o);    \
    bool operator==(const TransformParticle& o)const;             \
    void SetMatrix(Matrix& m) const;                              \
    bool IsIdentity() const;                                      \
private:                                                          \
    _type m_x;                                                    \
    _type m_y;                                                    \
};

// Note that SetMatrix is not implemented here; it must be
// implemented manually for each derivation.
#define TRANSFORM_PARTICLE_XY_IMPL(_name,_type)                               \
    _name ## TransformParticle::_name ## TransformParticle(_type x,_type y)   \
    : m_x(x)                                                                  \
    , m_y(y)                                                                  \
    {}                                                                        \
                                                                              \
    ATOM::TransformParticle::TransformParticleType  _name ## TransformParticle::Type() const  \
    { return TransformParticle::ke ## _name; }                                \
                                                                              \
    _type _name ## TransformParticle::_name ## X() const                      \
    { return m_x; }                                                           \
    _type _name ## TransformParticle::_name ## Y() const                      \
    { return m_y; }                                                           \
                                                                              \
    TransformParticle* _name ## TransformParticle::Clone() const              \
    { return new _name ## TransformParticle(m_x,m_y); }                       \
                                                                              \
    TransformParticle& _name ## TransformParticle::operator= (const TransformParticle& o) \
    {                                                                         \
        if(Type() == o.Type()) {                                              \
            this->m_x = ((const _name ## TransformParticle&)o).m_x;           \
            this->m_y = ((const _name ## TransformParticle&)o).m_y;           \
        }                                                                     \
        return *this;                                                         \
    }                                                                         \
    bool _name ## TransformParticle::operator==(const TransformParticle& o)const\
    {   if (Type() != o.Type())                                               \
            return false;                                                     \
        const _name ## TransformParticle& oOther = (const _name ## TransformParticle&)o; \
        return this->m_x == oOther.m_x && this->m_y == oOther.m_y;            \
    }

// Note that SetMatrix is not implemented here; it must be
// implemented manually for each derivation.
#define TRANSFORM_PARTICLE_DECL(_name,_type)                      \
class _name ## TransformParticle: public TransformParticle        \
{                                                                 \
public:                                                           \
    _name ## TransformParticle(_type v);                          \
    TransformParticleType Type() const;                           \
                                                                  \
    _type _name() const;                                          \
                                                                  \
    TransformParticle* Clone() const;                             \
    TransformParticle& operator= (const TransformParticle& o);    \
    bool operator==(const TransformParticle& o)const;             \
    void SetMatrix(Matrix& m) const;                              \
    bool IsIdentity() const;                                      \
private:                                                          \
    _type  m_v;                                                   \
};

#define TRANSFORM_PARTICLE_IMPL(_name,_type)                                  \
    _name ## TransformParticle::_name ## TransformParticle(_type v)           \
    : m_v(v)                                                                  \
    {}                                                                        \
                                                                              \
    ATOM::TransformParticle::TransformParticleType  _name ## TransformParticle::Type() const  \
    { return TransformParticle::ke ## _name; }                                \
                                                                              \
    _type _name ## TransformParticle::_name () const                          \
    { return m_v; }                                                           \
                                                                              \
    TransformParticle* _name ## TransformParticle::Clone() const              \
    { return new _name ## TransformParticle(m_v); }                           \
                                                                              \
    TransformParticle& _name ## TransformParticle::operator= (const TransformParticle& o) \
    {                                                                         \
        if(Type() == o.Type()) {                                              \
            this->m_v = ((const _name ## TransformParticle&)o).m_v;           \
        }                                                                     \
        return *this;                                                         \
    }                                                                         \
    bool _name ## TransformParticle::operator==(const TransformParticle& o)const\
    {   if (Type() != o.Type())                                               \
            return false;                                                     \
        const _name ## TransformParticle& oOther = (const _name ## TransformParticle&)o; \
        return this->m_v == oOther.m_v;                                       \
    }



TRANSFORM_PARTICLE_XY_DECL(Scale,         NUMBER)
TRANSFORM_PARTICLE_XY_DECL(Skew,          RadialMeasure)
TRANSFORM_PARTICLE_XY_DECL(Translation,   NUMBER)
TRANSFORM_PARTICLE_DECL(   Rotation,      RadialMeasure)
TRANSFORM_PARTICLE_DECL(   Arbitrary,     Matrix);


// Describes a complete transformation.
class ITransform
{
public:
    // Populates the indicated matrix with the cumulative transform as represented
    // by the separate TransformParticles described below.
    // Return is the union of all TransformParticle types: for example,
    //   keNone           indicates no transform in effect,
    //   keScale | keSkew indicates that both of these transforms are in effect
    // You would need to traverse the Description() list to know their order,
    // if that was important to you.
    virtual       TransformParticle::TransformParticleType AsMatrix(Matrix* )       const = 0;

    // A list of the individual TransformParticles that go into making the matrix.
    virtual const TransformParticle*                       Description()            const = 0;
};

// Describes how the transform has changed since the last TextRun,
// in addition to how the overall transformation state (inherited).
class ITransformChange: public ITransform
{
public:
    virtual const TransformParticle*                       Deltas()                 const = 0;
};


/**********************************************************************
 *
 *  REFERENCE RESOLVER
 *
 **********************************************************************/
// Fields or other inserted codes not intrinsically understood by the
// markup language are resolved by this mechansim.
// For example: MTEXT %<...>%, XML &Entities;, etc.
//

class IEnvironment; // forward reference
class IReferenceResolver
{
public:
    // Allows the resolver to set up or allocate
    virtual Status Initialize()                    = 0;

    // Requests the resolver to resolve a reference.
    virtual Status Resolve(const StRange sParserName,
                           const StRange sReference,
                                 StRange& sResult,
                                 IEnvironment* pEnv) = 0;

    // Allows the resolver to clean up.
    virtual Status Terminate()                     = 0;
};

/**********************************************************************
 *
 *  ENVIRONMENT
 *
 **********************************************************************/

class ISink; // forward declaration.

// This interface contains settings pertinent to the parsing/rendering
// operation.
class IEnvironment
{
public:
    // This is the "default" style that is in effect in the absence
    // of any other markup.
    virtual const IStyleDescription*  AmbientStyle()         const = 0;

    // Any transform that is in effect.  Cumulative w/ TextRun.
    virtual const ITransform*         AmbientTransform()     const = 0;

    // This contains the complete repertoire of styles known.
    // Note that these may be defined out-of-band (not within the
    // markup) though the parser can augment the dictionary if
    // necessary.
    virtual const IStyleTable*        StyleDictionary()      const = 0;

    // This is the recipient of a parser's effort.
    virtual       ISink*              Sink()                 const = 0;

    // This is the mechanism whereby the parser can expand fields
    // (which may not be governed by the markup language.)
    virtual       IReferenceResolver* References()           const = 0;

    // This is the color that the text is being rendered over.
    virtual       Color               CanvasColor()          const = 0;
};




/**********************************************************************
 *
 *  LOCATION
 *
 **********************************************************************/

//
class LocationParticle: public Particle
{
public:
    enum LocationParticleType {
        keBookmark = ATOM_LOCATION_PARTICLE_BASE,// Defines a bookmark location.
        keReturnToBookmark,                      // Request to return to a bookmark.
        keConditionalReturnToBookmark,           // ... under some condition.
        keRelative,                              // Move a relative amount.
        kePoint,                                 // Location is absolute position.
        kePath,                                  // Location is a path.
        keLineBreak,                             // Move to the beginning of the next line.
    };

    virtual LocationParticleType Type()                                const = 0;
    virtual LocationParticle*    Clone()                               const = 0;
    virtual LocationParticle&    operator= (const LocationParticle& o)       = 0;
    virtual bool                 operator==(const LocationParticle& o) const = 0;

    const LocationParticle* Next() const
    {
        return (LocationParticle*) Particle::Next();
    }
    void Append(const LocationParticle* pEnd)
    {
        Particle::Append(pEnd);
    }
    void SetNext(LocationParticle* pNext)
    {
        Particle::SetNext(pNext);
    }

protected:
    LocationParticle() {}

private:
    LocationParticle* m_pNext;
};


// This particle requests the Sink to remember the current
// location in the indicated location of a point array, as
// it will be referenced by a future particle.  (Since text
// metrics aren't necessarily available to the markup, this
// is information only the sink or other renderer is likely to
// maintain.)
class BookmarkLocationParticle: public LocationParticle
{
public:
    BookmarkLocationParticle(int iIndex);

    ATOM_PARTICLE_DECL(Location,Bookmark)

    // The bookmark number to save.
    int Index() const;

private:
    int m_iIndex;
};

// This particle requests the Sink to update its current
// location to be the previously bookmarked location.
class ReturnToBookmarkLocationParticle: public LocationParticle
{
public:
    ReturnToBookmarkLocationParticle(int iIndex);

    ATOM_PARTICLE_DECL(Location,ReturnToBookmark)

    // The bookmark to return to.
    int Index() const;

private:
    int m_iIndex;
};

class ConditionalReturnToBookmarkLocationParticle: public LocationParticle
{
public:
    enum ConditionType {
        keFarthestAdvance,         // if Bookmark[i] is farther along advance vector,
        keLeastAdvance,            // if Bookmark[i] is less father along (behind)
    };
    ConditionalReturnToBookmarkLocationParticle(int iIndex,ConditionType eType);

    ATOM_PARTICLE_DECL(Location,ConditionalReturnToBookmark)

    // The bookmark to return to.
    int Index() const;

    // Condition under which to return
    ConditionType Condition() const;
private:
    int m_iIndex;
    ConditionType m_eCondition;
};

// The sink should keep a table of points at least this size.
const int kiBookmarkTableSize = 8;

// This particle describes a relative location (ie, a vector)
// The Sink should update the "current" location using this
// vector, transformed by the IEnvironment->AmbientTransform()
// but not this TextRun's Transform, prior to rendering the current
// TextRun.
class RelativeLocationParticle: public LocationParticle
{
public:
    RelativeLocationParticle(Measure mAdvance,Measure mRise);

    ATOM_PARTICLE_DECL(Location,Relative)

    // The amount to advance along the baseline (in horizontal text, this
    // is parallel with X axis.)
    Measure Advance()   const;
    // The amount to move perpendicular to the baseline (y axis in horizontal text)
    // Positive values represent "up" relative to letter.
    Measure Rise()      const;

private:
    Measure m_mAdvance;
    Measure m_mRise;
};

#ifdef X
// #pragma message("atom.h: warning: detected #define X ...; this has been #undef'd to avoid conflict with internal definition.") -- is there a linux equivalent?
#undef X
#endif
#ifdef Y
// #pragma message("atom.h: warning: detected #define Y ...; this has been #undef'd to avoid conflict with internal definition.") -- is there a linux equivalent?
#undef Y
#endif

// The particle describes an absolute location (ie, a point)
// The Sink should update
class PointLocationParticle: public LocationParticle
{
public:
    PointLocationParticle(NUMBER x,NUMBER y);

    ATOM_PARTICLE_DECL(Location,Point)

    // These values represent a pre-translated point.
    NUMBER X()   const;
    NUMBER Y()   const;

private:
    NUMBER m_x;
    NUMBER m_y;
};

// Identifies the beginning of a new line.  In left-justified
// (aka flush-left, ragged-right) text, this is synonymous with
// a Return to the start of the previous line and a Relative
// movement of Rise = -1 x line height, however other justifications
// can only predict the Rise; the actual Advance computation needs
// to be left to the Sink.  Semantics will tell the Sink if the
// break is at a paragraph boundary.
class LineBreakLocationParticle: public LocationParticle
{
public:
    LineBreakLocationParticle();

    ATOM_PARTICLE_DECL(Location,LineBreak)
};


class PathLocationParticle: public LocationParticle
{
public:
    PathLocationParticle(/* path description*/);

    ATOM_PARTICLE_DECL(Location,Path)
};



// This describes any change in location of the rendered text.  It is
// assumed that the Sink is maintaining a "current" location as determined
// by each TextRun's extent: that is, as the Sink renders each TextRun,
// it updates a position that indicates where the next TextRun will be
// rendered.  The descriptions here modify the "current" location
// immediately prior to the rendering of the current Contents.
class ILocation
{
public:
    enum SemanticType {
        keNormal        = 0x0001, // Normal advance.
        keParagraph     = 0x0002, // Begin a paragraph (implicitly also begin a line)
        keLine          = 0x0004, // Begin a line
        keTabColumn     = 0x0008, // Begin a tab column

        keTable         = 0x0010, // Begin a table       // Note: Table semantics might
        keEndTable      = 0x0020, // End a table         // require the sink to defer
        keRow           = 0x0040, // Begin a table row   // rendering until the end of
        keCell          = 0x0080, // Begin a table cell  // table, to accumulate cell extents.

        keSuperscript   = 0x0100, // Begins a Superscript
        keEndSuperscript= 0x0200, // Ends a Superscript

        keSubscript     = 0x0400, // Begins a Subscript
        keEndSubscript  = 0x0800, // Ends a Subscript

        keInlineBlock   = 0x1000, // Begins an inline "block" of more complex text
        keEndInlineBlock= 0x2000, // Ends an inline "block" of more complex text

        keFlowColumn    = 0x4000, // Begins a new column of text
    };

    // Describes the nature of the location change.
    virtual SemanticType Semantics()       const = 0;

    // Zero or more operations to effect the location change.
    virtual LocationParticle* Operations() const = 0;
};



/**********************************************************************
 *
 *  TEXT RUN "STRUCTURE"
 *
 **********************************************************************/
class Shape
{
public:
    enum Type {
        keFlow,      // markup is describing a flowing sequence, a la HTML <span>
        keBlock      // markup is describing a rectangular block, a la HTML <div>
    };
};

// Provides structural information of the markup.  Of more interest
// to converter sinks than rendering sinks.
class IStructure
{
public:
    // Current depth within the markup.
    virtual int         Depth()      const = 0;

    // Pointer to an outer IStructure (with Depth()-1)
    virtual IStructure* Outer()      const = 0;

    // What is the "shape" of the run?  Does it flow and wrap, or... ?
    virtual Shape::Type GetShape()      const = 0;

    // Is selection considered continuous with previous run?
    virtual bool        Continuous() const = 0;
};


/**********************************************************************
 *
 *  TEXT RUN INTERFACE
 *
 **********************************************************************/

// A text run describes a consecutive sequence of characters all sharing
// a common style, graphical transformation, and/or location, as reported
// by the markup.
class ITextRun
{
public:
    // 1. --------------------------------------------------
    // Structural information about the markup being parsed.
    virtual const IStructure*       Structure()   const = 0;

    // 2. --------------------------------------------------
    // The actual style characteristics in effect for the
    // text run, and what's changed from the previous run.
    virtual const IStyleChange*     Style()       const = 0;

    // 3. --------------------------------------------------
    // The transformation in effect for the text run, and
    // the component transforms.
    virtual const ITransformChange* Transform()   const = 0;

    // 4. --------------------------------------------------
    // The location of the indicated contents.
    virtual const ILocation*        Location()    const = 0;

    // 5. --------------------------------------------------
    // The contents of the text run described.
    virtual const StRange           Contents()    const = 0;
};



/**********************************************************************
 *
 *  MARKUP PARSER INTERFACE
 *
 **********************************************************************/

class IGenerator; // forward declaration
// A Markup Parser
class IParser
{
public:
    // Parse a markup string by creating an "environment" with all the
    // ambient settings, then combine that with a string to parse, and
    // give it to this method.
    virtual Status Parse(const StRange sMarkup,IEnvironment* pEnv) = 0;

    // Gives you a pointer to its generator, if any.
    // CAN BE NULL.
    virtual IGenerator* GetGenerator()                             = 0;
};


/**********************************************************************
 *
 *  ABANDONMENT INTERFACE
 *
 **********************************************************************/
class IAbandonment
{
public:
    // Indicates the reason for abandonment
    virtual const Status   Reason()    = 0;

    // The string being parsed
    virtual const StRange& Markup()    = 0;

    // Local context of the string being parsed
    // (the line on which it occurs, maybe including adjacent
    // lines)
    virtual const StRange& Context()   = 0;

    // The specific location where the error Occurred.
    virtual const StRange& Position()  = 0;
};


/**********************************************************************
 *
 *  SINK CAPABILITY
 *
 **********************************************************************/

class CapabilityParticle: public Particle
{
public:
    enum CapabilityParticleType {
      keUnrestricted = ATOM_CAPABILITY_PARTICLE_BASE,
      keBool,
      keEnum,
      keRange,
    };

    virtual CapabilityParticleType Type()                             const = 0;
};

class UnrestrictedCapabilityParticle: public CapabilityParticle
{
public:
    CapabilityParticleType Type() const { return keUnrestricted; }
};


// Is the capability essentially boolean?
class BoolCapabilityParticle: public CapabilityParticle
{
public:
    CapabilityParticleType Type() const { return keBool; }
    // true-false
};


// Is the capability described as a finite enumeration of items?
class EnumCapabilityParticle: public CapabilityParticle
{
public:
    CapabilityParticleType Type() const { return keEnum; }
    EnumCapabilityParticle()
    {
    }

    Particle* Enumeration() const
    {
    }

    Status AddEnumeration(const Particle& o)
    {
        return Particle::AddToList(m_pParticle,o);
    }

private:
    Particle* m_pParticle;
};

// Is the capability described as a range from min to max (inclusive)?
class RangeCapabilityParticle: public CapabilityParticle
{
public:
    CapabilityParticleType Type() const { return keRange; }

    RangeCapabilityParticle(const Particle& oMin,const Particle& oMax)
    : m_pMin(oMin.Clone())
    , m_pMax(oMax.Clone())
    {}

    ~RangeCapabilityParticle()
    {
        delete(m_pMin);
        delete(m_pMax);
    }

    Particle* Clone() const
    {
        return new RangeCapabilityParticle(*m_pMin,*m_pMax);
    }

    Particle* Minimum() const
    { return m_pMin; }
    Particle* Maximum() const
    { return m_pMax; }
private:
    Particle* m_pMin;
    Particle* m_pMax;
};

/*
// Is the capability described as an interval?
class IntervalCapabilityParticle: public CapabilityParticle
{
    // min, max, interval particles.
};
*/


class ICapability
{
public:
    virtual const CapabilityParticle* NextCapability()            = 0;

    virtual       Status              Reset()                     = 0;

    virtual const CapabilityParticle* Supports(StyleParticle::StyleParticleType        ) = 0;
//  virtual const CapabilityParticle* Supports(TransformParticle::TransformParticleType) = 0;
//  virtual const CapabilityParticle* Supports(LocationParticle::LocationParticleType  ) = 0;
};

/**********************************************************************
 *
 *  SINK INTERFACE
 *
 **********************************************************************/


// The Sink must implement this interface:
class ISink {
public:

    enum SinkStateType {
        keWaiting,      // Before Initialize, and/or after Terminate
        keInitialized,  // After Initialize, before Terminate or Abandon
        keAbandoned,    // After Abandon, before Terminate
    };
    // Allows the sink to report on its current state.
    virtual SinkStateType SinkState()                   = 0;

    // Calls upon the sink to identify what it is capable of consuming
    // Typically called while SinkState is keWaiting, but theoretically
    // should be valid at any time.
    // Returns:
    //  NULL - Accepts everything.
    //  ICapability* - Specifics of capabilities are enumerated
    virtual ICapability* GetMarkupCapabilities(/*...*/) = 0;

    // This always starts the Parsing event stream
    // Entry condition: SinkState() == keWaiting.
    // Exit condition if successful: SinkState() == keInitialized
    // Returns:
    //   keOk
    //   keNotReady: SinkState() is not keWaiting
    virtual Status Initialize(IEnvironment*)            = 0;

    // You get zero or more of these, based on the string.
    // Entry condition: SinkState() == keInitialized
    // Exit condition: SinkState() == keInitialized
    // Returns:
    //   keOk:
    //   keContinue:
    //   keAbandoned: request abandonment.
    //   keNotReady: SinkState() is not keInitialized
    virtual Status TextRun(ITextRun*,IEnvironment*)     = 0;

    // An error is detected by Parser.  About to terminate; info on what's wrong.
    // Entry condition: SinkState() == keInitialized
    // Exit condition: SinkState() == keAbandoned
    // Returns:
    //   keContinue: attempt to continue parsing?
    //   keAbandoned: abandoned.
    //   other: informational return.
    virtual Status Abandon(IAbandonment*,IEnvironment*) = 0;

    // This always ends the event stream.
    // Entry condition: SinkState() == keInitialized or == keAbandoned
    // Exit condition: SinkState() == keWaiting
    // Returns:
    //   keOk:
    //   keNotReady: SinkState() is not keInitialized or keAbandoned
    virtual Status Terminate(IEnvironment*)             = 0;

    // Gives you a pointer to its generator, if any.
    // THIS CAN RETURN NULL (say, for an app-hosted sink)
    virtual IGenerator* GetGenerator()                  = 0;
};



/**********************************************************************
 *
 *  CONSTRUCTION INTERFACE
 *
 **********************************************************************/
// The parser implements this tiny object, a class factory and
// lifetime manager for that parser type.  Theoretically, this is
// a singleton in the module that the parser resides.  When that
// module is loaded, the generator registers itself into the
// ATOM::IUniverse (via the implementation's constructor) and
// unregisters itself when unloaded.
class IGenerator
{
public:
    // The name of the markup this parser represents
    // such as "SVG" or "RTF" or ...
    virtual const StRange Name() const                            = 0;

    // Documentation of the parser/generator (for
    // version reporting, etc.)  A human-readable string.
    virtual const StRange Description() const                     = 0;

    // Creates an instance to a new Parser
    virtual Status Create(IParser**)                              = 0;

    // Takes a pointer to an existing parser and destroys it.
    virtual Status Destroy(IParser*)                              = 0;

    // Inexpensive way of determining if there's an associated sink.
    virtual bool HasSink() const                                  = 0;

    // And when you actually want one, this is how to get it.
    // Creates an instance to a new Parser
    virtual Status Create(ISink**)                                = 0;

    // Takes a pointer to an existing parser and destroys it.
    virtual Status Destroy(ISink*)                                = 0;
};


// The hosting application implements this interface to manage the various
// parsers.  Parsers (or more precisely, their Generators) use this to
// register themselves.  The application then queries to utilize whatever
// parsers are registered.
class IUniverse
{
public:
    // Registers a Parser's Generator, used by the parsing module
    // when introduced to the universe.
    virtual Status Register(IGenerator*)                          = 0;

    // Unregisters a Parser's Generator.
    virtual Status Unregister(IGenerator*)                        = 0;

    // How many parser/generators are registered?
    virtual int RegisteredCount()                                 = 0;

    // Gets a parser generator (by position in registration list)
    // to allow the application to begin a parsing operation.
    // 0 <= iIndex < RegisteredCount();
    //
    // Note: the iIndex is NOT a key, as registration may change
    // the order of Parser Generators... USE ONLY Name() TO
    // GET A PERSISTENT KEY for any specific Parser Generator.
    virtual IGenerator* GetGenerator(int iIndex)                  = 0;

    // Same as above, but indexed off the IGenerator::Name()
    // method.  USING Name() IS THE ONLY ASSURED WAY OF GETTING
    // THE RIGHT PARSER.
    virtual IGenerator* GetGenerator(const StRange& sName)        = 0;
};

// Implementing platforms need to implement this
// one standard method.
IUniverse* BigBang();

}} // namespace RichText::ATOM

#endif//_ATOM_H
