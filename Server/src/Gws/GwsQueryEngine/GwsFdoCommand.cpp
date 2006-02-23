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

///////////////////////////////////////////////////////////////////////////////
//
// Includes
//
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "GwsQueryEngineImp.h"


///////////////////////////////////////////////////////////////////////////////
//
// class CGwsFdoCommand
//
///////////////////////////////////////////////////////////////////////////////
CGwsFdoCommand::CGwsFdoCommand(
)
{
    m_revisionprop = L""; //
    m_geometricSC = L"";
    m_bIsPrepared = false;
    m_bSupportBlobs = false;
    m_bSupportClobs = false;
    m_bSkipUnsupportedProperties = true;
    m_bSupportLocking = false;
}

CGwsFdoCommand::CGwsFdoCommand(
    FdoIConnection           * sess,
    const GWSQualifiedName   & classname
)
{
    assert (sess !=  NULL);
    m_connection =  sess;
    sess->AddRef ();

    m_classname = classname;
    m_revisionprop = L""; //
    m_geometricSC = L"";
    m_bIsPrepared = false;
    m_bSupportBlobs = false;
    m_bSupportClobs = false;
    m_bSkipUnsupportedProperties = true;
    m_bSupportLocking = false;
}

///////////////////////////////////////////////////////////////////////////////
CGwsFdoCommand::~CGwsFdoCommand ()
{
    ObjectPropCommands::iterator iter;
    for (iter = m_objPropCommands.begin (); iter != m_objPropCommands.end (); iter ++) {
        CGwsFdoCommand* pCommand = (*iter).second;
        delete pCommand;
    }
    m_objPropCommands.clear();
}

///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoCommand::Init (const wchar_t* pFDOCommandClass /*NULL*/)
{
    pFDOCommandClass; // For "unreferenced formal parameter" warning

    // initilaize class definition
    GwsCommonFdoUtils::GetClassDefinition (m_connection,
                                           m_classname,
                                           m_schema.p,
                                           m_classDef.p);

    return eGwsOk;
}

///////////////////////////////////////////////////////////////////////////////
void CGwsFdoCommand::SetCSConverter (
    IGWSCoordinateSystemConverter * converter
)
{
    m_converter = converter;
    if (converter)
        converter->AddRef ();
}

///////////////////////////////////////////////////////////////////////////////
void CGwsFdoCommand::GetCSConverter (
    IGWSCoordinateSystemConverter ** converter
)
{
    * converter = m_converter;
    if (* converter)
        (* converter)->AddRef ();
}


///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoCommand::Prepare ()
{
    EGwsStatus stat = eGwsOk;

    try {
        assert(m_classDef != NULL);

        if (!GwsCommonFdoUtils::GetFdoClassIdentityProperties (m_classDef, m_identity.p)) {
            PushStatus (eGwsFailed);
            return eGwsFailed;
        }
        GisPtr<FdoISchemaCapabilities> ptrCap;
        ptrCap = m_connection->GetSchemaCapabilities();
        assert (ptrCap);
        m_bSupportBlobs = false;
        m_bSupportClobs = false;
        if (ptrCap) {
            GisInt32 len = 0;
            // we want to skip these properties
            FdoDataType* types = ptrCap->GetDataTypes(len);
            for (int i = 0; i < len; i ++) {
                if (types [i] == FdoDataType_BLOB)
                    m_bSupportBlobs = true;
                else if (types [i] == FdoDataType_CLOB)
                    m_bSupportClobs = true;
            }
        }
        m_bSupportLocking = false;
        GisPtr<FdoIConnectionCapabilities> connCap;
        connCap = m_connection->GetConnectionCapabilities();
        if (connCap) {
            if (connCap->SupportsLocking()) {
                GisPtr<FdoClassCapabilities> cap = m_classDef->GetCapabilities ();
                if (cap != NULL) {
                    m_bSupportLocking = cap->SupportsLocking ();
                }
            }
        }

        DiscoverRevisionProperty ();

        DiscoverActiveLongTransaction ();

        PrepareInternal ();

        m_pCommand->Prepare();

        m_bIsPrepared = true;

    } catch (GisException * ex) {
        PushGisException (eGwsFailed, ex);
        ex->Release ();
        stat = eGwsFailed;

    }
    return stat;
}

