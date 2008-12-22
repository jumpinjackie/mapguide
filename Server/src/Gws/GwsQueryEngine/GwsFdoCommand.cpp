//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
    static_cast<void>(&(pFDOCommandClass)); // For "unreferenced formal parameter" warning

    // initilaize class definition
    if(m_classDef == NULL)
    {
        FdoPtr<FdoFeatureSchema> pSchema;
        GwsCommonFdoUtils::GetClassDefinition (m_connection,
                                               m_classname,
                                               pSchema.p,
                                               m_classDef.p);
    }
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

        GwsCommonFdoUtils::GetFdoClassIdentityProperties (m_classDef, m_identity.p);

        FdoPtr<FdoISchemaCapabilities> ptrCap;
        ptrCap = m_connection->GetSchemaCapabilities();
        assert (ptrCap);
        m_bSupportBlobs = false;
        m_bSupportClobs = false;
        if (ptrCap) {
            FdoInt32 len = 0;
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
        FdoPtr<FdoIConnectionCapabilities> connCap;
        connCap = m_connection->GetConnectionCapabilities();
        if (connCap) {
            if (connCap->SupportsLocking()) {
                FdoPtr<FdoClassCapabilities> cap = m_classDef->GetCapabilities ();
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

    } catch (FdoException * ex) {
        PushFdoException (eGwsFailed, ex);
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
FdoPropertyDefinition * CGwsFdoCommand::GetPropertyDefinition (FdoString * propname)
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
    static_cast<void>(&(feature)); // For "unreferenced formal parameter" warning
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
    static_cast<void>(&(featid)); // For "unreferenced formal parameter" warning
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
    static_cast<void>(&(featids)); // For "unreferenced formal parameter" warning
    static_cast<void>(&(lbound)); // For "unreferenced formal parameter" warning
    static_cast<void>(&(ubound)); // For "unreferenced formal parameter" warning
    return eGwsNotSupported;
}


///////////////////////////////////////////////////////////////////////////////
void CGwsFdoCommand::PrepareInternal ()
{
}


///////////////////////////////////////////////////////////////////////////////
void CGwsFdoCommand::PrepareNonKeyProperties ()
{
    FdoInt32 idx;

    FdoPtr<FdoClassDefinition>  classDef = m_classDef;
    m_geometricprop.clear ();
    for (; classDef != NULL ; classDef = classDef->GetBaseClass ())
    {
        FdoPtr<FdoPropertyDefinitionCollection> propdsc = classDef->GetProperties ();
        // discover geometric property name. Use the first one if there are many.
        for (idx = 0; idx < propdsc->GetCount(); idx ++) {
            FdoPtr<FdoPropertyDefinition>   prop;
            prop = propdsc->GetItem (idx);

            m_propdefs.insert(PropertyDefinitionMap::value_type(prop->GetName (), prop));

            if (m_geometricprop.empty () &&
                prop->GetPropertyType () == FdoPropertyType_GeometricProperty)
            {
                m_geometricprop = prop->GetName ();
                FdoGeometricPropertyDefinition* geomProp = static_cast<FdoGeometricPropertyDefinition*>(prop.p);
                FdoString* pSC = geomProp->GetSpatialContextAssociation();
                if(pSC != NULL) {
                    m_geometricSC = pSC;

                    // need to initialize converter for every command
                    // spatial filter needs to be processed
                    if (m_converter) {
                        // check to see whether converter is already
                        // initialized by the source cs system
                        if (m_converter->SourceCS ().IsEmpty ()) {
                            GwsSpatialContextDescription scdesc;
                            EGwsStatus stat =
                                GwsCommonFdoUtils::DescribeSC (m_connection,
                                                            m_geometricSC.c_str (),
                                                            scdesc);
                            // cordinate system may not be recogenized
                            // currently this fact is silently ignored
                            // Should we throw an exception instead?
                            if (!IGWSException::IsError (stat))
                                m_converter->SetSourceCS (scdesc.CsName ());
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


///////////////////////////////////////////////////////////////////////////////
void CGwsFdoCommand::PrepareObjectProperties ()
{
#ifdef IMPLEMENT_THIS_METHOD // and test
    FdoPtr<FdoClassDefinition>  classDef = m_classDef;
    for (; classDef != NULL; classDef = classDef->GetBaseClass ()) {
        FdoPtr<FdoPropertyDefinitionCollection> propdsc = classDef->GetProperties();
        for (FdoInt32 idx = 0; idx < propdsc->GetCount (); idx ++) {
            FdoPtr<FdoPropertyDefinition>   prop;
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

                FdoPtr<FdoClassDefinition> pDef = objprop->GetClass();
                FdoPtr<FdoFeatureSchema> pSchema = pDef->GetFeatureSchema();

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


///////////////////////////////////////////////////////////////////////////////
void CGwsFdoCommand::PrepareFilter (FdoFilter * filter, bool bAlignPolygon /*FALSE*/)
{
    if (filter == NULL)
        return;
    if (m_converter == NULL)
        return;
    CGwsCSQueryProcessor qcconverter (m_converter);

    if(bAlignPolygon)
        qcconverter.SetAlignPolygonFilter(true);

#ifdef _DEBUG
    FdoString * str1 = filter->ToString ();
#endif

    filter->Process (& qcconverter);
#ifdef _DEBUG
    FdoString * str2 = filter->ToString ();
#endif

    if (IGWSException::IsError (qcconverter.Status()))
        GWS_THROW (qcconverter.Status());

    m_bFilterPrepared = true;
}


///////////////////////////////////////////////////////////////////////////////
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

    FdoPtr<FdoPropertyValue>     pPropertyValue;
    FdoPtr<FdoIdentifier>        ident = pPropVal->GetName ();

    pPropertyValue = pPropertyValues->FindItem (ident->GetName ());
    if (pPropertyValue != NULL) {
        FdoPtr<FdoValueExpression> valExpr = pPropVal->GetValue ();
        pPropertyValue->SetValue(valExpr);
    } else {
        pPropertyValues->Add(pPropVal);
    }
}


///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoCommand::SetProperties (
    CGwsMutableFeature & feature,
    bool                 bSetIdentityProperties
)
{
    EGwsStatus stat = eGwsOk;

    for (int i = 0; i < feature.GetCount (); i ++) {
        FdoPtr<FdoPropertyValue> val = feature.GetPropertyValue (i);
        FdoPtr<FdoIdentifier> ident = val->GetName ();
        PropertyDefinitionMap::iterator iter = m_propdefs.find (ident->GetName ());
        if (iter == m_propdefs.end ()) {
            stat = eGwsInvalidPropertyName;
            break;
        }
        if ((*iter).second->GetPropertyType () == FdoPropertyType_DataProperty) {
            FdoDataPropertyDefinition * dataprop = static_cast<FdoDataPropertyDefinition*>((*iter).second);

            // do not set identity properties if not requested
            if (! bSetIdentityProperties && m_identity->Contains (dataprop))
                continue;

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
            FdoPtr<FdoPropertyValue> cval = ConvertGeometryProperty (
                                                    feature.GetCSName (),
                                                    val);
            SetValue (cval);
        }
    }

    return stat;
}


///////////////////////////////////////////////////////////////////////////////
FdoPropertyValue * CGwsFdoCommand::ConvertGeometryProperty (
    const GWSCoordinateSystem & csname,
    FdoPropertyValue          * val
)
{
    // no conversion
    if (m_converter == NULL ||
        csname.IsEmpty () ||
        m_converter->SourceCS ().IsEmpty ())
    {
        val->AddRef ();
        return val;
    }

    // initialize converter by the destination cs
    // potential subject of optimization.
    EGwsStatus es = eGwsOk;
    if (! (csname == m_converter->DestinationCS ()))
        es = m_converter->SetDestinationCS (csname);

    if (IGWSException::IsError (es)) {
        PushStatus (es);
        val->AddRef ();
        return val;
    }

    // cs converter converts in place. In order not to
    // change source geometry, create a copy of it
    // not very efficient, but looks like the only way ...
    FdoPtr<FdoValueExpression> gval = val->GetValue ();
    FdoPtr<FdoIdentifier>      ident = val->GetName ();
    FdoGeometryValue         * geomval = dynamic_cast<FdoGeometryValue *> (gval.p);
    assert (geomval);

    FdoPtr<FdoByteArray>       barray = geomval->GetGeometry();
    assert (barray);    // this is geometry condition
    FdoPtr<FdoByteArray>       carray = FdoByteArray::Create (barray->GetData (),
                                                              barray->GetCount ());

    eGwsOkThrow (m_converter->ConvertBackward (carray));

    FdoPtr<FdoGeometryValue> cgeomval = FdoGeometryValue::Create (carray);
    return FdoPropertyValue::Create (ident, cgeomval);
}


///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoCommand::BuildInFilter (
    const GwsFeaturesIdVector     & featIds,
    int                             lbound,
    int                             ubound,
    FdoFilter                    *& pOutFilter
)
{
    EGwsStatus  stat  = eGwsOk;
    try {
        if (m_identity == NULL) {
            PushStatus (eGwsFeatureClassHasNoIdentity);
            return eGwsFeatureClassHasNoIdentity;
        }

        stat = CGwsFdoCommand::BuildInFilter(m_identity, featIds, lbound, ubound, pOutFilter);
    } catch (FdoException *e) {
        PushFdoException (eGwsFailed, e);
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
    FdoPtr<FdoValueExpressionCollection>   invalues;
    FdoPtr<FdoInCondition>                 inclause;

    assert (identity->GetCount () == 1);
    assert (lbound >= 0);
    assert (ubound <= (int)featIds.size ());

    invalues = FdoValueExpressionCollection::Create ();

    for (int i = lbound; i < ubound; i ++) {
        assert (featIds[i].GetCount () == 1);
        FdoPtr<FdoDataValue>      pValue = featIds[i].GetItem (0);
        invalues->Add (pValue);
    }
    FdoPtr<FdoPropertyDefinition> prop = identity->GetItem (0);
    FdoPtr<FdoIdentifier>         name = FdoIdentifier::Create(prop->GetName());

    inclause = FdoInCondition::Create (name, invalues);

    pOutFilter = inclause;
    if (inclause != NULL)
        inclause.p->AddRef ();

    return stat;
}


///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoCommand::BuildFilter (
    const GWSFeatureId       & featId,
    FdoFilter               *& pOutFilter
)
{
    EGwsStatus stat             = eGwsOk;
    try {
        if (m_identity == NULL) {
            PushStatus (eGwsFeatureClassHasNoIdentity);
            return eGwsFeatureClassHasNoIdentity;
        }
        stat = CGwsFdoCommand::BuildFilter(m_identity, featId, pOutFilter);
    } catch (FdoException *e) {
        PushFdoException (eGwsFailed, e);
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

    FdoPtr<FdoFilter> pFilter;
    FdoInt32          idx;

    for (idx = 0; idx < identity->GetCount(); idx ++) {
        FdoPtr<FdoPropertyDefinition>   prop;
        prop = identity->GetItem (idx);

        FdoPtr<FdoIdentifier> pFeatureNameProperty = FdoIdentifier::Create(prop->GetName());
        FdoPtr<FdoDataValue>  pFeatureNameValue = featId.GetItem (idx);
        if (idx == 0) {
            pFilter = FdoComparisonCondition::Create(pFeatureNameProperty,
                                                    FdoComparisonOperations_EqualTo,
                                                    pFeatureNameValue);
        } else {
            FdoPtr<FdoFilter> pRhsFilter = FdoComparisonCondition::Create(pFeatureNameProperty,
                                                                        FdoComparisonOperations_EqualTo,
                                                                        pFeatureNameValue);
            FdoPtr<FdoFilter> pCombinedFilter = FdoFilter::Combine(pFilter,
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


///////////////////////////////////////////////////////////////////////////////
EGwsStatus CGwsFdoCommand::BuildFilter (
    const GwsFeaturesIdVector    & featids,
    int                            lbound,
    int                            ubound,
    FdoFilter                 *  & pOutFilter
)
{
    EGwsStatus   stat  = eGwsOk;

    try {
        if (m_identity == NULL) {
            PushStatus (eGwsFeatureClassHasNoIdentity);
            return eGwsFeatureClassHasNoIdentity;
        }

        stat = BuildFilter(m_connection, m_identity, featids, lbound, ubound, pOutFilter);
    } catch (FdoException *e) {
        PushFdoException (eGwsFailed, e);
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
        FdoPtr<FdoIFilterCapabilities> fltCap;
        fltCap = conn->GetFilterCapabilities();
        if(fltCap != NULL) {
            FdoInt32 size = 0;
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

    FdoPtr<FdoFilter> filter;
    for (int i = lbound; i < ubound; i ++) {
        FdoPtr<FdoFilter> f;
        BuildFilter (identity, featids[i], f.p);
        if ( i == 0)
            filter = f;
        else {
            FdoPtr<FdoFilter> pCombinedFilter = FdoFilter::Combine(filter,
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
    if (m_identity == NULL || m_identity->GetCount () != 1)
        return eGwsFdoInvalidPropertyType;

    // do not build in filter (for now)
    EGwsStatus   stat  = eGwsOk;

    try {
        FdoPtr<FdoPropertyDefinition>   prop;
        prop = m_identity->GetItem (0);

        // these are data types we support in this case
        if (prop->GetPropertyType () != FdoPropertyType_DataProperty)
            return eGwsFdoInvalidPropertyType;

        FdoDataPropertyDefinition * dataprop = (FdoDataPropertyDefinition *) prop.p;
        if (dataprop->GetDataType () != FdoDataType_Int32 &&
            dataprop->GetDataType () != FdoDataType_Int64)
            return eGwsFdoInvalidPropertyType;

        FdoPtr<FdoIdentifier> pName   = FdoIdentifier::Create(prop->GetName());
        FdoPtr<FdoFilter>     filter;

        for (int i = 0; i < nCount; i ++) {
            FdoPtr<FdoDataValue>  pValue  = FdoInt32Value::Create (idSet->Id(i));
            FdoPtr<FdoFilter>     f       = FdoComparisonCondition::Create(
                                                     pName,
                                                     FdoComparisonOperations_EqualTo,
                                                     pValue);
            if ( i == 0)
                filter = f;
            else {
                FdoPtr<FdoFilter> pCombinedFilter = FdoFilter::Combine(filter,
                                                                       FdoBinaryLogicalOperations_Or,
                                                                       f);
                filter = pCombinedFilter;
            }

        }
        pOutFilter = filter;
        if (filter != NULL)
            filter.p->AddRef ();
    } catch (FdoException *e) {
        PushFdoException (eGwsFailed, e);
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
                FdoString      *    owner = pReader->GetLockOwner();
                CGwsStatus stat (eGwsFdoLockConflict);
                stat.SetParameter (L"Username", owner);
                stat.SetParameter (L"FeatureId", wbuff);
                PushStatus (stat);
                failed.insert (GwsFailedStatus::value_type (featId.FeatureId (),stat));

            } else if (ctype == FdoConflictType_VersionConflict) {
                FdoString     *     version = pReader->GetLongTransaction ();
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

        } catch (FdoException * e) {
            assert ("Fdo exception while getting lock conflicts");
            e->Release ();  // just silenly eat this
        }
        stat = eGwsFdoLockConflict;
        delete str;
        str = NULL;
    }
    return stat;
}
