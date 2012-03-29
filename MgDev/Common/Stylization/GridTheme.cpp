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
#include "GridTheme.h"
#include <float.h>

//
// GridBucket
//
const double GridBucket::sm_MinDouble = -DBL_MAX;
const double GridBucket::sm_MaxDouble = +DBL_MAX;

GridBucket::GridBucket()
{
    SetRange(sm_MinDouble, false, sm_MaxDouble, false);
}

GridBucket::~GridBucket()
{
}

void GridBucket::SetRange(double dBeginValue, bool bEqualBeginValue, double dEndValue, bool bEqualEndValue)
{
    m_dBeginValue = dBeginValue;
    m_bEqualBeginValue = bEqualBeginValue;

    m_compareMask = 0;
    if (bEqualBeginValue)
        m_compareMask |= 0x1; //set bit 0 if we need to check begin value for equals

    m_dEndValue = dEndValue;
    m_bEqualEndValue = bEqualEndValue;

    if (bEqualEndValue)
        m_compareMask |= 0x2; //set bit 1 if we need to check end value for equals
}

const Color& GridBucket::GetColor() const
{
    return m_color;
}

void GridBucket::SetColor(const Color &color)
{
    m_color = color;
}

GridBucket::CompareResultType GridBucket::CompareBuckets(const GridBucket &left, const GridBucket &right)
{
    CompareResultType result = GridBucket::kOverlapped;
    // kGreater
    int nCompResult = CompareDoubles(left.m_dBeginValue, right.m_dEndValue);
    if (nCompResult > 0
        || (nCompResult == 0 && (!left.m_bEqualBeginValue || !right.m_bEqualEndValue)))
    {
        result = GridBucket::kGreater;
    }
    // kLess
    nCompResult = CompareDoubles(left.m_dEndValue, right.m_dBeginValue);
    if (nCompResult < 0
        || (nCompResult == 0 && (!left.m_bEqualEndValue || !right.m_bEqualBeginValue)))
    {
        result = GridBucket::kLess;
    }
    return result;
}

double GridBucket::GetBeginValue() const
{
    return m_dBeginValue;
}

bool GridBucket::IsEqualToBeginValue() const
{
    return m_bEqualBeginValue;
}

double GridBucket::GetEndValue() const
{
    return m_dEndValue;
}

bool GridBucket::IsEqualToEndValue() const
{
    return m_bEqualEndValue;
}

bool GridBucket::IsValueInBucket(double value) const
{
    switch (m_compareMask)
    {
    case 0:
        if (value >= m_dEndValue) return false;
        if (value <= m_dBeginValue) return false;
        return true;
    case 1:
        if (value >= m_dEndValue) return false;
        if (value < m_dBeginValue) return false;
        return true;
    case 2:
        if (value > m_dEndValue) return false;
        if (value <= m_dBeginValue) return false;
        return true;
    case 3:
        if (value > m_dEndValue) return false;
        if (value < m_dBeginValue) return false;
        return true;
    default:
        return false;
    }
}


//
// GridTheme
//

GridTheme::GridTheme()
{
}

GridTheme::~GridTheme()
{
}

bool GridTheme::SearchColorByValue(Color &color, double value) const
{
    int i = 0;
    int nCount = m_buckets.GetCount();
    for (; i < nCount; ++i)
    {
        GridBucket *pBucket = m_buckets.GetAt(i);
        if (pBucket->IsValueInBucket(value))
        {
            color = pBucket->GetColor();
            break;
        }
    }
    return (i < nCount);
}

const MdfModel::MdfString& GridTheme::GetBandName() const
{
    return m_sBandName;
}

void GridTheme::SetBandName(const MdfModel::MdfString &sBandName)
{
    m_sBandName = sBandName;
}

const MdfModel::MdfString& GridTheme::GetType() const
{
    return m_sType;
}

void GridTheme::SetType(const MdfModel::MdfString &sType)
{
    m_sType = sType;
}

int GridTheme::GetBucketCount()
{
    return m_buckets.GetCount();
}

bool GridTheme::AdoptBucket(GridBucket *pBucket)
{
    assert(pBucket);
    bool bAdopt = false;
    if (m_buckets.GetCount() == 0)
    {
        m_buckets.Adopt(pBucket);
        bAdopt = true;
    }
    else
    {
        GridBucket *pTailBucket = m_buckets.GetAt(m_buckets.GetCount() - 1);
        GridBucket::CompareResultType nCompResult = GridBucket::CompareBuckets(*pBucket, *pTailBucket);
        if (nCompResult == GridBucket::kOverlapped)
        {
            bAdopt = false;
        }
        else if (nCompResult == GridBucket::kGreater)
        {
            m_buckets.Adopt(pBucket);
            bAdopt = true;
        }
        else
        {   // Search to head bucket to find a position.
            int k;
            for (k = m_buckets.GetCount() - 2; k >= 0; --k)
            {
                GridBucket *pCurBucket = m_buckets.GetAt(k);
                nCompResult = GridBucket::CompareBuckets(*pBucket, *pCurBucket);
                if (nCompResult == GridBucket::kOverlapped)
                {
                    // Overlapped with the current bucket.
                    bAdopt = false;
                    break;
                }
                else if (nCompResult == GridBucket::kGreater)
                {
                    m_buckets.AdoptAt(k + 1, pBucket);
                    bAdopt = true;
                    break;
                }
            }
            if (k < 0)
            {
                // It is the least bucket.
                m_buckets.AdoptAt(0, pBucket);
                bAdopt = true;
            }
        }
    }
    return bAdopt;
}

GridBucket* GridTheme::OrphanBucketAt(int nIndex)
{
    return m_buckets.OrphanAt(nIndex);
}

