#ifndef DESKTOP_FEATURE_UTIL
#define DESKTOP_FEATURE_UTIL

#include "MgDesktop.h"
#include "Fdo.h"

#define EQUAL_CATEGORY      0
#define STDEV_CATEGORY      1
#define QUANTILE_CATEGORY   2
#define JENK_CATEGORY       3
#define MINIMUM             4
#define MAXIMUM             5
#define MEAN                6
#define STANDARD_DEV        7
#define UNIQUE              8
#define EXTENT              9

typedef vector <double> VECTOR;
typedef vector <INT64> VECTOR_INT64;

#define DoubleMaxValue  DBL_MAX

#define DoubleNegativeInfinity numeric_limits<double>::infinity()
#define DoublePositiveInfinity numeric_limits<double>::infinity()

static const STRING MG_SCHEMA_NAME_HASH_PREFIX      = L"sn"; // NOXLATE
static const STRING MG_FEATURE_SOURCE_HASH_PREFIX   = L"ns"; // NOXLATE

class MgReader;

class MG_DESKTOP_API MgdFeatureUtil
{
public:

    static INT32 GetMgPropertyType(FdoDataType fdoDataType);
    static FdoOrderingOption GetFdoOrderingOption(INT32 orderingOption);
    static STRING GetMessage(CREFSTRING messageId);
    static bool FindCustomFunction(CREFSTRING funcName, INT32 &index);
    static INT16 GetMgPropertyType(MgPropertyDefinition* propDef);
    static MgPropertyDefinitionCollection* GetPropertyDefinitions(MgReader* reader);
    static INT32 GetPropertyDefinition(MgReader* reader, REFSTRING propName);
    static bool FindCustomFunction(FdoFunction* customFunction, INT32 &index);
    static void GetArguments(FdoFunction* customFunction, REFSTRING name, int &numCats,
                             double& dataMin, double& dataMax, INT16 type);

    static void ValidateCustomConstraints(FdoFunction* customFunction);
    static double Minimum(VECTOR& v);
    static INT64 Minimum(VECTOR_INT64& v);
    static double Maximum(VECTOR& v);
    static INT64 Maximum(VECTOR_INT64& v);
    static FdoSpatialOperations GetFdoSpatialOperation(INT32 spatialOp);
    static INT32 GetMgFeaturePropertyType(FdoPropertyType fdoPropType);
    static MgRaster* GetMgRaster(FdoIRaster* raster, STRING propName);
    static MgByteReader* GetRaster(FdoIReader* reader, CREFSTRING rasterPropName,INT32 xSize, INT32 ySize);
    static FdoPropertyValue* MgPropertyToFdoProperty(MgProperty* srcProp);
    static FdoParameterValue* MgPropertyToFdoParameter(MgProperty* srcProp);
    static FdoParameterValue* MgParameterToFdoParameter(MgParameter* srcParam);
    static FdoLiteralValue* MgPropertyToFdoDataValue(MgProperty* srcProp);
    static MgProperty* FdoParameterValueToMgProperty(FdoParameterValue* fdoParamValue);
    static MgParameter* FdoParameterValueToMgParameter(FdoParameterValue* fdoParamValue);
    static FdoParameterValueCollection* CreateFdoParameterCollection(MgPropertyCollection* srcCol);
    static void FillFdoPropertyCollection(MgPropertyCollection* srcCol, FdoPropertyValueCollection* paramCol);
    static void FillFdoParameterCollection(MgParameterCollection* source, FdoParameterValueCollection* target);
    static void FillParameterCollection(FdoParameterValueCollection* source, MgParameterCollection* target);
    static void UpdateParameterCollection(FdoParameterValueCollection* source, MgParameterCollection* target);
    static INT32 FdoOrderTypeToMgOrderingOption(FdoOrderType type);
    static FdoOrderType MgOrderingOptionToFdoOrderType(INT32 type);
    static INT32 FdoObjectTypeToMgObjectPropertyType(FdoObjectType type);
    static FdoObjectType MgObjectPropertyTypeToFdoObjectType(INT32 type);
    static bool FdoClassExist(const wchar_t* name, FdoClassCollection* clsCol);

    static MgStringCollection* FdoToMgStringCollection(FdoStringCollection* fdoStrs, bool includeEmptyStrings);
    static FdoStringCollection* MgToFdoStringCollection(MgStringCollection* mgStrs, bool includeEmptyStrings);

