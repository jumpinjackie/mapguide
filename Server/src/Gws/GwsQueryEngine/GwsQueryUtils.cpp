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

/////////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GwsQueryEngineImp.h"
#include "FdoExpressionEngineFilterProcessor.h"


/////////////////////////////////////////////////////////////////////
//
// namespace GwsQueryFdoUtils
//
/////////////////////////////////////////////////////////////////////

FdoDataValue * GwsQueryUtils::GetDataPropertyValue (
    FdoIReader  *   reader,
    FdoDataType     dataprop,
    FdoString   *   name
)
{
    if (reader->IsNull (name))
        return FdoDataValue::Create (dataprop);

    switch (dataprop) {
    case FdoDataType_Boolean:
        return FdoDataValue::Create (reader->GetBoolean (name));

    case FdoDataType_Byte:
        return FdoDataValue::Create (reader->GetByte (name));

    case FdoDataType_Decimal:
    case FdoDataType_Double:
        return FdoDataValue::Create (reader->GetDouble (name),
                                     dataprop);

    case FdoDataType_Int16:
        return FdoDataValue::Create (reader->GetInt16 (name));

    case FdoDataType_Int32:
        return FdoDataValue::Create (reader->GetInt32 (name));

    case FdoDataType_Int64:
        return FdoDataValue::Create (reader->GetInt64 (name));

    case FdoDataType_Single:
        return FdoDataValue::Create (reader->GetSingle (name));

    case FdoDataType_String:
        return FdoDataValue::Create (reader->GetString (name));

    case FdoDataType_BLOB:
    case FdoDataType_CLOB:
        {
            FdoPtr<FdoLOBValue> bval = (FdoBLOBValue *) reader->GetLOB (name);
            FdoPtr<FdoByteArray> bdata = bval->GetData ();
            return FdoDataValue::Create (bdata, dataprop);
        }
    case FdoDataType_DateTime:
        return FdoDataValue::Create (reader->GetDateTime (name));

    default:
        // Unknown data type
        assert(false);
    }
    return NULL;
}