#pragma message ("Re-implement QualifiedClassName() when FdoClassDefinition's NULL parent bug is fixed.")
WSTR CGwsFdoCommand::QualifiedClassName()
{
    return GwsCommonFdoUtils::MakeFdoQualifiedName (m_classname);
}


///////////////////////////////////////////////////////////////////////////////
FdoPropertyDefinition * CGwsFdoCommand::GetPropertyDefinition (GisString * propname)
{
    PropertyDefinitionMap::iterator iter = m_propdefs.find (propname);
    if (iter == m_propdefs.end ()) {
        return NULL;
    }
    FdoPropertyDefinition * pDef = (*iter).second;
    if (pDef != NULL) {
        pDef->AddRef ();
    }
    return pDef;
}

///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoCommand::Execute (CGwsMutableFeature & feature)
{
    feature; // For "unreferenced formal parameter" warning
    return eGwsNotSupported;
}

///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoCommand::Execute (IGWSMutableFeature * feature)
{
    CGwsMutableFeature * mfeature = dynamic_cast<CGwsMutableFeature *> (feature);

    if (mfeature == NULL)
        return eGwsFailed;
    return Execute (*mfeature);
}


///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoCommand::Execute (const GWSFeatureId & featid)
{
    featid; // For "unreferenced formal parameter" warning
    return eGwsNotSupported;
}

///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoCommand::Execute (const GwsFeaturesIdVector & featids)
{
    return Execute (featids, 0, (int)featids.size ());
}

///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoCommand::Execute (
    const GwsFeaturesIdVector    & featids,
    int                            lbound,
    int                            ubound
)
{
    featids; // For "unreferenced formal parameter" warning
    lbound; // For "unreferenced formal parameter" warning
    ubound; // For "unreferenced formal parameter" warning
    return eGwsNotSupported;
}

///////////////////////////////////////////////////////////////////////////////
void CGwsFdoCommand::PrepareInternal ()
{
}

