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

// DESCRIPTION:
// Implementation of the BIDIConverter class which creates a string
// used for LTR display purposes from a bidi-unicode string.

#include "stdafx.h"
#include "BIDIConverter.h"
#include <stack>

#pragma warning(disable : 4482)


// generate the static mappings at module load time to avoid threading issues
static bool bInit = BIDIConverter::GenerateMappings();


/////////////////// Glyph Mapping support data (shaping and mirroring) ///////////////////

// source glyph, right join, left join, mid join, join type {0 = right join, 1 = dual joining}
unsigned int glyphShapeMapping[][5] = {
        {0x621, 0xFE80, 0xFE80, 0xFE80, 1},     // Hamza
        {0x622, 0xFE82, 0xFE81, 0xFE82, 0},     // Alef With Madda Above
        {0x623, 0xFE84, 0xFE83, 0xFE84, 0},     // Alef With Hamza Above
        {0x624, 0xFE86, 0xFE85, 0xFE86, 0},     // Waw  With Hamza Above
        {0x625, 0xFE88, 0xFE87, 0xFE88, 0},     // Alef With Hamza Below
        {0x626, 0xFE8A, 0xFE8B, 0xFE8C, 1},     // Yeh  With Hamza Above
        {0x627, 0xFE8E, 0xFE8D, 0xFE8E, 0},     // Alef
        {0x628, 0xFE90, 0xFE91, 0xFE92, 1},     // Beh
        {0x629, 0xFE94, 0xFE93, 0xFE93, 0},     // Teh Marburta
        {0x62A, 0xFE96, 0xFE97, 0xFE98, 1},     // Teh
        {0x62B, 0xFE9A, 0xFE9B, 0xFE9C, 1},     // Theh
        {0x62C, 0xFE9E, 0xFE9F, 0xFEA0, 1},     // Jeem
        {0x62D, 0xFEA2, 0xFEA3, 0xFEA4, 1},     // Hah
        {0x62E, 0xFEA6, 0xFEA7, 0xFEA8, 1},     // Khah
        {0x62F, 0xFEAA, 0xFEA9, 0xFEAA, 0},     // Dal

        {0x630, 0xFEAC, 0xFEAB, 0xFEAC, 0},     // Thal
        {0x631, 0xFEAE, 0xFEAD, 0xFEAE, 0},     // Reh
        {0x632, 0xFEB0, 0xFEAF, 0xFEB0, 0},     // Zain
        {0x633, 0xFEB2, 0xFEB3, 0xFEB4, 1},     // Seen
        {0x634, 0xFEB6, 0xFEB7, 0xFEB8, 1},     // Sheen
        {0x635, 0xFEBA, 0xFEBB, 0xFEBC, 1},     // Sad
        {0x636, 0xFEBE, 0xFEBF, 0xFEC0, 1},     // Dad
        {0x637, 0xFEC2, 0xFEC3, 0xFEC4, 1},     // Tah
        {0x638, 0xFEC6, 0xFEC7, 0xFEC8, 1},     // Zah
        {0x639, 0xFECA, 0xFECB, 0xFECC, 1},     // Ain
        {0x63A, 0xFECE, 0xFECF, 0xFED0, 1},     // Ghain

        {0x641, 0xFED2, 0xFED3, 0xFED4, 1},     // Feh
        {0x642, 0xFED6, 0xFED7, 0xFED8, 1},     // Qaf
        {0x643, 0xFEDA, 0xFEDB, 0xFEDC, 1},     // Kaf
        {0x644, 0xFEDE, 0xFEDF, 0xFEE0, 1},     // Lam
        {0x645, 0xFEE2, 0xFEE3, 0xFEE4, 1},     // Meem
        {0x646, 0xFEE6, 0xFEE7, 0xFEE8, 1},     // Noon
        {0x647, 0xFEEA, 0xFEEB, 0xFEEC, 1},     // Heh
        {0x648, 0xFEEE, 0xFEED, 0xFEEE, 0},     // Waw
        {0x649, 0xFEF0, 0xFEEF, 0xFEF0, 0},     // Alef Maksura
        {0x64A, 0xFEF2, 0xFEF3, 0xFEF4, 1}};    // Yeh

// input glyph 1, input glyph 2, replacement glyph
unsigned int glyphLigatureMapping[][3] = {
        {0xFEDF, 0xFE82, 0xFEF5},   // Lam (initial) + Alef With Madda above (final) => Lam-Alef With Madda above (isolated)
        {0xFEE0, 0xFE82, 0xFEF6},   // Lam ( medial) + Alef With Madda above (final) => Lam-Alef With Madda above (   final)
        {0xFEDF, 0xFE84, 0xFEF7},   // Lam (initial) + Alef With Hamza above (final) => Lam-Alef With Hamza above (isolated)
        {0xFEE0, 0xFE84, 0xFEF8},   // Lam ( medial) + Alef With Hamza above (final) => Lam-Alef With Hamza above (   final)
        {0xFEDF, 0xFE88, 0xFEF9},   // Lam (initial) + Alef With Hamza below (final) => Lam-Alef With Hamza below (isolated)
        {0xFEE0, 0xFE88, 0xFEFA},   // Lam ( medial) + Alef With Hamza below (final) => Lam-Alef With Hamza below (   final)
        {0xFEDF, 0xFE8E, 0xFEFB},   // Lam (initial) + Alef                  (final) => Lam-Alef                  (isolated)
        {0xFEE0, 0xFE8E, 0xFEFC}};  // Lam ( medial) + Alef                  (final) => Lam-Alef                  (   final)


enum ELinkageType
{
    eNoLinkage  = 0,
    eLinkAfter  = 1,
    eLinkBefore = 2,
    eLinkMid    = 3
};


// this is a list of codepoint pairs that replace each other
unsigned int glyphMirrorMapping[][2] = {
    {0x0028, 0x0029}, {0x003C, 0x003E}, {0x005B, 0x005D}, {0x007B, 0x007D},
    {0x00AB, 0x00BB}, {0x0F3A, 0x0F3B}, {0x0F3C, 0x0F3D}, {0x169B, 0x169C},
    {0x2039, 0x203A}, {0x2045, 0x2046}, {0x207D, 0x207E}, {0x208D, 0x208E},
    {0x2208, 0x220B}, {0x220A, 0x220D}, {0x220C, 0x2209}, {0x2215, 0x29F5},
    {0x223D, 0x223C}, {0x2252, 0x2253}, {0x2254, 0x2255}, {0x2264, 0x2265},
    {0x2266, 0x2267}, {0x2268, 0x2269}, {0x226A, 0x226B}, {0x226E, 0x226F},
    {0x2270, 0x2271}, {0x2272, 0x2273}, {0x2274, 0x2275}, {0x2276, 0x2277},
    {0x2278, 0x2279}, {0x227A, 0x227B}, {0x227C, 0x227D}, {0x227E, 0x227F},
    {0x2280, 0x2281}, {0x2282, 0x2283}, {0x2284, 0x2285}, {0x2286, 0x2287},
    {0x2288, 0x2289}, {0x228A, 0x228B}, {0x228F, 0x2290}, {0x2291, 0x2292},
    {0x2298, 0x29B8}, {0x22A3, 0x22A2}, {0x22A8, 0x2AE4}, {0x22AB, 0x2AE5},
    {0x22B1, 0x22B0}, {0x22B3, 0x22B2}, {0x22B5, 0x22B4}, {0x22B7, 0x22B6},
    {0x22CA, 0x22C9}, {0x22CC, 0x22CB}, {0x22D0, 0x22D1}, {0x22D6, 0x22D7},
    {0x22D8, 0x22D9}, {0x22DA, 0x22DB}, {0x22DC, 0x22DD}, {0x22DE, 0x22DF},
    {0x22E0, 0x22E1}, {0x22E2, 0x22E3}, {0x22E4, 0x22E5}, {0x22E6, 0x22E7},
    {0x22E8, 0x22E9}, {0x22EA, 0x22EB}, {0x22EC, 0x22ED}, {0x22F0, 0x22F1},
    {0x22F2, 0x22FA}, {0x22F4, 0x22FC}, {0x22F7, 0x22FE}, {0x22FB, 0x22F3},
    {0x22FD, 0x22F6}, {0x2308, 0x2309}, {0x230A, 0x230B}, {0x2329, 0x232A},
    {0x2768, 0x2769}, {0x276A, 0x276B}, {0x276C, 0x276D}, {0x276E, 0x276F},
    {0x2770, 0x2771}, {0x2772, 0x2773}, {0x2774, 0x2775}, {0x27C3, 0x27C4},
    {0x27C5, 0x27C6}, {0x27C8, 0x27C9}, {0x27D5, 0x27D6}, {0x27DD, 0x27DE},
    {0x27E2, 0x27E3}, {0x27E4, 0x27E5}, {0x27E6, 0x27E7}, {0x27E8, 0x27E9},
    {0x27EA, 0x27EB}, {0x27EC, 0x27ED}, {0x27EE, 0x27EF}, {0x2983, 0x2984},
    {0x2985, 0x2986}, {0x2987, 0x2988}, {0x2989, 0x298A}, {0x298B, 0x298C},
    {0x298D, 0x2990}, {0x298F, 0x298E}, {0x2991, 0x2992}, {0x2993, 0x2994},
    {0x2995, 0x2996}, {0x2997, 0x2998}, {0x29B8, 0x2298}, {0x29C1, 0x29C0},
    {0x29C5, 0x29C4}, {0x29D0, 0x29CF}, {0x29D2, 0x29D1}, {0x29D5, 0x29D4},
    {0x29D9, 0x29D8}, {0x29DB, 0x29DA}, {0x29F8, 0x29F9}, {0x29FC, 0x29FD},
    {0x2A2B, 0x2A2C}, {0x2A2D, 0x2A2E}, {0x2A34, 0x2A35}, {0x2A3C, 0x2A3D},
    {0x2A64, 0x2A65}, {0x2A79, 0x2A7A}, {0x2A7D, 0x2A7E}, {0x2A7F, 0x2A80},
    {0x2A81, 0x2A82}, {0x2A83, 0x2A84}, {0x2A8B, 0x2A8C}, {0x2A91, 0x2A92},
    {0x2A93, 0x2A94}, {0x2A95, 0x2A96}, {0x2A97, 0x2A98}, {0x2A99, 0x2A9A},
    {0x2A9B, 0x2A9C}, {0x2AA1, 0x2AA2}, {0x2AA6, 0x2AA7}, {0x2AA8, 0x2AA9},
    {0x2AAA, 0x2AAB}, {0x2AAC, 0x2AAD}, {0x2AAF, 0x2AB0}, {0x2AB3, 0x2AB4},
    {0x2ABB, 0x2ABC}, {0x2ABD, 0x2ABE}, {0x2ABF, 0x2AC0}, {0x2AC1, 0x2AC2},
    {0x2AC3, 0x2AC4}, {0x2AC5, 0x2AC6}, {0x2ACD, 0x2ACE}, {0x2ACF, 0x2AD0},
    {0x2AD1, 0x2AD2}, {0x2AD3, 0x2AD4}, {0x2AD5, 0x2AD6}, {0x2ADE, 0x22A6},
    {0x2AE4, 0x22A8}, {0x2AEC, 0x2AED}, {0x2AF7, 0x2AF8}, {0x2AF9, 0x2AFA},
    {0x2E02, 0x2E03}, {0x2E04, 0x2E05}, {0x2E09, 0x2E0A}, {0x2E0C, 0x2E0D},
    {0x2E1C, 0x2E1D}, {0x2E20, 0x2E21}, {0x2E22, 0x2E23}, {0x2E24, 0x2E25},
    {0x2E26, 0x2E27}, {0x2E28, 0x2E29}, {0x3008, 0x3009}, {0x300A, 0x300B},
    {0x300C, 0x300D}, {0x300E, 0x300F}, {0x3010, 0x3011}, {0x3014, 0x3015},
    {0x3016, 0x3017}, {0x3018, 0x3019}, {0x301A, 0x301B}, {0xFE59, 0xFE5A},
    {0xFE5B, 0xFE5C}, {0xFE5D, 0xFE5E}, {0xFE64, 0xFE65}, {0xFF08, 0xFF09},
    {0xFF1C, 0xFF1E}, {0xFF3B, 0xFF3D}, {0xFF5B, 0xFF5D}, {0xFF5F, 0xFF60},
    {0xFF62, 0xFF63}};

