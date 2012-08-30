#include "FeatureUtil.h"
#include "FeatureDefs.h"
#include "Services/ByteSourceRasterStreamImpl.h"

static std::map<FdoPropertyType, INT32>                 s_FdoPropertyType;
static std::map<FdoDataType, INT32>                     s_FeatureUtilFdoDataType;
static std::map<INT32, FdoOrderingOption>               s_FdoOrderingOption;
static std::map<INT32, STRING>                          s_CustomFunctions;
static std::map<INT32, FdoSpatialOperations>            s_FdoSpatialOperation;

bool MgdFeatureUtil::m_isInitialized = MgdFeatureUtil::Initialize();

INT32 MgdFeatureUtil::GetMgPropertyType(FdoDataType fdoDataType)
{
    return s_FeatureUtilFdoDataType[fdoDataType];
}

bool MgdFeatureUtil::FindCustomFunction(CREFSTRING funcName, INT32 &index)
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

FdoOrderingOption MgdFeatureUtil::GetFdoOrderingOption(INT32 orderingOption)
{
    MgOrderingOption::ValidateRange(orderingOption);

    return s_FdoOrderingOption[orderingOption];
}

bool MgdFeatureUtil::Initialize()
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

    // Implementation Note:  FdoDataType_Decimal is currently mapped to MgPropertyType::Double.
    // An MgDecimalProperty class should be implemented in a future release.
    s_FeatureUtilFdoDataType[FdoDataType_Decimal]  = MgPropertyType::Double;

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

STRING MgdFeatureUtil::GetMessage(CREFSTRING messageId)
{
    STRING msg = MgUtil::GetResourceMessage(MgResources::FeatureService, messageId);
    return msg;
}