GridBucket* GridTheme::OrphanBucket(GridBucket *pBucket)
{
    return m_buckets.Orphan(pBucket);
}

GridBucket* GridTheme::GetBucketAt(int nIndex)
{
    return m_buckets.GetAt(nIndex);
}

//
// GridThemeHashTable
//

GridThemeHashTable::GridThemeHashTable()
{
    Clear();
}

GridThemeHashTable::~GridThemeHashTable()
{
}

bool GridThemeHashTable::Initialize(GridTheme *pTheme)
{
    // Clear the current data to prepare making new hash table.
    Clear();
    // Put the buckets in hash table for improving performace.
    if (pTheme->GetBucketCount() > 1)
    {
        int nBucketCount = pTheme->GetBucketCount();
        int nCalculatedBucketCount = nBucketCount;
        double dStart, dEnd;
        bool bBeginMin(false), bEndMax(false);
        GridBucket *pHead = pTheme->GetBucketAt(0);
        GridBucket *pTail = pTheme->GetBucketAt(nBucketCount - 1);
        if (CompareDoubles(pHead->GetBeginValue(), GridBucket::sm_MinDouble) == 0)
        {
            dStart = pHead->GetEndValue();
            --nCalculatedBucketCount;
            bBeginMin = true;
        }
        else
        {
            dStart = pHead->GetBeginValue();
            bBeginMin = false;
        }
        if (CompareDoubles(pTail->GetEndValue(), GridBucket::sm_MaxDouble) == 0)
        {
            dEnd = pTail->GetBeginValue();
            --nCalculatedBucketCount;
            bEndMax = true;
        }
        else
        {
            dEnd = pTail->GetEndValue();
            bEndMax = false;
        }
        if (nCalculatedBucketCount == 0)
        {
            // Don't need to generate the hash table.
            return false;
        }
        else
        {
            // May change 2 to antoher number to generate more rows.
            int nHashRowCount = nCalculatedBucketCount * 2;
            double dInterval = (dEnd - dStart) / nHashRowCount;
            // Increase the nHashRowCount by 2 for the regin which is less than one value,
            // or greater than a value.
            nHashRowCount += 2;
            // Divide all the real value to nHashRowCout regions.
            m_vHashRows.resize(nHashRowCount);
            int i = 0;
            int nCount = nBucketCount;
            if (bBeginMin)
            {
                i = 1;
                m_vHashRows[0].push_back(pTheme->GetBucketAt(0));
            }
            if (bEndMax)
            {
                nCount = nBucketCount - 1;
                m_vHashRows[nHashRowCount - 1].push_back(pTheme->GetBucketAt(nBucketCount - 1));
            }
            for (; i < nCount; ++i)
            {
                GridBucket *pBucket = pTheme->GetBucketAt(i);
                double dBeginPos = (pBucket->GetBeginValue() - dStart) / dInterval;
                // Adjust the double if it is like this 1.9999999999999.
                int nBeginPos = static_cast<int>(dBeginPos + 1E-10) + 1;
                m_vHashRows[nBeginPos].push_back(pBucket);
                double dEndPos = (pBucket->GetEndValue() - dStart) / dInterval;
                // Adjust the double if it is like this 1.9999999999999.
                int nEndPos = static_cast<int>(dEndPos + 1E-10);
                if (0 != CompareDoubles(dEndPos, nEndPos))
                    ++nEndPos;
                while (nEndPos > nBeginPos)
                    m_vHashRows[nEndPos--].push_back(pBucket);
            }
            m_dStart = dStart;
            m_dInterval = dInterval;
            m_dInvInterval = 1.0 / dInterval;
            return true;
        }
    }
    else
    {
        // Only one bucket, don't need to generate the hash table.
        return false;
    }
}

void GridThemeHashTable::Clear()
{
    m_dStart = 0;
    m_dInterval = GridBucket::sm_MaxDouble;
    m_dInvInterval = 0.0;
    m_vHashRows.clear();
}

bool GridThemeHashTable::SearchColorByValue(Color &color, double value) const
{
    size_t nIndex = 0;
    double dIndex = (value - m_dStart) * m_dInvInterval;
    int temp = Double2Int(dIndex);

    if (dIndex < 0) // That mean it locates at the first hash row.
    {
        nIndex = 0;
    }
    else if (dIndex > m_vHashRows.size() - 2)
    {
        nIndex = m_vHashRows.size() - 1;
    }
    else if (CompareDoubles(dIndex, temp) == 0)
    {
        // The value is right at the edge of the range.
        assert(temp + 1 < static_cast<int>(m_vHashRows.size()));
        nIndex = temp;
        if (!m_vHashRows[nIndex].empty()
            && m_vHashRows[nIndex].back()->IsValueInBucket(value))
        {
            color = m_vHashRows[nIndex].back()->GetColor();
            return true;
        }
        ++nIndex;
        if (!m_vHashRows[nIndex].empty()
            && m_vHashRows[nIndex].front()->IsValueInBucket(value))
        {
            color = m_vHashRows[nIndex].front()->GetColor();
            return true;
        }
        return false;
    }
    else
    {
        nIndex = static_cast<size_t>(dIndex) + 1;
    }
    assert(nIndex < m_vHashRows.size());
    std::vector<GridBucket*>::const_iterator it, end;
    const std::vector<GridBucket*>& bucket = m_vHashRows[nIndex];
    it = bucket.begin();
    end = bucket.end();

    for (; it != end; ++it)
    {
        GridBucket *pBucket = *it;
        if (pBucket->IsValueInBucket(value))
        {
            color = pBucket->GetColor();
            break;
        }
    }

    return it != end;
}