///////////////// end Glyph Mapping support data (shaping and mirroring) /////////////////


// default constructor
BIDIConverter::BIDIConverter()
    : m_OriginalString(L"")
    , m_ConvertedString(L"")
    , m_Converted(true)
    , m_Mirrored(false)
{
}


// destructor
BIDIConverter::~BIDIConverter()
{
}


// Optimized method to convert an input string.  If the string has no
// right-to-left characters then:
// * the method will directly return the supplied string
// * the m_OriginalString and m_ConvertedString member variables are
//   not updated (for performance reasons)
const DisplayStr& BIDIConverter::ConvertString(const DisplayStr& str)
{
    // return the input string if no conversion is needed
    if (!NeedsBIDIConversion(str))
        return str;

    // otherwise process the string
    SetOriginalString(str);

    return m_ConvertedString;
}


// returns the original string
const DisplayStr& BIDIConverter::OriginalString()
{
    return m_OriginalString;
}


// sets the original string
void BIDIConverter::SetOriginalString(const DisplayStr& str)
{
    m_OriginalString = str;
    m_ConvertedString = L"";

    m_Converted = false;;
    m_Mirrored = false;;

    // if it's empty then don't worry about it
    if (m_OriginalString.empty())
    {
        m_Converted = true;
    }
    else
    {
        SetConvertedString();
    }
}


// return the converted string
const DisplayStr& BIDIConverter::ConvertedString()
{
    return m_ConvertedString;
}


// Although technically mirror belongs in the internal API there are sometimes
// pieces of code that will automatically reverse strings in the Hebrew or Arabic
// Language Settings.  This method allows us to mirror the converted string before
// displaying so that when it's reversed it shows up correctly.
bool BIDIConverter::Mirror()
{
    try
    {
        reverse(m_ConvertedString);
        m_Mirrored = true;
    }
    catch (...)
    {
        return false;
    }
    return true;
}


// shapes a converted string
void BIDIConverter::ShapeString()
{
    ShapeGlyphs& shapeGlyphs = GetShapeMapping();

    DisplayStr tempString = m_ConvertedString;
    std::vector<unsigned int> glyphMappings;
    std::vector<unsigned int> currentGlyphMappings;

    glyphMappings.resize(4);
    for (size_t i=0; i<tempString.size(); ++i)
    {
        // check if this is a shaped glyph
        if (shapeGlyphs.find(tempString[i]) != shapeGlyphs.end())
        {
            currentGlyphMappings = shapeGlyphs[tempString[i]];
            int nLinkage = eNoLinkage;

            // if it's not at the end of the string and the following character has linkage
            if (i > 0 && (shapeGlyphs.find(tempString[i-1]) != shapeGlyphs.end()))
            {
                nLinkage |= eLinkAfter;
            }

            // if it's not at the beginning of the string and the preceding character has linkage
            if (i < (tempString.size() - 1) && (shapeGlyphs.find(tempString[i+1]) != shapeGlyphs.end()))
            {
                glyphMappings = shapeGlyphs[tempString[i+1]];
                if (glyphMappings[3] == 1)
                {
                    nLinkage |= eLinkBefore;
                }
            }

            // change glyph mapping depending on linkage
            switch (nLinkage)
            {
            case eLinkAfter:  // just linked after
                m_ConvertedString[i] = static_cast<wchar_t>(currentGlyphMappings[1]);
                break;
            case eLinkBefore: // just linked before
                m_ConvertedString[i] = static_cast<wchar_t>(currentGlyphMappings[0]);
                break;
            case eLinkMid:    // linked both directions
                m_ConvertedString[i] = static_cast<wchar_t>(currentGlyphMappings[2]);
                break;
            }
        }
    }

    // enforce ligature
    std::vector<DisplayStr>& ligaturePairs = GetLigaturePairs();
    size_t numVals = ligaturePairs.size();
    for (size_t i=0; i<numVals;)
    {
        DisplayStr& key = ligaturePairs[i++];
        DisplayStr& val = ligaturePairs[i++];

        size_t pos = 0;
        size_t findPos;
        while ((findPos = m_ConvertedString.find(key, pos)) != std::wstring::npos)
        {
            m_ConvertedString.replace(findPos, key.size(), val);
            pos = findPos + val.size();
        }
    }
}


// returns true if the supplied string needs to be run through the conversion algorithm
bool BIDIConverter::NeedsBIDIConversion(const DisplayStr& str)
{
    const wchar_t* pStr = str.c_str();

    while (*pStr)
    {
        unsigned int cChar = static_cast<unsigned int>(*pStr++);

        // For now, do a very simple check.  If the string contains any
        // character outside the range [0..255] then we'll BIDI convert it.
        // The vast majority of strings have all their characters in this
        // range, so this minimizes the performance impact for most use
        // cases.
        if (cChar > 0xFF)
            return true;
/*
        // If we ever need this method to be more accurate then we can
        // use the code below.

        // check if in default right-to-left Arabic range
        if (   (cChar >= 0x600 && cChar <= 0x7BF)
            || (cChar >= 0xFB50 && cChar <= 0xFDFF)
            || (cChar >= 0xFE70 && cChar <= 0xFEFF))
        {
            return true;
        }

        // check if in default right-to-left range
        if (   (cChar >= 0x590 && cChar <= 0x5FF)
            || (cChar >= 0x7C0 && cChar <= 0x8FF)
            || (cChar >= 0xFB1D && cChar <= 0xFB4F)
            || (cChar >= 0x10800 && cChar <= 0x10FFF)
            || (cChar == 0x200F))
        {
            return true;
        }

        // check for right-to-left embedding
        if (0x202B == cChar)
            return true;

        // check for right-to-left override
        if (0x202E == cChar)
            return true;
*/
    }

    return false;
}


// runs the algorithm and sets the converted string data member
void BIDIConverter::SetConvertedString()
{
    ClassifyCharacters();
    _ASSERT(m_OriginalString.length() == m_ClassificationArray.size());

    // Programmer's note: At this point in the algorithm we should be
    // separating the text into paragraphs, but for the first rev of this
    // development we're going to assume that the text in question is a
    // single paragraph.

    // Get the starting base level (either 0 or 1) based on initial
    // character.  This implements P2 and P3 of the unicode spec.
    int nBaseLevel = GetStartingBaseLevel();
    _ASSERT(0 == nBaseLevel || 1 == nBaseLevel);

    // resolve explicit ordering characters; this implements X1-X9
    ResolveExplicit(nBaseLevel);

    // resolve weak characters; this implements W1-W6
    ResolveWeak(nBaseLevel);

    // resolve neutral characters; this implmenets N1-N2
    ResolveNeutral(nBaseLevel);

    // resolve implicit characters; this implements I1-I2
    ResolveImplied();

    // reorders the text; this implements L1-L4
    Reorder();

    m_Converted = true;
}


// returns the starting base level (i.e. either 0 or 1) based on initial character
int BIDIConverter::GetStartingBaseLevel()
{
    int nRetLevel = 0;

    // find the first strong direction and base the level off of that
    for (size_t i=0; i<m_ClassificationArray.size(); ++i)
    {
        int nCharacter = m_ClassificationArray[i];
        if (ECharacterTypes::L == nCharacter || ECharacterTypes::LRO == nCharacter)
        {
            return 0;
        }

        if (ECharacterTypes::R   == nCharacter ||
            ECharacterTypes::RLO == nCharacter ||
            ECharacterTypes::AL  == nCharacter)
        {
            return 1;
        }
    }

    return nRetLevel;
}


// classify all the characters in the input string
bool BIDIConverter::ClassifyCharacters()
{
    try
    {
        // clear out existing array
        m_ClassificationArray.clear();
        m_ClassificationArray.resize(m_OriginalString.size());

        // Iterate characters in input string and classify them character by
        // character.  Keep a running tab on which direction the character is
        // so we can determine if it's all left or right and short circuit the
        // algorithm.
        for (size_t i=0; i<m_OriginalString.length(); ++i)
        {
            m_ClassificationArray[i] = ClassifyCharacter(m_OriginalString[i]);
        }
    }
    catch (...)
    {
        return false;
    }

    return true;
}


// classify character in input str
ECharacterType BIDIConverter::ClassifyCharacter(wchar_t cChar)
{
    return ClassifyCharacter(static_cast<unsigned int>(cChar));
}


