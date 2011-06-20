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

#ifndef BANDDATA_H_
#define BANDDATA_H_
#include "Foundation.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////////

///<summary>
/// Internal used classes.
/// Abstract clss for the different BandData.
///</summary>
class BandData
{
public:

    ///<summary>
    /// It is a abstract class, this method just called by his child classes.
    ///</summary>
                            BandData(unsigned char size, unsigned int width, unsigned int height);

    /// <summary>
    ///   Destroys an instance of this type class.
    /// </summary>
    virtual                 ~BandData();

public:
    /// <summary>
    ///   Get the size of per value in the band.
    ///   To the BitBandData the value is how many bits.
    ///   To the ByteBandData the value is how many bytes.
    /// </summary>
    unsigned char            GetSize() const
                                {return m_size;}

    /// <summary>
    ///   Get the width of the band.
    /// </summary>
    int                        GetWidth() const
                                {return m_width;}

    /// <summary>
    ///   Get the height of the band.
    /// </summary>
    int                     GetHeight() const
                                {return m_height;}

    /// <summary>
    ///   Get a raw pointer to the data.
    /// </summary>
    unsigned char* GetRawPointer() const
    {
        return (unsigned char*)(m_pData);
    }

    /// <summary>
    ///   Pure virtual method. Just get/set value at [x,y] as _int8.
    ///   Internal it just use memory copy to get and set value.
    /// </summary>
    virtual bool            GetValue(unsigned int x, unsigned int y, INT8& value) const   = 0;
    virtual bool            SetValue(unsigned int x, unsigned int y, const INT8* pvalue)  = 0;

    /// <summary>
    ///   Pure virtual method. Just get/set value at [x,y] as _int16.
    ///   Internal it just use memory copy to get and set value.
    /// </summary>
    virtual bool            GetValue(unsigned int x, unsigned int y, INT16& value) const  = 0;
    virtual bool            SetValue(unsigned int x, unsigned int y, const INT16* pvalue) = 0;

    /// <summary>
    ///   Pure virtual method. Just get/set value at [x,y] as _int32.
    ///   Internal it just use memory copy to get and set value.
    /// </summary>
    virtual bool            GetValue(unsigned int x, unsigned int y, INT32& value) const  = 0;
    virtual bool            SetValue(unsigned int x, unsigned int y, const INT32* pvalue) = 0;

    /// <summary>
    ///   Pure virtual method. Just get/set value at [x,y] as _int64.
    ///   Internal it just use memory copy to get and set value.
    /// </summary>
    virtual bool            GetValue(unsigned int x, unsigned int y, INT64& value) const  = 0;
    virtual bool            SetValue(unsigned int x, unsigned int y, const INT64* pvalue) = 0;

    /// <summary>
    ///   virtual method. Just set values at the row in rownum as INT8, INT16, INT32, _int64.
    ///   Internal it just use memory copy to set the value.
    /// </summary>
    virtual bool            SetRowValue(unsigned int rownum, const INT8  * pFrom) = 0;
    virtual bool            SetRowValue(unsigned int rownum, const INT16 * pFrom) = 0;
    virtual bool            SetRowValue(unsigned int rownum, const INT32 * pFrom) = 0;
    virtual bool            SetRowValue(unsigned int rownum, const INT64 * pFrom) = 0;

     /// <summary>
    ///   virtual method. Just set part of a row value at the row in y as INT8, INT16, INT32, _int64.
    ///   Internal it just use memory copy to set the value.
    /// </summary>
    virtual bool            SetRowValue(unsigned int xFrom, unsigned int y, const INT8  * pFrom, unsigned int nCount) = 0;
    virtual bool            SetRowValue(unsigned int xFrom, unsigned int y, const INT16 * pFrom, unsigned int nCount) = 0;
    virtual bool            SetRowValue(unsigned int xFrom, unsigned int y, const INT32 * pFrom, unsigned int nCount) = 0;
    virtual bool            SetRowValue(unsigned int xFrom, unsigned int y, const INT64 * pFrom, unsigned int nCount) = 0;

