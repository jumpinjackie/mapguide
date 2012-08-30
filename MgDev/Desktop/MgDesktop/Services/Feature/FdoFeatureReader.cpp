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
#include <assert.h>
#include "Fdo.h"
#include "FdoCommon.h"
#include "FdoFeatureReader.h"

#include <limits>       // For quiet_NaN()
using namespace std;


// Class MgdFdoFeatureReader
MgdFdoFeatureReader::MgdFdoFeatureReader() : m_currentReaderId(-1)
{
    m_readerDepleted = false;
    m_readerCollection = MgdFdoReaderCollection::Create();
}

MgdFdoFeatureReader::MgdFdoFeatureReader(MgdFdoReaderCollection *readerCollection) : m_currentReaderId(-1)
{
    m_readerDepleted = false;
    m_readerCollection = FDO_SAFE_ADDREF(readerCollection);
}

MgdFdoFeatureReader::~MgdFdoFeatureReader()
{
    MG_TRY()

    Close();

    MG_CATCH_AND_RELEASE()
}

void MgdFdoFeatureReader::Dispose()
{
    delete this;
}

FdoClassDefinition *MgdFdoFeatureReader::GetClassDefinition()
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetClassDefinition();
}


int MgdFdoFeatureReader::GetDepth()
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetDepth();
}

FdoString* MgdFdoFeatureReader::GetPropertyName(FdoInt32 index)
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetPropertyName(index);
}

FdoInt32 MgdFdoFeatureReader::GetPropertyIndex(FdoString* propertyName)
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetPropertyIndex(propertyName);
}

FdoIFeatureReader* MgdFdoFeatureReader::GetFeatureObject( const wchar_t* propertyName )
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetFeatureObject(propertyName);
}

FdoIFeatureReader* MgdFdoFeatureReader::GetFeatureObject( FdoInt32 index )
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetFeatureObject(index);
}

bool MgdFdoFeatureReader::GetBoolean( const wchar_t *propertyName )
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetBoolean(propertyName);
}

bool MgdFdoFeatureReader::GetBoolean( FdoInt32 index )
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetBoolean(index);
}

FdoByte MgdFdoFeatureReader::GetByte( const wchar_t *propertyName )
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetByte(propertyName);
}

FdoByte MgdFdoFeatureReader::GetByte( FdoInt32 index )
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetByte(index);
}

FdoDateTime MgdFdoFeatureReader::GetDateTime( const wchar_t *propertyName )
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetDateTime(propertyName);
}

FdoDateTime MgdFdoFeatureReader::GetDateTime( FdoInt32 index )
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetDateTime(index);
}

double MgdFdoFeatureReader::GetDouble( const wchar_t *propertyName )
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetDouble(propertyName);
}

double MgdFdoFeatureReader::GetDouble( FdoInt32 index )
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetDouble(index);
}

const wchar_t* MgdFdoFeatureReader::GetString( const wchar_t *propertyName )
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetString(propertyName);

}

const wchar_t* MgdFdoFeatureReader::GetString( FdoInt32 index )
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetString(index);

}

short MgdFdoFeatureReader::GetInt16( const wchar_t *propertyName )
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetInt16(propertyName);
}

short MgdFdoFeatureReader::GetInt16( FdoInt32 index )
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetInt16(index);
}

int MgdFdoFeatureReader::GetInt32( const wchar_t *propertyName )
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetInt32(propertyName);
}

int MgdFdoFeatureReader::GetInt32( FdoInt32 index )
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetInt32(index);
}

FdoInt64 MgdFdoFeatureReader::GetInt64( const wchar_t *propertyName )
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetInt64(propertyName);
}

FdoInt64 MgdFdoFeatureReader::GetInt64( FdoInt32 index )
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetInt64(index);
}

float MgdFdoFeatureReader::GetSingle( const wchar_t *propertyName )
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetSingle(propertyName);
}

float MgdFdoFeatureReader::GetSingle( FdoInt32 index )
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetSingle(index);
}

///////////////////////////////////////////////////////////////////////////////
FdoIStreamReader* MgdFdoFeatureReader::GetLOBStreamReader(const wchar_t* propertyName)
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetLOBStreamReader(propertyName);
}

FdoIStreamReader* MgdFdoFeatureReader::GetLOBStreamReader(FdoInt32 index)
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetLOBStreamReader(index);
}
///////////////////////////////////////////////////////////////////////////////
FdoLOBValue* MgdFdoFeatureReader::GetLOB(const wchar_t* propertyName)
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetLOB(propertyName);
}

FdoLOBValue* MgdFdoFeatureReader::GetLOB(FdoInt32 index)
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetLOB(index);
}

bool MgdFdoFeatureReader::IsNull( const wchar_t *propertyName )
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->IsNull(propertyName);
}

bool MgdFdoFeatureReader::IsNull( FdoInt32 index )
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->IsNull(index);
}

FdoByteArray* MgdFdoFeatureReader::GetGeometry(const wchar_t* propertyName)
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetGeometry(propertyName);
}

FdoByteArray* MgdFdoFeatureReader::GetGeometry(FdoInt32 index)
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetGeometry(index);
}

const FdoByte * MgdFdoFeatureReader::GetGeometry(const wchar_t* propertyName, FdoInt32 * count)
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetGeometry(propertyName, count);
}

const FdoByte * MgdFdoFeatureReader::GetGeometry(FdoInt32 index, FdoInt32 * count)
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetGeometry(index, count);
}

/// <summary>Gets the raster object of the specified property.
/// Because no conversion is performed, the property must be
/// of Raster type; otherwise, an exception is thrown.</summary>
/// <param name="propertyName">Input the property name.</param>
/// <returns>Returns the raster object.</returns>
FdoIRaster* MgdFdoFeatureReader::GetRaster(FdoString* propertyName)
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetRaster(propertyName);
}

FdoIRaster* MgdFdoFeatureReader::GetRaster(FdoInt32 index)
{
    if (m_currentReader == NULL)
    {
        Initialize();
    }

    return m_currentReader->GetRaster(index);
}

bool MgdFdoFeatureReader::ReadNext( )
{
    if (m_readerDepleted)
        return false;

   if (m_currentReader == NULL)
       Initialize();

   if (m_currentReader->ReadNext())
       return true;

   m_currentReader = NULL;
   m_currentReaderId++;
   if ((m_readerCollection->GetCount() > m_currentReaderId))
       m_currentReader = static_cast<FdoIFeatureReader*>(m_readerCollection->GetItem(m_currentReaderId));

   if (m_currentReader != NULL)
       return this->ReadNext();

   m_readerDepleted = true;
   return false;
}

void MgdFdoFeatureReader::Close( )
{
    if (m_currentReader)
    {
        m_currentReader->Close();
    }
}

void MgdFdoFeatureReader::Add(FdoIFeatureReader *reader)
{
    m_readerCollection->Add(reader);
}

void MgdFdoFeatureReader::Initialize()
{
    if (m_currentReader == NULL)
    {
        if (m_readerCollection->GetCount())
        {
            m_currentReader = (FdoIFeatureReader*)m_readerCollection->GetItem(0);
            m_currentReaderId = 0;
        }
    }
}