void GwsQueryUtils::ToString (
    IGWSFeature            * feature,
    const CGwsPropertyDesc & desc,
    wchar_t                * buff,
    int                      len
)
{
    if (desc.m_name.empty ()) {
        wcsncpy (buff,  L"*ERROR*", len);
        return;
    }

    if (feature->IsNull (desc.m_name.c_str ())) {
        wcsncpy (buff,  L"NULL", len);
        return;
    }

    wchar_t tbuff [256];

    switch (desc.m_ptype) {
    case FdoPropertyType_ObjectProperty:
        wcsncpy (buff,  L"ObjectProperty", len);
        return;

    case FdoPropertyType_GeometricProperty:
        {
            FdoPtr<FdoByteArray> barr = feature->GetGeometry(desc.m_name.c_str ());
            wcsncpy (buff,  L"GeometricProperty", len);
        }
        return;

    case FdoPropertyType_AssociationProperty:
        wcsncpy (buff,  L"AssociationProperty", len);
        return;

    case FdoPropertyType_RasterProperty:
        wcsncpy (buff,  L"RasterProperty", len);
        return;

    case FdoPropertyType_DataProperty:
        switch (desc.m_dataprop) {
        case FdoDataType_Boolean: {
                bool val = feature->GetBoolean (desc.m_name.c_str ());
                wcsncpy (buff,  val ? L"TRUE" : L"FALSE", len);
            }
            break;

        case FdoDataType_Byte: {
                FdoByte val = feature->GetByte (desc.m_name.c_str ());
                swprintf (tbuff, 256, L"%d", val);
                wcsncpy (buff,  tbuff, len);
            }
            break;

        case FdoDataType_Decimal:
        case FdoDataType_Double: {
                double val = feature->GetDouble (desc.m_name.c_str ());
                swprintf (tbuff, 256, L"%f", val);
                wcsncpy (buff,  tbuff, len);
            }
            break;

        case FdoDataType_Int16: {
                FdoInt16 val = feature->GetInt16 (desc.m_name.c_str ());
                swprintf (tbuff, 256, L"%d", val);
                wcsncpy (buff,  tbuff, len);
            }
            break;

        case FdoDataType_Int32: {
                FdoInt32 val = feature->GetInt32 (desc.m_name.c_str ());
                swprintf (tbuff, 256, L"%ld", val);
                wcsncpy (buff,  tbuff, len);
            }
            break;

        case FdoDataType_Int64: {
                FdoInt64 val = feature->GetInt64 (desc.m_name.c_str ());
                #ifdef _WIN32
                _i64tow (val, tbuff, 10);
                #else
                swprintf(tbuff, 256, L"%lli", val);
                #endif
                wcsncpy (buff,  tbuff, len);
            }
            break;

        case FdoDataType_Single:{
                float val = feature->GetSingle (desc.m_name.c_str ());
                swprintf (tbuff, 256, L"%f", val);
                wcsncpy (buff,  tbuff, len);
            }
            break;

        case FdoDataType_String:
            wcsncpy (buff,  feature->GetString (desc.m_name.c_str ()), len);
            break;

        case FdoDataType_CLOB:
            {
                FdoPtr<FdoCLOBValue> lob = (FdoCLOBValue *) feature->GetLOB (desc.m_name.c_str ());
                FdoPtr<FdoByteArray> bdata = lob->GetData ();
                WSTR                 wbuff;
                FdoByte            * bytes = bdata->GetData ();

                // what is clob representation?
                // unicode? codepage if not unicode?

                // assuming single byte character set

                for (int i = 0; i < bdata->GetCount (); i ++) {
                    wchar_t symbol [16];
                    if (i != 0)
                        wbuff.append (L",");
                    swprintf (symbol, 16, L"%c", bytes[i]);
                    wbuff.append (symbol);

                }
                wcsncpy (buff,  wbuff.c_str (), len);
            }
            break;

        case FdoDataType_BLOB:
            {
                FdoPtr<FdoLOBValue>  lob = feature->GetLOB (desc.m_name.c_str ());
                FdoPtr<FdoByteArray> bdata = lob->GetData ();
                WSTR                 wbuff;
                FdoByte            * bytes = bdata->GetData ();
                for (int i = 0; i < bdata->GetCount (); i ++) {
                    wchar_t symbol [16];
                    if (i != 0)
                        wbuff.append (L",");
                    swprintf (symbol, 16, L"%2hx", bytes[i]);
                    wbuff.append (symbol);

                }
                wcsncpy (buff,  wbuff.c_str (), len);
            }
            break;

        case FdoDataType_DateTime: {
            FdoDateTime dt = feature->GetDateTime (desc.m_name.c_str ());
            swprintf (tbuff, 256, L"%d-%d-%d %2d:%2d:%2.4f",
                             dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.seconds);
            wcsncpy (buff,  tbuff, len);
            break;
        }

        default:
            wcsncpy (buff,  L"Unknown data", len);

        }
        return;
    }
    assert (false);
    return;
}


//compare two string lists
bool GwsQueryUtils::CompareStringCollection(
    FdoStringCollection* firstString
    , FdoStringCollection* secondString)
{
    if(NULL == firstString && NULL == secondString) return true;

    int n1 = firstString != NULL ? firstString->GetCount () : 0;
    int n2 = secondString != NULL ? secondString->GetCount () : 0;

    if (n1 != n2)
        return false;

    if(firstString->ToString() != secondString->ToString()) return false;

    return true;
}