    /// <summary>
    ///  virtual method, set all of the data member to the same value.
    /// </summary>
    virtual bool            SetAllToValue (const INT8*  pvalue) = 0;
    virtual bool            SetAllToValue (const INT16* pvalue) = 0;
    virtual bool            SetAllToValue (const INT32* pvalue) = 0;
    virtual bool            SetAllToValue (const INT64* pvalue) = 0;

    /// <summary>
    ///   Copy a souce BandData to this BandData.
    /// </summary>
    virtual bool            DeepCopy(const BandData* pSource)                               = 0;

protected:
    /// <summary>
    ///   Check is x, y in the bounding box.
    /// </summary>
    inline bool CheckInBound(unsigned int& x, unsigned int& y) const
    {
        if (( x >= m_width ) || ( y >= m_height))
            return false;

        return true;
    }

    /// <summary>
    ///   It is a pure virtual method. Used to get the memory size that allocate for the band.
    /// </summary>
    virtual int             memsize()                                                       = 0;

protected:
    ///   The size of per value in the band.
    ///   To the BitBandData the value is how many bits.
    ///   To the ByteBandData the value is how many bytes.
    unsigned char            m_size;

    unsigned int            m_width;
    unsigned int            m_height;

    ///   Memory saved the band data.
    INT8*                    m_pData;
};

//////////////////////////////////////////////////////////////////////////


///<summary>
/// Internal used classes.
/// class used to support the type bool, bit2, bit4 for the GisBand
///
/// IMPORTANT NOTE:: size must less than 8;
///</summary>

class BitBandData: public BandData
{
public:
    ///<summary>
    /// Constructs an instance of this class.
    ///</summary>
                            BitBandData(unsigned char size, unsigned int width, unsigned int height);

    /// <summary>
    ///   Destroys an instance of this type class.
    /// </summary>
    virtual                 ~BitBandData();

    /// <summary>
    ///   Just get/set value at [x,y] as _int8.
    ///   Internal it just use something like memory copy to get and set value.
    /// </summary>
    virtual bool            GetValue(unsigned int x, unsigned int y, INT8& value) const;
    virtual bool            SetValue(unsigned int x, unsigned int y, const INT8* pvalue);

    /// <summary>
    ///   Just get/set value at [x,y] as _int16.
    ///   Internal it just use something like memory copy to get and set value.
    /// </summary>
    virtual bool            GetValue(unsigned int x, unsigned int y, INT16& value) const;
    virtual bool            SetValue(unsigned int x, unsigned int y, const INT16* pvalue);

    /// <summary>
    ///   Just get/set value at [x,y] as _int32.
    ///   Internal it just use something like memory copy to get and set value.
    /// </summary>
    virtual bool            GetValue(unsigned int x, unsigned int y, INT32& value) const;
    virtual bool            SetValue(unsigned int x, unsigned int y, const INT32* pvalue);

    /// <summary>
    ///   Just get/set value at [x,y] as _int64.
    ///   Internal it just use something like memory copy to get and set value.
    /// </summary>
    virtual bool            GetValue(unsigned int x, unsigned int y, INT64& value) const;
    virtual bool            SetValue(unsigned int x, unsigned int y, const INT64* pvalue);

    /// <summary>
    ///   Copy a souce BitBandData to this BandData.
    /// </summary>
    virtual bool            DeepCopy(const BandData* pSource);

    /// <summary>
    ///   virtual method. Just set values at the row in rownum as INT8, INT16, INT32, _int64.
    ///   Internal it just use memory copy to set the value.
    /// </summary>
    virtual bool            SetRowValue(unsigned int rownum, const INT8  * pFrom);
    virtual bool            SetRowValue(unsigned int rownum, const INT16 * pFrom);
    virtual bool            SetRowValue(unsigned int rownum, const INT32 * pFrom);
    virtual bool            SetRowValue(unsigned int rownum, const INT64 * pFrom);