    static MgClassDefinition* GetMgClassDefinition(FdoClassDefinition* fdoClassDefinition, bool bSerialize);
    static MgByteReader* SerializeToXml(FdoClassDefinition* classDef);
    static void GetClassProperties(MgPropertyDefinitionCollection* propDefCol, FdoPropertyDefinitionCollection* fdoPropDefCol);
    static void GetClassProperties(MgPropertyDefinitionCollection* propDefCol, FdoDataPropertyDefinitionCollection* fdoPropDefCol);
    static MgPropertyDefinition* GetMgPropertyDefinition(FdoPropertyDefinition* fdoPropDef);
    static MgDataPropertyDefinition* GetDataPropertyDefinition(FdoDataPropertyDefinition* fdoPropDef);
    static MgObjectPropertyDefinition* GetObjectPropertyDefinition(FdoObjectPropertyDefinition* fdoPropDef);
    static MgGeometricPropertyDefinition* GetGeometricPropertyDefinition(FdoGeometricPropertyDefinition* fdoPropDef);
    static MgRasterPropertyDefinition* GetRasterPropertyDefinition(FdoRasterPropertyDefinition* fdoPropDef);
    static MgProperty* GetMgProperty(MgReader* reader, CREFSTRING qualifiedPropName, INT16 type);

    static FdoFeatureSchemaCollection* GetFdoFeatureSchemaCollection(MgFeatureSchemaCollection* mgSchemaCol);
    static FdoFeatureSchema* GetFdoFeatureSchema(MgFeatureSchema* mgSchema);
    static void GetFdoClassCollection(FdoClassCollection* fdoClassCol, MgClassDefinitionCollection* mgClassDefCol);
    static FdoClassDefinition* GetFdoClassDefinition(MgClassDefinition* mgClassDef, FdoClassCollection* fdoClassCol);
    static void GetClassProperties(FdoPropertyDefinitionCollection* fdoPropDefCol,
                            MgPropertyDefinitionCollection* propDefCol,
                            FdoClassCollection* fdoClassCol);
    static void GetClassProperties(FdoDataPropertyDefinitionCollection* fdoPropDefCol,
                            MgPropertyDefinitionCollection* mgPropDefCol);
    static FdoPropertyDefinition* GetFdoPropertyDefinition(MgPropertyDefinition* mgPropDef, FdoClassCollection* fdoClassCol);
    static FdoDataPropertyDefinition* GetDataPropertyDefinition(MgDataPropertyDefinition* mgPropDef);
    static FdoGeometricPropertyDefinition* GetGeometricPropertyDefinition(MgGeometricPropertyDefinition* mgPropDef);
    static FdoRasterPropertyDefinition* GetRasterPropertyDefinition(MgRasterPropertyDefinition* mgPropDef);
    static FdoDataType GetFdoDataType(INT32 awPropType);
    static FdoObjectPropertyDefinition* GetObjectPropertyDefinition(MgObjectPropertyDefinition* objPropDef, FdoClassCollection* fdoClassCol);
    static FdoParameterDirection GetFdoParameterDirection(INT32 paramDirection);
    static INT32 GetMgParameterDirection(FdoParameterDirection fdoParamDirection);

    static void UpdateFdoFeatureSchema(MgFeatureSchema* mgSchema, FdoFeatureSchema* fdoSchema);
    static void UpdateFdoClassCollection(MgClassDefinitionCollection* mgClassDefCol, FdoClassCollection* fdoClassCol);
    static void UpdateFdoClassDefinition(MgClassDefinition* mgClassDef, FdoClassDefinition* fdoClassDef, FdoClassCollection* fdoClassCol);
    static void UpdateClassProperties(MgPropertyDefinitionCollection* propDefCol, FdoPropertyDefinitionCollection* fdoPropDefCol, FdoClassCollection* fdoClassCol = NULL);
    static void UpdateClassProperties(FdoDataPropertyDefinitionCollection* fdoPropDefCol, MgPropertyDefinitionCollection* mgPropDefCol);
    static void UpdateFdoPropertyDefinition(MgPropertyDefinition* mgPropDef, FdoPropertyDefinition* fdoPropDef, FdoClassCollection* fdoClassCol);
    static void UpdateDataPropertyDefinition(MgDataPropertyDefinition* mgPropDef, FdoDataPropertyDefinition* fdoPropDef);
    static void UpdateObjectPropertyDefinition(MgObjectPropertyDefinition* objPropDef, FdoObjectPropertyDefinition* fdoPropDef, FdoClassCollection* fdoClassCol);
    static void UpdateGeometricPropertyDefinition(MgGeometricPropertyDefinition* mgPropDef, FdoGeometricPropertyDefinition* fdoPropDef);
    static void UpdateRasterPropertyDefinition(MgRasterPropertyDefinition* mgPropDef, FdoRasterPropertyDefinition* fdoPropDef);

    static MgClassDefinition* CloneMgClassDefinition(MgClassDefinition* classDef);
    static MgFeatureSchema* CloneMgSchema(MgFeatureSchema* schema);
    static void UpdateFdoPropertyValue(FdoPropertyValue* fp, MgProperty* prop);
    static FdoIdentifierCollection* ExtractIdentifiers(FdoExpression* expr);
private:
    static bool Initialize();

private:
    static bool m_isInitialized;
};

#endif