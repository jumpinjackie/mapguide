//
//  Copyright (C) 2005-2011 by Autodesk, Inc.
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
#include "GridThemeParser.h"
#include "GridTheme.h"
#include <wchar.h>

//
// GridThemeParser
//

const MdfModel::MdfString GridThemeParser::sm_AspectType = L"Aspect";
const MdfModel::MdfString GridThemeParser::sm_SlopeType  = L"Slope";
const MdfModel::MdfString GridThemeParser::sm_HeightType = L"Height";

GridThemeParser::GridThemeParser()
{
}

GridThemeParser::~GridThemeParser()
{
}

GridTheme* GridThemeParser::ParseThemeColorRules(const MdfModel::RuleCollection *pRules)
{
    // Parse the first rule to get the type and band name.
    GridTheme *pTheme = NULL;
    int i = 0;
    int nCount = pRules->GetCount();
    // Find the first rule that can be parsed to a theme.
    for (i = 0; i < nCount; ++i)
    {
        pTheme = ParseThemeColorRule(dynamic_cast<MdfModel::GridColorRule*>(pRules->GetAt(i)));
        if (pTheme != NULL)
        {
            break;
        }
    }
    if (pTheme == NULL)
    {
        return pTheme;
    }
    for (++i; i < nCount; ++i)
    {
        std::auto_ptr<GridTheme> pSingleTheme(ParseThemeColorRule(dynamic_cast<MdfModel::GridColorRule*>(pRules->GetAt(i))));
        if (pSingleTheme.get() == NULL)
        {
            // Wrong filter string, continue to next filter.
            continue;
        }
        if (pSingleTheme->GetType() != pTheme->GetType()
            || pSingleTheme->GetBandName() != pTheme->GetBandName())
        {
            // Unmatch Type or Band Name.
            continue;
        }
        if (pTheme->AdoptBucket(pSingleTheme->GetBucketAt(0)))
        {
            pSingleTheme->OrphanBucketAt(0);
        }
    }
    return pTheme;
}