//compare two identifier lists
bool GwsQueryUtils::CompareIdentifierCollection(
    FdoIdentifierCollection* firstIdentifiers
    , FdoIdentifierCollection* secondIdentifiers)
{
    if(NULL == firstIdentifiers && NULL == secondIdentifiers) return true;

    int n1 = firstIdentifiers != NULL ? firstIdentifiers->GetCount () : 0;
    int n2 = secondIdentifiers != NULL ? secondIdentifiers->GetCount () : 0;

    if (n1 != n2)
        return false;

    for ( int i = 0; i < n1; i++ )
    {
        FdoPtr<FdoIdentifier> firstIdentifier = firstIdentifiers->GetItem(i);
        FdoPtr<FdoIdentifier> secondIdentifier = secondIdentifiers->GetItem(i);

        if ( !GwsQueryUtils::CompareToStringValues<FdoIdentifier>(firstIdentifier, secondIdentifier) )
            return false;
    }

    return true;
}


bool GwsQueryUtils::QueryDefinitionsEqual (
    IGWSQueryDefinition * qdef1,
    IGWSQueryDefinition * qdef2,
    bool excludeSelectList
)
{
    if (qdef1 == qdef2)
        return true;

    if (qdef1 == NULL || qdef2 == NULL)
        return false;

    if (qdef1->Type () != qdef2->Type ())
        return false;

    FdoPtr<FdoStringCollection> slist1;
    FdoPtr<FdoStringCollection> slist2;

    FdoPtr<FdoIdentifierCollection> ilist1;
    FdoPtr<FdoIdentifierCollection> ilist2;

    if (qdef1->Type () == eGwsQueryFeature) {
        IGWSFeatureQueryDefinition * fqdef1 = dynamic_cast<IGWSFeatureQueryDefinition *> (qdef1);
        IGWSFeatureQueryDefinition * fqdef2 = dynamic_cast<IGWSFeatureQueryDefinition *> (qdef2);

        if (! (fqdef1->ClassName () == fqdef2->ClassName ()))
            return false;

        FdoPtr<FdoFilter> flt1 = fqdef1->Filter ();
        FdoPtr<FdoFilter> flt2 = fqdef2->Filter ();

        FdoString * fstr1 = flt1 != NULL ? flt1->ToString () : /*MSG0*/L"f";
        FdoString * fstr2 = flt2 != NULL ? flt2->ToString () : /*MSG0*/L"f";
        if (wcscmp (fstr1, fstr2) != 0)
            return false;

        if( excludeSelectList )
            return true;

        ilist1 =  fqdef1->SelectList ();
        ilist2 =  fqdef2->SelectList ();

        if (! CompareIdentifierCollection (ilist1, ilist2))
            return false;

    } else {

        IGWSJoinQueryDefinition * jqdef1 = dynamic_cast<IGWSJoinQueryDefinition *> (qdef1);
        IGWSJoinQueryDefinition * jqdef2 = dynamic_cast<IGWSJoinQueryDefinition *> (qdef2);

        slist1 =  jqdef1->LeftJoinAttributes ();
        slist2 =  jqdef2->LeftJoinAttributes ();
        if (! CompareStringCollection (slist1, slist2))
            return false;

        if(jqdef1->ForceOneToOne() != jqdef2->ForceOneToOne())
            return false;

        slist1 =  jqdef1->RightJoinAttributes ();
        slist2 =  jqdef2->RightJoinAttributes ();
        if (! CompareStringCollection (slist1, slist2))
            return false;

        FdoPtr<IGWSQueryDefinition> jqdefp1;
        FdoPtr<IGWSQueryDefinition> jqdefp2;

        jqdefp1 = jqdef1->LeftQueryDefinition ();
        jqdefp2 = jqdef2->LeftQueryDefinition ();

        // Should not exclude the list in case we suport joining on calculated properties(very unlikely)
        if (! QueryDefinitionsEqual (jqdefp1, jqdefp2, excludeSelectList ))
            return false;

        jqdefp1 = jqdef1->RightQueryDefinition ();
        jqdefp2 = jqdef2->RightQueryDefinition ();
        if (! QueryDefinitionsEqual (jqdefp1, jqdefp2, excludeSelectList ))
            return false;
    }
    return true;

}