INT16 MgdFeatureUtil::GetMgPropertyType(MgPropertyDefinition* propDef)
{
    CHECKNULL((MgPropertyDefinition*)propDef, L"MgdFeatureUtil.GetMgPropertyType")

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


MgPropertyDefinitionCollection* MgdFeatureUtil::GetPropertyDefinitions(MgReader* reader)
{
    CHECKNULL(reader, L"MgdFeatureUtil.GetPropertyDefinitions");

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

    return propDefCol.Detach();
}


void MgdFeatureUtil::GetArguments(FdoFunction* customFunction, REFSTRING name, int &numCats, double& dataMin, double& dataMax, INT16 type)
{
    CHECKNULL(customFunction, L"MgdFeatureDistribution.GetArguments");

    dataMin = -DoubleMaxValue;
    dataMax = DoubleMaxValue;
    numCats = 0;

    FdoPtr<FdoExpressionCollection> exprCol = customFunction->GetArguments();
    FdoInt32 cnt = exprCol->GetCount();

    if (cnt < 2)
    {
        STRING message = MgdFeatureUtil::GetMessage(L"MgInvalidComputedProperty");

        MgStringCollection arguments;
        arguments.Add(customFunction->GetName());
        throw new MgFeatureServiceException(L"MgdFeatureUtil.GetArguments", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    FdoString *arg0 = NULL, *arg1 = NULL, *arg2 = NULL, *arg3 = NULL;

    // TODO: Change the logic below to access function arguments through an iterator

    // We skip first argument as it is name of property
    FdoInt32 argCnt = 0;
    FdoPtr<FdoExpression> expr0 = exprCol->GetItem(argCnt);
    CHECKNULL((FdoExpression*)expr0, L"MgdFeatureUtil.GetArguments");
    arg0 = expr0->ToString(); // Name of function

    FdoPtr<FdoExpression> expr1 = exprCol->GetItem(argCnt+1);
    CHECKNULL((FdoExpression*)expr1, L"MgdFeatureUtil.GetArguments");
    arg1 = expr1->ToString(); // No of categories

    if (cnt > 2)
    {
        FdoPtr<FdoExpression> expr2 = exprCol->GetItem(argCnt+2);
        CHECKNULL((FdoExpression*)expr2, L"MgdFeatureUtil.GetArguments");
        arg2 = expr2->ToString(); // Min Range

        if (cnt > 3)
        {
            FdoPtr<FdoExpression> expr3 = exprCol->GetItem(argCnt+3);
            CHECKNULL((FdoExpression*)expr3, L"MgdFeatureUtil.GetArguments");
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
                dataMin = dateTime->ToNumber();
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
                dataMax = dateTime->ToNumber();
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

INT32 MgdFeatureUtil::GetPropertyDefinition(MgReader* reader, REFSTRING propName)
{
    CHECKNULL((MgReader*)reader, L"MgdFeatureUtil.GetPropertyDefinition");

    INT32 cnt = reader->GetPropertyCount();
    if (cnt != 1)
    {
        STRING message = MgdFeatureUtil::GetMessage(L"MgOnlyOnePropertyAllowed");

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgFeatureServiceException(L"MgdFeatureUtil.GetPropertyDefinition", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // Get the name of property
    propName = reader->GetPropertyName(cnt-1);
    // Get the data type of property
    INT32 type = reader->GetPropertyType(propName);

    return type;
}

bool MgdFeatureUtil::FindCustomFunction(FdoFunction* customFunction, INT32 &index)
{
    bool supported = false;
    STRING funcName;

    if (customFunction != NULL)
    {
        FdoString* func = customFunction->GetName();
        if (func != NULL)
        {
            funcName = STRING(func);
        }
    }
    supported = FindCustomFunction(funcName, index);

    return supported;
}


void MgdFeatureUtil::ValidateCustomConstraints(FdoFunction* customFunction)
{
    if (customFunction == NULL) { return; } // Nothing to do here

    // Custom (Distribution) methods should have atleast two properties. Name of property and Number of categories
    FdoPtr<FdoExpressionCollection> exprCol = customFunction->GetArguments();
    FdoInt32 cnt = exprCol->GetCount();

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
                    throw new MgFeatureServiceException(L"MgdFeatureUtil.ValidateCustomConstraints",
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
                    throw new MgFeatureServiceException(L"MgdFeatureUtil.ValidateCustomConstraints",
                        __LINE__, __WFILE__, &arguments, L"", NULL);
                }
                break;
            }
        }
    }
}

double MgdFeatureUtil::Minimum(VECTOR& v)
{
    int cnt = (int)v.size();
    if (cnt <= 0)
        throw new MgIndexOutOfRangeException(L"MgdFeatureUtil.Minimum", __LINE__, __WFILE__, NULL, L"", NULL);
    double min = v[0];
    for (int i = 1; i < cnt; ++i)
    {
        double val = v[i];
        if (val < min)
            min = val;
    }
    return min;
}

INT64 MgdFeatureUtil::Minimum(VECTOR_INT64& v)
{
    int cnt = (int)v.size();
    if (cnt <= 0)
        throw new MgIndexOutOfRangeException(L"MgdFeatureUtil.Minimum", __LINE__, __WFILE__, NULL, L"", NULL);
    INT64 min = v[0];
    for (int i = 1; i < cnt; ++i)
    {
        INT64 val = v[i];
        if (val < min)
            min = val;
    }
    return min;
}

double MgdFeatureUtil::Maximum(VECTOR& v)
{
    int cnt = (int)v.size();
    if (cnt <= 0)
        throw new MgIndexOutOfRangeException(L"MgdFeatureUtil.Maximum", __LINE__, __WFILE__, NULL, L"", NULL);
    double max = v[0];
    for (int i = 1; i < cnt; ++i)
    {
        double val = v[i];
        if (val > max)
            max = val;
    }
    return max;
}

INT64 MgdFeatureUtil::Maximum(VECTOR_INT64& v)
{
    int cnt = (int)v.size();
    if (cnt <= 0)
        throw new MgIndexOutOfRangeException(L"MgdFeatureUtil.Maximum", __LINE__, __WFILE__, NULL, L"", NULL);
    INT64 max = v[0];
    for (int i = 1; i < cnt; ++i)
    {
        INT64 val = v[i];
        if (val > max)
            max = val;
    }
    return max;
}

FdoSpatialOperations MgdFeatureUtil::GetFdoSpatialOperation(INT32 spatialOp)
{
    MgFeatureSpatialOperations::ValidateRange(spatialOp);

    return s_FdoSpatialOperation[spatialOp];
}


INT32 MgdFeatureUtil::GetMgFeaturePropertyType(FdoPropertyType fdoPropType)
{
    return s_FdoPropertyType[fdoPropType];
}


MgRaster* MgdFeatureUtil::GetMgRaster(FdoIRaster* raster, STRING propName)
{
    Ptr<MgRaster> retVal;

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

            FdoPtr<FdoByteArray> agfBounds = (FdoByteArray*) raster->GetBounds();
            Ptr<MgByteSource> byteSource = new MgByteSource(agfBounds->GetData(), agfBounds->GetCount());
            Ptr<MgByteReader> byteReader = byteSource->GetReader();
            MgAgfReaderWriter agfReader;
            Ptr<MgGeometry> geom = agfReader.Read(byteReader);

            Ptr<MgEnvelope> mgEnvelope = geom->Envelope();

            retVal->SetImageXSize(xSize);
            retVal->SetImageYSize(ySize);
            retVal->SetBounds(mgEnvelope);

            FdoPtr<FdoRasterDataModel> dm = raster->GetDataModel();

            retVal->SetBitsPerPixel(dm->GetBitsPerPixel());
            retVal->SetDataModelType(dm->GetDataModelType());

            //if image is color mapped, get the palette
            if (dm->GetBitsPerPixel() == 8)
            {
                //try to get the palette -- if there isn't one,
                //an exception will be thrown. It's simpler to catch
                //it than check if such a property exists.
                try
                {
                    FdoPtr<FdoIRasterPropertyDictionary> props = raster->GetAuxiliaryProperties();
                    FdoPtr<FdoDataValue> dv = props->GetProperty(L"Palette");
                    if (dv.p)
                    {
                        FdoBLOBValue* blob = (FdoBLOBValue*)(dv.p);
                        FdoPtr<FdoByteArray> ba = blob->GetData();

                        Ptr<MgByte> pal = new MgByte(ba->GetData(), ba->GetCount());
                        retVal->SetPalette(pal);
                    }
                }
                catch (FdoException* e)
                {
                    e->Release();
                    retVal->SetPalette(NULL);
                }
            }
        }
        retVal->SetPropertyName(propName);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureUtil.GetRaster")

    return retVal.Detach();
}


MgByteReader* MgdFeatureUtil::GetRaster(FdoIReader* reader, CREFSTRING rasterPropName,INT32 xSize, INT32 ySize)
{
    // Null Reader is invalid
    CHECKNULL(reader, L"MgdFeatureUtil.GetRaster");

    // Empty property name is invalid
    if (rasterPropName.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgdFeatureUtil.GetRaster",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    Ptr<MgByteReader> byteReader;

    FdoPtr<FdoIRaster> fdoRaster = reader->GetRaster(rasterPropName.c_str());
    CHECKNULL((FdoIRaster*)fdoRaster, L"MgdFeatureUtil.GetRaster");

    // TODO: Should we support RasterModel properties and Tiling information ???
    fdoRaster->SetImageXSize(xSize);
    fdoRaster->SetImageYSize(ySize);

    // Get the stream reader
    FdoPtr<FdoIStreamReader> streamReader;
    try
    {
        streamReader = fdoRaster->GetStreamReader();
    }
    catch (FdoException* e)
    {
        // There is no stream so there is no raster data
        e->Release();
    }

    if(streamReader)
    {
        // TODO: FDO Defect - streamReader->GetType() is private;
        FdoStreamReaderType type = FdoStreamReaderType_Byte;

        if (type == FdoStreamReaderType_Byte)
        {
            FdoPtr<FdoIStreamReaderTmpl<FdoByte> > byteStreamReader =
                FDO_SAFE_ADDREF(dynamic_cast<FdoIStreamReaderTmpl<FdoByte>*>((FdoIStreamReader*)streamReader));
            CHECKNULL((FdoIStreamReaderTmpl<FdoByte>*)byteStreamReader, L"MgdFeatureUtil.GetRaster");

            ByteSourceRasterStreamImpl* rasterStream = new ByteSourceRasterStreamImpl(byteStreamReader);
            Ptr<MgByteSource> byteSource = new MgByteSource(rasterStream);
            byteSource->SetMimeType(MgMimeType::Binary);
            byteReader = byteSource->GetReader();
        }
    }

    return byteReader.Detach();
}

void MgdFeatureUtil::FillFdoParameterCollection(MgParameterCollection* source, FdoParameterValueCollection* target)
{
    CHECKNULL(source, L"MgdFeatureUtil.FillFdoParameterCollection")
    CHECKNULL(target, L"MgdFeatureUtil.FillFdoParameterCollection")

    INT32 cnt = source->GetCount();
    for (INT32 i = 0; i < cnt; i++)
    {
        Ptr<MgParameter> param = source->GetItem(i);
        FdoPtr<FdoParameterValue> fdoParam = MgParameterToFdoParameter(param);
        target->Add(fdoParam);
    }
}

void MgdFeatureUtil::FillParameterCollection(FdoParameterValueCollection* source, MgParameterCollection* target)
{
    CHECKNULL(source, L"MgdFeatureUtil.FillParameterCollection")
    CHECKNULL(target, L"MgdFeatureUtil.FillParameterCollection")

    INT32 cnt = source->GetCount();
    for (INT32 i = 0; i < cnt; i++)
    {
        FdoPtr<FdoParameterValue> fdoParam = source->GetItem(i);
        Ptr<MgParameter> param = FdoParameterValueToMgParameter(fdoParam);
        target->Add(param);
    }
}

void MgdFeatureUtil::UpdateParameterCollection(FdoParameterValueCollection* source, MgParameterCollection* target)
{
    CHECKNULL(source, L"MgdFeatureUtil.UpdateParameterCollection")
    CHECKNULL(target, L"MgdFeatureUtil.UpdateParameterCollection")

    if (source->GetCount() != target->GetCount())
    {
        throw new MgInvalidArgumentException(L"MgdFeatureUtil.UpdateParameterCollection",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    INT32 cnt = source->GetCount();
    for (INT32 i = 0; i < cnt; i++)
    {
        FdoPtr<FdoParameterValue> fdoParam = source->GetItem(i);
        Ptr<MgParameter> param = target->GetItem(i);
        FdoParameterDirection fdoParamDirection = fdoParam->GetDirection();
        if (fdoParamDirection == FdoParameterDirection_InputOutput ||
            fdoParamDirection == FdoParameterDirection_Output ||
            fdoParamDirection == FdoParameterDirection_Return)
        {
            Ptr<MgNullableProperty> prop = dynamic_cast<MgNullableProperty*>(FdoParameterValueToMgProperty(fdoParam));
            param->SetProperty(prop.p);
        }
    }
}

void MgdFeatureUtil::FillFdoPropertyCollection(MgPropertyCollection* srcCol, FdoPropertyValueCollection* paramCol)
{
    CHECKNULL(srcCol, L"MgdFeatureUtil.FillFdoPropertyCollection")
    CHECKNULL(paramCol, L"MgdFeatureUtil.FillFdoPropertyCollection")

    INT32 cnt = srcCol->GetCount();
    for (INT32 i = 0; i < cnt; i++)
    {
        Ptr<MgProperty> prop = srcCol->GetItem(i);
        FdoPtr<FdoPropertyValue> fdoProp = MgPropertyToFdoProperty(prop);
        paramCol->Add(fdoProp);
    }
}

FdoParameterValueCollection* MgdFeatureUtil::CreateFdoParameterCollection(MgPropertyCollection* srcCol)
{
    CHECKNULL(srcCol, L"MgdFeatureUtil.FillFdoPropertyCollection")
    FdoPtr<FdoParameterValueCollection> paramCol = FdoParameterValueCollection::Create();

    INT32 cnt = srcCol->GetCount();
    for (INT32 i = 0; i < cnt; i++)
    {
        Ptr<MgProperty> prop = srcCol->GetItem(i);
        FdoPtr<FdoParameterValue> fdoProp = MgPropertyToFdoParameter(prop);
        paramCol->Add(fdoProp);
    }

    return paramCol.Detach();
}

MgParameter* MgdFeatureUtil::FdoParameterValueToMgParameter(FdoParameterValue* fdoParamValue)
{
    CHECKNULL(fdoParamValue, L"MgdFeatureUtil.FdoParameterValueToMgParameter")
    Ptr<MgNullableProperty> prop = dynamic_cast<MgNullableProperty*>(FdoParameterValueToMgProperty(fdoParamValue));
    CHECKNULL(prop, L"MgdFeatureUtil.FdoParameterValueToMgParameter")

    Ptr<MgParameter> param = new MgParameter();
    param->SetProperty(prop);

    FdoParameterDirection fdoParamDirection = fdoParamValue->GetDirection();
    INT32 mgParameterDirection = GetMgParameterDirection(fdoParamDirection);
    param->SetDirection(mgParameterDirection);
    return param.Detach();
}

MgProperty* MgdFeatureUtil::FdoParameterValueToMgProperty(FdoParameterValue* fdoParamValue)
{
    CHECKNULL(fdoParamValue, L"MgdFeatureUtil.FdoParameterValueToMgProperty")

    FdoPtr<FdoLiteralValue> fdoLiteralValue = fdoParamValue->GetValue();
    CHECKNULL(fdoLiteralValue, L"MgdFeatureUtil.FdoParameterValueToMgProperty")

    Ptr<MgNullableProperty> prop = NULL;
    STRING paramName = fdoParamValue->GetName();
    FdoLiteralValueType fdoValueType = fdoLiteralValue->GetLiteralValueType();
    if (FdoLiteralValueType_Data == fdoValueType)
    {
        FdoDataValue* fdoDataValue = dynamic_cast<FdoDataValue*>(fdoLiteralValue.p);
        FdoDataType fdoDataType = fdoDataValue->GetDataType();
        bool isNull = fdoDataValue->IsNull();
        switch (fdoDataType)
        {
            case FdoDataType_Boolean:
                if (isNull)
                {
                    prop = new MgBooleanProperty();
                    prop->SetName(paramName);
                    prop->SetNull(true);
                }
                else
                {
                    FdoBooleanValue* value = dynamic_cast<FdoBooleanValue*>(fdoDataValue);
                    prop = new MgBooleanProperty(paramName, value->GetBoolean());
                }
                break;

            case FdoDataType_Byte:
                if (isNull)
                {
                    prop = new MgByteProperty();
                    prop->SetName(paramName);
                    prop->SetNull(true);
                }
                else
                {
                    FdoByteValue* value = dynamic_cast<FdoByteValue*>(fdoDataValue);
                    prop = new MgByteProperty(paramName, value->GetByte());
                }
                break;

            case FdoDataType_DateTime:
                if (isNull)
                {
                    prop = new MgDateTimeProperty();
                    prop->SetName(paramName);
                    prop->SetNull(true);
                }
                else
                {
                    FdoDateTimeValue* value = dynamic_cast<FdoDateTimeValue*>(fdoDataValue);
                    FdoDateTime fdoDateTime = value->GetDateTime();
                    Ptr<MgDateTime> dateTime = new MgDateTime(fdoDateTime.year, fdoDateTime.month, fdoDateTime.day,
                                                              fdoDateTime.hour, fdoDateTime.minute, fdoDateTime.seconds);
                    prop = new MgDateTimeProperty(paramName, dateTime);
                }
                break;

            case FdoDataType_Decimal:
                if (isNull)
                {
                    prop = new MgDoubleProperty();
                    prop->SetName(paramName);
                    prop->SetNull(true);
                }
                else
                {
                    FdoDecimalValue* value = dynamic_cast<FdoDecimalValue*>(fdoDataValue);
                    prop = new MgDoubleProperty(paramName, value->GetDecimal());
                }
                break;

            case FdoDataType_Double:
                if (isNull)
                {
                    prop = new MgDoubleProperty();
                    prop->SetName(paramName);
                    prop->SetNull(true);
                }
                else
                {
                    FdoDoubleValue* value = dynamic_cast<FdoDoubleValue*>(fdoDataValue);
                    prop = new MgDoubleProperty(paramName, value->GetDouble());
                }
                break;

            case FdoDataType_Int16:
                if (isNull)
                {
                    prop = new MgInt16Property();
                    prop->SetName(paramName);
                    prop->SetNull(true);
                }
                else
                {
                    FdoInt16Value* value = dynamic_cast<FdoInt16Value*>(fdoDataValue);
                    prop = new MgInt16Property(paramName, value->GetInt16());
                }
                break;

            case FdoDataType_Int32:
                if (isNull)
                {
                    prop = new MgInt32Property();
                    prop->SetName(paramName);
                    prop->SetNull(true);
                }
                else
                {
                    FdoInt32Value* value = dynamic_cast<FdoInt32Value*>(fdoDataValue);
                    prop = new MgInt32Property(paramName, value->GetInt32());
                }
                break;

            case FdoDataType_Int64:
                if (isNull)
                {
                    prop = new MgInt64Property();
                    prop->SetName(paramName);
                    prop->SetNull(true);
                }
                else
                {
                    FdoInt64Value* value = dynamic_cast<FdoInt64Value*>(fdoDataValue);
                    prop = new MgInt64Property(paramName, value->GetInt64());
                }
                break;

            case FdoDataType_Single:
                if (isNull)
                {
                    prop = new MgSingleProperty();
                    prop->SetName(paramName);
                    prop->SetNull(true);
                }
                else
                {
                    FdoSingleValue* value = dynamic_cast<FdoSingleValue*>(fdoDataValue);
                    prop = new MgSingleProperty(paramName, value->GetSingle());
                }
                break;

            case FdoDataType_String:
                if (isNull)
                {
                    prop = new MgStringProperty();
                    prop->SetName(paramName);
                    prop->SetNull(true);
                }
                else
                {
                    FdoStringValue* value = dynamic_cast<FdoStringValue*>(fdoDataValue);
                    prop = new MgStringProperty(paramName, value->GetString());
                }
                break;

            case FdoDataType_BLOB:
                if (isNull)
                {
                    prop = new MgBlobProperty();
                    prop->SetName(paramName);
                    prop->SetNull(true);
                }
                else
                {
                    FdoBLOBValue* value = dynamic_cast<FdoBLOBValue*>(fdoDataValue);
                    FdoPtr<FdoByteArray> fdoByteArray = value->GetData();
                    Ptr<MgByteReader> mgReader = new MgByteReader( fdoByteArray->GetData(), fdoByteArray->GetCount(), MgMimeType::Binary);
                    prop = new MgBlobProperty(paramName, mgReader);
                }
                break;

            case FdoDataType_CLOB:
                if (isNull)
                {
                    prop = new MgClobProperty();
                    prop->SetName(paramName);
                    prop->SetNull(true);
                }
                else
                {
                    FdoCLOBValue* value = dynamic_cast<FdoCLOBValue*>(fdoDataValue);
                    FdoPtr<FdoByteArray> fdoByteArray = value->GetData();
                    Ptr<MgByteReader> mgReader = new MgByteReader( fdoByteArray->GetData(), fdoByteArray->GetCount(), MgMimeType::Binary);
                    prop = new MgClobProperty(paramName, mgReader);
                }
                break;
        }
    }
    else if (FdoLiteralValueType_Geometry == fdoValueType)
    {
        FdoGeometryValue* fdoGeoVal = dynamic_cast<FdoGeometryValue*>(fdoLiteralValue.p);
        bool isNull = fdoGeoVal->IsNull();
        if (isNull)
        {
            prop = new MgGeometryProperty();
            prop->SetName(paramName);
            prop->SetNull(true);
        }
        else
        {
            FdoPtr<FdoByteArray> fdoByteArray = fdoGeoVal->GetGeometry();
            Ptr<MgByteReader> mgReader = new MgByteReader(fdoByteArray->GetData(), fdoByteArray->GetCount(), MgMimeType::Agf);
            prop = new MgGeometryProperty(paramName, mgReader);
        }
    }

    return prop.Detach();
}

FdoLiteralValue* MgdFeatureUtil::MgPropertyToFdoDataValue(MgProperty* srcProp)
{
    CHECKNULL(srcProp, L"MgdFeatureUtil.MgPropertyToFdoProperty")

    INT16 propType = srcProp->GetPropertyType();
    FdoPtr<FdoLiteralValue> fdoVal;

    switch(propType)
    {
        case MgPropertyType::Blob:
        {
            if (((MgNullableProperty*)srcProp)->IsNull())
            {
                fdoVal = FdoBLOBValue::Create();
            }
            else
            {
                Ptr<MgByteReader> value = ((MgBlobProperty*)srcProp)->GetValue();

                MgByteSink sink(value);
                Ptr<MgByte> byte = sink.ToBuffer();
                INT32 length = byte->GetLength();
                BYTE_ARRAY_OUT bytes = byte->Bytes();
                FdoPtr<FdoByteArray> array = FdoByteArray::Create((FdoByte*)bytes, length);

                fdoVal = FdoBLOBValue::Create(array);
            }
            break;
        }
        case MgPropertyType::Boolean:
        {
            if (((MgNullableProperty*)srcProp)->IsNull())
            {
                fdoVal = FdoBooleanValue::Create();
            }
            else
            {
                bool value = ((MgBooleanProperty*)srcProp)->GetValue();

                fdoVal = FdoBooleanValue::Create(value);
            }
            break;
        }
        case MgPropertyType::Byte:
        {
            if (((MgNullableProperty*)srcProp)->IsNull())
            {
                fdoVal = FdoByteValue::Create();
            }
            else
            {
                BYTE value = ((MgByteProperty*)srcProp)->GetValue();

                fdoVal = FdoByteValue::Create((FdoByte)value);
            }
            break;
        }
        case MgPropertyType::Clob:
        {
            if (((MgNullableProperty*)srcProp)->IsNull())
            {
                fdoVal = FdoCLOBValue::Create();
            }
            else
            {
                Ptr<MgByteReader> value = ((MgClobProperty*)srcProp)->GetValue();

                MgByteSink sink(value);
                Ptr<MgByte> byte = sink.ToBuffer();
                INT32 length = byte->GetLength();
                BYTE_ARRAY_OUT bytes = byte->Bytes();
                FdoPtr<FdoByteArray> array = FdoByteArray::Create((FdoByte*)bytes, length);

                fdoVal = FdoCLOBValue::Create(array);
            }
            break;
        }
        case MgPropertyType::DateTime:
        {
            if (((MgNullableProperty*)srcProp)->IsNull())
            {
                fdoVal = FdoDateTimeValue::Create();
            }
            else
            {
                Ptr<MgDateTime> value = ((MgDateTimeProperty*)srcProp)->GetValue();

                FdoDateTime time;
                time.day = (FdoInt8)value->GetDay();
                time.hour = (FdoInt8)value->GetHour();
                time.minute = (FdoInt8)value->GetMinute();
                time.month = (FdoInt8)value->GetMonth();
                time.seconds = (float)(value->GetSecond() + (value->GetMicrosecond() / 1000000.0));
                time.year = (FdoInt16)value->GetYear();

                fdoVal = FdoDateTimeValue::Create(time);
            }
            break;
        }
        case MgPropertyType::Double:
        {
            if (((MgNullableProperty*)srcProp)->IsNull())
            {
                fdoVal = FdoDoubleValue::Create();
            }
            else
            {
                double value = ((MgDoubleProperty*)srcProp)->GetValue();
                fdoVal = FdoDoubleValue::Create(value);
            }
            break;
        }
        case MgPropertyType::Geometry:
        {
            if (((MgNullableProperty*)srcProp)->IsNull())
            {
                fdoVal = FdoGeometryValue::Create();
            }
            else
            {
                Ptr<MgByteReader> value = ((MgGeometryProperty*)srcProp)->GetValue();

                MgByteSink sink(value);
                Ptr<MgByte> byte = sink.ToBuffer();
                INT32 length = byte->GetLength();
                BYTE_ARRAY_OUT bytes = byte->Bytes();

                FdoPtr<FdoByteArray> array = FdoByteArray::Create((FdoByte*)bytes, length);
                fdoVal = FdoGeometryValue::Create(array);
            }
            break;
        }
        case MgPropertyType::Int16:
        {
            if (((MgNullableProperty*)srcProp)->IsNull())
            {
                fdoVal = FdoInt16Value::Create();
            }
            else
            {
                INT16 value = ((MgInt16Property*)srcProp)->GetValue();
                fdoVal = FdoInt16Value::Create((FdoInt16)value);
            }
            break;
        }
        case MgPropertyType::Int32:
        {
            if (((MgNullableProperty*)srcProp)->IsNull())
            {
                fdoVal = FdoInt32Value::Create();
            }
            else
            {
                INT32 value = ((MgInt32Property*)srcProp)->GetValue();
                fdoVal = FdoInt32Value::Create((FdoInt32)value);
            }
            break;
        }
        case MgPropertyType::Int64:
        {
            if (((MgNullableProperty*)srcProp)->IsNull())
            {
                fdoVal = FdoInt64Value::Create();
            }
            else
            {
                INT64 value = ((MgInt64Property*)srcProp)->GetValue();
                fdoVal = FdoInt64Value::Create((FdoInt64)value);
            }
            break;
        }
        case MgPropertyType::Single:
        {
            if (((MgNullableProperty*)srcProp)->IsNull())
            {
                fdoVal = FdoSingleValue::Create();
            }
            else
            {
                float value = ((MgSingleProperty*)srcProp)->GetValue();
                fdoVal = FdoSingleValue::Create(value);
            }
            break;
        }
        case MgPropertyType::String:
        {
            if (((MgNullableProperty*)srcProp)->IsNull())
            {
                fdoVal = FdoStringValue::Create();
            }
            else
            {
                STRING value = ((MgStringProperty*)srcProp)->GetValue();
                fdoVal = FdoStringValue::Create(value.c_str());
            }
            break;
        }
        case MgPropertyType::Feature:
        {
            throw new MgInvalidPropertyTypeException(L"MgdFeatureUtil.MgPropertyToFdoProperty",
                __LINE__, __WFILE__, NULL, L"", NULL);
            break;
        }
        case MgPropertyType::Raster:
        {
            throw new MgInvalidPropertyTypeException(L"MgdFeatureUtil.MgPropertyToFdoProperty",
                __LINE__, __WFILE__, NULL, L"", NULL);
            break;
        }
    }

    return fdoVal.Detach();
}

FdoParameterValue* MgdFeatureUtil::MgPropertyToFdoParameter(MgProperty* srcProp)
{
    FdoPtr<FdoLiteralValue> fdoValue = MgPropertyToFdoDataValue(srcProp);
    STRING str = srcProp->GetName();

    assert(!str.empty());

    return FdoParameterValue::Create(str.c_str(), fdoValue);
}

FdoParameterValue* MgdFeatureUtil::MgParameterToFdoParameter(MgParameter* srcParam)
{
    Ptr<MgProperty> srcProp = srcParam->GetProperty();
    assert(srcProp);

    FdoPtr<FdoParameterValue> value = MgPropertyToFdoParameter(srcProp);
    FdoParameterDirection paramDirection = GetFdoParameterDirection(srcParam->GetDirection());
    value->SetDirection(paramDirection);
    return value.Detach();
}

FdoPropertyValue* MgdFeatureUtil::MgPropertyToFdoProperty(MgProperty* srcProp)
{
    FdoPtr<FdoLiteralValue> fdoValue = MgPropertyToFdoDataValue(srcProp);
    STRING str = srcProp->GetName();

    assert(!str.empty());

    return FdoPropertyValue::Create(str.c_str(), fdoValue);
}

INT32 MgdFeatureUtil::FdoOrderTypeToMgOrderingOption(FdoOrderType type)
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

FdoOrderType MgdFeatureUtil::MgOrderingOptionToFdoOrderType(INT32 type)
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

INT32 MgdFeatureUtil::FdoObjectTypeToMgObjectPropertyType(FdoObjectType type)
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

FdoObjectType MgdFeatureUtil::MgObjectPropertyTypeToFdoObjectType(INT32 type)
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

MgStringCollection* MgdFeatureUtil::FdoToMgStringCollection(FdoStringCollection* fdoStrs, bool includeEmptyStrings)
{
    Ptr<MgStringCollection> mgStrs;

    if (NULL != fdoStrs)
    {
        FdoInt32 count = fdoStrs->GetCount();
        mgStrs = new MgStringCollection();

        for (FdoInt32 i = 0; i < count; ++i)
        {
            FdoStringP currStr = fdoStrs->GetString(i);

            if ((currStr == NULL || 0 == currStr.GetLength()) && includeEmptyStrings)
            {
                mgStrs->Add(STRING(L""));
            }
            else
            {
                mgStrs->Add(STRING(currStr));
            }
        }
    }

    return mgStrs.Detach();
}

FdoStringCollection* MgdFeatureUtil::MgToFdoStringCollection(MgStringCollection* mgStrs, bool includeEmptyStrings)
{
    FdoPtr<FdoStringCollection> fdoStrs;

    if (NULL != mgStrs)
    {
        INT32 count = mgStrs->GetCount();
        fdoStrs = FdoStringCollection::Create();

        for (INT32 i = 0; i < count; ++i)
        {
            STRING currStr = mgStrs->GetItem(i);

            if (includeEmptyStrings || !currStr.empty())
            {
                fdoStrs->Add(currStr.c_str());
            }
        }
    }

    return fdoStrs.Detach();
}

bool MgdFeatureUtil::FdoClassExist(const wchar_t* name, FdoClassCollection* clsCol)
{
    if (name == NULL) // Empty name is an error
        return false;

    return (clsCol->IndexOf(name) >= 0);
}

MgClassDefinition* MgdFeatureUtil::GetMgClassDefinition(FdoClassDefinition* fdoClassDefinition, bool bSerialize)
{
    CHECKNULL(fdoClassDefinition, L"MgdFeatureUtil.GetMgClassDefinition");

    // Create MgClassDefinition
    Ptr<MgClassDefinition> mgClassDef = new MgClassDefinition();
    CHECKNULL((MgClassDefinition*)mgClassDef, L"MgdFeatureUtil.GetMgClassDefinition");

    // Get PropertyDefinitionCollection to store property definitions
    Ptr<MgPropertyDefinitionCollection> propDefCol = mgClassDef->GetProperties();
    CHECKNULL((MgPropertyDefinitionCollection*)propDefCol, L"MgdFeatureUtil.GetMgClassDefinition");

    // Get PropertyDefinitionCollection to store key property definitions i.e. which makes key for this feature class
    Ptr<MgPropertyDefinitionCollection> identityPropDefCol = mgClassDef->GetIdentityProperties();
    CHECKNULL((MgPropertyDefinitionCollection*)identityPropDefCol, L"MgdFeatureUtil.GetMgClassDefinition");

    // description
    FdoString* desc = fdoClassDefinition->GetDescription();
    if (desc != NULL)
    {
        mgClassDef->SetDescription(STRING(desc));
    }

    // Class name
    FdoString* className = fdoClassDefinition->GetName();
    if (className != NULL)
    {
        mgClassDef->SetName(STRING(className));
    }

    // Schema Name
    FdoPtr<FdoFeatureSchema> schema = fdoClassDefinition->GetFeatureSchema();
    if (schema.p != NULL)
    {
        FdoString* schemaName = schema->GetName();
        if (schemaName != NULL)
        {
            mgClassDef->SetSchemaName(STRING(schemaName));
        }
    }

    bool isComputed = fdoClassDefinition->GetIsComputed();
    if (isComputed)
    {
        mgClassDef->MakeClassComputed(isComputed);
    }

    bool isAbstract = fdoClassDefinition->GetIsAbstract();
    if (isAbstract)
    {
        mgClassDef->MakeClassAbstract(isAbstract);
    }

    // Retrieve Class properties from FDO
    FdoPtr<FdoPropertyDefinitionCollection> fpdc = fdoClassDefinition->GetProperties();
    CHECKNULL((FdoPropertyDefinitionCollection*)fpdc, L"MgdFeatureUtil.GetMgClassDefinition");

    // Retrieve Base class properties from FDO
    // TODO: Should we add Base class properties into the list of properties?
    // TODO: Can this be null?
    FdoPtr<FdoReadOnlyPropertyDefinitionCollection> frpdc = fdoClassDefinition->GetBaseProperties();

    // Retrieve identity properties from FDO
    // TODO: Can this be null?
    FdoPtr<FdoDataPropertyDefinitionCollection> fdpdc = fdoClassDefinition->GetIdentityProperties();

    // Add properties
    MgdFeatureUtil::GetClassProperties(propDefCol, fpdc);

    FdoClassType classType = fdoClassDefinition->GetClassType();
    if (classType == FdoClassType_FeatureClass)
    {
        FdoPtr<FdoGeometricPropertyDefinition> geomPropDef = ((FdoFeatureClass*)fdoClassDefinition)->GetGeometryProperty();
        if (geomPropDef != NULL)
        {
            FdoString* defaultGeomName = geomPropDef->GetName();
            if (defaultGeomName != NULL)
            {
                mgClassDef->SetDefaultGeometryPropertyName(STRING(defaultGeomName));
            }
        }
    }

    // Add identity properties
    MgdFeatureUtil::GetClassProperties(identityPropDefCol, fdpdc);

    // Add base properties
//  this->GetClassProperties(propDefCol, frpdc);

    if (bSerialize)
    {
        STRING str;
        Ptr<MgByteReader> byteReader = SerializeToXml(fdoClassDefinition);
        str = byteReader->ToString();

        STRING str1 = L"";

        size_t idx = str.find(L"?>");

        if (idx >= 0)
        {
            str1 = str.substr(idx+2);
        }
        else if (idx < 0)
        {
            idx = str.find(L"<Class");
            if (idx >= 0)
            {
                str1 = str.substr(idx);
            }
        }

        mgClassDef->SetSerializedXml(str1);
    }

    FdoPtr<FdoClassDefinition> baseDefinition = fdoClassDefinition->GetBaseClass();
    if (baseDefinition != NULL)
    {
        Ptr<MgClassDefinition> mgBaseClsDef = GetMgClassDefinition(baseDefinition, bSerialize);
        mgClassDef->SetBaseClassDefinition(mgBaseClsDef);
    }

    return mgClassDef.Detach();
}

MgByteReader* MgdFeatureUtil::SerializeToXml(FdoClassDefinition* classDef)
{
    CHECKNULL(classDef, L"MgdFeatureUtil.SerializeToXml");

    FdoString* className = classDef->GetName();
    FdoFeatureSchemaP pSchema = classDef->GetFeatureSchema();
    FdoFeatureSchemaP tempSchema;
    FdoClassDefinitionP featClass;
    FdoInt32 index = 0;

    if (pSchema != NULL)
    {
        //Get the position of the class in the collecion
        FdoPtr<FdoClassCollection> fcc = pSchema->GetClasses();
        index = fcc->IndexOf( className );

        // Move class of interest to its own schema
        tempSchema = FdoFeatureSchema::Create( pSchema->GetName(), L"" );
        featClass = FdoClassesP(pSchema->GetClasses())->GetItem( className );
        FdoClassesP(pSchema->GetClasses())->Remove(featClass);
        FdoClassesP(tempSchema->GetClasses())->Add(featClass);
    }
    else
    {
        // NOTE: schema name needs to be something non-empty in order to
        // avoid XSL warnings
        tempSchema = FdoFeatureSchema::Create(L"TempSchema", L"" );
        FdoClassesP(tempSchema->GetClasses())->Add(classDef);

        // Add the base class
        FdoPtr<FdoClassDefinition> fdoBaseClass = classDef->GetBaseClass();
        while (fdoBaseClass != NULL)
        {
            FdoClassesP(tempSchema->GetClasses())->Add(fdoBaseClass);
            fdoBaseClass = fdoBaseClass->GetBaseClass();
        }
    }

    FdoIoMemoryStreamP fmis = FdoIoMemoryStream::Create();
    tempSchema->WriteXml(fmis);
    fmis->Reset();

    FdoInt64 len = fmis->GetLength();
    FdoByte *bytes = new FdoByte[(size_t)len];
    CHECKNULL(bytes, L"MgdFeatureUtil::SerializeToXml");

    fmis->Read(bytes, (FdoSize)len);

    // Get byte reader from memory stream
    Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)bytes, (INT32)len);
    byteSource->SetMimeType(MgMimeType::Xml);
    Ptr<MgByteReader> byteReader = byteSource->GetReader();

    // Cleanup the above addition/deletion of class definition
    if (pSchema != NULL)
    {
        if (featClass != NULL)
        {
            FdoClassesP(tempSchema->GetClasses())->Remove(featClass);
            FdoClassesP(pSchema->GetClasses())->Insert(index, featClass);
        }
    }
    else
    {
        FdoClassesP(tempSchema->GetClasses())->Remove(classDef);
    }

    delete[] bytes;

    return byteReader.Detach();
}

void MgdFeatureUtil::GetClassProperties(MgPropertyDefinitionCollection* propDefCol,
                                             FdoPropertyDefinitionCollection* fdoPropDefCol)
{
    if (NULL == fdoPropDefCol)
        return;

    FdoInt32 cnt = fdoPropDefCol->GetCount();
    for (FdoInt32 i =0; i < cnt; i++)
    {
        // Get Fdo Property
        FdoPtr<FdoPropertyDefinition> fpd = fdoPropDefCol->GetItem(i);
        CHECKNULL((FdoPropertyDefinition*)fpd, L"MgdFeatureUtil.GetClassProperties");

        // Create MgProperty
        Ptr<MgPropertyDefinition> prop = MgdFeatureUtil::GetMgPropertyDefinition(fpd);

        // Add it to class definition
        if (prop != NULL)
        {
            propDefCol->Add(prop);
        }
    }
}


void MgdFeatureUtil::GetClassProperties(MgPropertyDefinitionCollection* propDefCol,
                                             FdoDataPropertyDefinitionCollection* fdoPropDefCol)
{
    if (NULL == fdoPropDefCol)
        return;

    FdoInt32 cnt  = fdoPropDefCol->GetCount();
    for (FdoInt32 i =0; i < cnt; i++)
    {
        // Get Fdo Property
        FdoPtr<FdoPropertyDefinition> fpd = fdoPropDefCol->GetItem(i);
        CHECKNULL((FdoPropertyDefinition*)fpd, L"MgdFeatureUtil.GetClassProperties");

        // Create MgProperty
        Ptr<MgPropertyDefinition> prop = MgdFeatureUtil::GetMgPropertyDefinition(fpd);

        // Add it to class definition
        if (prop != NULL)
        {
            propDefCol->Add(prop);
        }
    }
}

MgPropertyDefinition* MgdFeatureUtil::GetMgPropertyDefinition(FdoPropertyDefinition* fdoPropDef)
{
    CHECKNULL((FdoPropertyDefinition*)fdoPropDef, L"MgdFeatureUtil.GetMgPropertyDefinition");

    Ptr<MgPropertyDefinition> propDef;

    FdoPropertyType fpt = fdoPropDef->GetPropertyType();
    switch (fpt)
    {
        // Represents a Data Property type.
        case FdoPropertyType_DataProperty:
        {
            propDef = MgdFeatureUtil::GetDataPropertyDefinition((FdoDataPropertyDefinition*)fdoPropDef);
            break;
        }
        // Represents an Object Property type.
        case FdoPropertyType_ObjectProperty:
        {
            propDef = MgdFeatureUtil::GetObjectPropertyDefinition((FdoObjectPropertyDefinition*)fdoPropDef);
            break;
        }

        // Represents a Geometric Property type.
        case FdoPropertyType_GeometricProperty:
        {
            propDef = MgdFeatureUtil::GetGeometricPropertyDefinition((FdoGeometricPropertyDefinition*)fdoPropDef);
            break;
        }
        // Represents an Association Property type.
        case FdoPropertyType_AssociationProperty:
        {
            // TODO:
            break;
        }

        // Represents a Raster (image) Property type.
        case FdoPropertyType_RasterProperty:
        {
            propDef = MgdFeatureUtil::GetRasterPropertyDefinition((FdoRasterPropertyDefinition*)fdoPropDef);
            break;
        }
    }

    return propDef.Detach();
}

MgDataPropertyDefinition* MgdFeatureUtil::GetDataPropertyDefinition(FdoDataPropertyDefinition* fdoPropDef)
{
    if (fdoPropDef == NULL)
    {
        return NULL;
    }

    STRING name = STRING(fdoPropDef->GetName());
    Ptr<MgDataPropertyDefinition> propDef = new MgDataPropertyDefinition(name);

    // Get data members from FDO
    FdoString* defaultVal = fdoPropDef->GetDefaultValue();
    FdoInt32 length = fdoPropDef->GetLength();
    bool isReadOnly = fdoPropDef->GetReadOnly();
    FdoString* desc = fdoPropDef->GetDescription();
    FdoInt32 precision = fdoPropDef->GetPrecision();
    bool isNullable = fdoPropDef->GetNullable();
    FdoStringP qname = fdoPropDef->GetQualifiedName();
    FdoInt32 scale = fdoPropDef->GetScale();
    bool isAutoGenerated = fdoPropDef->GetIsAutoGenerated();

    // Set it for MapGuide
    FdoDataType dataType = fdoPropDef->GetDataType();
    propDef->SetDataType(GetMgPropertyType(dataType));

    if (defaultVal != NULL)
    {
        propDef->SetDefaultValue(STRING(defaultVal));
    }

    propDef->SetLength((INT32)length);
    propDef->SetReadOnly(isReadOnly);

    if (desc != NULL)
    {
        propDef->SetDescription(STRING(desc));
    }

    propDef->SetPrecision((INT32)precision);
    propDef->SetNullable(isNullable);

    FdoString* qualifiedName = (FdoString*)qname;
    if (qualifiedName != NULL)
    {
        propDef->SetQualifiedName(STRING(qualifiedName));
    }

    propDef->SetAutoGeneration(isAutoGenerated);

    propDef->SetScale((INT32)scale);

    return propDef.Detach();
}

MgObjectPropertyDefinition* MgdFeatureUtil::GetObjectPropertyDefinition(FdoObjectPropertyDefinition* fdoPropDef)
{
    CHECKNULL((FdoObjectPropertyDefinition*)fdoPropDef, L"MgdFeatureUtil.GetObjectPropertyDefinition");

    STRING name = STRING(fdoPropDef->GetName());
    Ptr<MgObjectPropertyDefinition> propDef = new MgObjectPropertyDefinition(name);

    FdoString* desc = fdoPropDef->GetDescription();
    FdoStringP qname = fdoPropDef->GetQualifiedName();
    FdoString* qualifiedName = (FdoString*)qname;

    if (qualifiedName != NULL)
    {
        propDef->SetQualifiedName(STRING(qualifiedName));
    }

    // Set it for MapGuide
    if (desc != NULL)
    {
        propDef->SetDescription(STRING(desc));
    }

    FdoPtr<FdoClassDefinition> fdoClsDef = fdoPropDef->GetClass();
    CHECKNULL((FdoClassDefinition*)fdoClsDef, L"MgdFeatureUtil.GetObjectPropertyDefinition")

    FdoPtr<FdoDataPropertyDefinition> idenProp = fdoPropDef->GetIdentityProperty(); // Can return NULL

    FdoObjectType objType = fdoPropDef->GetObjectType();
    FdoOrderType orderType = fdoPropDef->GetOrderType();

    Ptr<MgClassDefinition> objPropClsDef = MgdFeatureUtil::GetMgClassDefinition(fdoClsDef, true);
    Ptr<MgDataPropertyDefinition> objIdenProp = MgdFeatureUtil::GetDataPropertyDefinition(idenProp);
    INT32 orderOption = MgdFeatureUtil::FdoOrderTypeToMgOrderingOption(orderType);
    INT32 mgObjType = MgdFeatureUtil::FdoObjectTypeToMgObjectPropertyType(objType);

    propDef->SetClassDefinition(objPropClsDef);
    propDef->SetIdentityProperty(objIdenProp);
    propDef->SetOrderType(orderOption);
    propDef->SetObjectType(mgObjType);

    return propDef.Detach();
}

MgGeometricPropertyDefinition* MgdFeatureUtil::GetGeometricPropertyDefinition(FdoGeometricPropertyDefinition* fdoPropDef)
{
    CHECKNULL((FdoGeometricPropertyDefinition*)fdoPropDef, L"MgdFeatureUtil.GetGeometricPropertyDefinition");

    STRING name = STRING(fdoPropDef->GetName());
    Ptr<MgGeometricPropertyDefinition> propDef = new MgGeometricPropertyDefinition(name);

    // Get data members from FDO
    FdoString* desc = fdoPropDef->GetDescription();
    FdoInt32 geomTypes = fdoPropDef->GetGeometryTypes();
    FdoInt32 specificGeomCount = 0;
    FdoGeometryType * specificGeomTypes = fdoPropDef->GetSpecificGeometryTypes(specificGeomCount);
    bool hasElev = fdoPropDef->GetHasElevation();
    bool hasMeasure = fdoPropDef->GetHasMeasure();
    FdoStringP qname = fdoPropDef->GetQualifiedName();
    bool isReadOnly = fdoPropDef->GetReadOnly();
    FdoString* spatialContextName = fdoPropDef->GetSpatialContextAssociation();

    // Set it for MapGuide
    if (desc != NULL)
    {
        propDef->SetDescription(STRING(desc));
    }

    MgIntCollection geomTypeColl;
    INT32 geomTypeCount = (INT32)specificGeomCount;
    for (INT32 i=0; i<geomTypeCount && i<MG_MAX_GEOMETRY_TYPE_SIZE; ++i)
    {
        geomTypeColl.Add((INT32)specificGeomTypes[i]);
    }
    Ptr<MgGeometryTypeInfo> geomTypeInfo = new MgGeometryTypeInfo;
    geomTypeInfo->SetTypes(&geomTypeColl);
    propDef->SetGeometryTypes((INT32)geomTypes);
    propDef->SetSpecificGeometryTypes(geomTypeInfo);
    propDef->SetHasElevation(hasElev);
    propDef->SetHasMeasure(hasMeasure);
    FdoString* qualifiedName = (FdoString*)qname;
    if (qualifiedName != NULL)
    {
        propDef->SetQualifiedName(STRING(qualifiedName));
    }
    propDef->SetReadOnly(isReadOnly);

    if (spatialContextName != NULL)
    {
        propDef->SetSpatialContextAssociation(STRING(spatialContextName));
    }

    return propDef.Detach();
}

MgRasterPropertyDefinition* MgdFeatureUtil::GetRasterPropertyDefinition(FdoRasterPropertyDefinition* fdoPropDef)
{
    CHECKNULL((FdoRasterPropertyDefinition*)fdoPropDef, L"MgdFeatureUtil.GetRasterPropertyDefinition");

    STRING name = STRING(fdoPropDef->GetName());
    Ptr<MgRasterPropertyDefinition> propDef = new MgRasterPropertyDefinition(name);

    // Get data members from FDO
    FdoString* desc = fdoPropDef->GetDescription();
    FdoInt32 xsize = fdoPropDef->GetDefaultImageXSize();
    FdoInt32 ysize = fdoPropDef->GetDefaultImageYSize();
    bool isNullable = fdoPropDef->GetNullable();
    FdoStringP qname = fdoPropDef->GetQualifiedName();
    bool isReadOnly = fdoPropDef->GetReadOnly();
    FdoStringP spatialContextName = fdoPropDef->GetSpatialContextAssociation();

    // Set it for MapGuide
    if (desc != NULL)
    {
        propDef->SetDescription(STRING(desc));
    }

    propDef->SetDefaultImageXSize((INT32)xsize);
    propDef->SetDefaultImageYSize((INT32)ysize);
    propDef->SetNullable(isNullable);

    FdoString* qualifiedName = (FdoString*)qname;
    if (qualifiedName != NULL)
    {
        propDef->SetQualifiedName(STRING(qualifiedName));
    }
    propDef->SetReadOnly(isReadOnly);

    if (spatialContextName != NULL)
    {
        propDef->SetSpatialContextAssociation(STRING(spatialContextName));
    }

    return propDef.Detach();
}

MgProperty* MgdFeatureUtil::GetMgProperty(MgReader* reader, CREFSTRING qualifiedPropName, INT16 type)
{
    // Null Reader is invalid
    CHECKNULL(reader, L"MgdFeatureUtil.GetMgProperty");

    // No propertyname specified, return NULL
    if (qualifiedPropName.empty())
        return NULL;

    Ptr<MgNullableProperty> prop;

    switch(type)
    {
        case MgPropertyType::Boolean: /// Boolean true/false value
        {
            bool val = false;
            bool isNull = true;

            if (!reader->IsNull(qualifiedPropName.c_str()))
            {
                val = reader->GetBoolean(qualifiedPropName.c_str());
                isNull = false;
            }

            prop = new MgBooleanProperty(qualifiedPropName, val);
            prop->SetNull(isNull);
            break;
        }
        case MgPropertyType::Byte: /// Unsigned 8 bit value
        {
            FdoByte val = 0;
            bool isNull = true;

            if (!reader->IsNull(qualifiedPropName.c_str()))
            {
                val = reader->GetByte(qualifiedPropName.c_str());
                isNull = false;
            }

            prop = new MgByteProperty(qualifiedPropName, (BYTE)val);
            prop->SetNull(isNull);
            break;
        }
        case MgPropertyType::DateTime: /// DateTime object
        {
            Ptr<MgDateTime> dateTime;
            bool isNull = true;

            if (!reader->IsNull(qualifiedPropName.c_str()))
            {
                dateTime = reader->GetDateTime(qualifiedPropName.c_str());
                isNull = false;
            }

            prop = new MgDateTimeProperty(qualifiedPropName, dateTime);
            prop->SetNull(isNull);
            break;
        }
        case MgPropertyType::Single: /// Single precision floating point value
        {
            float val = 0.0f;
            bool isNull = true;

            if (!reader->IsNull(qualifiedPropName.c_str()))
            {
                val = reader->GetSingle(qualifiedPropName.c_str());
                isNull = false;
            }

            prop = new MgSingleProperty(qualifiedPropName, (float)val);
            prop->SetNull(isNull);
            break;
        }
        case MgPropertyType::Double: /// Double precision floating point value
        {
            double val = 0.0;
            bool isNull = true;

            if (!reader->IsNull(qualifiedPropName.c_str()))
            {
                val = reader->GetDouble(qualifiedPropName.c_str());
                isNull = false;
            }

            prop = new MgDoubleProperty(qualifiedPropName, (double)val);
            prop->SetNull(isNull);
            break;
        }
        case MgPropertyType::Int16: /// 16 bit signed integer value
        {
            FdoInt16 val = 0;
            bool isNull = true;

            if (!reader->IsNull(qualifiedPropName.c_str()))
            {
                val = reader->GetInt16(qualifiedPropName.c_str());
                isNull = false;
            }

            prop = new MgInt16Property(qualifiedPropName, (INT16)val);
            prop->SetNull(isNull);
            break;
        }
        case MgPropertyType::Int32: // 32 bit signed integer value
        {
            FdoInt32 val = 0;
            bool isNull = true;

            if (!reader->IsNull(qualifiedPropName.c_str()))
            {
                val = reader->GetInt32(qualifiedPropName.c_str());
                isNull = false;
            }

            prop = new MgInt32Property(qualifiedPropName, (INT32)val);
            prop->SetNull(isNull);
            break;
        }
        case MgPropertyType::Int64: // 64 bit signed integer value
        {
            FdoInt64 val = 0;
            bool isNull = true;

            if (!reader->IsNull(qualifiedPropName.c_str()))
            {
                val = reader->GetInt64(qualifiedPropName.c_str());
                isNull = false;
            }

            prop = new MgInt64Property(qualifiedPropName, (INT64)val);
            prop->SetNull(isNull);
            break;
        }
        case MgPropertyType::String: // String MgProperty
        {
            STRING val = L"";
            bool isNull = true;

            if (!reader->IsNull(qualifiedPropName.c_str()))
            {
                // A try/catch block is used here for case where the FDO computed
                // property field is used.  When the property value is null, the computed
                // property isNull flag is not set  which causes the IsNull() test to fail, and
                // leading to GetString() to result in an exception.
                // Instead, it will be handled by catching the exception and setting the isNull flag.
                try
                {
                    val = reader->GetString(qualifiedPropName.c_str());
                    isNull = false;
                }
                catch (FdoException* e)
                {
                    isNull = true;
                    FDO_SAFE_RELEASE(e);
                }
                catch (MgException* e)
                {
                    isNull = true;
                    SAFE_RELEASE(e);
                }
                catch (...)
                {
                    isNull = true;
                }
            }

            prop = new MgStringProperty(qualifiedPropName, val);
            prop->SetNull(isNull);
            break;
        }
        case MgPropertyType::Blob: // BLOB
        {
            Ptr<MgByteReader> val;
            bool isNull = true;

            if (!reader->IsNull(qualifiedPropName.c_str()))
            {
                isNull = false;
                val = reader->GetBLOB(qualifiedPropName.c_str());
            }

            prop = new MgBlobProperty(qualifiedPropName, val);
            prop->SetNull(isNull);
            break;
        }
        case MgPropertyType::Clob: // CLOB
        {
            Ptr<MgByteReader> val;
            bool isNull = true;

            if (!reader->IsNull(qualifiedPropName.c_str()))
            {
                isNull = false;
                val = reader->GetCLOB(qualifiedPropName.c_str());
            }

            prop = new MgClobProperty(qualifiedPropName, val);
            prop->SetNull(isNull);
            break;
        }
        case MgPropertyType::Feature: // Feature object
        {
            Ptr<MgFeatureReader> val;
            bool isNull = true;

            if (!reader->IsNull(qualifiedPropName.c_str()))
            {
                isNull = false;
                val = ((MgFeatureReader*)(reader))->GetFeatureObject(qualifiedPropName.c_str());
            }

            prop = new MgFeatureProperty(qualifiedPropName, val);
            prop->SetNull(isNull);
            break;
        }
        case MgPropertyType::Geometry: // Geometry object
        {
            Ptr<MgByteReader> val;
            bool isNull = true;

            if (!reader->IsNull(qualifiedPropName.c_str()))
            {
                val = reader->GetGeometry(qualifiedPropName.c_str());
                isNull = false;
            }

            prop = new MgGeometryProperty(qualifiedPropName, val);
            prop->SetNull(isNull);
            break;
        }
        case MgPropertyType::Raster: // Raster object
        {
            Ptr<MgRaster> val;
            bool isNull = true;

            if (!reader->IsNull(qualifiedPropName.c_str()))
            {
                val = reader->GetRaster(qualifiedPropName.c_str());
                isNull = false;
            }

            prop = new MgRasterProperty(qualifiedPropName, val);
            prop->SetNull(isNull);
            break;
        }
    }

    return prop.Detach();
}

//////////////////////////////////////////////////////////////////
//TODO: I will take this out as I do not know how to instantiate FdoFeatureSchemaCollection
FdoFeatureSchemaCollection* MgdFeatureUtil::GetFdoFeatureSchemaCollection(
    MgFeatureSchemaCollection* mgSchemaCol)
{
    FdoPtr<FdoFeatureSchemaCollection> fdoSchemaCol;

    MG_FEATURE_SERVICE_TRY()

    CHECKNULL(mgSchemaCol, L"MgdFeatureUtil.GetFdoFeatureSchemaCollection");
    fdoSchemaCol = FdoFeatureSchemaCollection::Create((FdoSchemaElement*)NULL);

    INT32 i = 0;
    INT32 count = mgSchemaCol->GetCount();

    for (i=0; i<count; i++)
    {
        Ptr<MgFeatureSchema> mgSchema = mgSchemaCol->GetItem(i);
        FdoPtr<FdoFeatureSchema> fdoSchema = GetFdoFeatureSchema(mgSchema);
        if (fdoSchemaCol->Contains(fdoSchema))
        {
            throw new MgDuplicateObjectException(L"MgdFeatureUtil.GetFdoFeatureSchemaCollection", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        else
        {
            fdoSchemaCol->Add(fdoSchema);
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureUtil.GetFdoFeatureSchemaCollection")

    return fdoSchemaCol.Detach();
}


//////////////////////////////////////////////////////////////////
FdoFeatureSchema* MgdFeatureUtil::GetFdoFeatureSchema(
    MgFeatureSchema* mgSchema)
{
    FdoPtr<FdoFeatureSchema> fdoSchema;

    MG_FEATURE_SERVICE_TRY()
    CHECKNULL(mgSchema, L"MgdFeatureUtil.GetFdoFeatureSchema");

    fdoSchema = FdoFeatureSchema::Create();
    CHECKNULL(fdoSchema, L"MgdFeatureUtil.GetFdoFeatureSchema");

    STRING name = mgSchema->GetName();
    if (!name.empty())
    {
        fdoSchema->SetName((FdoString*) name.c_str());
    }

    STRING description = mgSchema->GetDescription();
    if (!description.empty())
    {
        fdoSchema->SetDescription((FdoString*) description.c_str());
    }

    FdoPtr<FdoClassCollection> fdoClassCol = fdoSchema->GetClasses();
    Ptr<MgClassDefinitionCollection> awClassCol = mgSchema->GetClasses();

    GetFdoClassCollection(fdoClassCol, awClassCol);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureUtil.GetFdoFeatureSchema")

    return fdoSchema.Detach();
}


//////////////////////////////////////////////////////////////////
void MgdFeatureUtil::GetFdoClassCollection(
    FdoClassCollection* fdoClassCol,
    MgClassDefinitionCollection* mgClassDefCol)
{
    MG_FEATURE_SERVICE_TRY()
    CHECKNULL((MgClassDefinitionCollection*)mgClassDefCol, L"MgdFeatureUtil.GetFdoClassCollection");

    INT32 count = mgClassDefCol->GetCount();
    INT32 i = 0;

    for (i=0; i<count; i++)
    {
        Ptr<MgClassDefinition> mgClassDef = mgClassDefCol->GetItem(i);
        FdoPtr<FdoClassDefinition> fdoClassDef = GetFdoClassDefinition(mgClassDef, fdoClassCol);
        CHECKNULL(fdoClassDef, L"MgdFeatureUtil.GetFdoClassCollection")

        FdoStringP name = fdoClassDef->GetName();
        if (!FdoClassExist(name, fdoClassCol))
            fdoClassCol->Add(fdoClassDef);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureUtil.GetFdoClassCollection")
}


//////////////////////////////////////////////////////////////////
FdoClassDefinition* MgdFeatureUtil::GetFdoClassDefinition(
    MgClassDefinition* mgClassDef,
    FdoClassCollection* fdoClassCol)
{
    FdoPtr<FdoClassDefinition> fdoClassDef;

    MG_FEATURE_SERVICE_TRY()
    CHECKNULL(mgClassDef, L"MgdFeatureUtil.GetFdoClassDefinition");
    CHECKNULL(fdoClassCol, L"MgdFeatureUtil.GetFdoClassDefinition");

    STRING name = mgClassDef->GetName();
    assert(!name.empty());

    fdoClassDef = fdoClassCol->FindItem(name.c_str());

    if (fdoClassDef != NULL) // Class is already available and therefore return from here.
        return fdoClassDef.Detach();

    //Create FdoClassDefinition
    STRING geomName = mgClassDef->GetDefaultGeometryPropertyName();

    //If we have geometry create feature class
    if (!geomName.empty())
    {
        fdoClassDef = FdoFeatureClass::Create();
    }
    else
    {
        fdoClassDef = FdoClass::Create();
    }

    CHECKNULL(fdoClassDef, L"MgdFeatureUtil.GetFdoClassDefinition");

    FdoPtr<FdoPropertyDefinitionCollection> fdoPropDefCol = fdoClassDef->GetProperties();
    CHECKNULL((FdoPropertyDefinitionCollection*) fdoPropDefCol, L"MgdFeatureUtil.GetFdoClassDefinition");

    FdoPtr<FdoDataPropertyDefinitionCollection> fdoIdentityPropDefCol = (FdoDataPropertyDefinitionCollection*)fdoClassDef->GetIdentityProperties();
    CHECKNULL((FdoDataPropertyDefinitionCollection*)fdoIdentityPropDefCol, L"MgdFeatureUtil.GetFdoClassDefinition");

    //Set description
    STRING desc = mgClassDef->GetDescription();
    if (!desc.empty())
    {
        fdoClassDef->SetDescription((FdoString*) desc.c_str());
    }

    if (!name.empty())
    {
        fdoClassDef->SetName((FdoString*) name.c_str());
    }

    bool isComputed = mgClassDef->IsComputed();
    if (isComputed)
    {
        fdoClassDef->SetIsComputed(isComputed);
    }

    bool isAbstract = mgClassDef->IsAbstract();
    if (isAbstract)
    {
        fdoClassDef->SetIsAbstract(isAbstract);
    }

    // Retrieve Class properties
    Ptr<MgPropertyDefinitionCollection> mgPropDefCol = mgClassDef->GetProperties();
    CHECKNULL((MgPropertyDefinitionCollection*) mgPropDefCol, L"MgdFeatureUtil.GetFdoClassDefinition");

    //Retrieve identity properties
    Ptr<MgPropertyDefinitionCollection> awDataPropDefCol = mgClassDef->GetIdentityProperties();

    //Add properties
    GetClassProperties(fdoPropDefCol, mgPropDefCol, fdoClassCol);

    //Add identity properties
    GetClassProperties(fdoIdentityPropDefCol, awDataPropDefCol);

    Ptr<MgClassDefinition> awBaseDef = mgClassDef->GetBaseClassDefinition();
    if (awBaseDef != NULL)
    {
        FdoPtr<FdoClassDefinition> fdoBaseDef;
        STRING bname = awBaseDef->GetName();
        assert(!bname.empty());
        if (!bname.empty()) // Empty name is an error
        {
            fdoBaseDef = fdoClassCol->FindItem(bname.c_str());
            if (fdoBaseDef == NULL) // Not found
            {
                fdoBaseDef = GetFdoClassDefinition(awBaseDef, fdoClassCol); // Create a new one
                if (fdoBaseDef != NULL)
                {
                    FdoStringP nameBase = fdoBaseDef->GetName();
                    if (!FdoClassExist(nameBase, fdoClassCol))
                        fdoClassCol->Add(fdoBaseDef);
                }
            }
            fdoClassDef->SetBaseClass(fdoBaseDef);
        }
    }

    if (!geomName.empty())
    {
        FdoPtr<FdoGeometricPropertyDefinition> defaultGeom = (FdoGeometricPropertyDefinition*)fdoPropDefCol->GetItem(geomName.c_str());
        FdoPtr<FdoFeatureClass> ffClass = FDO_SAFE_ADDREF((FdoFeatureClass*)((FdoClassDefinition*)fdoClassDef));
        ffClass->SetGeometryProperty(defaultGeom);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureUtil.GetFdoClassDefinition")

    return fdoClassDef.Detach();
}


//////////////////////////////////////////////////////////////////
void MgdFeatureUtil::GetClassProperties(
    FdoPropertyDefinitionCollection* fdoPropDefCol,
    MgPropertyDefinitionCollection* propDefCol,
    FdoClassCollection* fdoClassCol)
{
    MG_FEATURE_SERVICE_TRY()

    if (NULL == propDefCol)
        return;

    INT32 cnt = propDefCol->GetCount();
    INT32 i=0;
    for (i=0; i<cnt; i++)
    {
        // Get Mg Property
        Ptr<MgPropertyDefinition> awpd = propDefCol->GetItem(i);
        CHECKNULL((MgPropertyDefinition*)awpd, L"MgdFeatureUtil.GetClassProperties");

        // Create Fdo Property
        FdoPtr<FdoPropertyDefinition> fdoProp = GetFdoPropertyDefinition(awpd, fdoClassCol);
        // Add it to class definition
        if (fdoProp != NULL)
        {
            fdoPropDefCol->Add(fdoProp);
        }
    }
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureUtil.GetClassProperties")
}


//////////////////////////////////////////////////////////////////
// TODO: Not sure why FdoDataPropertyDefinitionCollection derives from SchemaCollection??
void MgdFeatureUtil::GetClassProperties(
    FdoDataPropertyDefinitionCollection* fdoPropDefCol,
    MgPropertyDefinitionCollection* mgPropDefCol)
{
    MG_FEATURE_SERVICE_TRY()

    if (NULL == mgPropDefCol)
        return;

    INT32 cnt = mgPropDefCol->GetCount();
    INT32 i=0;
    for (i=0; i<cnt; i++)
    {
        //Get property
        Ptr<MgPropertyDefinition> mgPropDef = mgPropDefCol->GetItem(i);
        CHECKNULL((MgPropertyDefinition*) mgPropDef, L"MgdFeatureUtil.GetClassProperties");

        //Create Fdo property
        FdoPtr<FdoDataPropertyDefinition> fdoPropDef = (FdoDataPropertyDefinition*)GetFdoPropertyDefinition(mgPropDef, NULL);
        //Add it to class definition
        if (fdoPropDef != NULL)
        {
            fdoPropDefCol->Add(fdoPropDef);
        }
    }
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureUtil.GetClassProperties")
}


//////////////////////////////////////////////////////////////////
FdoPropertyDefinition* MgdFeatureUtil::GetFdoPropertyDefinition(
    MgPropertyDefinition* mgPropDef,
    FdoClassCollection* fdoClassCol)
{
    CHECKNULL((MgPropertyDefinition*)mgPropDef, L"MgdFeatureUtil.GetFdoPropertyDefinition");

    FdoPtr<FdoPropertyDefinition> fdoPropDef;
    MG_FEATURE_SERVICE_TRY()

    INT16 fpt = mgPropDef->GetPropertyType();

    switch (fpt)
    {
        // Represents a Data Property type.
        case MgFeaturePropertyType::DataProperty:
        {
            fdoPropDef = GetDataPropertyDefinition((MgDataPropertyDefinition*)mgPropDef);
            break;
        }
        // Represents an Object Property type.
        case MgFeaturePropertyType::ObjectProperty:
        {
            fdoPropDef = GetObjectPropertyDefinition((MgObjectPropertyDefinition*)mgPropDef, fdoClassCol);
            break;
        }

        // Represents a Geometric Property type.
        case MgFeaturePropertyType::GeometricProperty:
        {
            fdoPropDef = GetGeometricPropertyDefinition((MgGeometricPropertyDefinition*)mgPropDef);
            break;
        }
        // Represents an Association Property type.
        case MgFeaturePropertyType::AssociationProperty:
        {
            // TODO:
            break;
        }

        // Represents a Raster (image) Property type.
        case MgFeaturePropertyType::RasterProperty:
        {
            fdoPropDef = GetRasterPropertyDefinition((MgRasterPropertyDefinition*)mgPropDef);
            break;
        }
    }
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureUtil.GetFdoPropertyDefinition")

    return fdoPropDef.Detach();
}


//////////////////////////////////////////////////////////////////
FdoDataPropertyDefinition* MgdFeatureUtil::GetDataPropertyDefinition(
    MgDataPropertyDefinition* mgPropDef)
{
    FdoPtr<FdoDataPropertyDefinition> fdoPropDef;

    MG_FEATURE_SERVICE_TRY()

    // Null can be a valid values in some cases and therefore if NULL is encountered return NULL.
    if (mgPropDef == NULL)
    {
        return NULL;
    }

    fdoPropDef = FdoDataPropertyDefinition::Create();
    STRING name = mgPropDef->GetName();
    fdoPropDef->SetName((FdoString*)name.c_str());

    //Get data members
    STRING defaultVal = mgPropDef->GetDefaultValue();
    INT32 length = mgPropDef->GetLength();
    bool isReadOnly = mgPropDef->GetReadOnly();
    STRING desc = mgPropDef->GetDescription();
    INT32 precision = mgPropDef->GetPrecision();
    bool isNullable = mgPropDef->GetNullable();
    STRING qname = mgPropDef->GetQualifiedName();
    INT32 scale = mgPropDef->GetScale();
    bool isAutoGenerated = mgPropDef->IsAutoGenerated();

    //Set it for Fdo
    FdoDataType dataType = GetFdoDataType(mgPropDef->GetDataType());
    fdoPropDef->SetDataType(dataType);

    if (!defaultVal.empty())
    {
        fdoPropDef->SetDefaultValue((FdoString*) defaultVal.c_str());
    }

    fdoPropDef->SetLength((FdoInt32)length);
    fdoPropDef->SetReadOnly(isReadOnly);

    if (!desc.empty())
    {
        fdoPropDef->SetDescription((FdoString*)desc.c_str());
    }

    fdoPropDef->SetPrecision((FdoInt32)precision);
    fdoPropDef->SetNullable(isNullable);

    fdoPropDef->SetScale((FdoInt32)scale);
    fdoPropDef->SetIsAutoGenerated(isAutoGenerated);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureUtil.GetDataPropertyDefinition")

    return fdoPropDef.Detach();
}


//////////////////////////////////////////////////////////////////
FdoObjectPropertyDefinition* MgdFeatureUtil::GetObjectPropertyDefinition(
    MgObjectPropertyDefinition* objPropDef,
    FdoClassCollection* fdoClassCol)
{
    FdoPtr<FdoObjectPropertyDefinition> fdoPropDef;

    MG_FEATURE_SERVICE_TRY()
    CHECKNULL((MgObjectPropertyDefinition*)objPropDef, L"MgdFeatureUtil.GetObjectPropertyDefinition");

    fdoPropDef = FdoObjectPropertyDefinition::Create();
    // Retrieve data from MgObjectProperty
    STRING name = objPropDef->GetName();
    STRING desc = objPropDef->GetDescription();
    INT32 objType = objPropDef->GetObjectType();
    INT32 orderType = objPropDef->GetOrderType();
    Ptr<MgClassDefinition> clsDef = objPropDef->GetClassDefinition();
    Ptr<MgDataPropertyDefinition> idProp = objPropDef->GetIdentityProperty();
    // Convert MgObjectProperty data members to Fdo data members
    FdoPtr<FdoClassDefinition> fdoClsDef = GetFdoClassDefinition(clsDef, fdoClassCol);
    CHECKNULL(fdoClsDef, L"MgdFeatureUtil.GetObjectPropertyDefinition")

    FdoPtr<FdoDataPropertyDefinition> fdoDataPropDef = GetDataPropertyDefinition(idProp);
    FdoObjectType fdoObjectType = MgdFeatureUtil::MgObjectPropertyTypeToFdoObjectType(objType);
    FdoOrderType fdoOrderType = MgdFeatureUtil::MgOrderingOptionToFdoOrderType(orderType);
    // Set them to Fdo object property
    fdoPropDef->SetName(name.c_str());
    fdoPropDef->SetDescription(desc.c_str());
    fdoPropDef->SetObjectType(fdoObjectType);
    fdoPropDef->SetOrderType(fdoOrderType);
    fdoPropDef->SetClass(fdoClsDef);
    fdoPropDef->SetIdentityProperty(fdoDataPropDef);

    if (fdoClassCol != NULL)
    {
        FdoStringP clsName = fdoClsDef->GetName();
        if (!FdoClassExist(clsName, fdoClassCol))
            fdoClassCol->Add(fdoClsDef);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureUtil.GetObjectPropertyDefinition")

    return fdoPropDef.Detach();
}


//////////////////////////////////////////////////////////////////
FdoGeometricPropertyDefinition* MgdFeatureUtil::GetGeometricPropertyDefinition(
    MgGeometricPropertyDefinition* mgPropDef)
{
    FdoPtr<FdoGeometricPropertyDefinition> fdoPropDef;

    MG_FEATURE_SERVICE_TRY()
    CHECKNULL((MgGeometricPropertyDefinition*)mgPropDef, L"MgdFeatureUtil.GetGeometricPropertyDefinition");

    STRING name = mgPropDef->GetName();
    fdoPropDef = FdoGeometricPropertyDefinition::Create();
    fdoPropDef->SetName((FdoString*) name.c_str());

    //Get data members
    STRING desc = mgPropDef->GetDescription();
    INT32 geomTypes = mgPropDef->GetGeometryTypes();
    Ptr<MgGeometryTypeInfo> geomTypeInfo = mgPropDef->GetSpecificGeometryTypes();
    bool hasElev = mgPropDef->GetHasElevation();
    bool hasMeasure = mgPropDef->GetHasMeasure();
    STRING qname = mgPropDef->GetQualifiedName();
    bool isReadOnly = mgPropDef->GetReadOnly();
    STRING spatialContextName = mgPropDef->GetSpatialContextAssociation();

    //Set it for Fdo
    if (!desc.empty())
    {
        fdoPropDef->SetDescription((FdoString*) desc.c_str());
    }

    FdoGeometryType geomTypeList[MG_MAX_GEOMETRY_TYPE_SIZE];
    FdoInt32 geomTypeCount = (FdoInt32) geomTypeInfo->GetCount();
    for (FdoInt32 i=0; i<geomTypeCount && i<MG_MAX_GEOMETRY_TYPE_SIZE; ++i)
    {
        geomTypeList[i] = (FdoGeometryType)geomTypeInfo->GetType((INT32)i);
    }
    fdoPropDef->SetGeometryTypes((FdoInt32)geomTypes);
    fdoPropDef->SetSpecificGeometryTypes(geomTypeList, geomTypeCount);
    fdoPropDef->SetHasElevation(hasElev);
    fdoPropDef->SetHasMeasure(hasMeasure);

    fdoPropDef->SetReadOnly(isReadOnly);
    if(!spatialContextName.empty())
    {
        fdoPropDef->SetSpatialContextAssociation((FdoString*)spatialContextName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureUtil.GetGeometricPropertyDefinition")

    return fdoPropDef.Detach();
}


//////////////////////////////////////////////////////////////////
FdoRasterPropertyDefinition* MgdFeatureUtil::GetRasterPropertyDefinition(
    MgRasterPropertyDefinition* mgPropDef)
{
    FdoPtr<FdoRasterPropertyDefinition> fdoPropDef;

    MG_FEATURE_SERVICE_TRY()
    CHECKNULL((MgRasterPropertyDefinition*)mgPropDef, L"MgdFeatureUtil.GetRasterPropertyDefinition");

    fdoPropDef = FdoRasterPropertyDefinition::Create();
    STRING name = mgPropDef->GetName();
    fdoPropDef->SetName((FdoString*) name.c_str());

    //Get data members
    STRING desc = mgPropDef->GetDescription();
    INT32 xsize = mgPropDef->GetDefaultImageXSize();
    INT32 ysize = mgPropDef->GetDefaultImageYSize();
    bool isNullable = mgPropDef->GetNullable();
    STRING qname = mgPropDef->GetQualifiedName();
    bool isReadOnly = mgPropDef->GetReadOnly();
    STRING spatialContextName = mgPropDef->GetSpatialContextAssociation();
    //Set it for Fdo
    if (!desc.empty())
    {
        fdoPropDef->SetDescription((FdoString*) desc.c_str());
    }

    fdoPropDef->SetDefaultImageXSize((FdoInt32)xsize);
    fdoPropDef->SetDefaultImageYSize((FdoInt32)ysize);
    fdoPropDef->SetNullable(isNullable);

    //Cannot set qualified name in fdo
    fdoPropDef->SetReadOnly(isReadOnly);

    if(!spatialContextName.empty())
    {
        fdoPropDef->SetSpatialContextAssociation((FdoString*) spatialContextName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureUtil.GetRasterPropertyDefinition")

    return fdoPropDef.Detach();
}


//////////////////////////////////////////////////////////////////
FdoDataType MgdFeatureUtil::GetFdoDataType(INT32 awPropType)
{
    FdoDataType fdoDataType;
    switch(awPropType)
    {
        // Represents a Boolean value of true or false.
        case MgPropertyType::Boolean:
        {
            fdoDataType = FdoDataType_Boolean;
            break;
        }
        // Represents unsigned 8-bit integers with values between 0 and 255.
        case MgPropertyType::Byte:
        {
            fdoDataType = FdoDataType_Byte;
            break;
        }
        // Represents a date and time value.
        case MgPropertyType::DateTime:
        {
            fdoDataType = FdoDataType_DateTime;
            break;
        }
        // Represents a floating point value ranging from approximately 5.0 x
        // 10^-324 to 1.7 x 10^308 with a precision of 15-16 digits.
        // Implementation Note:  FdoDataType_Decimal is currently mapped to MgPropertyType::Double.
        // An MgDecimalProperty class should be implemented in a future release.
        case MgPropertyType::Double:
        {
            fdoDataType = FdoDataType_Double;
            break;
        }
        // Represents signed 16-bit integers with values between -32768 and 32767.
        case MgPropertyType::Int16:
        {
            fdoDataType = FdoDataType_Int16;
            break;
        }
        // Represents signed 32-bit integers with values between -2147483648 and
        // 2147483647.
        case MgPropertyType::Int32:
        {
            fdoDataType = FdoDataType_Int32;
            break;
        }
        // Represents signed 64-bit integers with values between
        // -9223372036854775808 and 9223372036854775807.
        case MgPropertyType::Int64:
        {
            fdoDataType = FdoDataType_Int64;
            break;
        }
        // Represents floating point values ranging from approximately 1.5 x 10^-45
        // to 3.4 x 10^38: with a precision of 7 digits.
        case MgPropertyType::Single:
        {
            fdoDataType = FdoDataType_Single;
            break;
        }
        // Represents a decimal value.
        case MgPropertyType::Decimal:
        {
            fdoDataType = FdoDataType_Decimal;
            break;
        }
        // Represents a Unicode character strings.
        case MgPropertyType::String:
        {
            fdoDataType = FdoDataType_String;
            break;
        }
        // Represents a binary large object stored as a collection of bytes.
        case MgPropertyType::Blob:
        {
            fdoDataType = FdoDataType_BLOB;
            break;
        }
        // Represents a character large object stored as a collection of
        // characters.
        case MgPropertyType::Clob:
        {
            fdoDataType = FdoDataType_CLOB;
            break;
        }
        default:
        {
            // We will probably never hit this code
            STRING buffer;
            MgUtil::Int32ToString(awPropType, buffer);

            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgdFeatureUtil.GetFdoDataType",
                __LINE__, __WFILE__, &arguments, L"MgInvalidPropertyType", NULL);
        }
    }

    return fdoDataType;
}

FdoParameterDirection MgdFeatureUtil::GetFdoParameterDirection(INT32 paramDirection)
{
    FdoParameterDirection fdoParameterDirection;
    switch(paramDirection)
    {
        case MgParameterDirection::Input:
        {
            fdoParameterDirection = FdoParameterDirection_Input;
            break;
        }
        case MgParameterDirection::InputOutput:
        {
            fdoParameterDirection = FdoParameterDirection_InputOutput;
            break;
        }
        case MgParameterDirection::Output:
        {
            fdoParameterDirection = FdoParameterDirection_Output;
            break;
        }
        case MgParameterDirection::Ret:
        {
            fdoParameterDirection = FdoParameterDirection_Return;
            break;
        }
        default:
        {
            // We will probably never hit this code
            STRING buffer;
            MgUtil::Int32ToString(paramDirection, buffer);

            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgdFeatureUtil.GetFdoParameterDirection",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
    }

    return fdoParameterDirection;
}

INT32 MgdFeatureUtil::GetMgParameterDirection(FdoParameterDirection fdoParamDirection)
{
    INT32 parameterDirection;
    switch(fdoParamDirection)
    {
        case FdoParameterDirection_Input:
        {
            parameterDirection = MgParameterDirection::Input;
            break;
        }
        case FdoParameterDirection_InputOutput:
        {
            parameterDirection = MgParameterDirection::InputOutput;
            break;
        }
        case FdoParameterDirection_Output:
        {
            parameterDirection = MgParameterDirection::Output;
            break;
        }
        case FdoParameterDirection_Return:
        {
            parameterDirection = MgParameterDirection::Ret;
            break;
        }
        default:
        {
            // We will probably never hit this code
            STRING buffer;
            MgUtil::Int32ToString(fdoParamDirection, buffer);

            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgdFeatureUtil.GetMgParameterDirection",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
    }

    return parameterDirection;
}

//////////////////////////////////////////////////////////////////
void MgdFeatureUtil::UpdateFdoFeatureSchema(
    MgFeatureSchema* mgSchema,
    FdoFeatureSchema* fdoSchema)
{
    CHECKNULL(mgSchema, L"MgdFeatureUtil.UpdateFdoFeatureSchema");
    CHECKNULL(fdoSchema, L"MgdFeatureUtil.UpdateFdoFeatureSchema");

    MG_FEATURE_SERVICE_TRY()

    STRING description = mgSchema->GetDescription();
    if (description != fdoSchema->GetDescription())
    {
        fdoSchema->SetDescription((FdoString*) description.c_str());
    }

    FdoPtr<FdoClassCollection> fdoClassCol = fdoSchema->GetClasses();
    Ptr<MgClassDefinitionCollection> mgClassCol = mgSchema->GetClasses();

    UpdateFdoClassCollection(mgClassCol, fdoClassCol);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureUtil.UpdateFdoFeatureSchema")
}


//////////////////////////////////////////////////////////////////
void MgdFeatureUtil::UpdateFdoClassCollection(
    MgClassDefinitionCollection* mgClassDefCol,
    FdoClassCollection* fdoClassCol)
{
    CHECKNULL(mgClassDefCol, L"MgdFeatureUtil.UpdateFdoClassCollection");
    CHECKNULL(fdoClassCol, L"MgdFeatureUtil.UpdateFdoClassCollection");

    MG_FEATURE_SERVICE_TRY()

    INT32 i = 0;
    INT32 count = mgClassDefCol->GetCount();
    for (i = 0; i < count; i++)
    {
        Ptr<MgClassDefinition> mgClassDef = mgClassDefCol->GetItem(i);
        STRING className = mgClassDef->GetName();
        FdoPtr<FdoClassDefinition> fdoOldClassDef = fdoClassCol->FindItem(className.c_str());

        if (NULL == fdoOldClassDef)
        {
            if (!mgClassDef->IsDeleted())
            {
                FdoPtr<FdoClassDefinition> fdoNewClassDef = MgdFeatureUtil::GetFdoClassDefinition(mgClassDef, fdoClassCol);
                CHECKNULL(fdoNewClassDef, L"MgdFeatureUtil.UpdateFdoClassCollection")
                fdoClassCol->Add(fdoNewClassDef);
            }
        }
        else
        {
            if (!mgClassDef->IsDeleted())
                UpdateFdoClassDefinition(mgClassDef, fdoOldClassDef, fdoClassCol);
            else
                fdoOldClassDef->Delete();
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureUtil.UpdateFdoClassCollection")
}


//////////////////////////////////////////////////////////////////
void MgdFeatureUtil::UpdateFdoClassDefinition(
    MgClassDefinition* mgClassDef,
    FdoClassDefinition* fdoClassDef,
    FdoClassCollection* fdoClassCol)
{
    CHECKNULL(mgClassDef, L"MgdFeatureUtil.UpdateFdoClassDefinition");
    CHECKNULL(fdoClassDef, L"MgdFeatureUtil.UpdateFdoClassDefinition");
    CHECKNULL(fdoClassCol, L"MgdFeatureUtil.UpdateFdoClassDefinition");

    MG_FEATURE_SERVICE_TRY()

    FdoPtr<FdoPropertyDefinitionCollection> fdoPropDefCol = fdoClassDef->GetProperties();
    CHECKNULL((FdoPropertyDefinitionCollection*) fdoPropDefCol, L"MgdFeatureUtil.UpdateFdoClassDefinition");

    FdoPtr<FdoDataPropertyDefinitionCollection> fdoIdentityPropDefCol = (FdoDataPropertyDefinitionCollection*)fdoClassDef->GetIdentityProperties();
    CHECKNULL((FdoDataPropertyDefinitionCollection*)fdoIdentityPropDefCol, L"MgdFeatureUtil.UpdateFdoClassDefinition");

    //Set description
    STRING desc = mgClassDef->GetDescription();
    if (desc != fdoClassDef->GetDescription())
    {
        fdoClassDef->SetDescription((FdoString*) desc.c_str());
    }

    bool isComputed = mgClassDef->IsComputed();
    if (isComputed != fdoClassDef->GetIsComputed())
    {
        fdoClassDef->SetIsComputed(isComputed);
    }

    bool isAbstract = mgClassDef->IsAbstract();
    if (isAbstract != fdoClassDef->GetIsAbstract())
    {
        fdoClassDef->SetIsAbstract(isAbstract);
    }

    // Retrieve Class properties
    Ptr<MgPropertyDefinitionCollection> mgPropDefCol = mgClassDef->GetProperties();
    CHECKNULL((MgPropertyDefinitionCollection*) mgPropDefCol, L"MgdFeatureUtil.UpdateFdoClassDefinition");

    //Retrieve identity properties
    Ptr<MgPropertyDefinitionCollection> awDataPropDefCol = mgClassDef->GetIdentityProperties();

    //Update properties
    UpdateClassProperties(mgPropDefCol, fdoPropDefCol, fdoClassCol);

    //Update identity properties
    UpdateClassProperties(fdoIdentityPropDefCol, awDataPropDefCol);

    Ptr<MgClassDefinition> awBaseDef = mgClassDef->GetBaseClassDefinition();
    if (awBaseDef != NULL)
    {
        STRING bname = awBaseDef->GetName();
        assert(!bname.empty());
        if (!bname.empty()) // Empty name is an error
        {
            FdoPtr<FdoClassDefinition> fdoBaseDef = fdoClassCol->FindItem(bname.c_str());
            if (fdoBaseDef == NULL) // Not found
            {
                fdoBaseDef = MgdFeatureUtil::GetFdoClassDefinition(awBaseDef, fdoClassCol); // Create a new one
                if (fdoBaseDef != NULL)
                {
                    FdoStringP nameBase = fdoBaseDef->GetName();
                    if (!MgdFeatureUtil::FdoClassExist(nameBase, fdoClassCol))
                        fdoClassCol->Add(fdoBaseDef);
                }
                fdoClassDef->SetBaseClass(fdoBaseDef);
            }
            else
            {
                FdoPtr<FdoClassDefinition> fdoOldBaseDef = fdoClassDef->GetBaseClass();
                if (fdoOldBaseDef == NULL || bname != fdoOldBaseDef->GetName())
                    fdoClassDef->SetBaseClass(fdoBaseDef);
            }
        }
    }
    else
    {
        FdoPtr<FdoClassDefinition> fdoOldBaseDef = fdoClassDef->GetBaseClass();
        if (fdoOldBaseDef != NULL)
            fdoClassDef->SetBaseClass(NULL);
    }

    STRING geomName = mgClassDef->GetDefaultGeometryPropertyName();
    if (!geomName.empty())
    {
        FdoFeatureClass* fdoFeatureClass = dynamic_cast<FdoFeatureClass*>(fdoClassDef);
        if (NULL == fdoFeatureClass)
            throw new MgInvalidArgumentException(L"MgdFeatureUtil.UpdateFdoClassDefinition", __LINE__, __WFILE__, NULL, L"", NULL);

        FdoPtr<FdoGeometricPropertyDefinition> defaultGeom = (FdoGeometricPropertyDefinition*)fdoPropDefCol->GetItem(geomName.c_str());
        fdoFeatureClass->SetGeometryProperty(defaultGeom);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureUtil.UpdateFdoClassDefinition")
}


//////////////////////////////////////////////////////////////////
void MgdFeatureUtil::UpdateClassProperties(
    MgPropertyDefinitionCollection* propDefCol,
    FdoPropertyDefinitionCollection* fdoPropDefCol,
    FdoClassCollection* fdoClassCol)
{
    MG_FEATURE_SERVICE_TRY()

    if (NULL == propDefCol)
        return;

    INT32 cnt = propDefCol->GetCount();
    INT32 i = 0;
    for ( i = 0; i < cnt; i++)
    {
        // Get Mg Property
        Ptr<MgPropertyDefinition> awpd = propDefCol->GetItem(i);
        CHECKNULL((MgPropertyDefinition*)awpd, L"MgdFeatureUtil.UpdateClassProperties");
        STRING propName = awpd->GetName();

        FdoPtr<FdoPropertyDefinition> fdoOldProp = fdoPropDefCol->FindItem(propName.c_str());
        if (NULL == fdoOldProp)
        {
            if (!awpd->IsDeleted())
            {
                // Create Fdo Property
                FdoPtr<FdoPropertyDefinition> fdoProp = MgdFeatureUtil::GetFdoPropertyDefinition(awpd, fdoClassCol);
                // Add it to class definition
                if (fdoProp != NULL)
                {
                    fdoPropDefCol->Add(fdoProp);
                }
            }
        }
        else
        {
            if (!awpd->IsDeleted())
                UpdateFdoPropertyDefinition(awpd, fdoOldProp, NULL);
            else
            {
                fdoOldProp->Delete();
                //fdoPropDefCol->Remove(fdoOldProp);
            }
        }
    }
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureUtil.UpdateClassProperties")
}


//////////////////////////////////////////////////////////////////
void MgdFeatureUtil::UpdateClassProperties(
    FdoDataPropertyDefinitionCollection* fdoPropDefCol,
    MgPropertyDefinitionCollection* mgPropDefCol)
{
    MG_FEATURE_SERVICE_TRY()

    if (NULL == mgPropDefCol)
        return;

    INT32 cnt = mgPropDefCol->GetCount();
    INT32 i=0;
    for ( i = 0; i < cnt; i++)
    {
        //Get property
        Ptr<MgPropertyDefinition> mgPropDef = mgPropDefCol->GetItem(i);
        CHECKNULL((MgPropertyDefinition*) mgPropDef, L"MgdFeatureUtil.UpdateClassProperties");
        STRING propName = mgPropDef->GetName();

        FdoPtr<FdoDataPropertyDefinition> fdoOldProp = fdoPropDefCol->FindItem(propName.c_str());
        if (NULL == fdoOldProp)
        {
            if (!mgPropDef->IsDeleted())
            {
                // Create Fdo Property
                FdoPtr<FdoDataPropertyDefinition> fdoProp = (FdoDataPropertyDefinition*)GetFdoPropertyDefinition(mgPropDef, NULL);
                // Add it to class definition
                if (fdoProp != NULL)
                {
                    fdoPropDefCol->Add(fdoProp);
                }
            }
        }
        else
        {
            if (!mgPropDef->IsDeleted())
                UpdateFdoPropertyDefinition(mgPropDef, fdoOldProp, NULL);
            else
            {
                fdoOldProp->Delete();
                //fdoPropDefCol->Remove(fdoOldProp);
            }
        }
    }
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureUtil.UpdateClassProperties")
}


//////////////////////////////////////////////////////////////////
void MgdFeatureUtil::UpdateFdoPropertyDefinition(
    MgPropertyDefinition* mgPropDef,
    FdoPropertyDefinition* fdoPropDef,
    FdoClassCollection* fdoClassCol)
{
    CHECKNULL((MgPropertyDefinition*)mgPropDef, L"MgdFeatureUtil.UpdateFdoPropertyDefinition");

    MG_FEATURE_SERVICE_TRY()

    INT16 fpt = mgPropDef->GetPropertyType();
    switch (fpt)
    {
        // Represents a Data Property type.
        case MgFeaturePropertyType::DataProperty:
        {
            FdoDataPropertyDefinition* fdoDataPropDef = dynamic_cast<FdoDataPropertyDefinition*>(fdoPropDef);
            if (NULL == fdoDataPropDef)
                throw new MgInvalidArgumentException(L"MgdFeatureUtil.UpdateFdoClassDefinition", __LINE__, __WFILE__, NULL, L"", NULL);

            UpdateDataPropertyDefinition((MgDataPropertyDefinition*)mgPropDef, fdoDataPropDef);
            break;
        }
        // Represents an Object Property type.
        case MgFeaturePropertyType::ObjectProperty:
        {
            FdoObjectPropertyDefinition* fdoObjectPropDef = dynamic_cast<FdoObjectPropertyDefinition*>(fdoPropDef);
            if (NULL == fdoObjectPropDef)
                throw new MgInvalidArgumentException(L"MgdFeatureUtil.UpdateFdoClassDefinition", __LINE__, __WFILE__, NULL, L"", NULL);

            UpdateObjectPropertyDefinition((MgObjectPropertyDefinition*)mgPropDef, fdoObjectPropDef, fdoClassCol);
            break;
        }

        // Represents a Geometric Property type.
        case MgFeaturePropertyType::GeometricProperty:
        {
            FdoGeometricPropertyDefinition* fdoGeometricPropDef = dynamic_cast<FdoGeometricPropertyDefinition*>(fdoPropDef);
            if (NULL == fdoGeometricPropDef)
                throw new MgInvalidArgumentException(L"MgdFeatureUtil.UpdateFdoClassDefinition", __LINE__, __WFILE__, NULL, L"", NULL);

            UpdateGeometricPropertyDefinition((MgGeometricPropertyDefinition*)mgPropDef, fdoGeometricPropDef);
            break;
        }
        // Represents an Association Property type.
        case MgFeaturePropertyType::AssociationProperty:
        {
            // TODO:
            break;
        }

        // Represents a Raster (image) Property type.
        case MgFeaturePropertyType::RasterProperty:
        {
            FdoRasterPropertyDefinition* fdoRasterPropDef = dynamic_cast<FdoRasterPropertyDefinition*>(fdoPropDef);
            if (NULL == fdoRasterPropDef)
                throw new MgInvalidArgumentException(L"MgdFeatureUtil.UpdateFdoClassDefinition", __LINE__, __WFILE__, NULL, L"", NULL);

            UpdateRasterPropertyDefinition((MgRasterPropertyDefinition*)mgPropDef, fdoRasterPropDef);
            break;
        }
    }
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureUtil.UpdateFdoPropertyDefinition")
}


//////////////////////////////////////////////////////////////////
void MgdFeatureUtil::UpdateDataPropertyDefinition(
    MgDataPropertyDefinition* mgPropDef,
    FdoDataPropertyDefinition* fdoPropDef)
{
    CHECKNULL(mgPropDef, L"MgdFeatureUtil.UpdateFdoPropertyDefinition");
    CHECKNULL(fdoPropDef, L"MgdFeatureUtil.UpdateFdoPropertyDefinition");

    MG_FEATURE_SERVICE_TRY()

    //Get data members
    STRING defaultVal = mgPropDef->GetDefaultValue();
    INT32 length = mgPropDef->GetLength();
    bool isReadOnly = mgPropDef->GetReadOnly();
    STRING desc = mgPropDef->GetDescription();
    INT32 precision = mgPropDef->GetPrecision();
    bool isNullable = mgPropDef->GetNullable();
    STRING qname = mgPropDef->GetQualifiedName();
    INT32 scale = mgPropDef->GetScale();
    bool isAutoGenerated = mgPropDef->IsAutoGenerated();

    //Set it for Fdo
    FdoDataType dataType = GetFdoDataType(mgPropDef->GetDataType());
    if (dataType != fdoPropDef->GetDataType())
        fdoPropDef->SetDataType(dataType);

    if (defaultVal != fdoPropDef->GetDefaultValue())
        fdoPropDef->SetDefaultValue((FdoString*) defaultVal.c_str());

    if (length != fdoPropDef->GetLength())
        fdoPropDef->SetLength((FdoInt32)length);

    if (isReadOnly != fdoPropDef->GetReadOnly())
        fdoPropDef->SetReadOnly(isReadOnly);

    if (desc != fdoPropDef->GetDescription())
        fdoPropDef->SetDescription((FdoString*)desc.c_str());

    if (precision != fdoPropDef->GetPrecision())
        fdoPropDef->SetPrecision((FdoInt32)precision);

    if (isNullable != fdoPropDef->GetNullable())
        fdoPropDef->SetNullable(isNullable);

    if (scale != fdoPropDef->GetScale())
        fdoPropDef->SetScale((FdoInt32)scale);

    if (isAutoGenerated != fdoPropDef->GetIsAutoGenerated())
        fdoPropDef->SetIsAutoGenerated(isAutoGenerated);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureUtil.UpdateDataPropertyDefinition")
}


//////////////////////////////////////////////////////////////////
void MgdFeatureUtil::UpdateObjectPropertyDefinition(
    MgObjectPropertyDefinition* objPropDef,
    FdoObjectPropertyDefinition* fdoPropDef,
    FdoClassCollection* fdoClassCol)
{
    CHECKNULL((MgObjectPropertyDefinition*)objPropDef, L"MgdFeatureUtil.UpdateObjectPropertyDefinition");
    CHECKNULL((FdoObjectPropertyDefinition*)fdoPropDef, L"MgdFeatureUtil.UpdateObjectPropertyDefinition");

    MG_FEATURE_SERVICE_TRY()

    // Retrieve data from MgObjectProperty
    STRING name = objPropDef->GetName();
    STRING desc = objPropDef->GetDescription();
    INT32 objType = objPropDef->GetObjectType();
    INT32 orderType = objPropDef->GetOrderType();
    Ptr<MgClassDefinition> clsDef = objPropDef->GetClassDefinition();
    Ptr<MgDataPropertyDefinition> idProp = objPropDef->GetIdentityProperty();

    if (fdoClassCol != NULL)
    {
        STRING clsName = clsDef->GetName();
        if (!FdoClassExist(clsName.c_str(), fdoClassCol))
        {
            FdoPtr<FdoClassDefinition> fdoClsDef = GetFdoClassDefinition(clsDef, fdoClassCol);
            CHECKNULL(fdoClsDef, L"MgdFeatureUtil.UpdateObjectPropertyDefinition")
            fdoClassCol->Add(fdoClsDef);
        }
        else
        {
            FdoPtr<FdoClassDefinition> fdoClsDef = fdoClassCol->FindItem(clsName.c_str());
            UpdateFdoClassDefinition(clsDef, fdoClsDef, fdoClassCol);
            if (fdoClsDef->GetElementState() != FdoSchemaElementState_Unchanged)
                fdoPropDef->SetClass(fdoClsDef);
        }
    }

    FdoPtr<FdoDataPropertyDefinition> fdoDataPropDef = fdoPropDef->GetIdentityProperty();
    UpdateDataPropertyDefinition(idProp, fdoDataPropDef);


    FdoObjectType fdoObjectType = MgObjectPropertyTypeToFdoObjectType(objType);
    FdoOrderType fdoOrderType = MgOrderingOptionToFdoOrderType(orderType);

    // Set them to Fdo object property
    if (name != fdoPropDef->GetName())
        fdoPropDef->SetName(name.c_str());

    if (desc != fdoPropDef->GetDescription())
        fdoPropDef->SetDescription(desc.c_str());

    if (fdoObjectType != fdoPropDef->GetObjectType())
        fdoPropDef->SetObjectType(fdoObjectType);

    if (fdoOrderType != fdoPropDef->GetOrderType())
        fdoPropDef->SetOrderType(fdoOrderType);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureUtil.UpdateObjectPropertyDefinition")
}


//////////////////////////////////////////////////////////////////
void MgdFeatureUtil::UpdateGeometricPropertyDefinition(
    MgGeometricPropertyDefinition* mgPropDef,
    FdoGeometricPropertyDefinition* fdoPropDef)
{
    CHECKNULL((MgGeometricPropertyDefinition*)mgPropDef, L"MgdFeatureUtil.UpdateGeometricPropertyDefinition");
    CHECKNULL((FdoGeometricPropertyDefinition*)fdoPropDef, L"MgdFeatureUtil.UpdateGeometricPropertyDefinition");

    MG_FEATURE_SERVICE_TRY()

    //Get data members
    STRING desc = mgPropDef->GetDescription();
    INT32 geomTypes = mgPropDef->GetGeometryTypes();
    Ptr<MgGeometryTypeInfo> geomTypeInfo = mgPropDef->GetSpecificGeometryTypes();
    bool hasElev = mgPropDef->GetHasElevation();
    bool hasMeasure = mgPropDef->GetHasMeasure();
    STRING qname = mgPropDef->GetQualifiedName();
    bool isReadOnly = mgPropDef->GetReadOnly();
    STRING spatialContextName = mgPropDef->GetSpatialContextAssociation();

    //Set it for Fdo
    if (desc != fdoPropDef->GetDescription())
        fdoPropDef->SetDescription((FdoString*) desc.c_str());

    FdoGeometryType geomTypeList[MG_MAX_GEOMETRY_TYPE_SIZE];
    FdoInt32 geomTypeCount = (FdoInt32) geomTypeInfo->GetCount();
    FdoInt32 i = 0;
    for (i =0 ; i<geomTypeCount && i<MG_MAX_GEOMETRY_TYPE_SIZE; ++i)
    {
        geomTypeList[i] = (FdoGeometryType)geomTypeInfo->GetType((INT32)i);
    }

    FdoGeometryType     * fdoGeomTypes = NULL;
    FdoInt32              fdoGeomTypeCount = 0;
    fdoGeomTypes = fdoPropDef->GetSpecificGeometryTypes(fdoGeomTypeCount);
    if( fdoGeomTypeCount != geomTypeCount )
        fdoPropDef->SetSpecificGeometryTypes(geomTypeList, geomTypeCount);
    else
    {
        FdoInt32 j = 0;
        for (i = 0; i < geomTypeCount; ++i)
        {
            for (j = 0; j < fdoGeomTypeCount; j++)
            {
                if( fdoGeomTypes[j] == geomTypeList[i] )
                    break;
            }
            if (j >= fdoGeomTypeCount) break;
        }
        if (i < geomTypeCount)
            fdoPropDef->SetSpecificGeometryTypes(geomTypeList, geomTypeCount);
    }

    if (geomTypes != fdoPropDef->GetGeometryTypes())
        fdoPropDef->SetGeometryTypes((FdoInt32)geomTypes);

    if (hasElev != fdoPropDef->GetHasElevation())
        fdoPropDef->SetHasElevation(hasElev);

    if (hasMeasure != fdoPropDef->GetHasMeasure())
        fdoPropDef->SetHasMeasure(hasMeasure);

    if (isReadOnly != fdoPropDef->GetReadOnly())
        fdoPropDef->SetReadOnly(isReadOnly);

    if(spatialContextName != fdoPropDef->GetSpatialContextAssociation())
        fdoPropDef->SetSpatialContextAssociation((FdoString*)spatialContextName.c_str());

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureUtil.UpdateGeometricPropertyDefinition")
}


//////////////////////////////////////////////////////////////////
void MgdFeatureUtil::UpdateRasterPropertyDefinition(
    MgRasterPropertyDefinition* mgPropDef,
    FdoRasterPropertyDefinition* fdoPropDef)
{
    CHECKNULL((MgRasterPropertyDefinition*)mgPropDef, L"MgdFeatureUtil.UpdateRasterPropertyDefinition");
    CHECKNULL((FdoRasterPropertyDefinition*)fdoPropDef, L"MgdFeatureUtil.UpdateRasterPropertyDefinition");

    MG_FEATURE_SERVICE_TRY()

    //Get data members
    STRING desc = mgPropDef->GetDescription();
    INT32 xsize = mgPropDef->GetDefaultImageXSize();
    INT32 ysize = mgPropDef->GetDefaultImageYSize();
    bool isNullable = mgPropDef->GetNullable();
    STRING qname = mgPropDef->GetQualifiedName();
    bool isReadOnly = mgPropDef->GetReadOnly();
    STRING spatialContextName = mgPropDef->GetSpatialContextAssociation();

    //Set it for Fdo
    if (desc != fdoPropDef->GetDescription())
        fdoPropDef->SetDescription((FdoString*) desc.c_str());

    if (xsize != fdoPropDef->GetDefaultImageXSize())
        fdoPropDef->SetDefaultImageXSize((FdoInt32)xsize);

    if (ysize != fdoPropDef->GetDefaultImageYSize())
        fdoPropDef->SetDefaultImageYSize((FdoInt32)ysize);

    if (isNullable != fdoPropDef->GetNullable())
        fdoPropDef->SetNullable(isNullable);

    //Cannot set qualified name in fdo
    if (isReadOnly != fdoPropDef->GetReadOnly())
        fdoPropDef->SetReadOnly(isReadOnly);

    if (spatialContextName != fdoPropDef->GetSpatialContextAssociation())
        fdoPropDef->SetSpatialContextAssociation((FdoString*)spatialContextName.c_str());

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureUtil.UpdateRasterPropertyDefinition")
}

MgClassDefinition* MgdFeatureUtil::CloneMgClassDefinition(MgClassDefinition* classDef)
{
    CHECKNULL((MgClassDefinition*)classDef, L"MgdFeatureUtil.CloneMgClassDefinition");

    Ptr<MgClassDefinition> clone;

    MG_FEATURE_SERVICE_TRY()

    Ptr<MgMemoryStreamHelper> msh = new MgMemoryStreamHelper();
    Ptr<MgStream> stream = new MgStream(msh);

    classDef->Serialize(stream);
    clone = new MgClassDefinition();
    clone->Deserialize(stream);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureUtil.CloneMgClassDefinition")

    return clone.Detach();
}

MgFeatureSchema* MgdFeatureUtil::CloneMgSchema(MgFeatureSchema* schema)
{
    CHECKNULL((MgFeatureSchema*)schema, L"MgdFeatureUtil.CloneMgSchema");

    Ptr<MgFeatureSchema> clone;

    MG_FEATURE_SERVICE_TRY()

    clone = new MgFeatureSchema(schema->GetName(), schema->GetDescription());
    Ptr<MgClassDefinitionCollection> clonedClasses = clone->GetClasses();
    Ptr<MgClassDefinitionCollection> classes = schema->GetClasses();
    INT32 classCount = classes->GetCount();
    for (INT32 i = 0; i < classCount; i++)
    {
        Ptr<MgClassDefinition> klass = classes->GetItem(i);
        Ptr<MgClassDefinition> clonedClass = CloneMgClassDefinition(klass);

        CHECKNULL((MgClassDefinition*)clonedClass, L"MgdFeatureUtil.CloneMgSchema");
        clonedClasses->Add(clonedClass);        
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureUtil.CloneMgSchema")

    return clone.Detach();
}

void MgdFeatureUtil::UpdateFdoPropertyValue(FdoPropertyValue* fp, MgProperty* srcProp)
{
    CHECKNULL((FdoPropertyValue*)fp, L"MgdFeatureUtil.UpdateFdoPropertyValue");
    CHECKNULL((MgProperty*)srcProp, L"MgdFeatureUtil.UpdateFdoPropertyValue");

    FdoPtr<FdoValueExpression> expr = fp->GetValue();
    CHECKNULL((FdoValueExpression*)expr, L"MgdFeatureUtil.UpdateFdoPropertyValue");

    switch(srcProp->GetPropertyType())
    {
    case MgPropertyType::Blob:
    case MgPropertyType::Clob:
        {
            FdoLOBValue* pVal = static_cast<FdoLOBValue*>(expr.p);

            Ptr<MgByteReader> value = ((MgBlobProperty*)srcProp)->GetValue();
            MgByteSink sink(value);
            Ptr<MgByte> byte = sink.ToBuffer();
            INT32 length = byte->GetLength();
            BYTE_ARRAY_OUT bytes = byte->Bytes();
            FdoPtr<FdoByteArray> array = FdoByteArray::Create((FdoByte*)bytes, length);
            pVal->SetData(array);
        }
        break;
    case MgPropertyType::Boolean:
        {
            FdoBooleanValue* pVal = static_cast<FdoBooleanValue*>(expr.p);
            bool value = ((MgBooleanProperty*)srcProp)->GetValue();
            pVal->SetBoolean(value);
        }
        break;
    case MgPropertyType::Byte:
        {
            FdoByteValue* pVal = static_cast<FdoByteValue*>(expr.p);
            
            BYTE value = ((MgByteProperty*)srcProp)->GetValue();
            pVal->SetByte(value);
        }
        break;
    case MgPropertyType::DateTime:
        {
            FdoDateTimeValue* pVal = static_cast<FdoDateTimeValue*>(expr.p);
            
            Ptr<MgDateTime> value = ((MgDateTimeProperty*)srcProp)->GetValue();
            FdoDateTime time;
            time.day = (FdoInt8)value->GetDay();
            time.hour = (FdoInt8)value->GetHour();
            time.minute = (FdoInt8)value->GetMinute();
            time.month = (FdoInt8)value->GetMonth();
            time.seconds = (float)(value->GetSecond() + (value->GetMicrosecond() / 1000000.0));
            time.year = (FdoInt16)value->GetYear();
            pVal->SetDateTime(time);
        }
        break;
    case MgPropertyType::Decimal:
    case MgPropertyType::Double:
        {
            FdoDoubleValue* pVal = static_cast<FdoDoubleValue*>(expr.p);
            double value = ((MgDoubleProperty*)srcProp)->GetValue();
            pVal->SetDouble(value);
        }
        break;
    case MgPropertyType::Geometry:
        {
            FdoGeometryValue* pVal = static_cast<FdoGeometryValue*>(expr.p);
            
            Ptr<MgByteReader> value = ((MgGeometryProperty*)srcProp)->GetValue();
            MgByteSink sink(value);
            Ptr<MgByte> byte = sink.ToBuffer();
            INT32 length = byte->GetLength();
            BYTE_ARRAY_OUT bytes = byte->Bytes();

            FdoPtr<FdoByteArray> array = FdoByteArray::Create((FdoByte*)bytes, length);
            pVal->SetGeometry(array);
        }
        break;
    case MgPropertyType::Int16:
        {
            FdoInt16Value* pVal = static_cast<FdoInt16Value*>(expr.p);
            
            INT16 value = ((MgInt16Property*)srcProp)->GetValue();
            pVal->SetInt16(value);
        }
        break;
    case MgPropertyType::Int32:
        {
            FdoInt32Value* pVal = static_cast<FdoInt32Value*>(expr.p);
            
            INT32 value = ((MgInt32Property*)srcProp)->GetValue();
            pVal->SetInt32(value);
        }
        break;
    case MgPropertyType::Int64:
        {
            FdoInt64Value* pVal = static_cast<FdoInt64Value*>(expr.p);
            
            INT64 value = ((MgInt64Property*)srcProp)->GetValue();
            pVal->SetInt64(value);
        }
        break;
    case MgPropertyType::Single:
        {
            FdoSingleValue* pVal = static_cast<FdoSingleValue*>(expr.p);
            
            float value = ((MgSingleProperty*)srcProp)->GetValue();
            pVal->SetSingle(value);
        }
        break;
    case MgPropertyType::String:
        {
            FdoStringValue* pVal = static_cast<FdoStringValue*>(expr.p);
            
            STRING value = ((MgStringProperty*)srcProp)->GetValue();
            pVal->SetString(value.c_str());
        }
        break;
    }
}

FdoIdentifierCollection* MgdFeatureUtil::ExtractIdentifiers(FdoExpression* expr)
{
    CHECKNULL(expr, L"MgdFeatureUtil.ExtractIdentifiers");

    FdoPtr<FdoIdentifierCollection> ret;
    MG_FEATURE_SERVICE_TRY()

    ret = FdoIdentifierCollection::Create();

    switch(expr->GetExpressionType())
    {
    case FdoExpressionItemType_ComputedIdentifier:
        {
            FdoComputedIdentifier* comp = static_cast<FdoComputedIdentifier*>(expr);
            FdoPtr<FdoExpression> inner = comp->GetExpression();

            FdoPtr<FdoIdentifierCollection> result = ExtractIdentifiers(inner);
            for (FdoInt32 i = 0; i < result->GetCount(); i++)
            {
                FdoPtr<FdoIdentifier> resultItem = result->GetItem(i);
                ret->Add(resultItem);
            }
        }
        break;
    case FdoExpressionItemType_Function:
        {
            FdoFunction* func = static_cast<FdoFunction*>(expr);
            FdoExpressionCollection* funcArgs = func->GetArguments();
            for (FdoInt32 i = 0; i < funcArgs->GetCount(); i++)
            {
                FdoPtr<FdoExpression> arg = funcArgs->GetItem(i);
                FdoPtr<FdoIdentifierCollection> result = ExtractIdentifiers(arg);
                for (FdoInt32 j = 0; j < result->GetCount(); j++)
                {
                    FdoPtr<FdoIdentifier> resultItem = result->GetItem(j);
                    ret->Add(resultItem);
                }
            }
        }
        break;
    case FdoExpressionItemType_Identifier:
        ret->Add(static_cast<FdoIdentifier*>(expr));
        break;
    case FdoExpressionItemType_UnaryExpression:
        {
            FdoUnaryExpression* unexpr = static_cast<FdoUnaryExpression*>(expr);
            FdoExpression* inner = unexpr->GetExpression();

            FdoPtr<FdoIdentifierCollection> result = ExtractIdentifiers(inner);
            for (FdoInt32 i = 0; i < result->GetCount(); i++)
            {
                FdoPtr<FdoIdentifier> resultItem = result->GetItem(i);
                ret->Add(resultItem);
            }
        }
        break;
    case FdoExpressionItemType_SubSelectExpression:
        {
            FdoSubSelectExpression* subSelect = static_cast<FdoSubSelectExpression*>(expr);
            FdoPtr<FdoIdentifier> propName = subSelect->GetPropertyName();
            ret->Add(propName);
        }
        break;
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureUtil.ExtractIdentifiers")
    return ret.Detach();
}
