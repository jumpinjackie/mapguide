//  $Header: //GBU/Metropolis/Main/Server/Common/Stylization/GridThemeParser.cpp#1 $
// 
//  Copyright (C) 2005 Autodesk, Inc. All Rights Reserved.
//
//  By using this code, you are agreeing to the terms and conditions of
//  the License Agreement included in the documentation for this code.
// 
//  AUTODESK MAKES NO WARRANTIES, EXPRESS OR IMPLIED, AS TO THE
//  CORRECTNESS OF THIS CODE OR ANY DERIVATIVE WORKS WHICH INCORPORATE
//  IT. AUTODESK PROVIDES THE CODE ON AN "AS-IS" BASIS AND EXPLICITLY
//  DISCLAIMS ANY LIABILITY, INCLUDING CONSEQUENTIAL AND INCIDENTAL
//  DAMAGES FOR ERRORS, OMISSIONS, AND OTHER PROBLEMS IN THE CODE.
// 
//  Use, duplication, or disclosure by the U.S. Government is subject
//  to restrictions set forth in FAR 52.227-19 (Commercial Computer 
//  Software Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
//  (Rights in Technical Data and Computer Software), as applicable.
//

#include "stdafx.h"
#include "GridThemeParser.h"
#include "GridTheme.h"
#ifdef WIN32
#include <tchar.h>
#endif
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
            if (::wcsicmp(sLeftType.c_str(), sRightType.c_str()) != 0
                || ::wcsicmp(sLeftBandName.c_str(), sRightBandName.c_str()) != 0)
            {
                throw std::exception("Unmatch Type or Band Name");
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
            throw std::exception("Wrong Color Type");
        }
        std::wstringstream ss(pColorExplicit->GetExplicitColor());
        Color color;
        ss >> color;
        if (!ss)
        {
            throw std::exception("Wrong color value");
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
                throw std::exception("Wrong Operator");
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
                    throw std::exception("Wrong operator");
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
                    throw std::exception("Wrong operator");
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
                    throw std::exception("Wrong operator");
                }
            }
        }
        if (!pTheme->AdoptBucket(pBucket))
        {
            throw std::exception("Failed to Adopt Bucket");
        }
    }
    catch (std::exception &)
    {
        if (pTheme)
        {
            delete pTheme;
            pTheme = NULL;
        }
        if (pBucket)
        {
            delete pBucket;
            pBucket = NULL;
        }
    }
    return pTheme;
}

size_t GridThemeParser::FindType(
                                 MdfModel::MdfString       &type, 
                                 const MdfModel::MdfString &filter,
                                 size_t                    nStart) throw (std::exception)
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
                    throw std::exception("Too Short");
                }
                if (0 == ::wcsnicmp(filter.c_str() + i, sm_AspectType.c_str(), sm_AspectType.length()))
                {
                    type = sm_AspectType;                 
                    i += sm_AspectType.length(); // goto next word
                    bFound = true;
                }
                else
                {
                    throw std::exception("Unmatch Aspect");
                }
            }
            else if (filter[i] == ::towupper(sm_SlopeType[0]) || filter[i] == ::towlower(sm_SlopeType[0]))
            {
                // is it equal to slope.
                if (nLength < i + sm_SlopeType.length())
                {
                    throw std::exception("Too Short");
                }
                if (0 == ::wcsnicmp(filter.c_str() + i, sm_SlopeType.c_str(), sm_SlopeType.length()))
                {
                    type = sm_SlopeType;                 
                    i += sm_SlopeType.length(); // goto next word
                    bFound = true;
                }
                else
                {
                    throw std::exception("Unmatch Slope");
                }
            }
            else if (filter[i] == ::towupper(sm_HeightType[0]) || filter[i] == ::towlower(sm_HeightType[0]))
            {
                // is it equal to height.
                if (nLength < i + sm_HeightType.length())
                {
                    throw std::exception("Too Short");
                }
                if (0 == ::wcsnicmp(filter.c_str() + i, sm_HeightType.c_str(), sm_HeightType.length()))
                {
                    type = sm_HeightType;                 
                    i += sm_HeightType.length(); // goto next word
                    bFound = true;
                }
                else
                {
                    throw std::exception("Unmatch Height");
                }
            }
            else
            {
                throw std::exception("Unmatch Height");
            }
            break;
        }
    }

    if (!bFound)
    {
        throw std::exception("Not Find Type");
    }

    return i;
}

size_t GridThemeParser::FindBandName(
                                     MdfModel::MdfString       &bandname, 
                                     const MdfModel::MdfString &filter,
                                     size_t                     nStart) throw (std::exception)
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
                throw std::exception("No Bracket around Band Name");
            }
            size_t j = i + 1;
            while (j < nLength && filter[j] != ')')
            {
                ++j;
            }
            if (j == nLength)
            {
                throw std::exception("Unmatch brackets around Band Name");
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
                throw std::exception("Additional brackets around band name");
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
                        throw std::exception("Unmatch brackets around Type and Band Name");
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
            throw std::exception("No Brackets around Band Name");
        }
    }

    if (!bFound)
    {
        throw std::exception("Not Find Band Name");
    }

    return i;
}

size_t GridThemeParser::FindOperator(
                                     MdfModel::MdfString       &soperator, 
                                     const MdfModel::MdfString &filter,
                                     size_t                     nStart) throw (std::exception)
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
            throw std::exception("No Operator");
        }
    }

    if (!bFound)
    {
        throw std::exception("Not Find Operator");
    }

    return i;
}

size_t GridThemeParser::FindValue(    
                                  double                    &value, 
                                  const MdfModel::MdfString &filter,
                                  size_t                     nStart) throw (std::exception)
{
    bool bFound = false;
    size_t nLength = filter.length();
    size_t i = nStart;

    TCHAR szDecSep[4];
    *szDecSep = _T('\0');
    int nNoOfBytesWritten =
        ::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, szDecSep, sizeof(szDecSep)/sizeof(TCHAR));
    if (0 == nNoOfBytesWritten)
    {
        _ASSERT(false);
        _tcscpy(szDecSep, _T("."));
    }

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
                throw std::exception("Wrong data format");
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
                    throw std::exception("Unmatch brackets around value");
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
                        throw std::exception("Unmatch brackets around Type and Band Name");
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
            throw std::exception("No Value");
        }
    }

    if (!bFound)
    {
        throw std::exception("Not Find Operator");
    }

    return i;
}

size_t GridThemeParser::Find(    
                             const MdfModel::MdfString &findstring, 
                             const MdfModel::MdfString &filter,
                             size_t                     nStart) throw (std::exception)
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
                throw std::exception("No match word");
            }
            if (0 == ::wcsnicmp(filter.c_str() + i, findstring.c_str(), findstring.length()))
            {
                // find it
                i += findstring.length();
                bFound = true;
                break;
            }
            else
            {
                throw std::exception("No match word");
            }
            break; // jump out the loop.
        }
        else if (filter[i] == L' ')
        {
        }
        else
        {
            throw std::exception("No match word");
        }
    }

    if (!bFound)
    {
        throw std::exception("Not Find String");
    }

    return i;
}
