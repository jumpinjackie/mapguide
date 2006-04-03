//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "ServerFeatureServiceDefs.h"
#include "ServerFeatureUtil.h"
#include "ServerDataProcessor.h"
#include "ServerGetFeatures.h"
#include "ServerFeatureProcessor.h"
#include "ServerDataReaderPool.h"
#include "ServerFeatureReaderIdentifierPool.h"
#include "ByteSourceRasterStreamImpl.h"
#include "ObjectPropertyType.h"
#include "OrderingOption.h"

static std::map<FdoPropertyType, INT32>                 s_FdoPropertyType;
static std::map<FdoDataType, INT32>                     s_FeatureUtilFdoDataType;
static std::map<INT32, FdoOrderingOption>               s_FdoOrderingOption;
static std::map<INT32, STRING>                          s_CustomFunctions;
static std::map<INT32, FdoSpatialOperations> s_FdoSpatialOperation;

bool MgServerFeatureUtil::m_isInitialized = MgServerFeatureUtil::Initialize();

INT32 MgServerFeatureUtil::GetMgPropertyType(FdoDataType fdoDataType)
{
    return s_FeatureUtilFdoDataType[fdoDataType];
}

bool MgServerFeatureUtil::FindCustomFunction(CREFSTRING funcName, INT32 &index)
{
    int cnt = (int)s_CustomFunctions.size();
    bool supports = false;
    index = -1;

    // No function name is no custom function
    if (!funcName.empty())
    {
        // look for custom function name
        for (int i=0; i < cnt; i++)
        {
            index++;
            int cmp = _wcsicmp(s_CustomFunctions[i].c_str(), funcName.c_str());
            if (cmp == 0)
            {
                supports = true;
                break;
            }
        }
    }
    return supports;
}

FdoOrderingOption MgServerFeatureUtil::GetFdoOrderingOption(INT32 orderingOption)
{
    MgOrderingOption::ValidateRange(orderingOption);

    return s_FdoOrderingOption[orderingOption];
}

