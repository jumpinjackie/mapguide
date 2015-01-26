//
//  Copyright (C) 2004-2014 by Autodesk, Inc.
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
#include "MapGuideCommon.h"
#include "TransformedGeometryFeatureReader.h"

MgTransformedGeometryFeatureReader::MgTransformedGeometryFeatureReader(MgFeatureReader* reader, MgCoordinateSystemTransform* transform)
{
    m_innerReader = SAFE_ADDREF(reader);
    m_transform = SAFE_ADDREF(transform);
    m_agfRw = new MgAgfReaderWriter();
}

MgTransformedGeometryFeatureReader::~MgTransformedGeometryFeatureReader()
{
    m_innerReader = NULL;
    m_transform = NULL;
    m_agfRw = NULL;
}

MgByteReader* MgTransformedGeometryFeatureReader::GetGeometry(INT32 index)
{
    Ptr<MgByteReader> ret;

    MG_FEATURE_SERVICE_TRY()

    Ptr<MgByteReader> agf = ((MgReader*)m_innerReader)->GetGeometry(index);
    Ptr<MgGeometry> geom = m_agfRw->Read(agf, m_transform);
    ret = m_agfRw->Write(geom);
    
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetGeometry")

    return ret.Detach();
}

MgByteReader* MgTransformedGeometryFeatureReader::GetGeometry(CREFSTRING name)
{
    Ptr<MgByteReader> ret;

    MG_FEATURE_SERVICE_TRY()

    Ptr<MgByteReader> agf = ((MgReader*)m_innerReader)->GetGeometry(name);
    Ptr<MgGeometry> geom = m_agfRw->Read(agf, m_transform);
    ret = m_agfRw->Write(geom);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetGeometry")

    return ret.Detach();
}

void MgTransformedGeometryFeatureReader::Close() 
{
    MG_FEATURE_SERVICE_TRY()
    m_innerReader->Close();
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.Close")
}

bool MgTransformedGeometryFeatureReader::ReadNext() 
{ 
    bool ret = false;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->ReadNext(); 
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.ReadNext")
    return ret;
}