GridTheme* GridThemeParser::ParseThemeColorRule(const MdfModel::GridColorRule *pRule)
{
    GridTheme *pTheme = NULL;
    GridBucket *pBucket = NULL;

    MdfModel::MdfString sLeftType;
    MdfModel::MdfString sLeftBandName;
    MdfModel::MdfString sLeftOperator;
    double              dLeftValue = GridBucket::sm_MinDouble;

    MdfModel::MdfString sRightType;
    MdfModel::MdfString sRightBandName;
    MdfModel::MdfString sRightOperator;
    double              dRightValue = GridBucket::sm_MaxDouble;

    // Clear all the stacks.
    m_typeBracketStack.clear();
    m_bandNameBracketStack.clear();
    m_valueBracketStack.clear();

    try
    {
        const MdfModel::MdfString &filter = pRule->GetFilter();
        bool bTwo = false;
        size_t i = 0;
        //size_t nLength = filter.length();

        i = FindType(sLeftType, filter, i);
        i = FindBandName(sLeftBandName, filter, i);
        i = FindOperator(sLeftOperator, filter, i);
        i = FindValue(dLeftValue, filter, i);

        try
        {
            i = Find(L"And", filter, i);
            bTwo = true;
            i = FindType(sRightType, filter, i);
            i = FindBandName(sRightBandName, filter, i);
            i = FindOperator(sRightOperator, filter, i);
            i = FindValue(dRightValue, filter, i);
        }
        catch (std::exception &)
        {
            // Only one filter.
            bTwo = false;
        }

        if (bTwo)
        {
            // TODO: Perform case insenstive compare, and verify on Linux
            if (_wcsicmp(sLeftType.c_str(), sRightType.c_str()) != 0
                || _wcsicmp(sLeftBandName.c_str(), sRightBandName.c_str()) != 0)
            {
                // Unmatch Type or Band Name
                throw std::exception();
            }
        }

        pTheme  = new GridTheme;
        pBucket = new GridBucket;
        pTheme->SetBandName(sLeftBandName);
        pTheme->SetType(sLeftType);

        // Make sure the color is a kind of GridColorExplicit.
        const MdfModel::GridColorExplicit *pColorExplicit = dynamic_cast<const MdfModel::GridColorExplicit*>(pRule->GetGridColor());
        if (NULL == pColorExplicit) // Not theme style
        {
            // Wrong Color Type
            throw std::exception();
        }
        std::wstringstream ss(pColorExplicit->GetExplicitColor());
        Color color;
        ss >> color;
        if (!ss)
        {
            // Wrong Color Value
            throw std::exception();
        }
        pBucket->SetColor(color);

        if (!bTwo)
        {
            if (sLeftOperator == L"<")
            {
                pBucket->SetRange(GridBucket::sm_MinDouble, false, dLeftValue, false);
            }
            else if (sLeftOperator == L"<=")
            {
                pBucket->SetRange(GridBucket::sm_MinDouble, false, dLeftValue, true);
            }
            else if (sLeftOperator == L">")
            {
                pBucket->SetRange(dLeftValue, false, GridBucket::sm_MaxDouble, false);
            }
            else if (sLeftOperator == L">=")
            {
                pBucket->SetRange(dLeftValue, true, GridBucket::sm_MaxDouble, false);
            }
            else if (sLeftOperator == L"=")
            {
                pBucket->SetRange(dLeftValue, true, dLeftValue, true);
            }
            else
            {
                // Wrong Operator
                throw std::exception();
            }
        }
        else
        {
            int nCompResult = CompareDoubles(dLeftValue, dRightValue);
            if (nCompResult == 0)  // left == right
            {
                if ((sLeftOperator == L"<=" && sRightOperator == L">=")
                    || (sLeftOperator == L">=" && sRightOperator == L"<="))
                {
                    pBucket->SetRange(dLeftValue, true, dLeftValue, true);
                }
                else
                {
                    // Wrong operator
                    throw std::exception();
                }
            }
            else if (nCompResult < 0) // left < right
            {
                if ((sLeftOperator == L">" || sLeftOperator == L">=")
                    &&(sRightOperator == L"<" || sRightOperator == L"<="))
                {
                    pBucket->SetRange(dLeftValue, sLeftOperator == L">=", dRightValue, sRightOperator == L"<=");
                }
                else
                {
                    // Wrong operator
                    throw std::exception();
                }
            }
            else if (nCompResult > 0) // left > right
            {
                if ((sLeftOperator == L"<" || sLeftOperator == L"<=")
                    &&(sRightOperator == L">" || sRightOperator == L">="))
                {
                    pBucket->SetRange(dRightValue, sRightOperator == L">=", dLeftValue, sLeftOperator == L"<=");
                }
                else
                {
                    // Wrong operator
                    throw std::exception();
                }
            }
        }
        if (!pTheme->AdoptBucket(pBucket))
        {
            // Faild to Adopt Bucket
            throw std::exception();
        }
    }
    catch (std::exception &)
    {
        delete pTheme;
        pTheme = NULL;

        delete pBucket;
        pBucket = NULL;
    }
    return pTheme;
}

size_t GridThemeParser::FindType(MdfModel::MdfString       &type,
                                 const MdfModel::MdfString &filter,
                                 size_t                    nStart)
{
    bool bFound = false;
    size_t nLength = filter.length();
    // Find the first "aspect", "slope", "height"
    size_t i = nStart;
    for (; i < nLength; ++i)
    {
        // Skip '(', ' '
        if (filter[i] == L'(')
        {
            // Push to stack.
            m_typeBracketStack.push_back(i);
        }
        else if (filter[i] == L' ')
        {
            // Continue.
        }
        else
        {
            if (filter[i] == ::towupper(sm_AspectType[0]) || filter[i] == ::towlower(sm_AspectType[0]))
            {
                // Is it equal to aspect.
                if (nLength < i + sm_AspectType.length())
                {
                    // Too Short
                    throw std::exception();
                }
                // TODO: Perform case insenstive compare, and verify on Linux
                if (0 == _wcsnicmp(filter.c_str() + i, sm_AspectType.c_str(), sm_AspectType.length()))
                {
                    type = sm_AspectType;
                    i += sm_AspectType.length(); // goto next word
                    bFound = true;
                }
                else
                {
                    // Unmatch Aspect
                    throw std::exception();
                }
            }
            else if (filter[i] == ::towupper(sm_SlopeType[0]) || filter[i] == ::towlower(sm_SlopeType[0]))
            {
                // is it equal to slope.
                if (nLength < i + sm_SlopeType.length())
                {
                    // Too Short
                    throw std::exception();
                }
                // TODO: Perform case insenstive compare, and verify on Linux
                if (0 == _wcsnicmp(filter.c_str() + i, sm_SlopeType.c_str(), sm_SlopeType.length()))
                {
                    type = sm_SlopeType;
                    i += sm_SlopeType.length(); // goto next word
                    bFound = true;
                }
                else
                {
                    // Unmatch slope
                    throw std::exception();
                }
            }
            else if (filter[i] == ::towupper(sm_HeightType[0]) || filter[i] == ::towlower(sm_HeightType[0]))
            {
                // is it equal to height.
                if (nLength < i + sm_HeightType.length())
                {
                    // Too short
                    throw std::exception();
                }
                // TODO: Perform case insenstive compare, and verify on Linux
                if (0 == _wcsnicmp(filter.c_str() + i, sm_HeightType.c_str(), sm_HeightType.length()))
                {
                    type = sm_HeightType;
                    i += sm_HeightType.length(); // goto next word
                    bFound = true;
                }
                else
                {
                    // Unmatch height
                    throw std::exception();
                }
            }
            else
            {
                // Unmatch height
                throw std::exception();
            }
            break;
        }
    }

    if (!bFound)
    {
        // Not find type
        throw std::exception();
    }

    return i;
}