bool MgServerFeatureUtil::Initialize()
{
    s_FeatureUtilFdoDataType[FdoDataType_Boolean]  = MgPropertyType::Boolean;
    s_FeatureUtilFdoDataType[FdoDataType_Byte]     = MgPropertyType::Byte;
    s_FeatureUtilFdoDataType[FdoDataType_DateTime] = MgPropertyType::DateTime;
    s_FeatureUtilFdoDataType[FdoDataType_Double]   = MgPropertyType::Double;
    s_FeatureUtilFdoDataType[FdoDataType_Int16]    = MgPropertyType::Int16;
    s_FeatureUtilFdoDataType[FdoDataType_Int32]    = MgPropertyType::Int32;
    s_FeatureUtilFdoDataType[FdoDataType_Int64]    = MgPropertyType::Int64;
    s_FeatureUtilFdoDataType[FdoDataType_Single]   = MgPropertyType::Single;
    s_FeatureUtilFdoDataType[FdoDataType_String]   = MgPropertyType::String;
    s_FeatureUtilFdoDataType[FdoDataType_BLOB]     = MgPropertyType::Blob;
    s_FeatureUtilFdoDataType[FdoDataType_CLOB]     = MgPropertyType::Clob;

    s_FeatureUtilFdoDataType[FdoDataType_Decimal]  = MgPropertyType::Null; // We do not support decimal

    s_FdoOrderingOption[MgOrderingOption::Ascending]  = FdoOrderingOption_Ascending;
    s_FdoOrderingOption[MgOrderingOption::Descending] = FdoOrderingOption_Descending;

    s_CustomFunctions[EQUAL_CATEGORY]       = L"EQUAL_DIST";
    s_CustomFunctions[STDEV_CATEGORY]       = L"STDEV_DIST";
    s_CustomFunctions[QUANTILE_CATEGORY]    = L"QUANT_DIST";
    s_CustomFunctions[JENK_CATEGORY]        = L"JENK_DIST";
    s_CustomFunctions[MINIMUM]              = L"MINIMUM";
    s_CustomFunctions[MAXIMUM]              = L"MAXIMUM";
    s_CustomFunctions[MEAN]                 = L"MEAN";
    s_CustomFunctions[STANDARD_DEV]         = L"STD_DEV";
    s_CustomFunctions[UNIQUE]               = L"UNIQUE";
    s_CustomFunctions[EXTENT]               = L"EXTENT";


    s_FdoSpatialOperation[MgFeatureSpatialOperations::Contains]            =   FdoSpatialOperations_Contains;
    s_FdoSpatialOperation[MgFeatureSpatialOperations::Crosses]             =   FdoSpatialOperations_Crosses;
    s_FdoSpatialOperation[MgFeatureSpatialOperations::Disjoint]            =   FdoSpatialOperations_Disjoint;
    s_FdoSpatialOperation[MgFeatureSpatialOperations::Equals]              =   FdoSpatialOperations_Equals;
    s_FdoSpatialOperation[MgFeatureSpatialOperations::Intersects]          =   FdoSpatialOperations_Intersects;
    s_FdoSpatialOperation[MgFeatureSpatialOperations::Overlaps]            =   FdoSpatialOperations_Overlaps;
    s_FdoSpatialOperation[MgFeatureSpatialOperations::Touches]             =   FdoSpatialOperations_Touches;
    s_FdoSpatialOperation[MgFeatureSpatialOperations::Within]              =   FdoSpatialOperations_Within;
    s_FdoSpatialOperation[MgFeatureSpatialOperations::CoveredBy]           =   FdoSpatialOperations_CoveredBy;
    s_FdoSpatialOperation[MgFeatureSpatialOperations::Inside]              =   FdoSpatialOperations_Inside;
    s_FdoSpatialOperation[MgFeatureSpatialOperations::EnvelopeIntersects]  =   FdoSpatialOperations_EnvelopeIntersects;

    s_FdoPropertyType[FdoPropertyType_RasterProperty]       = MgFeaturePropertyType::RasterProperty;
    s_FdoPropertyType[FdoPropertyType_DataProperty]         = MgFeaturePropertyType::DataProperty;
    s_FdoPropertyType[FdoPropertyType_GeometricProperty]    = MgFeaturePropertyType::GeometricProperty;
    s_FdoPropertyType[FdoPropertyType_ObjectProperty]       = MgFeaturePropertyType::ObjectProperty;
    s_FdoPropertyType[FdoPropertyType_AssociationProperty]  = MgFeaturePropertyType::AssociationProperty;

    return true;
}

STRING MgServerFeatureUtil::GetMessage(CREFSTRING messageId)
{
    STRING msg = MgUtil::GetResourceMessage(MgResources::FeatureService, messageId);
    return msg;
}

INT16 MgServerFeatureUtil::GetMgPropertyType(MgPropertyDefinition* propDef)
{
    CHECKNULL((MgPropertyDefinition*)propDef, L"MgServerGetFeatures.GetMgPropertyType")

    INT32 mgPropType = 0;

    // Whether it is data,geometry,raster,object or association property
    INT16 type = propDef->GetPropertyType();
    switch(type)
    {
        case MgFeaturePropertyType::DataProperty:
        {
            mgPropType = ((MgDataPropertyDefinition*)propDef)->GetDataType();
            break;
        }
        case MgFeaturePropertyType::GeometricProperty:
        {
            mgPropType = MgPropertyType::Geometry;
            break;
        }
        case MgFeaturePropertyType::RasterProperty:
        {
            mgPropType = MgPropertyType::Raster;
            break;
        }
        case MgFeaturePropertyType::ObjectProperty:
        {
            // Object properties will return FeatureReader
            mgPropType = MgPropertyType::Feature;
            break;
        }
        case MgFeaturePropertyType::AssociationProperty:
        {
            // Association properties will return FeatureReader
            mgPropType = MgPropertyType::Feature;
            break;
        }
    }

    return ((INT16)mgPropType);
}