    /// <summary>
    ///   virtual method. Just set part of a row value at the row in y as INT8, INT16, INT32, _int64.
    ///   Internal it just use memory copy to set the value.
    /// </summary>
    virtual bool            SetRowValue(unsigned int xFrom, unsigned int y, const INT8  * pFrom, unsigned int nCount);
    virtual bool            SetRowValue(unsigned int xFrom, unsigned int y, const INT16 * pFrom, unsigned int nCount);
    virtual bool            SetRowValue(unsigned int xFrom, unsigned int y, const INT32 * pFrom, unsigned int nCount);
    virtual bool            SetRowValue(unsigned int xFrom, unsigned int y, const INT64 * pFrom, unsigned int nCount);


    /// <summary>
    ///  virtual method, set all of the data member to the same value.
    /// </summary>
    virtual bool            SetAllToValue (const INT8*  pvalue);
    virtual bool            SetAllToValue (const INT16* pvalue);
    virtual bool            SetAllToValue (const INT32* pvalue);
    virtual bool            SetAllToValue (const INT64* pvalue);

protected:
    /// <summary>
    ///  Used to get the memory size that allocate for the band.
    /// </summary>
    virtual int             memsize();

private:
    /// <summary>
    /// How many items save in one Byte.
    /// For example, if the size is 1, we can save 8 items in one Byte.
    /// </summary>
    int                        m_numberPerByte;

    /// <summary>
    /// How bytes taken by one row,
    /// It is a helper member to make sure each row starts at the begin of one byte.
    /// </summary>
    unsigned int            m_bytesPerRow;

    /// <summary>
    /// Because an item value is save in 1 byte, we have get some bits from the byte.
    /// It is a helper member to get some bits from a byte.
    /// </summary>
    INT8                    m_mask;
};

//////////////////////////////////////////////////////////////////////////


///<summary>
/// Internal used classes.
/// class used to support the type that size is more than 1 byte.
///
/// IMPORTANT NOTE:: size must equal or more than 1.
///</summary>
class ByteBandData : public BandData
{
public:
    ///<summary>
    /// Constructs an instance of this class.
    ///</summary>
                            ByteBandData(unsigned char size, unsigned int width, unsigned int height);

    /// <summary>
    ///   Destroys an instance of this type class.
    /// </summary>
    virtual                 ~ByteBandData();

    /// <summary>
    ///   Just get/set value at [x,y] as _int8.
    ///   Internal it just use memory copy to get and set value.
    /// </summary>
    virtual bool            GetValue(unsigned int x, unsigned int y, INT8& value) const;
    virtual bool            SetValue(unsigned int x, unsigned int y, const INT8* pvalue);

    /// <summary>
    ///   Just get/set value at [x,y] as _int16.
    ///   Internal it just use memory copy to get and set value.
    /// </summary>
    virtual bool            GetValue(unsigned int x, unsigned int y, INT16& value) const;
    virtual bool            SetValue(unsigned int x, unsigned int y, const INT16* pvalue);

    /// <summary>
    ///   Just get/set value at [x,y] as _int32.
    ///   Internal it just use memory copy to get and set value.
    /// </summary>
    virtual bool            GetValue(unsigned int x, unsigned int y, INT32& value) const;
    virtual bool            SetValue(unsigned int x, unsigned int y, const INT32* pvalue);

    /// <summary>
    ///   Just get/set value at [x,y] as _int64.
    ///   Internal it just use memory copy to get and set value.
    /// </summary>
    virtual bool            GetValue(unsigned int x, unsigned int y, INT64& value) const;
    virtual bool            SetValue(unsigned int x, unsigned int y, const INT64* pvalue);

    /// <summary>
    ///   Copy a souce ByteBandData to this BandData.
    /// </summary>
    virtual bool            DeepCopy(const BandData* pSource);

