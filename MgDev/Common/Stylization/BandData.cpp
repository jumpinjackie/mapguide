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
#include "BandData.h"
#include <memory.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
BandData::BandData(unsigned char size, unsigned int width, unsigned int height):
                   m_size(size), m_width(width), m_height(height), m_pData(0)
{}

//*************************************************************************************************************

BandData::~BandData()
{}

//*************************************************************************************************************

bool BandData::DeepCopy(const BandData* pSource)
{
    if (m_size  != pSource->GetSize()   ||
        m_width != (unsigned int)pSource->GetWidth()  ||
        m_height!= (unsigned int)pSource->GetHeight() )
    {
        return false;
    }

    memcpy( m_pData, pSource->m_pData, memsize() );
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
BitBandData::BitBandData(unsigned char size, unsigned int width, unsigned int height):
        BandData(size,width,height)
{
    //every INT8 is 8 bit
    m_numberPerByte = 8 / size;
    m_bytesPerRow = (width - 1) / m_numberPerByte + 1;
    m_mask = (1 << size) -1;

    m_pData = new INT8[ memsize() ];
}

//*************************************************************************************************************

int BitBandData::memsize()
{
    return m_bytesPerRow * m_height;
}

//*************************************************************************************************************

bool BitBandData::GetValue(unsigned int x, unsigned int y, INT8& value) const
{
    if (!CheckInBound(x,y))
    {
        return false;
    }

    int pos    = y * m_bytesPerRow + x / m_numberPerByte;
    UINT8 pos2 = (UINT8)(8 - m_size - x % m_numberPerByte * m_size);    //now, the elements are from high bit to low bit

    UINT8 temp = m_pData[pos];
    UINT8 tempmask = m_mask << pos2;
    UINT8 tempValue = temp & tempmask;
    value = tempValue >> pos2;

    return true;
}

//*************************************************************************************************************

bool BitBandData::SetValue(unsigned int x, unsigned int y, const INT8* pvalue)
{
    if (!CheckInBound(x,y))
    {
        return false;
    }

    int pos    = y * m_bytesPerRow + x / m_numberPerByte;
    UINT8 pos2 = (UINT8)(8 - m_size - x % m_numberPerByte * m_size);    //now, the elements are from high bit to low bit

    INT8& temp = m_pData[pos];
    INT8 tempvalue = (*pvalue) & m_mask; //set pvalue other bits to 0
    INT8 tempmask = m_mask << pos2;
    temp &= ~tempmask;
    temp |= (tempvalue << pos2);

    return true;
}

//*************************************************************************************************************

bool BitBandData::GetValue(unsigned int x, unsigned int y, INT16& value) const
{
    return GetValue(x,y,reinterpret_cast<INT8&>(value));
}

//*************************************************************************************************************

bool BitBandData::SetValue(unsigned int x, unsigned int y, const INT16* pvalue)
{
    return SetValue(x,y,reinterpret_cast<const INT8*>(pvalue));
}

//*************************************************************************************************************

bool BitBandData::GetValue(unsigned int x, unsigned int y, INT32& value) const
{
    return GetValue(x,y,reinterpret_cast<INT8&>(value));
}

//*************************************************************************************************************

bool BitBandData::SetValue(unsigned int x, unsigned int y, const INT32* pvalue)
{
    return SetValue(x,y,reinterpret_cast<const INT8*>(pvalue));
}

//*************************************************************************************************************

bool BitBandData::GetValue(unsigned int x, unsigned int y, INT64& value) const
{
    return GetValue(x,y,reinterpret_cast<INT8&>(value));
}

//*************************************************************************************************************

bool BitBandData::SetValue(unsigned int x, unsigned int y, const INT64* pvalue)
{
    return SetValue(x,y,reinterpret_cast<const INT8*>(pvalue));
}

//*************************************************************************************************************

bool BitBandData::DeepCopy(const BandData* pSource)
{
    const BitBandData* pBitBandData = dynamic_cast<const BitBandData*>(pSource);
    if (NULL == pBitBandData)
    {
        return false;
    }

    m_numberPerByte = pBitBandData->m_numberPerByte;
    m_mask          = pBitBandData->m_mask;

    return BandData::DeepCopy(pSource);
}


//*************************************************************************************************************
bool BitBandData::SetRowValue(unsigned int rownum, const INT8 * pFrom)
{
    if (rownum >= m_height)
    {
        return false;
    }

    unsigned int  pos = rownum * m_bytesPerRow;
    memcpy(&m_pData[pos], pFrom, m_bytesPerRow);
    return true;
}


bool BitBandData::SetRowValue(unsigned int rownum, const INT16 * pFrom)
{
    return SetRowValue (rownum, reinterpret_cast<const INT8*>(pFrom));
}


bool BitBandData::SetRowValue(unsigned int rownum, const INT32 * pFrom)
{
    return SetRowValue (rownum, reinterpret_cast<const INT8*>(pFrom));
}


bool BitBandData::SetRowValue(unsigned int rownum, const INT64 * pFrom)
{
    return SetRowValue (rownum, reinterpret_cast<const INT8*>(pFrom));
}

//*************************************************************************************************************
bool BitBandData::SetRowValue(unsigned int xFrom, unsigned int y, const INT8  * pFrom, unsigned int nCount)
{
    if (y >= m_height || (xFrom + nCount) > m_width)
    {
        return false;
    }

    if ( 0 == (xFrom % m_numberPerByte)) //start at the begin of one byte.
    {
        unsigned int pos = y * m_bytesPerRow + xFrom / m_numberPerByte;
        memcpy (&m_pData[pos], pFrom,  nCount / m_numberPerByte);
        if (nCount % m_numberPerByte) //o,not ends with a total byte, set it manually
        {
            unsigned int leftcount = nCount % m_numberPerByte;
            UINT8 lastvalue = *(pFrom + nCount / m_numberPerByte);//get the last value.

            while (leftcount)
            {
                lastvalue = (lastvalue << m_size) | (lastvalue >> (8 - m_size)); //Rotate m_size bits to the left
                SetValue (xFrom + nCount - leftcount , y, (INT8 *)&lastvalue);
                --leftcount;
            }
        }
    }
    else //not starts at the begin of one byte, so manauly set each value.
    {
        int tmp_numberPerByte;
        UINT8 tmpvalue;
        for (unsigned int i =0; i < nCount; ++i)
        {
            tmpvalue = * (pFrom + i / m_numberPerByte);
            tmp_numberPerByte = m_numberPerByte;

            while (tmp_numberPerByte && (i < nCount))
            {
                tmpvalue = tmpvalue << m_size | tmpvalue >> (8 - m_size); //Rotate m_size bits to the left
                SetValue (xFrom + i , y, (INT8 *) &tmpvalue);
                --tmp_numberPerByte;
                ++i;
            }
        }
    }
    return true;
}

bool BitBandData::SetRowValue(unsigned int xFrom, unsigned int y, const INT16 * pFrom, unsigned int nCount)
{
    return SetRowValue (xFrom, y, reinterpret_cast<const INT8*>(pFrom), nCount);
}

bool BitBandData::SetRowValue(unsigned int xFrom, unsigned int y, const INT32 * pFrom, unsigned int nCount)
{
    return SetRowValue (xFrom, y, reinterpret_cast<const INT8*>(pFrom), nCount);
}

bool BitBandData::SetRowValue(unsigned int xFrom, unsigned int y, const INT64 * pFrom, unsigned int nCount)
{
    return SetRowValue (xFrom, y, reinterpret_cast<const INT8*>(pFrom), nCount);
}

//*************************************************************************************************************
bool   BitBandData::SetAllToValue (const INT8*  pvalue)
{
    for (int i = 0; i < m_numberPerByte; ++i) //set all the datas in the first byte to *pvalue.
    {
        SetValue(i, 0, pvalue);
    }

    memset (m_pData, m_pData[0], memsize());  //ok, set all the bytes to the same value.
    return true;
}

bool   BitBandData::SetAllToValue (const INT16* pvalue)
{
    return SetAllToValue (reinterpret_cast<const INT8*> (pvalue));
}

bool   BitBandData::SetAllToValue (const INT32* pvalue)
{
    return SetAllToValue (reinterpret_cast<const INT8*> (pvalue));
}

bool   BitBandData::SetAllToValue (const INT64* pvalue)
{
    return SetAllToValue (reinterpret_cast<const INT8*> (pvalue));
}

//*************************************************************************************************************

BitBandData::~BitBandData()
{
    delete[] m_pData;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
ByteBandData::ByteBandData(unsigned char size, unsigned int width, unsigned int height):
        BandData(size,width,height)
{
    m_pData = new INT8[ m_width * m_height * m_size];
}

//*************************************************************************************************************

ByteBandData::~ByteBandData()
{
    delete[] m_pData;
}

//*************************************************************************************************************

int ByteBandData::memsize()
{
    return m_width * m_height * m_size;
}

//*************************************************************************************************************

bool ByteBandData::GetValue(unsigned int x, unsigned int y, INT8& value) const
{
    if (!CheckInBound(x,y))
    {
        return false;
    }

    int pos    = (x + y * m_width) * m_size;
    value    = m_pData[pos];

    return true;
}

//*************************************************************************************************************

bool ByteBandData::SetValue(unsigned int x, unsigned int y, const INT8* pvalue)
{
    if (!CheckInBound(x,y))
    {
        return false;
    }

    int pos        = (x + y * m_width) * m_size;

    INT8& temp = m_pData[pos];
    temp = *pvalue;

    return true;
}

//*************************************************************************************************************

bool ByteBandData::GetValue(unsigned int x, unsigned int y, INT16& value) const
{
    if (2 < m_size)
    {
        return false;
    }

    if (!CheckInBound(x,y))
    {
        return false;
    }

    int pos        = (x + y * m_width) * m_size;
    value = 0;
    memcpy(&value, &m_pData[pos], 2 );

    return true;
}

//*************************************************************************************************************

bool ByteBandData::SetValue(unsigned int x, unsigned int y, const INT16* pvalue)
{
    if (2 < m_size)
    {
        return false;
    }

    if (!CheckInBound(x,y))
    {
        return false;
    }

    int pos        = (x + y * m_width) * m_size;

    memcpy(&m_pData[pos], pvalue, m_size);

    return true;
}

//*************************************************************************************************************

bool ByteBandData::GetValue(unsigned int x, unsigned int y, INT32& value) const
{
    if (4 < m_size)
    {
        return false;
    }

    if (! CheckInBound(x,y))
    {
        return false;
    }

    int pos        = (x + y * m_width) * m_size;

    value = 0;
    memcpy(&value, &m_pData[pos], m_size);

    return true;
}

//*************************************************************************************************************

bool ByteBandData::SetValue(unsigned int x, unsigned int y, const INT32* pvalue)
{
    if (4 < m_size)
    {
        return false;
    }

    if (!CheckInBound(x,y))
    {
        return false;
    }

    int pos        = (x + y * m_width) * m_size;

    memcpy(&m_pData[pos], pvalue, m_size);

    return true;
}

//*************************************************************************************************************

bool ByteBandData::GetValue(unsigned int x, unsigned int y, INT64& value) const
{
    assert(m_size <= 8);

    if (!CheckInBound(x,y))
    {
        return false;
    }

    if (m_size == 8)
    {
        int pos = (x + y * m_width);
        value = m_pData[pos*8];
        return true;
    }

    int pos = (x + y * m_width) * m_size;
    value = 0;
    memcpy(&value, &m_pData[pos], m_size);

    return true;
}

//*************************************************************************************************************
bool ByteBandData::SetValue(unsigned int x, unsigned int y, const INT64* pvalue)
{
    if (8 < m_size)
    {
        return false;
    }

    if (!CheckInBound(x,y))
    {
        return false;
    }

    int pos        = (x + y * m_width) * m_size;

    memcpy(&m_pData[pos], pvalue, m_size);

    return true;
}


//*************************************************************************************************************
bool ByteBandData::SetRowValue(unsigned int rownum, const INT8 * pFrom)
{
    if (rownum >= m_height)
    {
        return false;
    }
    unsigned int  pos = (rownum * m_width) * m_size;
    memcpy(&m_pData[pos], pFrom, m_size * m_width);
    return true;
}


bool ByteBandData::SetRowValue(unsigned int rownum, const INT16 * pFrom)
{
    return SetRowValue (rownum, reinterpret_cast<const INT8*>(pFrom));
}


bool ByteBandData::SetRowValue(unsigned int rownum, const INT32 * pFrom)
{
    return SetRowValue (rownum, reinterpret_cast<const INT8*>(pFrom));
}


bool ByteBandData::SetRowValue(unsigned int rownum, const INT64 * pFrom)
{
    return SetRowValue (rownum, reinterpret_cast<const INT8*>(pFrom));
}

//*************************************************************************************************************
bool ByteBandData::SetRowValue(unsigned int xFrom, unsigned int y, const INT8  * pFrom, unsigned int nCount)
{
    if (y  >= m_height || (xFrom + nCount) > m_width)
    {
        return false;
    }
    unsigned int pos = (xFrom + y * m_width) * m_size;
    memcpy(&m_pData[pos], pFrom, m_size * nCount);
    return true;
}

bool ByteBandData::SetRowValue(unsigned int xFrom, unsigned int y, const INT16 * pFrom, unsigned int nCount)
{
    return SetRowValue (xFrom, y, reinterpret_cast<const INT8*>(pFrom), nCount);
}

bool ByteBandData::SetRowValue(unsigned int xFrom, unsigned int y, const INT32 * pFrom, unsigned int nCount)
{
    return SetRowValue (xFrom, y, reinterpret_cast<const INT8*>(pFrom), nCount);
}

bool ByteBandData::SetRowValue(unsigned int xFrom, unsigned int y, const INT64 * pFrom, unsigned int nCount)
{
    return SetRowValue (xFrom, y, reinterpret_cast<const INT8*>(pFrom), nCount);
}

//*************************************************************************************************************
bool   ByteBandData::SetAllToValue (const INT8*  pvalue)
{
    INT8& temp = m_pData[0];
    temp = *pvalue;
    memset (m_pData, m_pData[0], memsize());  //ok, set all the bytes to the same value.
    return true;
}

bool   ByteBandData::SetAllToValue (const INT16* pvalue)
{
    if (2 < m_size)
    {
        return false;
    }
    for (unsigned int x = 0; x < m_width; ++x)
    {
        SetValue (x, 0, pvalue);
    }
    CopyfirstRowToAll ();
    return true;
}

bool   ByteBandData::SetAllToValue (const INT32* pvalue)
{
    if (4 < m_size)
    {
        return false;
    }

    for (unsigned int x = 0; x < m_width; ++x)
    {
        SetValue (x, 0, pvalue);
    }
    CopyfirstRowToAll ();
    return true;

}

bool   ByteBandData::SetAllToValue (const INT64* pvalue)
{
    if (8 < m_size)
    {
        return false;
    }

    for (unsigned int x = 0; x < m_width; ++x)
    {
        SetValue (x, 0, pvalue);
    }
    CopyfirstRowToAll ();
    return true;
}

inline void ByteBandData::CopyfirstRowToAll ()
{
    for (unsigned int y = 1; y < m_height; ++y)
    {
        SetRowValue (y, m_pData);
    }
}

//*************************************************************************************************************
bool ByteBandData::DeepCopy(const BandData* pSource)
{
    const ByteBandData* pByteBandData = dynamic_cast<const ByteBandData*>(pSource);
    if (NULL == pByteBandData)
    {
        return false;
    }

    return BandData::DeepCopy(pSource);
}