FdoIdentifierCollection* GwsQueryUtils::CreateIdentifiersFromStrings(
    FdoStringCollection* strings
)
{
    FdoIdentifierCollection* identifiers = FdoIdentifierCollection::Create();

    if ( strings != NULL ) {
        for ( int i = 0; i < strings->GetCount(); i++ ) {
            FdoPtr<FdoIdentifier> identifier = FdoIdentifier::Create( strings->GetString(i) );
            identifiers->Add(identifier);
        }
    }

    return identifiers;
}


bool GwsQueryUtils::FilterHasCalculatedProperties( FdoFilter* pFilter, FdoIdentifierCollection *pSelectList )
{
    class CalculatedPropertiesFinder: public FdoExpressionEngineFilterProcessor
    {
     private:
        FdoIdentifierCollection* mSelectList;
     public:
        bool   HasCalculatedProperties;

        CalculatedPropertiesFinder( FdoIdentifierCollection* selectList )
        {
            HasCalculatedProperties = false;
            mSelectList = selectList;
        }

        virtual void ProcessIdentifier(FdoIdentifier& expr)
        {
            if( FdoPtr<FdoIdentifier>( mSelectList->FindItem( expr.GetName() ) ) != NULL )
                HasCalculatedProperties = true;
        }
        virtual void ProcessComputedIdentifier(FdoComputedIdentifier& ){ HasCalculatedProperties = true;  }
    };
    CalculatedPropertiesFinder finder(pSelectList);
    pFilter->Process( &finder );
    return finder.HasCalculatedProperties;
}