// This code is constructed from the DerivedBidiClass.txt.  At a
// certain point it might be reasonable to make it a little less cheesy.
ECharacterType BIDIConverter::ClassifyCharacter(unsigned int cChar)
{
    // default for the vast majority of cases.
    ECharacterType eRetVal = ECharacterTypes::L;

    // check special embedding marks
    if (cChar >= 8234 && cChar <= 8238)
    {
             if (0x202A == cChar) eRetVal = ECharacterTypes::LRE;
        else if (0x202B == cChar) eRetVal = ECharacterTypes::RLE;
        else if (0x202C == cChar) eRetVal = ECharacterTypes::PDF;
        else if (0x202D == cChar) eRetVal = ECharacterTypes::LRO;
        else if (0x202E == cChar) eRetVal = ECharacterTypes::RLO;
    }
    else // check for European separator
    if (   (cChar == 43)
        || (cChar == 45)
        || (cChar >= 8314 && cChar <= 8315)
        || (cChar >= 8330 && cChar <= 8331)
        || (cChar == 8722)
        || (cChar == 64297)
        || (cChar == 65122)
        || (cChar == 65123)
        || (cChar == 65291)
        || (cChar == 65293))
    {
        eRetVal = ECharacterTypes::ES;
    }
    else // check for European number
    if (   (cChar >= 48 && cChar <= 57)
        || (cChar >= 178 && cChar <= 179)
        || (cChar == 185)
        || (cChar >= 1776 && cChar <= 1785)
        || (cChar == 8304)
        || (cChar >= 8308 && cChar <= 8313)
        || (cChar >= 8320 && cChar <= 8329)
        || (cChar >= 9352 && cChar <= 9371)
        || (cChar >= 65296 && cChar <= 65305)
        || (cChar >= 120782 && cChar <= 120831)) // switched to hex at this point; probably should resolve above
    {
        eRetVal = ECharacterTypes::EN;
    }
    else // check for European terminator
    if (   (cChar >= 0x23 && cChar <= 0x25)
        || (cChar >= 0xA2 && cChar <= 0xA5)
        || (cChar >= 0xB0 && cChar <= 0xB1)
        || (cChar >= 0x609 && cChar <= 0x060A)
        || (cChar == 0x66A)
        || (cChar >= 0x9F2 && cChar <= 0x09F3)
        || (cChar == 0xAF1)
        || (cChar == 0xBF9)
        || (cChar == 0xE3F)
        || (cChar == 0x17DB)
        || (cChar >= 0x2030 && cChar <= 0x2034)
        || (cChar >= 0x20A0 && cChar <= 0x20B5)
        || (cChar == 0x212E)
        || (cChar == 0x2213)
        || (cChar == 0xFE5F)
        || (cChar >= 0xFE69 && cChar <= 0xFE6A)
        || (cChar >= 0xFF03 && cChar <= 0xFF05)
        || (cChar >= 0xFFE0 && cChar <= 0xFFE1)
        || (cChar >= 0xFFE5 && cChar <= 0xFFE6))
    {
        eRetVal = ECharacterTypes::ET;
    }
    else // check for Arabic number
    if (   (cChar >= 0x600 && cChar <= 0x0603)
        || (cChar >= 0x660 && cChar <= 0x669)
        || (cChar >= 0x66B && cChar <= 0x066C)
        || (cChar == 0x6DD))
    {
        eRetVal = ECharacterTypes::AN;
    }
    else // check for Common separator
    if (   (cChar == 0x2C)
        || (cChar >= 0x2E && cChar <= 0x2F)
        || (cChar == 0x3A)
        || (cChar == 0xA0)
        || (cChar == 0x60C)
        || (cChar == 0x202F)
        || (cChar == 0x2044)
        || (cChar == 0xFE50)
        || (cChar == 0xFE52)
        || (cChar == 0xFE55)
        || (cChar == 0xFF0C)
        || (cChar >= 0xFF0E && cChar <= 0xFF0F)
        || (cChar == 0xFF1A))
    {
        eRetVal = ECharacterTypes::CS;
    }
    else // check for paragraph separator
    if (   (cChar == 0xA)
        || (cChar == 0xD)
        || (cChar >= 0x1C && cChar <= 0x1E)
        || (cChar == 0x85)
        || (cChar == 0x2029))
    {
        eRetVal = ECharacterTypes::B;
    }
    else // check for segment separator
    if (   (cChar == 0x9)
        || (cChar == 0xB)
        || (cChar == 0x1F))
    {
        eRetVal = ECharacterTypes::S;
    }
    else // check for white space
    if (   (cChar == 0xC)
        || (cChar == 0x20)
        || (cChar == 0x1680)
        || (cChar == 0x180E)
        || (cChar >= 0x2000 && cChar <= 0x200A)
        || (cChar == 0x2028)
        || (cChar == 0x205F)
        || (cChar == 0x3000))
    {
        eRetVal = ECharacterTypes::WS;
    }
    else // check for Boundary Neutral
    if (   (cChar >= 0x0 && cChar <= 0x8)
        || (cChar >= 0xE && cChar <= 0x1B)
        || (cChar >= 0x7F && cChar <= 0x84)
        || (cChar >= 0x86 && cChar <= 0x9F)
        || (cChar == 0xAD)
        || (cChar == 0x70F)
        || (cChar >= 0x200B && cChar <= 0x200D)
        || (cChar >= 0x2060 && cChar <= 0x206F)
        || (cChar >= 0xFDD0 && cChar <= 0xFDEF)
        || (cChar == 0xFEFF)
        || (cChar >= 0xFFF0 && cChar <= 0xFFF8)
        || (cChar >= 0xFFFE && cChar <= 0xFFFF)
        || (cChar >= 0x1D173 && cChar <= 0x1D17A)
        || (cChar >= 0x1FFFE && cChar <= 0x1FFFF)
        || (cChar >= 0x2FFFE && cChar <= 0x2FFFF)
        || (cChar >= 0x3FFFE && cChar <= 0x3FFFF)
        || (cChar >= 0x4FFFE && cChar <= 0x4FFFF)
        || (cChar >= 0x5FFFE && cChar <= 0x5FFFF)
        || (cChar >= 0x6FFFE && cChar <= 0x6FFFF)
        || (cChar >= 0x7FFFE && cChar <= 0x7FFFF)
        || (cChar >= 0x8FFFE && cChar <= 0x8FFFF)
        || (cChar >= 0x9FFFE && cChar <= 0x9FFFF)
        || (cChar >= 0xAFFFE && cChar <= 0xAFFFF)
        || (cChar >= 0xBFFFE && cChar <= 0xBFFFF)
        || (cChar >= 0xCFFFE && cChar <= 0xCFFFF)
        || (cChar >= 0xDFFFE && cChar <= 0xE00FF)
        || (cChar >= 0xEFFFE && cChar <= 0xEFFFF)
        || (cChar >= 0xFFFFE && cChar <= 0xFFFFF)
        || (cChar >= 0x10FFFE && cChar <= 0x10FFFF))
    {
        eRetVal = ECharacterTypes::BN;
    }
    else // check for non-spacing mark
    if (   (cChar >= 0x300 && cChar <= 0x036F)
        || (cChar >= 0x483 && cChar <= 0x489)
        || (cChar >= 0x591 && cChar <= 0x5BD)
        || (cChar == 0x5BF)
        || (cChar >= 0x5C1 && cChar <= 0x5C2)
        || (cChar >= 0x5C4 && cChar <= 0x5C5)
        || (cChar == 0x5C7)
        || (cChar >= 0x610 && cChar <= 0x61A)
        || (cChar >= 0x64B && cChar <= 0x065E)
        || (cChar == 0x670)
        || (cChar >= 0x6D6 && cChar <= 0x6DC)
        || (cChar >= 0x6DE  && cChar <= 0x6E4)
        || (cChar >= 0x6E7 && cChar <= 0x6E8)
        || (cChar >= 0x06EA && cChar <= 0x6ED)
        || (cChar == 0x711)
        || (cChar >= 0x730 && cChar <= 0x074A)
        || (cChar >= 0x7A6 && cChar <= 0x7B0)
        || (cChar >= 0x7EB && cChar <= 0x7F3)
        || (cChar >= 0x901 && cChar <= 0x902)
        || (cChar == 0x93C)
        || (cChar >= 0x941 && cChar <= 0x948)
        || (cChar == 0x94D)
        || (cChar >= 0x951 && cChar <= 0x954)
        || (cChar >= 0x962 && cChar <= 0x963)
        || (cChar == 0x981)
        || (cChar == 0x9BC)
        || (cChar >= 0x9C1 && cChar <= 0x09C4)
        || (cChar == 0x9CD)
        || (cChar >= 0x9E2 && cChar <= 0x9E3)
        || (cChar >= 0xA01 && cChar <= 0xA02)
        || (cChar == 0xA3C)
        || (cChar >= 0xA41 && cChar <= 0xA42)
        || (cChar >= 0xA47 && cChar <= 0xA48)
        || (cChar >= 0xA4B && cChar <= 0xA4D)
        || (cChar == 0xA51)
        || (cChar >= 0xA70 && cChar <= 0xA71)
        || (cChar == 0xA75)
        || (cChar >= 0xA81 && cChar <= 0xA82)
        || (cChar == 0xABC)
        || (cChar >= 0xAC1 && cChar <= 0xAC5)
        || (cChar >= 0xAC7 && cChar <= 0xAC8)
        || (cChar == 0xACD)
        || (cChar >= 0xAE2 && cChar <= 0xAE3)
        || (cChar == 0xB01)
        || (cChar == 0xB3C)
        || (cChar == 0xB3F)
        || (cChar >= 0xB41 && cChar <= 0xB44)
        || (cChar == 0xB4D)
        || (cChar == 0xB56)
        || (cChar >= 0xB62 && cChar <= 0xB63)
        || (cChar == 0xB82)
        || (cChar == 0xBC0)
        || (cChar == 0xBCD)
        || (cChar >= 0xC3E && cChar <= 0xC40)
        || (cChar >= 0xC46 && cChar <= 0xC48)
        || (cChar >= 0xC4A && cChar <= 0xC4D)
        || (cChar >= 0xC55 && cChar <= 0xC56)
        || (cChar >= 0xC62 && cChar <= 0xC63)
        || (cChar == 0xCBC)
        || (cChar >= 0xCCC && cChar <= 0xCCD)
        || (cChar >= 0xCE2 && cChar <= 0xCE3)
        || (cChar >= 0xD41 && cChar <= 0xD44)
        || (cChar == 0xD4D)
        || (cChar >= 0xD62 && cChar <= 0xD63)
        || (cChar == 0xDCA)
        || (cChar >= 0xDD2 && cChar <= 0xDD4)
        || (cChar == 0xDD6)
        || (cChar == 0xE31)
        || (cChar >= 0xE34 && cChar <= 0xE3A)
        || (cChar >= 0xE47 && cChar <= 0xE4E)
        || (cChar == 0xEB1)
        || (cChar >= 0xEB4 && cChar <= 0xEB9)
        || (cChar >= 0xEBB && cChar <= 0xEBC)
        || (cChar >= 0xEC8 && cChar <= 0xECD)
        || (cChar >= 0xF18 && cChar <= 0xF19)
        || (cChar == 0xF35)
        || (cChar == 0xF37)
        || (cChar == 0xF39)
        || (cChar >= 0xF71 && cChar <= 0xF7E)
        || (cChar >= 0xF80 && cChar <= 0xF84)
        || (cChar >= 0xF86 && cChar <= 0xF87)
        || (cChar >= 0xF90 && cChar <= 0xF97)
        || (cChar >= 0xF99 && cChar <= 0xFBC)
        || (cChar == 0xFC6)
        || (cChar >= 0x102D && cChar <= 0x1030)
        || (cChar >= 0x1032 && cChar <= 0x1037)
        || (cChar >= 0x1039 && cChar <= 0x103A)
        || (cChar >= 0x103D && cChar <= 0x103E)
        || (cChar >= 0x1058 && cChar <= 0x1059)
        || (cChar >= 0x105E && cChar <= 0x1060)
        || (cChar >= 0x1071 && cChar <= 0x1074)
        || (cChar == 0x1082)
        || (cChar >= 0x1085 && cChar <= 0x1086)
        || (cChar == 0x108D)
        || (cChar == 0x135F)
        || (cChar >= 0x1712 && cChar <= 0x1714)
        || (cChar >= 0x1732 && cChar <= 0x1734)
        || (cChar >= 0x1752 && cChar <= 0x1753)
        || (cChar >= 0x1772 && cChar <= 0x1773)
        || (cChar >= 0x17B7 && cChar <= 0x17BD)
        || (cChar == 0x17C6)
        || (cChar >= 0x17C9 && cChar <= 0x17D3)
        || (cChar == 0x17DD)
        || (cChar >= 0x180B && cChar <= 0x180D)
        || (cChar == 0x18A9)
        || (cChar >= 0x1920 && cChar <= 0x1922)
        || (cChar >= 0x1927 && cChar <= 0x1928)
        || (cChar == 0x1932)
        || (cChar >= 0x1939 && cChar <= 0x193B)
        || (cChar >= 0x1A17 && cChar <= 0x1A18)
        || (cChar >= 0x1B00 && cChar <= 0x1B03)
        || (cChar == 0x1B34)
        || (cChar >= 0x1B36 && cChar <= 0x1B3A)
        || (cChar == 0x1B3C)
        || (cChar == 0x1B42)
        || (cChar >= 0x1B6B && cChar <= 0x1B73)
        || (cChar >= 0x1B80 && cChar <= 0x1B81)
        || (cChar >= 0x1BA2 && cChar <= 0x1BA5)
        || (cChar >= 0x1BA8 && cChar <= 0x1BA9)
        || (cChar >= 0x1C2C && cChar <= 0x1C33)
        || (cChar >= 0x1C36 && cChar <= 0x1C37)
        || (cChar >= 0x1DC0 && cChar <= 0x1DE6)
        || (cChar >= 0x1DFE && cChar <= 0x1DFF)
        || (cChar >= 0x20D0 && cChar <= 0x20DC)
        || (cChar >= 0x20DD && cChar <= 0x20E0)
        || (cChar >= 0x20E1 && cChar <= 0x20F0)
        || (cChar >= 0x2DE0 && cChar <= 0x2DFF)
        || (cChar >= 0x302A && cChar <= 0x302F)
        || (cChar >= 0x3099 && cChar <= 0x309A)
        || (cChar == 0xA66F)
        || (cChar >= 0xA670 && cChar <= 0xA672)
        || (cChar >= 0xA67C && cChar <= 0xA67D)
        || (cChar == 0xA802)
        || (cChar == 0xA806)
        || (cChar == 0xA80B)
        || (cChar >= 0xAC7 && cChar <= 0xAC8)
        || (cChar == 0xA66F)
        || (cChar >= 0xAC7 && cChar <= 0xAC8)
        || (cChar >= 0xA825 && cChar <= 0xA826)
        || (cChar == 0xA8C4)
        || (cChar >= 0xA926 && cChar <= 0xA92D)
        || (cChar >= 0xA947 && cChar <= 0xA951)
        || (cChar >= 0xAA29 && cChar <= 0xAA2E)
        || (cChar >= 0xAA31 && cChar <= 0xAA32)
        || (cChar >= 0xAA35 && cChar <= 0xAA36)
        || (cChar == 0xAA43)
        || (cChar == 0xAA4C)
        || (cChar == 0xFB1E)
        || (cChar >= 0xFE00 && cChar <= 0xFE0F)
        || (cChar >= 0xFE20 && cChar <= 0xFE26)
        || (cChar == 0x101FD)
        || (cChar >= 0x10A01 && cChar <= 0x10A03)
        || (cChar >= 0x10A05 && cChar <= 0x10A06)
        || (cChar >= 0x10A0C && cChar <= 0x10A0F)
        || (cChar >= 0x10A38 && cChar <= 0x10A3A)
        || (cChar == 0x10A3F)
        || (cChar >= 0x1D167 && cChar <= 0x1D169)
        || (cChar >= 0x1D17B && cChar <= 0x1D182)
        || (cChar >= 0x1D185 && cChar <= 0x1D18B)
        || (cChar >= 0x1D1AA && cChar <= 0x1D1AD)
        || (cChar >= 0x1D242 && cChar <= 0x1D244)
        || (cChar >= 0xE0100 && cChar <= 0xE01EF))
    {
        eRetVal = ECharacterTypes::NSM;
    }
    else // check for Other Neutral (note: these values were programmatically extracted from derivedbidi.txt)
    if (   (cChar >= 0x0021 && cChar <= 0x0022)
        || (cChar >= 0x0026 && cChar <= 0x0027)
        || (cChar == 0x0028)
        || (cChar == 0x0029)
        || (cChar == 0x002A)
        || (cChar == 0x003B)
        || (cChar >= 0x003C && cChar <= 0x003E)
        || (cChar >= 0x003F && cChar <= 0x0040)
        || (cChar == 0x005B)
        || (cChar == 0x005C)
        || (cChar == 0x005D)
        || (cChar == 0x005E)
        || (cChar == 0x005F)
        || (cChar == 0x0060)
        || (cChar == 0x007B)
        || (cChar == 0x007C)
        || (cChar == 0x007D)
        || (cChar == 0x007E)
        || (cChar == 0x00A1)
        || (cChar >= 0x00A6 && cChar <= 0x00A7)
        || (cChar == 0x00A8)
        || (cChar == 0x00A9)
        || (cChar == 0x00AB)
        || (cChar == 0x00AC)
        || (cChar == 0x00AE)
        || (cChar == 0x00AF)
        || (cChar == 0x00B4)
        || (cChar == 0x00B6)
        || (cChar == 0x00B7)
        || (cChar == 0x00B8)
        || (cChar == 0x00BB)
        || (cChar >= 0x00BC && cChar <= 0x00BE)
        || (cChar == 0x00BF)
        || (cChar == 0x00D7)
        || (cChar == 0x00F7)
        || (cChar >= 0x02B9 && cChar <= 0x02BA)
        || (cChar >= 0x02C2 && cChar <= 0x02C5)
        || (cChar >= 0x02C6 && cChar <= 0x02CF)
        || (cChar >= 0x02D2 && cChar <= 0x02DF)
        || (cChar >= 0x02E5 && cChar <= 0x02EB)
        || (cChar == 0x02EC)
        || (cChar == 0x02ED)
        || (cChar >= 0x02EF && cChar <= 0x02FF)
        || (cChar == 0x0374)
        || (cChar == 0x0375)
        || (cChar == 0x037E)
        || (cChar >= 0x0384 && cChar <= 0x0385)
        || (cChar == 0x0387)
        || (cChar == 0x03F6)
        || (cChar == 0x058A)
        || (cChar >= 0x0606 && cChar <= 0x0607)
        || (cChar >= 0x060E && cChar <= 0x060F)
        || (cChar == 0x06E9)
        || (cChar == 0x07F6)
        || (cChar >= 0x07F7 && cChar <= 0x07F9)
        || (cChar >= 0x0BF3 && cChar <= 0x0BF8)
        || (cChar == 0x0BFA)
        || (cChar >= 0x0C78 && cChar <= 0x0C7E)
        || (cChar >= 0x0CF1 && cChar <= 0x0CF2)
        || (cChar == 0x0F3A)
        || (cChar == 0x0F3B)
        || (cChar == 0x0F3C)
        || (cChar == 0x0F3D)
        || (cChar >= 0x1390 && cChar <= 0x1399)
        || (cChar == 0x169B)
        || (cChar == 0x169C)
        || (cChar >= 0x17F0 && cChar <= 0x17F9)
        || (cChar >= 0x1800 && cChar <= 0x1805)
        || (cChar == 0x1806)
        || (cChar >= 0x1807 && cChar <= 0x180A)
        || (cChar == 0x1940)
        || (cChar >= 0x1944 && cChar <= 0x1945)
        || (cChar >= 0x19DE && cChar <= 0x19DF)
        || (cChar >= 0x19E0 && cChar <= 0x19FF)
        || (cChar == 0x1FBD)
        || (cChar >= 0x1FBF && cChar <= 0x1FC1)
        || (cChar >= 0x1FCD && cChar <= 0x1FCF)
        || (cChar >= 0x1FDD && cChar <= 0x1FDF)
        || (cChar >= 0x1FED && cChar <= 0x1FEF)
        || (cChar >= 0x1FFD && cChar <= 0x1FFE)
        || (cChar >= 0x2010 && cChar <= 0x2015)
        || (cChar >= 0x2016 && cChar <= 0x2017)
        || (cChar == 0x2018)
        || (cChar == 0x2019)
        || (cChar == 0x201A)
        || (cChar >= 0x201B && cChar <= 0x201C)
        || (cChar == 0x201D)
        || (cChar == 0x201E)
        || (cChar == 0x201F)
        || (cChar >= 0x2020 && cChar <= 0x2027)
        || (cChar >= 0x2035 && cChar <= 0x2038)
        || (cChar == 0x2039)
        || (cChar == 0x203A)
        || (cChar >= 0x203B && cChar <= 0x203E)
        || (cChar >= 0x203F && cChar <= 0x2040)
        || (cChar >= 0x2041 && cChar <= 0x2043)
        || (cChar == 0x2045)
        || (cChar == 0x2046)
        || (cChar >= 0x2047 && cChar <= 0x2051)
        || (cChar == 0x2052)
        || (cChar == 0x2053)
        || (cChar == 0x2054)
        || (cChar >= 0x2055 && cChar <= 0x205E)
        || (cChar == 0x207C)
        || (cChar == 0x207D)
        || (cChar == 0x207E)
        || (cChar == 0x208C)
        || (cChar == 0x208D)
        || (cChar == 0x208E)
        || (cChar >= 0x2100 && cChar <= 0x2101)
        || (cChar >= 0x2103 && cChar <= 0x2106)
        || (cChar >= 0x2108 && cChar <= 0x2109)
        || (cChar == 0x2114)
        || (cChar >= 0x2116 && cChar <= 0x2118)
        || (cChar >= 0x211E && cChar <= 0x2123)
        || (cChar == 0x2125)
        || (cChar == 0x2127)
        || (cChar == 0x2129)
        || (cChar >= 0x213A && cChar <= 0x213B)
        || (cChar >= 0x2140 && cChar <= 0x2144)
        || (cChar == 0x214A)
        || (cChar == 0x214B)
        || (cChar >= 0x214C && cChar <= 0x214D)
        || (cChar >= 0x2153 && cChar <= 0x215F)
        || (cChar >= 0x2190 && cChar <= 0x2194)
        || (cChar >= 0x2195 && cChar <= 0x2199)
        || (cChar >= 0x219A && cChar <= 0x219B)
        || (cChar >= 0x219C && cChar <= 0x219F)
        || (cChar == 0x21A0)
        || (cChar >= 0x21A1 && cChar <= 0x21A2)
        || (cChar == 0x21A3)
        || (cChar >= 0x21A4 && cChar <= 0x21A5)
        || (cChar == 0x21A6)
        || (cChar >= 0x21A7 && cChar <= 0x21AD)
        || (cChar == 0x21AE)
        || (cChar >= 0x21AF && cChar <= 0x21CD)
        || (cChar >= 0x21CE && cChar <= 0x21CF)
        || (cChar >= 0x21D0 && cChar <= 0x21D1)
        || (cChar == 0x21D2)
        || (cChar == 0x21D3)
        || (cChar == 0x21D4)
        || (cChar >= 0x21D5 && cChar <= 0x21F3)
        || (cChar >= 0x21F4 && cChar <= 0x2211)
        || (cChar >= 0x2214 && cChar <= 0x22FF)
        || (cChar >= 0x2300 && cChar <= 0x2307)
        || (cChar >= 0x2308 && cChar <= 0x230B)
        || (cChar >= 0x230C && cChar <= 0x231F)
        || (cChar >= 0x2320 && cChar <= 0x2321)
        || (cChar >= 0x2322 && cChar <= 0x2328)
        || (cChar == 0x2329)
        || (cChar == 0x232A)
        || (cChar >= 0x232B && cChar <= 0x2335)
        || (cChar == 0x237B)
        || (cChar == 0x237C)
        || (cChar >= 0x237D && cChar <= 0x2394)
        || (cChar >= 0x2396 && cChar <= 0x239A)
        || (cChar >= 0x239B && cChar <= 0x23B3)
        || (cChar >= 0x23B4 && cChar <= 0x23DB)
        || (cChar >= 0x23DC && cChar <= 0x23E1)
        || (cChar >= 0x23E2 && cChar <= 0x23E7)
        || (cChar >= 0x2400 && cChar <= 0x2426)
        || (cChar >= 0x2440 && cChar <= 0x244A)
        || (cChar >= 0x2460 && cChar <= 0x2487)
        || (cChar >= 0x24EA && cChar <= 0x24FF)
        || (cChar >= 0x2500 && cChar <= 0x25B6)
        || (cChar == 0x25B7)
        || (cChar >= 0x25B8 && cChar <= 0x25C0)
        || (cChar == 0x25C1)
        || (cChar >= 0x25C2 && cChar <= 0x25F7)
        || (cChar >= 0x25F8 && cChar <= 0x25FF)
        || (cChar >= 0x2600 && cChar <= 0x266E)
        || (cChar == 0x266F)
        || (cChar >= 0x2670 && cChar <= 0x269D)
        || (cChar >= 0x26A0 && cChar <= 0x26AB)
        || (cChar >= 0x26AD && cChar <= 0x26BC)
        || (cChar >= 0x26C0 && cChar <= 0x26C3)
        || (cChar >= 0x2701 && cChar <= 0x2704)
        || (cChar >= 0x2706 && cChar <= 0x2709)
        || (cChar >= 0x270C && cChar <= 0x2727)
        || (cChar >= 0x2729 && cChar <= 0x274B)
        || (cChar == 0x274D)
        || (cChar >= 0x274F && cChar <= 0x2752)
        || (cChar == 0x2756)
        || (cChar >= 0x2758 && cChar <= 0x275E)
        || (cChar >= 0x2761 && cChar <= 0x2767)
        || (cChar == 0x2768)
        || (cChar == 0x2769)
        || (cChar == 0x276A)
        || (cChar == 0x276B)
        || (cChar == 0x276C)
        || (cChar == 0x276D)
        || (cChar == 0x276E)
        || (cChar == 0x276F)
        || (cChar == 0x2770)
        || (cChar == 0x2771)
        || (cChar == 0x2772)
        || (cChar == 0x2773)
        || (cChar == 0x2774)
        || (cChar == 0x2775)
        || (cChar >= 0x2776 && cChar <= 0x2793)
        || (cChar == 0x2794)
        || (cChar >= 0x2798 && cChar <= 0x27AF)
        || (cChar >= 0x27B1 && cChar <= 0x27BE)
        || (cChar >= 0x27C0 && cChar <= 0x27C4)
        || (cChar == 0x27C5)
        || (cChar == 0x27C6)
        || (cChar >= 0x27C7 && cChar <= 0x27CA)
        || (cChar == 0x27CC)
        || (cChar >= 0x27D0 && cChar <= 0x27E5)
        || (cChar == 0x27E6)
        || (cChar == 0x27E7)
        || (cChar == 0x27E8)
        || (cChar == 0x27E9)
        || (cChar == 0x27EA)
        || (cChar == 0x27EB)
        || (cChar == 0x27EC)
        || (cChar == 0x27ED)
        || (cChar == 0x27EE)
        || (cChar == 0x27EF)
        || (cChar >= 0x27F0 && cChar <= 0x27FF)
        || (cChar >= 0x2900 && cChar <= 0x2982)
        || (cChar == 0x2983)
        || (cChar == 0x2984)
        || (cChar == 0x2985)
        || (cChar == 0x2986)
        || (cChar == 0x2987)
        || (cChar == 0x2988)
        || (cChar == 0x2989)
        || (cChar == 0x298A)
        || (cChar == 0x298B)
        || (cChar == 0x298C)
        || (cChar == 0x298D)
        || (cChar == 0x298E)
        || (cChar == 0x298F)
        || (cChar == 0x2990)
        || (cChar == 0x2991)
        || (cChar == 0x2992)
        || (cChar == 0x2993)
        || (cChar == 0x2994)
        || (cChar == 0x2995)
        || (cChar == 0x2996)
        || (cChar == 0x2997)
        || (cChar == 0x2998)
        || (cChar >= 0x2999 && cChar <= 0x29D7)
        || (cChar == 0x29D8)
        || (cChar == 0x29D9)
        || (cChar == 0x29DA)
        || (cChar == 0x29DB)
        || (cChar >= 0x29DC && cChar <= 0x29FB)
        || (cChar == 0x29FC)
        || (cChar == 0x29FD)
        || (cChar >= 0x29FE && cChar <= 0x2AFF)
        || (cChar >= 0x2B00 && cChar <= 0x2B2F)
        || (cChar >= 0x2B30 && cChar <= 0x2B44)
        || (cChar >= 0x2B45 && cChar <= 0x2B46)
        || (cChar >= 0x2B47 && cChar <= 0x2B4C)
        || (cChar >= 0x2B50 && cChar <= 0x2B54)
        || (cChar >= 0x2CE5 && cChar <= 0x2CEA)
        || (cChar >= 0x2CF9 && cChar <= 0x2CFC)
        || (cChar == 0x2CFD)
        || (cChar >= 0x2CFE && cChar <= 0x2CFF)
        || (cChar >= 0x2E00 && cChar <= 0x2E01)
        || (cChar == 0x2E02)
        || (cChar == 0x2E03)
        || (cChar == 0x2E04)
        || (cChar == 0x2E05)
        || (cChar >= 0x2E06 && cChar <= 0x2E08)
        || (cChar == 0x2E09)
        || (cChar == 0x2E0A)
        || (cChar == 0x2E0B)
        || (cChar == 0x2E0C)
        || (cChar == 0x2E0D)
        || (cChar >= 0x2E0E && cChar <= 0x2E16)
        || (cChar == 0x2E17)
        || (cChar >= 0x2E18 && cChar <= 0x2E19)
        || (cChar == 0x2E1A)
        || (cChar == 0x2E1B)
        || (cChar == 0x2E1C)
        || (cChar == 0x2E1D)
        || (cChar >= 0x2E1E && cChar <= 0x2E1F)
        || (cChar == 0x2E20)
        || (cChar == 0x2E21)
        || (cChar == 0x2E22)
        || (cChar == 0x2E23)
        || (cChar == 0x2E24)
        || (cChar == 0x2E25)
        || (cChar == 0x2E26)
        || (cChar == 0x2E27)
        || (cChar == 0x2E28)
        || (cChar == 0x2E29)
        || (cChar >= 0x2E2A && cChar <= 0x2E2E)
        || (cChar == 0x2E2F)
        || (cChar == 0x2E30)
        || (cChar >= 0x2E80 && cChar <= 0x2E99)
        || (cChar >= 0x2E9B && cChar <= 0x2EF3)
        || (cChar >= 0x2F00 && cChar <= 0x2FD5)
        || (cChar >= 0x2FF0 && cChar <= 0x2FFB)
        || (cChar >= 0x3001 && cChar <= 0x3003)
        || (cChar == 0x3004)
        || (cChar == 0x3008)
        || (cChar == 0x3009)
        || (cChar == 0x300A)
        || (cChar == 0x300B)
        || (cChar == 0x300C)
        || (cChar == 0x300D)
        || (cChar == 0x300E)
        || (cChar == 0x300F)
        || (cChar == 0x3010)
        || (cChar == 0x3011)
        || (cChar >= 0x3012 && cChar <= 0x3013)
        || (cChar == 0x3014)
        || (cChar == 0x3015)
        || (cChar == 0x3016)
        || (cChar == 0x3017)
        || (cChar == 0x3018)
        || (cChar == 0x3019)
        || (cChar == 0x301A)
        || (cChar == 0x301B)
        || (cChar == 0x301C)
        || (cChar == 0x301D)
        || (cChar >= 0x301E && cChar <= 0x301F)
        || (cChar == 0x3020)
        || (cChar == 0x3030)
        || (cChar >= 0x3036 && cChar <= 0x3037)
        || (cChar == 0x303D)
        || (cChar >= 0x303E && cChar <= 0x303F)
        || (cChar >= 0x309B && cChar <= 0x309C)
        || (cChar == 0x30A0)
        || (cChar == 0x30FB)
        || (cChar >= 0x31C0 && cChar <= 0x31E3)
        || (cChar >= 0x321D && cChar <= 0x321E)
        || (cChar == 0x3250)
        || (cChar >= 0x3251 && cChar <= 0x325F)
        || (cChar >= 0x327C && cChar <= 0x327E)
        || (cChar >= 0x32B1 && cChar <= 0x32BF)
        || (cChar >= 0x32CC && cChar <= 0x32CF)
        || (cChar >= 0x3377 && cChar <= 0x337A)
        || (cChar >= 0x33DE && cChar <= 0x33DF)
        || (cChar == 0x33FF)
        || (cChar >= 0x4DC0 && cChar <= 0x4DFF)
        || (cChar >= 0xA490 && cChar <= 0xA4C6)
        || (cChar >= 0xA60D && cChar <= 0xA60F)
        || (cChar == 0xA673)
        || (cChar == 0xA67E)
        || (cChar == 0xA67F)
        || (cChar >= 0xA700 && cChar <= 0xA716)
        || (cChar >= 0xA717 && cChar <= 0xA71F)
        || (cChar >= 0xA720 && cChar <= 0xA721)
        || (cChar == 0xA788)
        || (cChar >= 0xA828 && cChar <= 0xA82B)
        || (cChar >= 0xA874 && cChar <= 0xA877)
        || (cChar == 0xFD3E)
        || (cChar == 0xFD3F)
        || (cChar == 0xFDFD)
        || (cChar >= 0xFE10 && cChar <= 0xFE16)
        || (cChar == 0xFE17)
        || (cChar == 0xFE18)
        || (cChar == 0xFE19)
        || (cChar == 0xFE30)
        || (cChar >= 0xFE31 && cChar <= 0xFE32)
        || (cChar >= 0xFE33 && cChar <= 0xFE34)
        || (cChar == 0xFE35)
        || (cChar == 0xFE36)
        || (cChar == 0xFE37)
        || (cChar == 0xFE38)
        || (cChar == 0xFE39)
        || (cChar == 0xFE3A)
        || (cChar == 0xFE3B)
        || (cChar == 0xFE3C)
        || (cChar == 0xFE3D)
        || (cChar == 0xFE3E)
        || (cChar == 0xFE3F)
        || (cChar == 0xFE40)
        || (cChar == 0xFE41)
        || (cChar == 0xFE42)
        || (cChar == 0xFE43)
        || (cChar == 0xFE44)
        || (cChar >= 0xFE45 && cChar <= 0xFE46)
        || (cChar == 0xFE47)
        || (cChar == 0xFE48)
        || (cChar >= 0xFE49 && cChar <= 0xFE4C)
        || (cChar >= 0xFE4D && cChar <= 0xFE4F)
        || (cChar == 0xFE51)
        || (cChar == 0xFE54)
        || (cChar >= 0xFE56 && cChar <= 0xFE57)
        || (cChar == 0xFE58)
        || (cChar == 0xFE59)
        || (cChar == 0xFE5A)
        || (cChar == 0xFE5B)
        || (cChar == 0xFE5C)
        || (cChar == 0xFE5D)
        || (cChar == 0xFE5E)
        || (cChar >= 0xFE60 && cChar <= 0xFE61)
        || (cChar >= 0xFE64 && cChar <= 0xFE66)
        || (cChar == 0xFE68)
        || (cChar == 0xFE6B)
        || (cChar >= 0xFF01 && cChar <= 0xFF02)
        || (cChar >= 0xFF06 && cChar <= 0xFF07)
        || (cChar == 0xFF08)
        || (cChar == 0xFF09)
        || (cChar == 0xFF0A)
        || (cChar == 0xFF1B)
        || (cChar >= 0xFF1C && cChar <= 0xFF1E)
        || (cChar >= 0xFF1F && cChar <= 0xFF20)
        || (cChar == 0xFF3B)
        || (cChar == 0xFF3C)
        || (cChar == 0xFF3D)
        || (cChar == 0xFF3E)
        || (cChar == 0xFF3F)
        || (cChar == 0xFF40)
        || (cChar == 0xFF5B)
        || (cChar == 0xFF5C)
        || (cChar == 0xFF5D)
        || (cChar == 0xFF5E)
        || (cChar == 0xFF5F)
        || (cChar == 0xFF60)
        || (cChar == 0xFF61)
        || (cChar == 0xFF62)
        || (cChar == 0xFF63)
        || (cChar >= 0xFF64 && cChar <= 0xFF65)
        || (cChar == 0xFFE2)
        || (cChar == 0xFFE3)
        || (cChar == 0xFFE4)
        || (cChar == 0xFFE8)
        || (cChar >= 0xFFE9 && cChar <= 0xFFEC)
        || (cChar >= 0xFFED && cChar <= 0xFFEE)
        || (cChar >= 0xFFF9 && cChar <= 0xFFFB)
        || (cChar >= 0xFFFC && cChar <= 0xFFFD)
        || (cChar == 0x10101)
        || (cChar >= 0x10140 && cChar <= 0x10174)
        || (cChar >= 0x10175 && cChar <= 0x10178)
        || (cChar >= 0x10179 && cChar <= 0x10189)
        || (cChar == 0x1018A)
        || (cChar >= 0x10190 && cChar <= 0x1019B)
        || (cChar == 0x1091F)
        || (cChar >= 0x1D200 && cChar <= 0x1D241)
        || (cChar == 0x1D245)
        || (cChar >= 0x1D300 && cChar <= 0x1D356)
        || (cChar >= 0x1F000 && cChar <= 0x1F02B)
        || (cChar >= 0x1F030 && cChar <= 0x1F093))
    {
        eRetVal = ECharacterTypes::ON;
    }
    else
    // check if in default AL range
    if (   (cChar >= 0x600 && cChar <= 0x7BF)
        || (cChar >= 0xFB50 && cChar <= 0xFDFF)
        || (cChar >= 0xFE70 && cChar <= 0xFEFF))
    {
        eRetVal = ECharacterTypes::AL;
    }
    else
    // check if in default R range
    if (   (cChar >= 0x590 && cChar <= 0x5FF)
        || (cChar >= 0x7C0 && cChar <= 0x8FF)
        || (cChar >= 0xFB1D && cChar <= 0xFB4F)
        || (cChar >= 0x10800 && cChar <= 0x10FFF)
        || (cChar == 0x200F))
    {
        eRetVal = ECharacterTypes::R;
    }

    return eRetVal;
}


