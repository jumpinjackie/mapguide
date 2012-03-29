//
//  Copyright (C) 2010-2011 by Autodesk, Inc.
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

#ifndef BIDICONVERTER_H_
#define BIDICONVERTER_H_

#include "StylizationAPI.h"
#include "StylizationDefs.h"
#include <vector>
#include <map>

// NOTE: this class should not be used outside of a UNICODE environment
#ifndef UNICODE
#pragma message("!!WARNING!! Attempting to use BIDIConverter.h in a non-UNICODE environment")
#endif


//===========================================================================
// various enums and typedefs needed for the algorithm

namespace ECharacterTypes
{
    // enum of the different directional traits unicode characters can have
    enum ECharacterType
    {
        ON = 0, // all others

        // strong direction
        L,      // Left-to-Right
        LRE,    // Left-to-Right Embedding
        LRO,    // Left-to-Right Override
        R,      // Right-to-Left
        RLE,    // Right-to-Left Embedding
        RLO,    // Right-to-Left Override
        AL,     // Right-to-Left Arabic

        // weak direction
        PDF,    // Pop Directional Format
        EN,     // European Number
        ES,     // European Number Separator
        ET,     // European Number Terminator
        AN,     // Arabic Number
        CS,     // Common Number Separator
        NSM,    // Nonspacing Mark
        BN,     // Boundary Neutral

        // neutral (no direction)
        B,      // Paragraph Separator
        S,      // Segment Separator
        WS,     // Whitespace
        N = ON  // Other Neutrals
    };

    typedef std::vector<ECharacterType> BIDIClassificationArray;
}

typedef std::wstring DisplayStr;
typedef std::map<unsigned int, std::vector<unsigned int> > ShapeGlyphs;
typedef std::map<unsigned int, unsigned int> MirrorGlyphs;

using namespace ECharacterTypes;


//===========================================================================
// The BIDIConverter class is used to create a LTR ordering of bi-directional
// unicode strings for the purpose of display.
class BIDIConverter
{
public:
    STYLIZATION_API BIDIConverter();
    STYLIZATION_API virtual ~BIDIConverter();

    // Optimized method to convert an input string.  If the string has no
    // right-to-left characters then:
    // * the method will directly return the supplied string
    // * the m_OriginalString and m_ConvertedString member variables are
    //   not updated (for performance reasons)
    STYLIZATION_API const DisplayStr& ConvertString(const DisplayStr& str);

    // methods to set/retrieve the original string
    STYLIZATION_API const DisplayStr& OriginalString();
    STYLIZATION_API void SetOriginalString(const DisplayStr& str);

    // method to retrieve the converted string
    STYLIZATION_API const DisplayStr& ConvertedString();

    // Although technically mirror belongs in the internal API there are sometimes
    // pieces of code that will automatically reverse strings in the Hebrew or Arabic
    // Language Settings.  This method allows us to mirror the converted string before
    // displaying so that when it's reversed it shows up correctly.
    STYLIZATION_API bool Mirror();

    STYLIZATION_API static const int _MaxNestedLevel = 61;

    // generates the static mappings
    STYLIZATION_API static bool GenerateMappings();

private:
    // returns true if the supplied string needs to be run through the conversion algorithm
    bool NeedsBIDIConversion(const DisplayStr& str);

    // runs the algorithm and sets the converted string data member
    void SetConvertedString();

    // classify all the characters in the input string
    bool ClassifyCharacters();

    // classify character in input str
    ECharacterType ClassifyCharacter(wchar_t cChar);
    ECharacterType ClassifyCharacter(unsigned int cChar);

    // returns the starting base level (i.e. either 0 or 1) based on initial character
    int GetStartingBaseLevel();

    // handles the initial explicit ordering by special characters; implements
    // X1-X9 of the bidi-unicode spec
    int ResolveExplicit(int nBaseLevel);

    // resolve Weak characters; implements W1-W7 of the bidi-unicode spec
    int ResolveWeak(int nBaseLevel);

    // resolve weak non-spacing marks; implements rule W1
    ECharacterType ResolveWeakNSM(int nIndex, int nLevel, int nLevelIndex);

    // resolve weak non-spacing marks; implements rule W2
    ECharacterType ResolveWeakEN(int nIndex, int nLevelIndex);

    // rule W3 is simple and implemented directly in ResolveWeak()

    // resolve European and common seperators between two European numbers;
    // implements rule W4
    ECharacterType ResolveWeakSeperator(int nIndex, int nLevel, int nLevelIndex, ECharacterType inputType);

    // resolve weak European terminators; implements rule W5
    ECharacterType ResolveWeakET(int nIndex, int nLevel);

    // rule W6 is simple and implemented directly in ResolveWeak()

    // replace EN with either L if found; implements rule W7
    ECharacterType ResolveWeakENFinal(int nIndex, int nLevel, int nLevelIndex);

    // resolve neutral characters; implements N1-N2 of the bidi-unicode spec
    int ResolveNeutral(int nBaseLevel);

    // resolve neutral chars in accordance with N1 and N2
    ECharacterType ResolveNeutralChar(int nIndex, int nLevel);

    // resolve implicit characters; implements l1-l2
    int ResolveImplied();

    // reorders the text; implements L1-L4 of the bidi-unicode spec
    int Reorder();

    // shapes a converted string
    void ShapeString();

    // reorders the parsed text and creates a single master string that
    // represents the converted text; implements rule L2
    int ReorderLevel(std::vector<std::pair<int, DisplayStr> >& stringLevelVector, unsigned int nLevel);

    // parses the string and level vector to a meaningful array of level/string pairs
    int ParseLevels(std::vector<std::pair<int, DisplayStr> >& stringLevelVector);

    // quick reverse function to mirror strings
    void reverse(DisplayStr& str);

    // returns the static mapping of glyphs to possible shaping code points
    static ShapeGlyphs& GetShapeMapping();

    // returns the static vector of strings (stored in pairs) used to enforce ligature
    static std::vector<DisplayStr>& GetLigaturePairs();

    // returns the static mapping of glyphs with mirror code points
    static MirrorGlyphs& GetMirrorMapping();

private:
    DisplayStr m_OriginalString;
    DisplayStr m_ConvertedString;

    bool m_Converted;
    bool m_Mirrored;

    // array of character types based on index
    BIDIClassificationArray m_ClassificationArray;

    std::vector<int> m_Levels;
};

#endif