    /// <summary>
    ///   virtual method. Just set values at the row in rownum as INT8, INT16, INT32, _int64.
    ///   Internal it just use memory copy to set the value.
    /// </summary>
    virtual bool            SetRowValue(unsigned int rownum, const INT8  * pFrom);
    virtual bool            SetRowValue(unsigned int rownum, const INT16 * pFrom);
    virtual bool            SetRowValue(unsigned int rownum, const INT32 * pFrom);
    virtual bool            SetRowValue(unsigned int rownum, const INT64 * pFrom);

    /// <summary>
    ///   virtual method. Just set part of a row value at the row in y as INT8, INT16, INT32, _int64.
    ///   Internal it just use memory copy to set the value.
    /// </summary>
    virtual bool            SetRowValue(unsigned int xFrom, unsigned int y, const INT8  * pFrom, unsigned int nCount);
    virtual bool            SetRowValue(unsigned int xFrom, unsigned int y, const INT16 * pFrom, unsigned int nCount);
    virtual bool            SetRowValue(unsigned int xFrom, unsigned int y, const INT32 * pFrom, unsigned int nCount);
    virtual bool            SetRowValue(unsigned int xFrom, unsigned int y, const INT64 * pFrom, unsigned int nCount);

    /// <summary>
    ///  virtual method, set all of the data member to the same value.
    /// </summary>
    virtual bool            SetAllToValue (const INT8*  pvalue);
    virtual bool            SetAllToValue (const INT16* pvalue);
    virtual bool            SetAllToValue (const INT32* pvalue);
    virtual bool            SetAllToValue (const INT64* pvalue);

    //faster, non-virtual accessors used by GisBand
#ifdef _WIN32
#define DECLARE_ACCESSOR_GET(name, type)    inline bool GetNative##name(unsigned int& x, unsigned int& y, ##type& val) \
                                            {                                                                          \
                                                assert(m_size == sizeof(##type));                                      \
                                                                                                                       \
                                                if (!CheckInBound(x,y))                                                \
                                                    return false;                                                      \
                                                                                                                       \
                                                int pos = (x + y * m_width);                                           \
                                                val = *(##type*)&m_pData[pos*sizeof(##type)];                          \
                                                return true;                                                           \
                                            }

#define DECLARE_ACCESSOR_SET(name, type)    inline bool SetNative##name(unsigned int& x, unsigned int& y, ##type& val) \
                                            {                                                                          \
                                                assert(m_size == sizeof(##type));                                      \
                                                                                                                       \
                                                if (!CheckInBound(x,y))                                                \
                                                    return false;                                                      \
                                                                                                                       \
                                                int pos = (x + y * m_width);                                           \
                                                *(##type*)&m_pData[pos*sizeof(##type)] = val;                          \
                                                return true;                                                           \
                                            }

#define DECLARE_ACCESSOR_GET_I(name, type)  inline type GetNative##name(unsigned int index)                            \
                                            {                                                                          \
                                                assert(m_size == sizeof(##type));                                      \
                                                return *(type*)&m_pData[index*sizeof(type)];                           \
                                            }

#define DECLARE_ACCESSOR_SET_I(name, type)  inline void SetNative##name(unsigned int index, type& val)                 \
                                            {                                                                          \
                                                assert(m_size == sizeof(type));                                        \
                                                *(type*)&m_pData[index*sizeof(type)] = val;                            \
                                            }

    DECLARE_ACCESSOR_GET(Double, double);
    DECLARE_ACCESSOR_SET(Double, double);
    DECLARE_ACCESSOR_GET(Float, float);
    DECLARE_ACCESSOR_SET(Float, float);
    DECLARE_ACCESSOR_GET(Int32, INT32);
    DECLARE_ACCESSOR_SET(Int32, INT32);

    DECLARE_ACCESSOR_GET_I(Double64, double);
    DECLARE_ACCESSOR_GET_I(Double32, float);
#else
    // Linux implementation here
#endif

protected:
    /// <summary>
    ///  Used to get the memory size that allocate for the band.
    /// </summary>
    virtual int             memsize();

    /// <summary>
    ///  helper function, to copy the first row to other rows.
    /// </summary>
    inline  void           CopyfirstRowToAll ();
};

#endif