// handles the initial explicit ordering by special characters; implements
// X1-X9 of the bidi-unicode spec
int BIDIConverter::ResolveExplicit(int nBaseLevel)
{
    m_Levels.clear();
    m_Levels.resize(m_ClassificationArray.size());

    // check input values
    _ASSERT(0 <= nBaseLevel && BIDIConverter::_MaxNestedLevel >= nBaseLevel);
    std::stack<std::pair<int, ECharacterType> > stkLevel;

    // pushes beyond the maximum are ignored; this keeps track of them
    int nIgnorePop = 0;

    std::pair<int, ECharacterType> prCurrentLevel = std::pair<int, ECharacterType>(nBaseLevel, ECharacterTypes::N);
    stkLevel.push(prCurrentLevel);

    // iterate input string and set levels based on explicit direction markers
    unsigned int i;
    for (i=0; i<m_ClassificationArray.size(); ++i)
    {
        switch (m_ClassificationArray[i])
        {
            // handle cases of LTR embedding and override
            case ECharacterTypes::LRO:
            case ECharacterTypes::LRE:
            {
                if (BIDIConverter::_MaxNestedLevel > prCurrentLevel.first)
                {
                    if (0 != i) // don't increment if the first character is an explicit override
                    {
                        (/*odd*/prCurrentLevel.first & 1)? prCurrentLevel.first += 1 : prCurrentLevel.first += 2;
                    }
                    if (ECharacterTypes::LRO == m_ClassificationArray[i])
                    {
                        prCurrentLevel.second = ECharacterTypes::L;
                    }
                    else
                    {
                        prCurrentLevel.second = ECharacterTypes::N;
                    }
                    stkLevel.push(prCurrentLevel);
                }
                else
                {
                    nIgnorePop++;
                }

                // set this as a neutral boundary
                m_ClassificationArray[i] = ECharacterTypes::BN;
                m_Levels[i] = prCurrentLevel.first;

                break;
            }

            // handle cases of RTL embedding and override
            case ECharacterTypes::RLO:
            case ECharacterTypes::RLE:
            {
                if (BIDIConverter::_MaxNestedLevel > prCurrentLevel.first)
                {
                    if (0 != i) // don't increment if the first character is an explicit override
                    {
                        (/*odd*/prCurrentLevel.first & 1)? prCurrentLevel.first += 2: prCurrentLevel.first += 1;
                    }
                    if (ECharacterTypes::RLO == m_ClassificationArray[i])
                    {
                        prCurrentLevel.second = ECharacterTypes::R;
                    }
                    else
                    {
                        prCurrentLevel.second = ECharacterTypes::N;
                    }
                    stkLevel.push(prCurrentLevel);
                }
                else
                {
                    nIgnorePop++;
                }

                // set this as a neutral boundary
                m_ClassificationArray[i] = ECharacterTypes::BN;

                break;
            }

            // end of previous embedding/override
            case ECharacterTypes::PDF:
            {
                if (0 < nIgnorePop)
                {
                    nIgnorePop--;
                }
                else
                {
                    // check if there is a pop here without a push
                    if (1 < stkLevel.size())
                    {
                        stkLevel.pop();
                    }
                    prCurrentLevel = stkLevel.top();
                }

                // set this as a neutral boundary
                m_ClassificationArray[i] = ECharacterTypes::BN;

                break;
            }

            default:
            {
                // These are all the other characters.  For now if there was an
                // override then set the character direction for that override,
                // otherwise leave for further processing.
                if (ECharacterTypes::N != prCurrentLevel.second)
                {
                    m_ClassificationArray[i] = prCurrentLevel.second;
                }

                break;
            }
        }

        m_Levels[i] = prCurrentLevel.first;
    }

    return i;
}