///////////////////////////////////////////////////////////////////////////////
void CGwsFdoCommand::PrepareNonKeyProperties ()
{
    GisInt32 idx;

    GisPtr<FdoClassDefinition>  classDef = m_classDef;
    m_geometricprop.clear ();
    for (; classDef != NULL ; classDef = classDef->GetBaseClass ())
    {
        GisPtr<FdoPropertyDefinitionCollection> propdsc = classDef->GetProperties ();
        // discover geometric property name. Use the first one if there are many.
        for (idx = 0; idx < propdsc->GetCount(); idx ++) {
            GisPtr<FdoPropertyDefinition>   prop;
            prop = propdsc->GetItem (idx);

            m_propdefs.insert(PropertyDefinitionMap::value_type(prop->GetName (), prop));

            if (m_geometricprop.empty () &&
                prop->GetPropertyType () == FdoPropertyType_GeometricProperty)
            {
                m_geometricprop = prop->GetName ();
                FdoGeometricPropertyDefinition* geomProp = static_cast<FdoGeometricPropertyDefinition*>(prop.p);
                GisString* pSC = geomProp->GetSpatialContextAssociation();
                if(pSC != NULL) {
                    m_geometricSC = pSC;

                    // need to initialize converter for every command
                    // spatial filter needs to be processed
                    if (m_converter) {
                        // check to see whether converter is already
                        // initialized by the source cs system
                        if (* m_converter->SourceCS () == 0) {
                            GwsSpatialContextDescription scdesc;
                            EGwsStatus stat =
                                GwsCommonFdoUtils::DescribeSC (m_connection,
                                                            m_geometricSC.c_str (),
                                                            scdesc);
                            // cordinate system may not be recogenized
                            // currently this fact is silently ignored
                            // Should we throw an exception instead?
                            if (!IGWSException::IsError (stat))
                                m_converter->SetSourceCS (scdesc.CsNameWkt ());
                        }
                    }
                }
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
void CGwsFdoCommand::PrepareKeyProperties ()
{
    // this method is void, since nothing is needed if parameters are not
    // used
}

void CGwsFdoCommand::PrepareObjectProperties ()
{
#ifdef IMPLEMENT_THIS_METHOD // and test
    GisPtr<FdoClassDefinition>  classDef = m_classDef;
    for (; classDef != NULL; classDef = classDef->GetBaseClass ()) {
        GisPtr<FdoPropertyDefinitionCollection> propdsc = classDef->GetProperties();
        for (GisInt32 idx = 0; idx < propdsc->GetCount (); idx ++) {
            GisPtr<FdoPropertyDefinition>   prop;
            prop = propdsc->GetItem (idx);
            assert (prop != NULL);
            if (prop->GetPropertyType () == FdoPropertyType_ObjectProperty)
            {
                //cache the object property names and the insert commands for those classes
                FdoObjectPropertyDefinition * objprop = static_cast<FdoObjectPropertyDefinition*>(prop.p);
                m_objPropNames.push_back(objprop->GetName());

                const std::wstring objectClassNameForInsert =
                    acmapFdoUtils::constructObjectClassName(classDef->GetName(),objprop->GetName());
                //save these for later, after the main feature is inserted
                ObjectPropCommands::iterator it = m_objPropCommands.find(objectClassNameForInsert);
                //should not be there
                assert(it == m_objPropCommands.end());

                GisPtr<FdoClassDefinition> pDef = objprop->GetClass();
                GisPtr<FdoFeatureSchema> pSchema = pDef->GetFeatureSchema();

                const char* pFeatureSourceName = NULL;
                const AcMapFdoFeatureSource* pFS = Session()->FeatureSource();
                if(pFS)
                    pFeatureSourceName = pFS->Name();
                assert(pFeatureSourceName != NULL);

                GWSQualifiedName objectClassName(pFeatureSourceName, wTOa(pSchema->GetName()), wTOa(pDef->GetName()));
                AcMapFdoInsertCommand* pInsertCmd = new AcMapFdoInsertCommand(m_pSession, objectClassName);

                eFdoOkThrow(pInsertCmd->Init(objectClassNameForInsert.c_str()));
                eFdoOkThrow(pInsertCmd->Prepare());

                m_objPropCommands.insert(ObjectPropCommands::value_type(objectClassNameForInsert, pInsertCmd));
            }
        }
    }
#endif
}

void CGwsFdoCommand::PrepareFilter (FdoFilter * filter)
{
    if (filter == NULL)
        return;
    if (m_converter == NULL)
        return;
    CGwsCSQueryProcessor qcconverter (m_converter);
    filter->Process (& qcconverter);
    if (IGWSException::IsError (qcconverter.Status()))
        GWS_THROW (qcconverter.Status());

    m_bFilterPrepared = true;
}


void CGwsFdoCommand::SetFeatureCommandFilter (FdoFilter * filter)
{
    if (m_pCommand == NULL)
        GWS_THROW (eGwsFdoQueryIsNotPrepared);
    SetFilterInternal (filter);
    m_bFilterPrepared = false;
}


///////////////////////////////////////////////////////////////////////////////
void CGwsFdoCommand::DiscoverRevisionProperty ()
{
    m_revisionprop = GwsCommonFdoUtils::GetRevisionProperty (m_classDef);
}

///////////////////////////////////////////////////////////////////////////////
void CGwsFdoCommand::DiscoverActiveLongTransaction ()
{
    // TODO: implelemt this method
    // m_pSession->GetLongTransactionManager ()->Implementation ()->GetCurrentLT (m_activelt);
}


///////////////////////////////////////////////////////////////////////////////
void CGwsFdoCommand::SetValue (
    FdoPropertyValue      *  pPropVal
)
{
    FdoPropertyValueCollection * pPropertyValues = GetPropertyValues();
    assert(pPropertyValues);

    GisPtr<FdoPropertyValue>     pPropertyValue;
    GisPtr<FdoIdentifier>        ident = pPropVal->GetName ();

    pPropertyValue = pPropertyValues->FindItem (ident->GetName ());
    if (pPropertyValue != NULL) {
        pPropertyValue->SetValue(pPropVal->GetValue ());
    } else {
        pPropertyValues->Add(pPropVal);
    }
}

///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoCommand::SetProperties (CGwsMutableFeature & feature)
{
    EGwsStatus stat = eGwsOk;

    for (int i = 0; i < feature.GetCount (); i ++) {
        GisPtr<FdoPropertyValue> val = feature.GetPropertyValue (i);
        GisPtr<FdoIdentifier> ident = val->GetName ();
        PropertyDefinitionMap::iterator iter = m_propdefs.find (ident->GetName ());
        if (iter == m_propdefs.end ()) {
            stat = eGwsInvalidPropertyName;
            break;
        }
        if ((*iter).second->GetPropertyType () == FdoPropertyType_DataProperty) {
            FdoDataPropertyDefinition * dataprop = static_cast<FdoDataPropertyDefinition*>((*iter).second);

            if (m_bSkipUnsupportedProperties) {

                // skipping these properties
                if (dataprop->GetDataType () == FdoDataType_BLOB &&
                    ! m_bSupportBlobs)
                    continue;

                if (dataprop->GetDataType () == FdoDataType_CLOB &&
                    ! m_bSupportClobs)
                    continue;
            }

            if (! dataprop->GetReadOnly ()) {
                SetValue (val);
            }
        } else if ((*iter).second->GetPropertyType () == FdoPropertyType_GeometricProperty) {
            // applying conversion
            GisPtr<FdoPropertyValue> cval = ConvertGeometryProperty (
                                                    feature.GetCSName (),
                                                    val);
            SetValue (cval);
        }
    }

    return stat;
}

FdoPropertyValue * CGwsFdoCommand::ConvertGeometryProperty (
    const GWSCoordinateSystem & csname,
    FdoPropertyValue          * val
)
{
    // no conversion
    if (m_converter == NULL ||
        csname.IsEmpty () == 0 ||
        m_converter->SourceCS ().IsEmpty ())
    {
        val->AddRef ();
        return val;
    }

    // initialize converter by the destination cs
    // potential subject of optimization.
    eGwsOkThrow (m_converter->SetDestinationCS (csname));

    // cs converter converts in place. In order not to
    // change source geometry, create a copy of it
    // not very efficient, but looks like the only way ...
    GisPtr<FdoValueExpression> gval = val->GetValue ();
    GisPtr<FdoIdentifier>      ident = val->GetName ();
    FdoGeometryValue         * geomval = dynamic_cast<FdoGeometryValue *> (gval.p);
    assert (geomval);

    GisPtr<GisByteArray>       barray = geomval->GetGeometry();
    GisPtr<GisByteArray>       carray = GisByteArray::Create (barray->GetData (),
                                                              barray->GetCount ());

    eGwsOkThrow (m_converter->ConvertBackward (carray));

    GisPtr<FdoGeometryValue> cgeomval = FdoGeometryValue::Create (carray);
    return FdoPropertyValue::Create (ident, cgeomval);
}


EGwsStatus CGwsFdoCommand::BuildInFilter (
    const GwsFeaturesIdVector     & featIds,
    int                             lbound,
    int                             ubound,
    FdoFilter                    *& pOutFilter
)
{
    EGwsStatus  stat  = eGwsOk;
    try {
        stat = CGwsFdoCommand::BuildInFilter(m_identity, featIds, lbound, ubound, pOutFilter);
    } catch (GisException *e) {
        PushGisException (eGwsFailed, e);
        e->Release();
        stat = eGwsFailed;
    }
    return stat;
}

///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoCommand::BuildInFilter (
    FdoDataPropertyDefinitionCollection* identity,
    const GwsFeaturesIdVector     & featIds,
    int                             lbound,
    int                             ubound,
    FdoFilter                    *& pOutFilter
)
{
    EGwsStatus                             stat  = eGwsOk;
    GisPtr<FdoValueExpressionCollection>   invalues;
    GisPtr<FdoInCondition>                 inclause;

    assert (identity->GetCount () == 1);
    assert (lbound >= 0);
    assert (ubound <= (int)featIds.size ());

    invalues = FdoValueExpressionCollection::Create ();

    for (int i = lbound; i < ubound; i ++) {
        assert (featIds[i].GetCount () == 1);
        GisPtr<FdoDataValue>      pValue = featIds[i].GetItem (0);
        invalues->Add (pValue);
    }
    GisPtr<FdoPropertyDefinition> prop = identity->GetItem (0);
    GisPtr<FdoIdentifier>         name = FdoIdentifier::Create(prop->GetName());

    inclause = FdoInCondition::Create (name, invalues);

    pOutFilter = inclause;
    if (inclause != NULL)
        inclause.p->AddRef ();

    return stat;
}

EGwsStatus CGwsFdoCommand::BuildFilter (
    const GWSFeatureId       & featId,
    FdoFilter               *& pOutFilter
)
{
    EGwsStatus stat             = eGwsOk;
    try {
        stat = CGwsFdoCommand::BuildFilter(m_identity, featId, pOutFilter);
    } catch (GisException *e) {
        PushGisException (eGwsFailed, e);
        e->Release();
        stat = eGwsFailed;
    }
    return stat;
}

///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoCommand::BuildFilter (
    FdoDataPropertyDefinitionCollection* identity,
    const GWSFeatureId       & featId,
    FdoFilter               *& pOutFilter
)
{
    EGwsStatus stat             = eGwsOk;

    GisPtr<FdoFilter> pFilter;
    GisInt32          idx;

    for (idx = 0; idx < identity->GetCount(); idx ++) {
        GisPtr<FdoPropertyDefinition>   prop;
        prop = identity->GetItem (idx);

        GisPtr<FdoIdentifier> pFeatureNameProperty = FdoIdentifier::Create(prop->GetName());
        GisPtr<FdoDataValue>  pFeatureNameValue = featId.GetItem (idx);
        if (idx == 0) {
            pFilter = FdoComparisonCondition::Create(pFeatureNameProperty,
                                                    FdoComparisonOperations_EqualTo,
                                                    pFeatureNameValue);
        } else {
            GisPtr<FdoFilter> pRhsFilter = FdoComparisonCondition::Create(pFeatureNameProperty,
                                                                        FdoComparisonOperations_EqualTo,
                                                                        pFeatureNameValue);
            GisPtr<FdoFilter> pCombinedFilter = FdoFilter::Combine(pFilter,
                                                                FdoBinaryLogicalOperations_And,
                                                                pRhsFilter);

            pFilter = pCombinedFilter;
        }
    }

    pOutFilter = pFilter;
    if (pFilter != NULL)
        pFilter.p->AddRef ();

    return stat;
}

EGwsStatus CGwsFdoCommand::BuildFilter (
    const GwsFeaturesIdVector    & featids,
    int                            lbound,
    int                            ubound,
    FdoFilter                 *  & pOutFilter
)
{
    EGwsStatus   stat  = eGwsOk;

    try {
        stat = BuildFilter(m_connection, m_identity, featids, lbound, ubound, pOutFilter);
    } catch (GisException *e) {
        PushGisException (eGwsFailed, e);
        e->Release();
        stat = eGwsFailed;
    }
    return stat;
}

///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoCommand::BuildFilter (
    FdoIConnection               * conn,
    FdoDataPropertyDefinitionCollection* identity,
    const GwsFeaturesIdVector    & featids,
    int                            lbound,
    int                            ubound,
    FdoFilter                 *  & pOutFilter
)
{
    assert (lbound >= 0);
    assert (ubound <= (int)featids.size ());

    bool                           bSupportIn = false;
    {
        GisPtr<FdoIFilterCapabilities> fltCap;
        fltCap = conn->GetFilterCapabilities();
        if(fltCap != NULL) {
            GisInt32 size = 0;
            FdoConditionType* types = fltCap->GetConditionTypes(size);
            for (int i = 0; i < size; i ++) {
                if (types [i] == FdoConditionType_In) {
                    bSupportIn = true;
                    break;
                }
            }
        }
    }

    // Build in clause if there is a single identity property
    if (identity->GetCount () == 1 && bSupportIn)
        return BuildInFilter (identity, featids, lbound, ubound, pOutFilter);

    EGwsStatus   stat  = eGwsOk;

    GisPtr<FdoFilter> filter;
    for (int i = lbound; i < ubound; i ++) {
        GisPtr<FdoFilter> f;
        BuildFilter (identity, featids[i], f.p);
        if ( i == 0)
            filter = f;
        else {
            GisPtr<FdoFilter> pCombinedFilter = FdoFilter::Combine(filter,
                                                                    FdoBinaryLogicalOperations_Or,
                                                                    f);
            filter = pCombinedFilter;
        }
    }
    pOutFilter = filter;
    if (filter != NULL)
        filter.p->AddRef ();
    return stat;
}


///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoCommand::BuildFilter (
    IGWSLongFeatureIdSet* idSet,
    FdoFilter  *  & pOutFilter
)
{
    int nCount = idSet->Size();
    assert (nCount >= 0);

    // Use only in case when identity is a single integer property
    if (m_identity->GetCount () != 1)
        return eGwsFdoInvalidPropertyType;

    // do not build in filter (for now)
    EGwsStatus   stat  = eGwsOk;

    try {
        GisPtr<FdoPropertyDefinition>   prop;
        prop = m_identity->GetItem (0);

        // these are data types we support in this case
        if (prop->GetPropertyType () != FdoPropertyType_DataProperty)
            return eGwsFdoInvalidPropertyType;

        FdoDataPropertyDefinition * dataprop = (FdoDataPropertyDefinition *) prop.p;
        if (dataprop->GetDataType () != FdoDataType_Int32 &&
            dataprop->GetDataType () != FdoDataType_Int64)
            return eGwsFdoInvalidPropertyType;

        GisPtr<FdoIdentifier> pName   = FdoIdentifier::Create(prop->GetName());
        GisPtr<FdoFilter>     filter;

        for (int i = 0; i < nCount; i ++) {
            GisPtr<FdoDataValue>  pValue  = FdoInt32Value::Create (idSet->Id(i));
            GisPtr<FdoFilter>     f       = FdoComparisonCondition::Create(
                                                     pName,
                                                     FdoComparisonOperations_EqualTo,
                                                     pValue);
            if ( i == 0)
                filter = f;
            else {
                GisPtr<FdoFilter> pCombinedFilter = FdoFilter::Combine(filter,
                                                                       FdoBinaryLogicalOperations_Or,
                                                                       f);
                filter = pCombinedFilter;
            }
        }
        pOutFilter = filter;
        if (filter != NULL)
            filter.p->AddRef ();
    } catch (GisException *e) {
        PushGisException (eGwsFailed, e);
        e->Release();
        stat = eGwsFailed;
    }
    return stat;
}