bool MgTransformedGeometryFeatureReader::IsNull(CREFSTRING propertyName) 
{ 
    bool ret = false;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->IsNull(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.IsNull")
    return ret;
}

bool MgTransformedGeometryFeatureReader::GetBoolean(CREFSTRING propertyName) 
{ 
    bool ret = false;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetBoolean(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetBoolean")
    return ret;
}

BYTE MgTransformedGeometryFeatureReader::GetByte(CREFSTRING propertyName) 
{ 
    BYTE ret = 0;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetByte(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetByte")
    return ret;
}

MgDateTime* MgTransformedGeometryFeatureReader::GetDateTime(CREFSTRING propertyName) 
{ 
    Ptr<MgDateTime> ret;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetDateTime(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetDateTime")
    return ret.Detach();
}

float MgTransformedGeometryFeatureReader::GetSingle(CREFSTRING propertyName) 
{ 
    float ret = 0.0f;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetSingle(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetSingle")
    return ret;
}

double MgTransformedGeometryFeatureReader::GetDouble(CREFSTRING propertyName) 
{ 
    double ret = 0.0;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetDouble(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetDouble")
    return ret;
}

INT16 MgTransformedGeometryFeatureReader::GetInt16(CREFSTRING propertyName) 
{ 
    INT16 ret = 0;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetInt16(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetInt16")
    return ret;
}

INT32 MgTransformedGeometryFeatureReader::GetInt32(CREFSTRING propertyName) 
{ 
    INT32 ret = 0;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetInt32(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetInt32")
    return ret;
}

INT64 MgTransformedGeometryFeatureReader::GetInt64(CREFSTRING propertyName) 
{ 
    INT64 ret = 0;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetInt64(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetInt32")
    return ret;
}

STRING MgTransformedGeometryFeatureReader::GetString(CREFSTRING propertyName) 
{ 
    STRING ret;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetString(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetString")
    return ret;
}

MgByteReader* MgTransformedGeometryFeatureReader::GetBLOB(CREFSTRING propertyName) 
{ 
    Ptr<MgByteReader> ret;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetBLOB(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetBLOB")
    return ret.Detach();
}

MgByteReader* MgTransformedGeometryFeatureReader::GetCLOB(CREFSTRING propertyName)
{ 
    Ptr<MgByteReader> ret;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetCLOB(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetCLOB")
    return ret.Detach();
}

//This won't be used by RS_FeatureReader because stylization will use SelectFeatures()
//that will not return this type of reader
BYTE_ARRAY_OUT MgTransformedGeometryFeatureReader::GetGeometry(CREFSTRING propertyName, INT32& length) 
{ 
    throw new MgNotImplementedException(L"MgTransformedGeometryFeatureReader.GetGeometry", __LINE__, __WFILE__, NULL, L"", NULL);
}

//This won't be used by RS_FeatureReader because stylization will use SelectFeatures()
//that will not return this type of reader
BYTE_ARRAY_OUT MgTransformedGeometryFeatureReader::GetGeometry(INT32 index, INT32& length) 
{ 
    throw new MgNotImplementedException(L"MgTransformedGeometryFeatureReader.GetGeometry", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgRaster* MgTransformedGeometryFeatureReader::GetRaster(CREFSTRING propertyName) 
{ 
    Ptr<MgRaster> ret;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetRaster(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetRaster")
    return ret.Detach();
}

//MgByteReader* MgTransformedGeometryFeatureReader::GetRaster(STRING rasterPropName, INT32 xSize, INT32 ySize)
//{
//    return m_innerReader->GetRaster(rasterPropName, xSize, ySize);
//}

bool MgTransformedGeometryFeatureReader::IsNull(INT32 index) 
{ 
    bool ret = false;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->IsNull(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.IsNull")
    return ret;
}

bool MgTransformedGeometryFeatureReader::GetBoolean(INT32 index) 
{
    bool ret = false;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetBoolean(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetBoolean")
    return ret;
}

BYTE MgTransformedGeometryFeatureReader::GetByte(INT32 index) 
{ 
    BYTE ret = 0;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetByte(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetByte")
    return ret;
}

MgDateTime* MgTransformedGeometryFeatureReader::GetDateTime(INT32 index) 
{ 
    Ptr<MgDateTime> ret;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetDateTime(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetDateTime")
    return ret.Detach();
}

float MgTransformedGeometryFeatureReader::GetSingle(INT32 index) 
{ 
    float ret = 0.0f;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetSingle(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetSingle")
    return ret;
}

double MgTransformedGeometryFeatureReader::GetDouble(INT32 index) 
{ 
    double ret = 0.0;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetDouble(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetDouble")
    return ret;
}

INT16 MgTransformedGeometryFeatureReader::GetInt16(INT32 index) 
{ 
    INT16 ret = 0;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetInt16(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetInt16")
    return ret;
}

INT32 MgTransformedGeometryFeatureReader::GetInt32(INT32 index)
{ 
    INT32 ret = 0;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetInt32(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetInt32")
    return ret;
}

INT64 MgTransformedGeometryFeatureReader::GetInt64(INT32 index) 
{ 
    INT64 ret = 0;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetInt64(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetInt64")
    return ret;
}

STRING MgTransformedGeometryFeatureReader::GetString(INT32 index) 
{ 
    STRING ret;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetString(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetString")
    return ret;
}

MgByteReader* MgTransformedGeometryFeatureReader::GetBLOB(INT32 index) 
{ 
    Ptr<MgByteReader> ret;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetBLOB(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetBLOB")
    return ret.Detach();
}

MgByteReader* MgTransformedGeometryFeatureReader::GetCLOB(INT32 index) 
{ 
    Ptr<MgByteReader> ret;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetCLOB(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetCLOB")
    return ret.Detach();
}

MgRaster* MgTransformedGeometryFeatureReader::GetRaster(INT32 index) 
{ 
    Ptr<MgRaster> ret;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetRaster(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetRaster")
    return ret.Detach();
}

INT32 MgTransformedGeometryFeatureReader::GetReaderType() 
{ 
    return m_innerReader->GetReaderType();
}

//EXTERNAL_API:

MgByteReader* MgTransformedGeometryFeatureReader::ToXml() 
{ 
    Ptr<MgByteReader> ret;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->ToXml();
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.ToXml")
    return ret.Detach();
}

//INTERNAL_API:

string MgTransformedGeometryFeatureReader::GetResponseElementName()
{
    return m_innerReader->GetResponseElementName();
}

string MgTransformedGeometryFeatureReader::GetBodyElementName()
{
    return m_innerReader->GetBodyElementName();
}

void MgTransformedGeometryFeatureReader::ResponseStartUtf8(string& str)
{
    m_innerReader->ResponseStartUtf8(str);
}

void MgTransformedGeometryFeatureReader::ResponseEndUtf8(string& str)
{
    m_innerReader->ResponseEndUtf8(str);
}

void MgTransformedGeometryFeatureReader::BodyStartUtf8(string& str)
{
    m_innerReader->BodyStartUtf8(str);
}

void MgTransformedGeometryFeatureReader::BodyEndUtf8(string& str)
{
    m_innerReader->BodyEndUtf8(str);
}

void MgTransformedGeometryFeatureReader::CurrentToStringUtf8(string& str)
{
    m_innerReader->CurrentToStringUtf8(str);
}

void MgTransformedGeometryFeatureReader::HeaderToStringUtf8(string& str)
{
    m_innerReader->HeaderToStringUtf8(str);
}

const wchar_t* MgTransformedGeometryFeatureReader::GetString(CREFSTRING propertyName, INT32& length) 
{
    return m_innerReader->GetString(propertyName, length);
}

void MgTransformedGeometryFeatureReader::Serialize(MgStream* stream) 
{ 
    INT32 count = 1; // Get value from MgConfiguration
    bool operationCompleted = false;
    Ptr<MgFeatureSet> featureSet;
    STRING featureReader = L"";

    MG_FEATURE_SERVICE_TRY()

    // Find out the counts from Configuration
    MgConfiguration* config = MgConfiguration::GetInstance();

    config->GetIntValue(MgConfigProperties::FeatureServicePropertiesSection,
                        MgConfigProperties::FeatureServicePropertyDataCacheSize,
                        count,
                        MgConfigProperties::DefaultFeatureServicePropertyDataCacheSize);

    // Collect the feature reader into a pool for ReadNext operation
    MgServerFeatureReaderPool* featPool = MgServerFeatureReaderPool::GetInstance();
    CHECKNULL(featPool, L"MgTransformedGeometryFeatureReader.Serialize");

    featureReader = featPool->GetReaderId(this);
    if (L"" == featureReader)
    {
        // The feature reader is not in the pool
        featureReader = featPool->Add(this); // Add the reference
    }

    featureSet = GetFeatures(count);

    operationCompleted = true;

    MG_FEATURE_SERVICE_CATCH(L"MgTransformedGeometryFeatureReader.Serialize");

    // Mark operation is completed successfully
    stream->WriteBoolean(operationCompleted);

    if (operationCompleted && (mgException == 0))
    {
        stream->WriteString(featureReader); // Write the reader ID so we can retrieve it for later use
        stream->WriteObject((MgFeatureSet*)featureSet); // Write the feature set
    }
    else
    {
        stream->WriteObject((MgException*)mgException);
    }
}

void MgTransformedGeometryFeatureReader::Deserialize(MgStream* stream) 
{ 
    throw new MgInvalidOperationException(L"MgTransformedGeometryFeatureReader.Deserialize",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

//MgFeatureReader
//PUBLISHED_API:
MgClassDefinition* MgTransformedGeometryFeatureReader::GetClassDefinition() 
{ 
    Ptr<MgClassDefinition> ret;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetClassDefinition();
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetClassDefinition")
    return ret.Detach();
}

//This is internal, but MgdMappingUtil needs it. So we have to implement it
MgClassDefinition* MgTransformedGeometryFeatureReader::GetClassDefinitionNoXml() 
{ 
    Ptr<MgClassDefinition> ret;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetClassDefinitionNoXml();
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetClassDefinitionNoXml")
    return ret.Detach();
}

MgFeatureReader* MgTransformedGeometryFeatureReader::GetFeatureObject(CREFSTRING propertyName) 
{ 
    Ptr<MgFeatureReader> ret;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetFeatureObject(propertyName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetFeatureObject")
    return ret.Detach();
}

MgFeatureReader* MgTransformedGeometryFeatureReader::GetFeatureObject(INT32 index) 
{ 
    Ptr<MgFeatureReader> ret;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetFeatureObject(index);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetFeatureObject")
    return ret.Detach();
}

//INTERNAL_API:

MgFeatureSet* MgTransformedGeometryFeatureReader::GetFeatures(INT32 count) 
{ 
    Ptr<MgFeatureSet> ret;
    MG_FEATURE_SERVICE_TRY()
    ret = m_innerReader->GetFeatures(count);
    if (NULL != m_transform.p)
    {
        for (INT32 i = 0; i < ret->GetCount(); i++)
        {
            Ptr<MgPropertyCollection> props = ret->GetFeatureAt(i);
            for (INT32 j = 0; j < props->GetCount(); j++)
            {
                Ptr<MgProperty> prop = props->GetItem(j);
                if (prop->GetPropertyType() == MgPropertyType::Geometry)
                {
                    MgGeometryProperty* geomProp = static_cast<MgGeometryProperty*>(prop.p);
                    if (!geomProp->IsNull())
                    {
                        Ptr<MgByteReader> agf = geomProp->GetValue();
                        Ptr<MgGeometry> geom = m_agfRw->Read(agf, m_transform);
                        Ptr<MgByteReader> txAgf = m_agfRw->Write(geom);
                        geomProp->SetValue(txAgf);
                    }
                }
            }
        }
    }
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgTransformedGeometryFeatureReader.GetFeatures")
    return ret.Detach();
}