// resolve Weak characters; implements W1-W7 of the bidi-unicode spec
int BIDIConverter::ResolveWeak(int nBaseLevel)
{
    // a pairing to hold the current level and the index into the array
    std::pair<int, int> indexLevel = std::pair<int, int>(nBaseLevel, 0);

    // stored in the loop so we don't have to keep testing levels - we
    // can just do a for loop
    int nHardIndexCount = 0;

    // The rules specify doing a pass for each rule.  Since the values
    // are changed each pass they cannot be done concurrently.  Iterate
    // the arrays and apply weak resolution rules.
    unsigned int i = 0;
    while (i < m_ClassificationArray.size())
    {
        // check if it's the start of a new run
        if (m_Levels[i] != indexLevel.first)
        {
            indexLevel.first = m_Levels[i];
            indexLevel.second = 0;
        }

        // implement the rules for this run

        // rule W1: per run, NSM gets the type of preceding characters or SoR
        //          (L or R depending on even or odd level
        while (i < m_ClassificationArray.size() && indexLevel.first == m_Levels[i])
        {
            if (ECharacterTypes::NSM == m_ClassificationArray[i])
            {
                m_ClassificationArray[i] = ResolveWeakNSM(i, indexLevel.first, indexLevel.second);
            }
            i++;
            indexLevel.second++;
        }

        // reset to beginning of level for next rule
        nHardIndexCount = indexLevel.second;
        i -= indexLevel.second;
        indexLevel.second = 0;

        // rule W2: replace EN with either EN or AN depending on preceding characters
        for (; indexLevel.second < nHardIndexCount; indexLevel.second++, i++)
        {
            if (ECharacterTypes::EN == m_ClassificationArray[i])
            {
                m_ClassificationArray[i] = ResolveWeakEN(i, indexLevel.second);
            }
        }
        i -= indexLevel.second;
        indexLevel.second = 0;

        // rule W3: replace all AL to R
        for (; indexLevel.second < nHardIndexCount; indexLevel.second++, i++)
        {
            if (ECharacterTypes::AL == m_ClassificationArray[i])
            {
                m_ClassificationArray[i] = ECharacterTypes::R;
            }
        }
        i -= indexLevel.second;
        indexLevel.second = 0;

        // rule W4: resolve European and common separators between two European numbers
        for (; indexLevel.second < nHardIndexCount; indexLevel.second++, i++)
        {
            if (ECharacterTypes::ES == m_ClassificationArray[i] ||
                ECharacterTypes::CS == m_ClassificationArray[i])
            {
                m_ClassificationArray[i] = ResolveWeakSeperator(i, indexLevel.first, indexLevel.second, m_ClassificationArray[i]);
            }
        }
        i -= indexLevel.second;
        indexLevel.second = 0;

        // rule W5: a sequence of European terminators adjacent to European numbers
        //          changes to all European numbers
        for (; indexLevel.second < nHardIndexCount; indexLevel.second++, i++)
        {
            if (ECharacterTypes::ET == m_ClassificationArray[i])
            {
                m_ClassificationArray[i] = ResolveWeakET(i, indexLevel.first);
            }
        }
        i -= indexLevel.second;
        indexLevel.second = 0;

        // rule W6: remaining separators and terminators are set to Other Neutral
        for (; indexLevel.second < nHardIndexCount; indexLevel.second++, i++)
        {
            ECharacterType eCharType = m_ClassificationArray[i];
            if (ECharacterTypes::ET == eCharType ||
                ECharacterTypes::ES == eCharType ||
                ECharacterTypes::CS == eCharType)
            {
                m_ClassificationArray[i] = ECharacterTypes::ON;
            }
        }
        i -= indexLevel.second;
        indexLevel.second = 0;

        // rule W7: Search backwards from each instance of a European number
        //          until the first strong type (R, L, or SoR) is found.  If
        //          an L is found then change the type of the European number
        //          to L.
        for (; indexLevel.second < nHardIndexCount; indexLevel.second++, i++)
        {
            if (ECharacterTypes::EN == m_ClassificationArray[i])
            {
                m_ClassificationArray[i] = ResolveWeakENFinal(i, indexLevel.first, indexLevel.second);
            }
        }
    }

    return i;
}