FdoPropertyDefinition * GwsQueryHelpers::ClonePropertyDefinition (
    FdoString             * newName,
    FdoPropertyDefinition * pDefIn,
    bool                    forceCloneReadOnly
)
{
    std::wstring wpubname;

    if (pDefIn == NULL)
        return NULL;

    if (newName != NULL)
        wpubname = newName;
    else
        wpubname = pDefIn->GetName ();

    FdoString * pubname = wpubname.c_str ();

    FdoPropertyDefinition * pDefOut = NULL;

    if (pDefIn->GetPropertyType () == FdoPropertyType_DataProperty) {
        pDefOut = FdoDataPropertyDefinition::Create (pubname, L"");
        FdoDataPropertyDefinition * pOut = dynamic_cast<FdoDataPropertyDefinition*> (pDefOut);
        FdoDataPropertyDefinition * pIn = dynamic_cast<FdoDataPropertyDefinition*> (pDefIn);

        pOut->SetDataType(pIn->GetDataType ());
        pOut->SetReadOnly(forceCloneReadOnly?true:pIn->GetReadOnly ());
        pOut->SetLength(pIn->GetLength ());
        pOut->SetPrecision(pIn->GetPrecision ());
        pOut->SetScale(pIn->GetScale ());
        pOut->SetNullable(pIn->GetNullable ());
        pOut->SetDefaultValue(pIn->GetDefaultValue ());
    }
    if (pDefIn->GetPropertyType () == FdoPropertyType_GeometricProperty) {

        pDefOut = FdoGeometricPropertyDefinition::Create (pubname, L"");
        FdoGeometricPropertyDefinition * pOut = dynamic_cast<FdoGeometricPropertyDefinition*> (pDefOut);
        FdoGeometricPropertyDefinition * pIn = dynamic_cast<FdoGeometricPropertyDefinition*> (pDefIn);

        pOut->SetGeometryTypes (pIn->GetGeometryTypes ());
        pOut->SetReadOnly(forceCloneReadOnly?true:pIn->GetReadOnly ());
        pOut->SetHasElevation(pIn->GetHasElevation ());
        pOut->SetHasMeasure (pIn->GetHasMeasure ());
        pOut->SetSpatialContextAssociation (pIn->GetSpatialContextAssociation ());
    }

    if (pDefIn->GetPropertyType () == FdoPropertyType_ObjectProperty) {
        pDefOut = FdoObjectPropertyDefinition::Create (pubname, L"");
        FdoObjectPropertyDefinition * pOut = dynamic_cast<FdoObjectPropertyDefinition*> (pDefOut);
        FdoObjectPropertyDefinition * pIn = dynamic_cast<FdoObjectPropertyDefinition*> (pDefIn);

        FdoPtr<FdoClassDefinition> clsdef = pIn->GetClass();
        pOut->SetClass (clsdef);
        FdoPtr<FdoDataPropertyDefinition> idprop = pIn->GetIdentityProperty ();
        pOut->SetIdentityProperty (idprop);
        pOut->SetObjectType (pIn->GetObjectType ());
        pOut->SetOrderType (pIn->GetOrderType ());
    }

    if (pDefIn->GetPropertyType () == FdoPropertyType_AssociationProperty) {
        pDefOut = FdoAssociationPropertyDefinition::Create (pubname, L"");
        FdoAssociationPropertyDefinition * pOut = dynamic_cast<FdoAssociationPropertyDefinition*> (pDefOut);
        FdoAssociationPropertyDefinition * pIn = dynamic_cast<FdoAssociationPropertyDefinition*> (pDefIn);

        FdoPtr<FdoClassDefinition> clsdef = pIn->GetAssociatedClass();
        pOut->SetAssociatedClass (clsdef);
        pOut->SetReverseName (pIn->GetReverseName ());
        pOut->SetDeleteRule (pIn->GetDeleteRule ());
        pOut->SetLockCascade (pIn->GetLockCascade ());
        pOut->SetIsReadOnly (forceCloneReadOnly?true:pIn->GetIsReadOnly ());
        pOut->SetMultiplicity (pIn->GetMultiplicity ());
        pOut->SetReverseMultiplicity (pIn->GetReverseMultiplicity ());
    }

    if (pDefIn->GetPropertyType () == FdoPropertyType_RasterProperty) {
        pDefOut = FdoRasterPropertyDefinition::Create (pubname, L"");
        FdoRasterPropertyDefinition * pOut = dynamic_cast<FdoRasterPropertyDefinition*> (pDefOut);
        FdoRasterPropertyDefinition * pIn = dynamic_cast<FdoRasterPropertyDefinition*> (pDefIn);

        pOut->SetReadOnly(forceCloneReadOnly?true:pIn->GetReadOnly ());
        pOut->SetNullable(pIn->GetNullable ());
        FdoPtr<FdoRasterDataModel> rdm = pIn->GetDefaultDataModel ();
        pOut->SetDefaultDataModel (rdm);
        pOut->SetDefaultImageXSize (pIn->GetDefaultImageXSize ());
        pOut->SetDefaultImageYSize (pIn->GetDefaultImageYSize ());
        pOut->SetSpatialContextAssociation (pIn->GetSpatialContextAssociation ());
    }
    return pDefOut;
}


void GwsQueryHelpers::CreateFlatFdoReader (
    IGWSFeatureIterator * pFrom,
    FdoIFeatureReader  ** pFdoReader
)
{
    * pFdoReader = new CGwsFlatFdoReader (pFrom);
}


void  GwsQueryHelpers::CreateFlatGwsIterator (
    IGWSFeatureIterator * pFrom,
    IGWSFeatureIterator ** pGwsIterator
)
{
    * pGwsIterator = new CGwsFlatGwsIterator (pFrom);
    (* pGwsIterator)->AddRef ();
}


void GwsQueryHelpers::BuildFlatDescription(IGWSQueryDefinition* pQryDef,
                                           IGWSExtendedFeatureDescription* pJoinDesc,
                                           IGWSExtendedFeatureDescription** ppResDesc) {
    CGwsFlatFdoReader::BuildFlatDescription(pQryDef,pJoinDesc,ppResDesc);
}