MgPropertyDefinitionCollection* MgServerFeatureUtil::GetPropertyDefinitions(MgReader* reader)
{
    CHECKNULL(reader, L"MgServerFeatureUtil.GetPropertyDefinitions");

    // Create a new collection
    Ptr<MgPropertyDefinitionCollection> propDefCol = new MgPropertyDefinitionCollection();

    // Collect all property names and types
    INT32 cnt = reader->GetPropertyCount();
    for (INT32 i = 0; i < cnt; i++)
    {
        STRING propName = reader->GetPropertyName(i);
        INT32 propType = reader->GetPropertyType(propName);
        Ptr<MgPropertyDefinition> propDef = new MgPropertyDefinition(propName, propType);
        propDefCol->Add(propDef);
    }

    return SAFE_ADDREF((MgPropertyDefinitionCollection*)propDefCol);
}


void MgServerFeatureUtil::GetArguments(FdoFunction* customFunction, REFSTRING name, int &numCats, double& dataMin, double& dataMax, INT16 type)
{
    CHECKNULL(customFunction, L"MgFeatureDistribution.GetArguments");

    dataMin = -DoubleMaxValue;
    dataMax = DoubleMaxValue;
    numCats = 0;

    GisPtr<FdoExpressionCollection> exprCol = customFunction->GetArguments();
    GisInt32 cnt = exprCol->GetCount();

    if (cnt < 2)
    {
        STRING message = MgServerFeatureUtil::GetMessage(L"MgInvalidComputedProperty");

        MgStringCollection arguments;
        arguments.Add(customFunction->GetName());
        throw new MgFeatureServiceException(L"MgServerFeatureUtil.GetArguments", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    GisString *arg0 = NULL, *arg1 = NULL, *arg2 = NULL, *arg3 = NULL;

    // TODO: Change the logic below to access function arguments through an iterator

    // We skip first argument as it is name of property
    GisInt32 argCnt = 0;
    GisPtr<FdoExpression> expr0 = exprCol->GetItem(argCnt);
    CHECKNULL((FdoExpression*)expr0, L"MgServerFeatureUtil.GetArguments");
    arg0 = expr0->ToString(); // Name of function

    GisPtr<FdoExpression> expr1 = exprCol->GetItem(argCnt+1);
    CHECKNULL((FdoExpression*)expr1, L"MgServerFeatureUtil.GetArguments");
    arg1 = expr1->ToString(); // No of categories

    if (cnt > 2)
    {
        GisPtr<FdoExpression> expr2 = exprCol->GetItem(argCnt+2);
        CHECKNULL((FdoExpression*)expr2, L"MgServerFeatureUtil.GetArguments");
        arg2 = expr2->ToString(); // Min Range

        if (cnt > 3)
        {
            GisPtr<FdoExpression> expr3 = exprCol->GetItem(argCnt+3);
            CHECKNULL((FdoExpression*)expr3, L"MgServerFeatureUtil.GetArguments");
            arg3 = expr3->ToString(); // Max Range
        }
    }

    // TODO: Handle Math Errors or we should probably check whether
    // these arguments are really a number or not.
    try
    {
        wchar_t* stoppedAt = NULL;

        if (arg0 != NULL)
            name = STRING(arg0);

        if (arg1 != NULL)
            numCats = (int)wcstoul(arg1,&stoppedAt,10);

        if (arg2 != NULL)
        {
            if (type == MgPropertyType::DateTime)
            {
                Ptr<MgDateTime> dateTime = new MgDateTime(STRING(arg2));
                dataMin = dateTime->ToSeconds();
            }
            else
            {
                dataMin = wcstod(arg2,&stoppedAt);
            }
        }

        if (arg3 != NULL)
        {
            if (type == MgPropertyType::DateTime)
            {
                Ptr<MgDateTime> dateTime = new MgDateTime(STRING(arg3));
                dataMax = dateTime->ToSeconds();
            }
            else
            {
                dataMax = wcstod(arg3,&stoppedAt);
            }
        }
    }
    catch(...)
    {
        /* TODO: Should we wrap these exceptions into MgLength, MgDomain exceptions */
    }
}

INT32 MgServerFeatureUtil::GetPropertyDefinition(MgReader* reader, REFSTRING propName)
{
    CHECKNULL((MgReader*)reader, L"MgServerFeatureUtil.GetPropertyDefinition");

    INT32 cnt = reader->GetPropertyCount();
    if (cnt != 1)
    {
        STRING message = MgServerFeatureUtil::GetMessage(L"MgOnlyOnePropertyAllowed");

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgFeatureServiceException(L"MgServerFeatureUtil.GetPropertyDefinition", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // Get the name of property
    propName = reader->GetPropertyName(cnt-1);
    // Get the data type of property
    INT32 type = reader->GetPropertyType(propName);

    return type;
}

bool MgServerFeatureUtil::FindCustomFunction(FdoFunction* customFunction, INT32 &index)
{
    bool supported = false;
    STRING funcName;

    if (customFunction != NULL)
    {
        GisString* func = customFunction->GetName();
        if (func != NULL)
        {
            funcName = STRING(func);
        }
    }
    supported = FindCustomFunction(funcName, index);

    return supported;
}


void MgServerFeatureUtil::ValidateCustomConstraints(FdoFunction* customFunction)
{
    if (customFunction == NULL) { return; } // Nothing to do here

    // Custom (Distribution) methods should have atleast two properties. Name of property and Number of categories
    GisPtr<FdoExpressionCollection> exprCol = customFunction->GetArguments();
    GisInt32 cnt = exprCol->GetCount();

    INT32 funcCode;
    bool supported = FindCustomFunction(STRING(customFunction->GetName()), funcCode);

    if (supported)
    {
        switch(funcCode)
        {
            case EQUAL_CATEGORY: // Equal Category
            case STDEV_CATEGORY: // StdDev Category
            case QUANTILE_CATEGORY: // Quantile Category
            case JENK_CATEGORY: // Jenk Category
            {
                if (cnt < 2)
                {
                    STRING message = GetMessage(L"MgInvalidComputedProperty");

                    MgStringCollection arguments;
                    arguments.Add(message);
                    throw new MgFeatureServiceException(L"MgServerFeatureUtil.ValidateCustomConstraints",
                        __LINE__, __WFILE__, &arguments, L"", NULL);
                }
                break;
            }
            case MINIMUM:
            case MAXIMUM:
            case MEAN:
            case STANDARD_DEV:
            case UNIQUE:
            case EXTENT:
            {
                if (cnt > 1)
                {
                    STRING message = GetMessage(L"MgInvalidComputedProperty");

                    MgStringCollection arguments;
                    arguments.Add(message);
                    throw new MgFeatureServiceException(L"MgServerFeatureUtil.ValidateCustomConstraints",
                        __LINE__, __WFILE__, &arguments, L"", NULL);
                }
                break;
            }
        }
    }
}

double MgServerFeatureUtil::Minimum(VECTOR& v)
{
    int cnt = (int)v.size();
    double min = DoubleMaxValue;
    for (int i=0; i < cnt; i++)
    {
        double val = v[i];
        if (val < min)
        {
            min = val;
        }
    }
    return min;
}

double MgServerFeatureUtil::Maximum(VECTOR& v)
{
    int cnt = (int)v.size();
    double max = -DoubleMaxValue;
    for (int i=0; i < cnt; i++)
    {
        double val = v[i];
        if (val > max)
            max = val;
    }
    return max;
}

FdoSpatialOperations MgServerFeatureUtil::GetFdoSpatialOperation(INT32 spatialOp)
{
    MgFeatureSpatialOperations::ValidateRange(spatialOp);

    return s_FdoSpatialOperation[spatialOp];
}


INT32 MgServerFeatureUtil::GetMgFeaturePropertyType(FdoPropertyType fdoPropType)
{
    return s_FdoPropertyType[fdoPropType];
}


MgRaster* MgServerFeatureUtil::GetMgRaster(FdoIRaster* raster, STRING propName)
{
    Ptr<MgRaster> retVal = (MgRaster*)NULL;

    MG_FEATURE_SERVICE_TRY()

    if (raster != NULL)
    {
        retVal = new MgRaster();
        bool isNull = raster->IsNull();
        if (isNull)
        {
            retVal->SetNull();
        }
        else
        {
            INT32 xSize = raster->GetImageXSize();
            INT32 ySize = raster->GetImageYSize();

            GisPtr<GisByteArray> agfBounds = (GisByteArray*) raster->GetBounds();
            Ptr<MgByteSource> byteSource = new MgByteSource(agfBounds->GetData(), agfBounds->GetCount());
            Ptr<MgByteReader> byteReader = byteSource->GetReader();
            MgAgfReaderWriter agfReader;
            Ptr<MgGeometry> geom = agfReader.Read(byteReader);

            Ptr<MgEnvelope> mgEnvelope = geom->Envelope();

            retVal->SetImageXSize(xSize);
            retVal->SetImageYSize(ySize);
            retVal->SetBounds(mgEnvelope);

            GisPtr<FdoRasterDataModel> dm = raster->GetDataModel();

            retVal->SetBitsPerPixel(dm->GetBitsPerPixel());
            retVal->SetDataModelType(dm->GetDataModelType());

            //if image is color mapped, get the palette
            if (dm->GetBitsPerPixel() == 8)
            {
                GisPtr<FdoIRasterPropertyDictionary> props = raster->GetAuxiliaryProperties();

                //try to get the palette -- if there isn't one,
                //an exception will be thrown. It's simpler to catch
                //it than check if such a property exists.
                try
                {
                    GisPtr<FdoDataValue> dv = props->GetProperty(L"Palette");
                    if (dv.p)
                    {
                        FdoBLOBValue* blob = (FdoBLOBValue*)(dv.p);
                        GisPtr<GisByteArray> ba = blob->GetData();

                        Ptr<MgByte> pal = new MgByte(ba->GetData(), ba->GetCount());
                        retVal->SetPalette(pal);
                    }
                }
                catch (GisException* e)
                {
                    e->Release();
                    retVal->SetPalette(NULL);
                }
            }
        }
        retVal->SetPropertyName(propName);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureUtil.GetRaster");

    return SAFE_ADDREF((MgRaster*)retVal);
}


MgByteReader* MgServerFeatureUtil::GetRaster(FdoIReader* reader, CREFSTRING rasterPropName,INT32 xSize, INT32 ySize)
{
    // Null Reader is invalid
    CHECKNULL(reader, L"MgServerFeatureUtil.GetRaster");

    // Empty property name is invalid
    if (rasterPropName.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgServerFeatureUtil.GetRaster",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    Ptr<MgByteReader> byteReader = (MgByteReader*)NULL;

    GisPtr<FdoIRaster> fdoRaster = reader->GetRaster(rasterPropName.c_str());
    CHECKNULL((FdoIRaster*)fdoRaster, L"MgServerFeatureUtil.GetRaster");

    // TODO: Should we support RasterModel properties and Tiling information ???
    fdoRaster->SetImageXSize(xSize);
    fdoRaster->SetImageYSize(ySize);

    // Get the stream reader
    GisPtr<GisIStreamReader> streamReader = fdoRaster->GetStreamReader();

    // TODO: FDO Defect - streamReader->GetType() is private;
    GisStreamReaderType type = GisStreamReaderType_Byte;

    if (type == GisStreamReaderType_Byte)
    {
        GisPtr<GisIStreamReaderTmpl<GisByte> > byteStreamReader =
            GIS_SAFE_ADDREF(dynamic_cast<GisIStreamReaderTmpl<GisByte>*>((GisIStreamReader*)streamReader));
        CHECKNULL((GisIStreamReaderTmpl<GisByte>*)byteStreamReader, L"MgServerFeatureUtil.GetRaster");

        ByteSourceRasterStreamImpl* rasterStream = new ByteSourceRasterStreamImpl(byteStreamReader);
        Ptr<MgByteSource> byteSource = new MgByteSource(rasterStream);
        byteSource->SetMimeType(MgMimeType::Binary);
        byteReader = byteSource->GetReader();
    }

    return SAFE_ADDREF((MgByteReader*)byteReader);
}


bool MgServerFeatureUtil::ValidatePoolEntry(INT32 readerId)
{
    bool isValid = false;

    MgServerFeatureProcessor* featureProcessor = ((MgServerFeatureProcessor*)(INT64)readerId);
    if (featureProcessor != NULL)
    {
        MgServerFeatureProcessorType fpType = featureProcessor->GetProcessorType();
        switch(fpType)
        {
            case msfptDataProcessor:
            {
                MgServerDataProcessor* dataProcessor = dynamic_cast<MgServerDataProcessor*>((MgServerFeatureProcessor*)featureProcessor);
                MgServerDataReaderPool* drPool = MgServerDataReaderPool::GetInstance();
                CHECKNULL(drPool, L"MgServerFeatureService.ValidatePoolEntry");
                if (drPool->Contains(dataProcessor))
                {
                    isValid = true;
                }
                break;
            }
            case msfptFeatureProcessor:
            {
                MgServerGetFeatures* getFeatureProcessor = dynamic_cast<MgServerGetFeatures*>((MgServerFeatureProcessor*)featureProcessor);
                MgServerFeatureReaderIdentifierPool* featPool = MgServerFeatureReaderIdentifierPool::GetInstance();
                CHECKNULL(featPool, L"MgServerFeatureService.ValidatePoolEntry");
                if (featPool->Contains(getFeatureProcessor))
                {
                    isValid = true;
                }
                break;
            }
        }
    }
    return isValid;
}

void MgServerFeatureUtil::FillFdoPropertyCollection(MgPropertyCollection* srcCol, FdoPropertyValueCollection* paramCol)
{
    CHECKNULL(srcCol, L"MgServerFeatureUtil.FillFdoPropertyCollection")
    CHECKNULL(paramCol, L"MgServerFeatureUtil.FillFdoPropertyCollection")

    INT32 cnt = srcCol->GetCount();
    for (INT32 i = 0; i < cnt; i++)
    {
        Ptr<MgProperty> prop = srcCol->GetItem(i);
        GisPtr<FdoPropertyValue> fdoProp = MgPropertyToFdoProperty(prop);
        paramCol->Add(fdoProp);
    }
}

FdoParameterValueCollection* MgServerFeatureUtil::CreateFdoParameterCollection(MgPropertyCollection* srcCol)
{
    CHECKNULL(srcCol, L"MgServerFeatureUtil.FillFdoPropertyCollection")
    GisPtr<FdoParameterValueCollection> paramCol = FdoParameterValueCollection::Create();

    INT32 cnt = srcCol->GetCount();
    for (INT32 i = 0; i < cnt; i++)
    {
        Ptr<MgProperty> prop = srcCol->GetItem(i);
        GisPtr<FdoParameterValue> fdoProp = MgPropertyToFdoParameter(prop);
        paramCol->Add(fdoProp);
    }

    return GIS_SAFE_ADDREF((FdoParameterValueCollection*)paramCol);
}

FdoLiteralValue* MgServerFeatureUtil::MgPropertyToFdoDataValue(MgProperty* srcProp)
{
    CHECKNULL(srcProp, L"MgServerFeatureUtil.MgPropertyToFdoProperty")

    INT16 propType = srcProp->GetPropertyType();
    GisPtr<FdoLiteralValue> fdoVal = (FdoLiteralValue*)NULL;

    switch(propType)
    {
        case MgPropertyType::Blob:
        {
            Ptr<MgByteReader> value = ((MgBlobProperty*)srcProp)->GetValue();

            MgByteSink sink(value);
            Ptr<MgByte> byte = sink.ToBuffer();
            INT32 length = byte->GetLength();
            BYTE_ARRAY_OUT bytes = byte->Bytes();
            GisPtr<GisByteArray> array = GisByteArray::Create((GisByte*)bytes, length);

            fdoVal = FdoBLOBValue::Create(array);
            break;
        }
        case MgPropertyType::Boolean:
        {
            bool value = ((MgBooleanProperty*)srcProp)->GetValue();

            fdoVal = FdoBooleanValue::Create(value);
            break;
        }
        case MgPropertyType::Byte:
        {
            BYTE value = ((MgByteProperty*)srcProp)->GetValue();

            fdoVal = FdoByteValue::Create((GisByte)value);
            break;
        }
        case MgPropertyType::Clob:
        {
            Ptr<MgByteReader> value = ((MgClobProperty*)srcProp)->GetValue();

            MgByteSink sink(value);
            Ptr<MgByte> byte = sink.ToBuffer();
            INT32 length = byte->GetLength();
            BYTE_ARRAY_OUT bytes = byte->Bytes();
            GisPtr<GisByteArray> array = GisByteArray::Create((GisByte*)bytes, length);

            fdoVal = FdoCLOBValue::Create(array);
            break;
        }
        case MgPropertyType::DateTime:
        {
            Ptr<MgDateTime> value = ((MgDateTimeProperty*)srcProp)->GetValue();

            GisDateTime time;
            time.day = (GisInt8)value->GetDay();
            time.hour = (GisInt8)value->GetHour();
            time.minute = (GisInt8)value->GetMinute();
            time.month = (GisInt8)value->GetMonth();
            time.seconds = (float)(value->GetSecond() + (value->GetMicrosecond() / 1000000.0));
            time.year = (GisInt16)value->GetYear();

            fdoVal = FdoDateTimeValue::Create(time);
            break;
        }
        case MgPropertyType::Double:
        {
            double value = ((MgDoubleProperty*)srcProp)->GetValue();
            fdoVal = FdoDoubleValue::Create(value);
            break;
        }
        case MgPropertyType::Geometry:
        {
            Ptr<MgByteReader> value = ((MgGeometryProperty*)srcProp)->GetValue();

            MgByteSink sink(value);
            Ptr<MgByte> byte = sink.ToBuffer();
            INT32 length = byte->GetLength();
            BYTE_ARRAY_OUT bytes = byte->Bytes();

            GisPtr<GisByteArray> array = GisByteArray::Create((GisByte*)bytes, length);
            fdoVal = FdoGeometryValue::Create(array);
            break;
        }
        case MgPropertyType::Int16:
        {
            INT16 value = ((MgInt16Property*)srcProp)->GetValue();
            fdoVal = FdoInt16Value::Create((GisInt16)value);
            break;
        }
        case MgPropertyType::Int32:
        {
            INT32 value = ((MgInt32Property*)srcProp)->GetValue();
            fdoVal = FdoInt32Value::Create((GisInt32)value);
            break;
        }
        case MgPropertyType::Int64:
        {
            INT64 value = ((MgInt64Property*)srcProp)->GetValue();
            fdoVal = FdoInt64Value::Create((GisInt64)value);
            break;
        }
        case MgPropertyType::Single:
        {
            float value = ((MgSingleProperty*)srcProp)->GetValue();
            fdoVal = FdoSingleValue::Create(value);
            break;
        }
        case MgPropertyType::String:
        {
            STRING value = ((MgStringProperty*)srcProp)->GetValue();
            fdoVal = FdoStringValue::Create(value.c_str());
            break;
        }
        case MgPropertyType::Feature:
        {
            throw new MgInvalidPropertyTypeException(L"MgServerFeatureUtil.MgPropertyToFdoProperty",
                __LINE__, __WFILE__, NULL, L"", NULL);
            break;
        }
        case MgPropertyType::Raster:
        {
            throw new MgInvalidPropertyTypeException(L"MgServerFeatureUtil.MgPropertyToFdoProperty",
                __LINE__, __WFILE__, NULL, L"", NULL);
            break;
        }
    }

    return GIS_SAFE_ADDREF((FdoLiteralValue*)fdoVal);
}

FdoParameterValue* MgServerFeatureUtil::MgPropertyToFdoParameter(MgProperty* srcProp)
{
    GisPtr<FdoLiteralValue> fdoValue = MgPropertyToFdoDataValue(srcProp);
    STRING str = srcProp->GetName();

    assert(!str.empty());

    GisPtr<FdoParameterValue> fdoParam = FdoParameterValue::Create(str.c_str(), fdoValue);

    return GIS_SAFE_ADDREF((FdoParameterValue*) fdoParam);
}

FdoPropertyValue* MgServerFeatureUtil::MgPropertyToFdoProperty(MgProperty* srcProp)
{
    GisPtr<FdoLiteralValue> fdoValue = MgPropertyToFdoDataValue(srcProp);
    STRING str = srcProp->GetName();

    assert(!str.empty());

    GisPtr<FdoPropertyValue> fdoProp = FdoPropertyValue::Create(str.c_str(), fdoValue);

    return GIS_SAFE_ADDREF((FdoPropertyValue*) fdoProp);
}

INT32 MgServerFeatureUtil::FdoOrderTypeToMgOrderingOption(FdoOrderType type)
{
    INT32 orderOption = MgOrderingOption::Ascending;
    switch(type)
    {
        case FdoOrderType_Ascending:
        {
            // It is already set as a default value
            break;
        }
        case FdoOrderType_Descending:
        {
            orderOption = MgOrderingOption::Descending;
            break;
        }
    }
    return orderOption;
}

FdoOrderType MgServerFeatureUtil::MgOrderingOptionToFdoOrderType(INT32 type)
{
    MgOrderingOption::ValidateRange(type);
    FdoOrderType orderType = FdoOrderType_Ascending;

    switch(type)
    {
        case MgOrderingOption::Ascending:
        {
            // It is already set as a default value
            break;
        }
        case MgOrderingOption::Descending:
        {
            orderType = FdoOrderType_Descending;
            break;
        }
    }
    return orderType;
}

INT32 MgServerFeatureUtil::FdoObjectTypeToMgObjectPropertyType(FdoObjectType type)
{
    INT32 objectType = MgObjectPropertyType::Value;
    switch(type)
    {
        case FdoObjectType_Value:
        {
            // It is already set as a default value
            break;
        }
        case FdoObjectType_Collection:
        {
            objectType = MgObjectPropertyType::Collection;
            break;
        }
        case FdoObjectType_OrderedCollection:
        {
            objectType = MgObjectPropertyType::OrderedCollection;
            break;
        }
    }
    return objectType;
}

FdoObjectType MgServerFeatureUtil::MgObjectPropertyTypeToFdoObjectType(INT32 type)
{
    MgObjectPropertyType::ValidateRange(type);
    FdoObjectType objectType = FdoObjectType_Value;

    switch(type)
    {
        case MgObjectPropertyType::Value:
        {
            // It is already set as a default value
            break;
        }
        case MgObjectPropertyType::Collection:
        {
            objectType = FdoObjectType_Collection;
            break;
        }
        case MgObjectPropertyType::OrderedCollection:
        {
            objectType = FdoObjectType_OrderedCollection;
            break;
        }
    }
    return objectType;
}