// implements rule W1: per run, NSM gets the type of preceding characters
// or SoR (L or R depending on even or odd level)
ECharacterType BIDIConverter::ResolveWeakNSM(int nIndex, int nLevel, int nLevelIndex)
{
    _ASSERT(ECharacterTypes::NSM == m_ClassificationArray[nIndex]);

    if (0 == nLevelIndex)
        return (nLevel & 1)? ECharacterTypes::R : ECharacterTypes::L;

    return m_ClassificationArray[nIndex-1];

}


// implements rule W2: replace EN with either EN or AN depending on preceding characters
ECharacterType BIDIConverter::ResolveWeakEN(int nIndex, int nLevelIndex)
{
    _ASSERT(ECharacterTypes::EN == m_ClassificationArray[nIndex]);

    int nWorkingLevelIndex = nLevelIndex;
    int nWorkingIndex = nIndex;
    while (nWorkingLevelIndex >= 0)
    {
        if (0 == nWorkingLevelIndex)
            return ECharacterTypes::EN;

        switch (m_ClassificationArray[nWorkingIndex])
        {
        case ECharacterTypes::R:
        case ECharacterTypes::L:
            return ECharacterTypes::EN;
            break;
        case ECharacterTypes::AL:
            return ECharacterTypes::AN;
            break;
        }
        nWorkingLevelIndex--;
        nWorkingIndex--;
    }

    return ECharacterTypes::EN;
}