size_t GridThemeParser::FindBandName(MdfModel::MdfString       &bandname,
                                     const MdfModel::MdfString &filter,
                                     size_t                     nStart)
{
    bool bFound = false;
    size_t nLength = filter.length();
    // Find the first "aspect", "slope", "height"
    size_t i = nStart;
    for (; i < nLength; ++i)
    {
        // Skip ' '
        if (filter[i] == L' ')
        {
            // Continue
        }
        else if (filter[i] == L'(')
        {
            // push to stack
            m_bandNameBracketStack.push_back(i);
        }
        else if (filter[i] != L')')
        {
            if (m_bandNameBracketStack.empty())
            {
                // No bracket around band name
                throw std::exception();
            }
            size_t j = i + 1;
            while (j < nLength && filter[j] != ')')
            {
                ++j;
            }
            if (j == nLength)
            {
                // Unmatch brackets around Band Name
                throw std::exception();
            }
            bandname = filter.substr(i, j - i);
            bFound = true;
            // Trim the tail space char.
            size_t k;
            for (k = j - i - 1; k >= 0; --k)
            {
                if (bandname[k] != L' ')
                {
                    break;
                }
            }
            bandname = bandname.substr(0, k + 1);
            i = j + 1; // goto next char.
            m_bandNameBracketStack.pop_back();
            if (!m_bandNameBracketStack.empty())
            {
                // Additional brackets around band name
                throw std::exception();
            }
            // Pop up the brackets around Type and Band Name.
            for (; i < nLength; ++i)
            {
                if (filter[i] == L' ')
                {
                    // continue.
                }
                else if (filter[i] == L')')
                {
                    if (m_typeBracketStack.empty())
                    {
                        // Unmatch brackets around Type and Band Name
                        throw std::exception();
                    }
                    m_typeBracketStack.pop_back();
                }
                else
                {
                    break;
                }
            }
            break; // jump out the loop.
        }
        else
        {
            // No Brackets around Band Name
            throw std::exception();
        }
    }

    if (!bFound)
    {
        // Not Find Band Name
        throw std::exception();
    }

    return i;
}

size_t GridThemeParser::FindOperator(MdfModel::MdfString       &soperator,
                                     const MdfModel::MdfString &filter,
                                     size_t                     nStart)
{
    bool bFound = false;
    size_t nLength = filter.length();
    size_t i = nStart;
    for (; i < nLength; ++i)
    {
        if (filter[i] == L' ')
        {
            // continue
        }
        else if (filter[i] == L'<')
        {
            if (((i + 1) < nLength)
                && (filter[i + 1] == L'='))
            {
                soperator = filter.substr(i, 2);
                i += 2;
            }
            else
            {
                soperator = filter[i];
                ++i;
            }
            bFound = true;
            break;
        }
        else if (filter[i] == L'>')
        {
            if (((i + 1) < nLength)
                && (filter[i + 1] == L'='))
            {
                soperator = filter.substr(i, 2);
                i += 2;
            }
            else
            {
                soperator = filter[i];
                ++i;
            }
            bFound = true;
            break;
        }
        else if (filter[i] == L'=')
        {
            soperator = filter[i];
            ++i;
            bFound = true;
            break;
        }
        else
        {
            // No Operator
            throw std::exception();
        }
    }

    if (!bFound)
    {
        // Not Find Operator
        throw std::exception();
    }

    return i;
}