///////////////////////////////////////////////////////////////////////////////
FdoPropertyValueCollection * CGwsFdoCommand::GetPropertyValues ()
{
    assert(false); //overidden in some derived classes
    return m_pProperties;
}


///////////////////////////////////////////////////////////////////////////////
void CGwsFdoCommand::ReleasePropertyValues ()
{
    if (m_pProperties) {
        m_pProperties->Clear ();
        m_pProperties.Release ();
    }
    m_pProperties = NULL;
}


///////////////////////////////////////////////////////////////////////////////
// process diagnostics for lock conflicts.
EGwsStatus CGwsFdoCommand::ProcessLockConflicts (
    FdoILockConflictReader      * pReader,
    GwsFailedStatus             & failed
) const
{
    EGwsStatus stat = eGwsOk;

    while (pReader != NULL && pReader->ReadNext()) {
        char                       * str = NULL;
        try {
            GWSExtendedFeatureId featId =
                                  GwsCommonFdoUtils::MakeFeatureId (m_classname,
                                                              pReader->GetIdentity (),
                                                              m_activelt.c_str());
            //  failed.insert (featId.FeatureId ());

            wchar_t wbuff [256];
            featId.ToString (wbuff, 256);

            FdoConflictType     ctype = pReader->GetConflictType ();

            if (ctype == FdoConflictType_LockConflict) {
                GisString      *    owner = pReader->GetLockOwner();
                CGwsStatus stat (eGwsFdoLockConflict);
                stat.SetParameter (L"Username", owner);
                stat.SetParameter (L"FeatureId", wbuff);
                PushStatus (stat);
                failed.insert (GwsFailedStatus::value_type (featId.FeatureId (),stat));

            } else if (ctype == FdoConflictType_VersionConflict) {
                GisString     *     version = pReader->GetLongTransaction ();
                CGwsStatus stat (eGwsFdoVersionConflict);
                stat.SetParameter (L"Version", version);
                stat.SetParameter (L"FeatureId", wbuff);
                PushStatus (stat);
                failed.insert (GwsFailedStatus::value_type (featId.FeatureId (),stat));


            } else {
                CGwsStatus stat (eGwsFdoLockConflict);
                stat.SetParameter (L"FeatureId", wbuff);
                PushStatus (stat);
                failed.insert (GwsFailedStatus::value_type (featId.FeatureId (),stat));
            }

        } catch (GisException * e) {
            assert ("Gis exception while getting lock conflicts");
            e->Release ();  // just silenly eat this
        }
        stat = eGwsFdoLockConflict;
        delete str;
        str = NULL;
    }
    return stat;
}