// implements rule W4: resolve European and common seperators between two
// European numbers
ECharacterType BIDIConverter::ResolveWeakSeperator(int nIndex, int nLevel, int nLevelIndex, ECharacterType inputType)
{
    ECharacterType eRetType = inputType;

    // if at the beginning or end of a run, drop out
    if (nLevelIndex == 0 ||
        nIndex + 1 >= (int)m_Levels.size() ||
        nLevel != m_Levels[nIndex+1])
    {
        return eRetType;
    }

    int nType = (m_ClassificationArray[nIndex-1] & m_ClassificationArray[nIndex + 1]);

    if (ECharacterTypes::AN == nType)
    {
        eRetType = ECharacterTypes::AN;
    }
    else if (ECharacterTypes::EN == nType)
    {
        eRetType = ECharacterTypes::EN;
    }

    return eRetType;
}


// implements rule W5: a sequence of European terminators adjacent to European
// numbers changes to all European numbers
ECharacterType BIDIConverter::ResolveWeakET(int nIndex, int nLevel)
{
    _ASSERT(ECharacterTypes::ET == m_ClassificationArray[nIndex]);
    int nWalker = nIndex + 1;

    // search forward for first occurance of character after ET
    while ((nWalker < (int)m_ClassificationArray.size()) && nLevel == m_Levels[nWalker])
    {
        if (ECharacterTypes::EN == m_ClassificationArray[nWalker])
            return ECharacterTypes::EN;

        // if something other than a terminator then it doesn't count
        if (ECharacterTypes::ET != m_ClassificationArray[nWalker])
            break;

        nWalker++;
    }

    // search backwards
    nWalker = nIndex - 1;
    while (nWalker >= 0 && nLevel != m_Levels[nWalker])
    {
        if (ECharacterTypes::EN == m_ClassificationArray[nWalker])
            return ECharacterTypes::EN;

        // if something other than a terminator then it doesn't count
        if (ECharacterTypes::ET != m_ClassificationArray[nWalker])
            break;

        nWalker--;
    }

    return ECharacterTypes::ET;
}


// implements rule W7: replace EN with either L if found
ECharacterType BIDIConverter::ResolveWeakENFinal(int nIndex, int nLevel, int nLevelIndex)
{
    _ASSERT(ECharacterTypes::EN == m_ClassificationArray[nIndex]);

    int nWorkingLevelIndex = nLevelIndex;
    int nWorkingIndex = nIndex;
    while (nWorkingLevelIndex >= 0)
    {
        // if at SOR then use embedded level
        if (0 == nWorkingLevelIndex)
            return (nLevel & 1)? ECharacterTypes::EN : ECharacterTypes::L;

        // look for strong direction in previous characters
        switch (m_ClassificationArray[nWorkingIndex])
        {
        case ECharacterTypes::R:
            return ECharacterTypes::EN;
            break;
        case ECharacterTypes::L:
            return ECharacterTypes::L;
            break;
        }
        nWorkingLevelIndex--;
        nWorkingIndex--;
    }

    return ECharacterTypes::EN;
}


// resolve neutral charcters characters; implements N1-N2 of the
// bidi-unicode spec
int BIDIConverter::ResolveNeutral(int nBaseLevel)
{
    // a pairing to hold the current level and the index into the array
    std::pair<int, int> indexLevel = std::pair<int, int>(nBaseLevel, 0);

    // The rules specify doing a pass for each rule.  Since the values
    // are changed each pass they cannot be done concurrently.  Iterate
    // the arrays and apply weak resolution rules.
    unsigned int i = 0;
    while (i < m_ClassificationArray.size())
    {
        // check if it's the start of a new run
        if (m_Levels[i] != indexLevel.first)
        {
            indexLevel.first = m_Levels[i];
            indexLevel.second = 0;
        }

        // implement the rules for this run

        // rule N1: a sequence of neutrals takes the direction of the surrounding
        //          strong text
        while (i < m_ClassificationArray.size() && indexLevel.first == m_Levels[i])
        {
            if (ECharacterTypes::ON == m_ClassificationArray[i] ||
                ECharacterTypes::WS == m_ClassificationArray[i])
            {
                m_ClassificationArray[i] = ResolveNeutralChar(i, indexLevel.first);
            }
            i++;
            indexLevel.second++;
        }
    }

    return i;
}