size_t GridThemeParser::FindValue(double                    &value,
                                  const MdfModel::MdfString &filter,
                                  size_t                     nStart)
{
    bool bFound = false;
    size_t nLength = filter.length();
    size_t i = nStart;

    wchar_t szDecSep[4];
    *szDecSep = L'\0';

#ifdef _WIN32
    int nNoOfBytesWritten =
        ::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, szDecSep, sizeof(szDecSep)/sizeof(wchar_t));
    if (0 == nNoOfBytesWritten)
    {
        assert(false);
        wcscpy(szDecSep, L".");
    }
#else
    // TODO: Linux implementation
#endif

    for (; i < nLength; ++i)
    {
        if (filter[i] == L' ')
        {
            // Continue.
        }
        else if (filter[i] == L'(')
        {
            // Push
            m_valueBracketStack.push_back(i);
        }
        else if (filter[i] == L'-'
                 || filter[i] == L'+'
                 || filter[i] == szDecSep[0]
                 || filter[i] == L'e'
                 || filter[i] == L'E'
                 || ::iswdigit(filter[i]))
        {
            size_t j = i + 1;
            while (j < nLength
                   && (filter[j] == L'-'
                       || filter[j] == L'+'
                       || filter[j] == szDecSep[0]
                       || filter[j] == L'e'
                       || filter[j] == L'E'
                       || ::iswdigit(filter[j])))
            {
                ++j;
            }
            const MdfModel::MdfString svalue = filter.substr(i, j - i);
            i = j;
            std::wstringstream ss(svalue);
            ss >> value;
            if (!ss)
            {
                // Wrond data format
                throw std::exception();
            }
            bFound = true;
            if (!m_valueBracketStack.empty())
            {
                for (; i < nLength; ++i)
                {
                    if (filter[i] == L' ')
                    {
                        // continue;
                    }
                    else if (filter[i] == L')')
                    {
                        m_valueBracketStack.pop_back();
                        if (m_valueBracketStack.empty())
                        {
                            ++i;
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                if (!m_valueBracketStack.empty())
                {
                    // Unmatch brackets around value
                    throw std::exception();
                }
            }
            // Pop up the brackets around Type, Band Name, operator and Value.
            for (; i < nLength; ++i)
            {
                if (filter[i] == L' ')
                {
                    // continue.
                }
                else if (filter[i] == L')')
                {
                    if (m_typeBracketStack.empty())
                    {
                        // Unmatch brackets around Type and Band Name
                        throw std::exception();
                    }
                    m_typeBracketStack.pop_back();
                }
                else
                {
                    break;
                }
            }
            break; // jump out the loop
        }
        else
        {
            // No Value
            throw std::exception();
        }
    }

    if (!bFound)
    {
        // Not Find Operator
        throw std::exception();
    }

    return i;
}

size_t GridThemeParser::Find(const MdfModel::MdfString &findstring,
                             const MdfModel::MdfString &filter,
                             size_t                     nStart)
{
    bool bFound = false;
    assert(findstring[0] != L' ');
    size_t nLength = filter.length();
    size_t i = nStart;
    for (; i < nLength; ++i)
    {
        if (filter[i] == ::towupper(findstring[0]) || filter[i] == ::towlower(findstring[0]))
        {
            if (nLength < i + findstring.length())
            {
                // No match word
                throw std::exception();
            }
            // TODO: Perform case insenstive compare, and verify on Linux
            if (0 == _wcsnicmp(filter.c_str() + i, findstring.c_str(), findstring.length()))
            {
                // find it
                i += findstring.length();
                bFound = true;
                break;
            }
            else
            {
                // No match word
                throw std::exception();
            }
            break; // jump out the loop.
        }
        else if (filter[i] == L' ')
        {
        }
        else
        {
            // No match word
            throw std::exception();
        }
    }

    if (!bFound)
    {
        // Not find String
        throw std::exception();
    }

    return i;
}