// resolve neutral chars in accordance with N1 and N2
ECharacterType BIDIConverter::ResolveNeutralChar(int nIndex, int nLevel)
{
    _ASSERT(ECharacterTypes::N == m_ClassificationArray[nIndex] ||
            ECharacterTypes::WS == m_ClassificationArray[nIndex]);

    ECharacterType eRetVal = (nLevel & 1)? ECharacterTypes::R : ECharacterTypes::L;
    ECharacterType eRightSide = eRetVal;
    ECharacterType eLeftSide = eRetVal;
    int nWalker = nIndex + 1;
    int nArraySize = static_cast<int>(m_ClassificationArray.size());

    // search forward for first occurance of character after ET
    while (nWalker < nArraySize && nLevel == m_Levels[nWalker])
    {
        if (ECharacterTypes::N  != m_ClassificationArray[nWalker] &&
            ECharacterTypes::WS != m_ClassificationArray[nWalker])
        {
            eRightSide = m_ClassificationArray[nWalker];
            break;
        }
        nWalker++;
    }

    // search backwards
    nWalker = nIndex -1;
    while (nWalker > 0 && nLevel == m_Levels[nWalker])
    {
        if (ECharacterTypes::N  != m_ClassificationArray[nWalker] &&
            ECharacterTypes::WS != m_ClassificationArray[nWalker])
        {
            eLeftSide = m_ClassificationArray[nWalker];
            break;
        }
        nWalker--;
    }

    // remaining numbers are considered Right
    if (ECharacterTypes::EN == eRightSide || ECharacterTypes::AN == eRightSide)
        eRightSide = ECharacterTypes::R;
    if (ECharacterTypes::EN == eLeftSide || ECharacterTypes::AN == eLeftSide)
        eLeftSide = ECharacterTypes::R;

    // if surrounding text has the same direction
    if (eRightSide == eLeftSide)
        eRetVal = eRightSide;

    return eRetVal;
}


// resolve implicit characters; implements l1-l2
int BIDIConverter::ResolveImplied()
{
    // iterate through the resolved text (all neutrals should be gone except
    // boundary neutrals)
    for (size_t i=0; i<m_ClassificationArray.size(); ++i)
    {
        ECharacterType eChar = m_ClassificationArray[i];
        if (m_Levels[i] & 1)
        {
            // if LTR characters are on an odd level, move them up
            if (ECharacterTypes::EN == eChar ||
                ECharacterTypes::AN == eChar ||
                ECharacterTypes::L  == eChar)
            {
                m_Levels[i] = m_Levels[i] + 1;
            }
        }
        else
        {
            // if LTR characters are on an even level, move them up
            if (ECharacterTypes::R  == eChar)
            {
                m_Levels[i] = m_Levels[i] + 1;
            }
            else // if they are numbers, move them up 2
            if (ECharacterTypes::EN  == eChar ||
                ECharacterTypes::AN  == eChar)
            {
                m_Levels[i] = m_Levels[i] + 2;
            }
        }
    }

    return 1;
}


// reorders the text; implements L1-L4 of the bidi-unicode spec
int BIDIConverter::Reorder()
{
    // rule L1: On each line, reset the embedding level of the following
    // characters to the paragraph embedding level:
    //   * segment separators,
    //   * paragraph separators,
    //   * any sequence of whitespace characters preceding a segment separator
    //     or paragraph separator
    //   * any sequence of white space characters at the end of the line

    // parse string and levels into a usable array
    std::vector<std::pair<int, DisplayStr> > stringLevelVector;

    // parse the string and level vectors into something meaningful
    int nLevels = ParseLevels(stringLevelVector);
    for (int i=nLevels; i>=0; --i)
    {
        // flatten highest level
        ReorderLevel(stringLevelVector, i);
    }

    // create the converted string by walking through the base levels
    m_ConvertedString = stringLevelVector[0].second;

    ShapeString();
    return (int)m_ConvertedString.size();
}


// reorders the parsed text and creates a single master string that
// represents the converted text; implements rule L2
int BIDIConverter::ReorderLevel(std::vector<std::pair<int, DisplayStr> >& stringLevelVector, unsigned int nLevel)
{
    // flatten level passed in
    for (size_t i=0; i<stringLevelVector.size(); ++i)
    {
        // iff this is the level to flatten
        if (stringLevelVector[i].first == (int)nLevel)
        {
            // now finish all equal level strings
            while ((i + 1) < stringLevelVector.size() && stringLevelVector[i+1].first == (int)nLevel)
            {
                stringLevelVector[i].second += stringLevelVector[i+1].second;
                stringLevelVector.erase(stringLevelVector.begin() + i + 1);
            }

            if (nLevel > 0)
            {
                // drop a level
                stringLevelVector[i].first = nLevel-1;

                // reverse the string
                reverse(stringLevelVector[i].second);
            }
        }
    }

    return nLevel;
}


// parses the string and level vector to a meaningful array of level/string pairs
int BIDIConverter::ParseLevels(std::vector<std::pair<int, DisplayStr> >& stringLevelVector)
{
    // parse string and levels into a usable array
    size_t nPos = 0;
    while (ECharacterTypes::BN == m_ClassificationArray[nPos] && nPos < m_ClassificationArray.size())
        nPos++;

    size_t nIndex = nPos;
    int nLevel = m_Levels[0];
    DisplayStr workingString;
    int nHighestLevel = nLevel;

    // go through vector of Levels, discern a level change and build a pairing
    // of levels and strings
    for (nPos=nIndex; nPos<m_Levels.size(); ++nPos)
    {
        // FUTURE TODO: remove the Substr and do a character by character copy (removing
        //              boundary neutrals)

        // if we're at the end of our text or at the end of our levels
        // then store the string and break out
        if (m_Levels.size() <= (nPos + 1))
        {
            workingString = m_OriginalString.substr(nIndex, (nPos - nIndex + 1));
            stringLevelVector.push_back(std::pair<int, DisplayStr>(nLevel, workingString));
            break;
        }
        else if (m_Levels[nPos+1] != nLevel)
        {
            workingString = m_OriginalString.substr(nIndex, (nPos - nIndex + 1));
            stringLevelVector.push_back(std::pair<int, DisplayStr>(nLevel, workingString));
            nIndex = nPos + 1;

            // skip boundary neutral characters
            if (ECharacterTypes::BN == m_ClassificationArray[nPos + 1])
            {
                nIndex++;
                nPos++;
            }

            nLevel = m_Levels[nPos+1];
            if (nLevel > nHighestLevel)
                nHighestLevel = nLevel;
        }
    }

    return nHighestLevel;
}


// quick reverse function to mirror strings
void BIDIConverter::reverse(DisplayStr& str)
{
    MirrorGlyphs& mirrorGlyphs = GetMirrorMapping();

    size_t nLength = str.size();
    wchar_t chTemp;
    for (size_t i=0; i< --nLength; ++i)
    {
        if (mirrorGlyphs.find(str[i]) != mirrorGlyphs.end())
        {
            chTemp = (wchar_t)mirrorGlyphs[str[i]];
        }
        else
        {
            chTemp = str[i];
        }

        if (mirrorGlyphs.find(str[nLength]) != mirrorGlyphs.end())
        {
            str[i] = (wchar_t)mirrorGlyphs[str[nLength]];
        }
        else
        {
            str[i] = str[nLength];
        }

        str[nLength] = chTemp;
    }

    // if odd length string, check the middle character for reversal
    if ((str.size() & 1) && (mirrorGlyphs.find(str[nLength]) != mirrorGlyphs.end()))
    {
        str[nLength] = (wchar_t)mirrorGlyphs[str[nLength]];
    }
}


// returns the static mapping of glyphs to possible shaping code points - built once
ShapeGlyphs& BIDIConverter::GetShapeMapping()
{
    static ShapeGlyphs s_ShapeGlyphMap;

    // initialize on first use
    if (0 == s_ShapeGlyphMap.size())
    {
        std::vector<unsigned int> glyphMappings;
        glyphMappings.resize(4);

        size_t nMax = sizeof(glyphShapeMapping) / (sizeof(unsigned int)*5);
        for (size_t i=0; i<nMax; ++i)
        {
            glyphMappings[0] = glyphShapeMapping[i][1];
            glyphMappings[1] = glyphShapeMapping[i][2];
            glyphMappings[2] = glyphShapeMapping[i][3];
            glyphMappings[3] = glyphShapeMapping[i][4];
            s_ShapeGlyphMap[glyphShapeMapping[i][0]] = glyphMappings;
        }
    }

    return s_ShapeGlyphMap;
}


// returns the static vector of strings (stored in pairs) used to enforce ligature - built once
std::vector<DisplayStr>& BIDIConverter::GetLigaturePairs()
{
    static std::vector<DisplayStr> s_LigaturePairs;

    // initialize on first use
    if (0 == s_LigaturePairs.size())
    {
        DisplayStr str0(L"xx"); // allocate 2 characters
        DisplayStr str1(L"x");  // allocate 1 character

        size_t nMax = sizeof(glyphLigatureMapping) / (sizeof(unsigned int)*3);
        for (size_t i=0; i<nMax; ++i)
        {
            // reverse character order for the input string
            str0[0] = (wchar_t)glyphLigatureMapping[i][1];
            str0[1] = (wchar_t)glyphLigatureMapping[i][0];
            s_LigaturePairs.push_back(str0);

            str1[0] = (wchar_t)glyphLigatureMapping[i][2];
            s_LigaturePairs.push_back(str1);
        }
    }

    return s_LigaturePairs;
}


// returns the static mapping of glyphs with mirror code points - built once
MirrorGlyphs& BIDIConverter::GetMirrorMapping()
{
    static MirrorGlyphs s_MirrorGlyphMap;

    // initialize on first use
    if (0 == s_MirrorGlyphMap.size())
    {
        size_t nMax = sizeof(glyphMirrorMapping) / (sizeof(unsigned int)*2);
        for (size_t i=0; i<nMax; ++i)
        {
            // map the ints to each other
            s_MirrorGlyphMap[glyphMirrorMapping[i][0]] = glyphMirrorMapping[i][1];
            s_MirrorGlyphMap[glyphMirrorMapping[i][1]] = glyphMirrorMapping[i][0];
        }
    }

    return s_MirrorGlyphMap;
}


// generates the static mappings
bool BIDIConverter::GenerateMappings()
{
    BIDIConverter::GetShapeMapping();
    BIDIConverter::GetLigaturePairs();
    BIDIConverter::GetMirrorMapping();
    return true;
}
