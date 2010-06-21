//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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
#include "GwsQueryEngineImp.h"
#include "GwsBinaryFeatureWriter.h"


unsigned char* GwsBinaryFeatureWriter::ToBuffer(int& bufLen)
{
    bufLen = m_wrtr.GetDataLen();
    return m_wrtr.GetData();
}


void GwsBinaryFeatureWriter::WriteProperty(FdoPropertyDefinition* pd, FdoIFeatureReader* reader)
{
    FdoDataPropertyDefinition* dpd = NULL;

    if (pd->GetPropertyType() == FdoPropertyType_DataProperty)
        dpd = (FdoDataPropertyDefinition*)pd;

    FdoString* name = pd->GetName();

    if (reader->IsNull(name))
    {
        //do not write anything if the reader does not have
        //data for this property
        //TODO: should we write the default property value instead?
        return;
    }

    if (dpd)
    {
        switch (dpd->GetDataType())
        {
        case FdoDataType_Boolean :
                m_wrtr.WriteByte(reader->GetBoolean(name) ? 1 : 0);
                break;

        case FdoDataType_Byte :
                m_wrtr.WriteByte(reader->GetByte(name));
                break;

        case FdoDataType_DateTime :
                m_wrtr.WriteDateTime(reader->GetDateTime(name));
                break;

        case FdoDataType_Decimal :
                m_wrtr.WriteDouble(reader->GetDouble(name));
                break;

        case FdoDataType_Double :
                m_wrtr.WriteDouble(reader->GetDouble(name));
                break;

        case FdoDataType_Int16 :
                m_wrtr.WriteInt16(reader->GetInt16(name));
                break;

        case FdoDataType_Int32 :
                m_wrtr.WriteInt32(reader->GetInt32(name));
                break;

        case FdoDataType_Int64 :
                m_wrtr.WriteInt64(reader->GetInt64(name));
                break;

        case FdoDataType_Single :
                m_wrtr.WriteSingle(reader->GetSingle(name));
                break;

        case FdoDataType_String :
                m_wrtr.WriteRawString(reader->GetString(name));
                break;

        case FdoDataType_BLOB :
        case FdoDataType_CLOB :
        default:
                GWS_THROW(eGwsNotSupported);
        }
    }
    else
    {
        //we have a geometric property
        FdoPtr<FdoByteArray> byteArray = reader->GetGeometry(name);

        //Note we do not need to write the length of a byte array since we know it
        //by subtracting the offsets into property values in the data record
        if (byteArray)
            m_wrtr.WriteBytes(byteArray->GetData(), byteArray->GetCount());
    }
}


void GwsBinaryFeatureWriter::WriteProperty(FdoPropertyDefinition* pd, FdoPropertyValue* pv,
                                           bool forAssociation)
{
    FdoDataPropertyDefinition* dpd = NULL;
    FdoPtr<FdoValueExpression> expression;

    if (pd->GetPropertyType() == FdoPropertyType_DataProperty)
        dpd = (FdoDataPropertyDefinition*)pd;

    if (pv == NULL)
    {
        //if the property value is NULL, do not write anything to the data record
        //the default value will be used if later the user asks for the value
        //of this porperty
        //TODO: should we write the default property value instead?
        return;
    }

    expression = pv->GetValue();

    if (dpd)
    {
        FdoDataValue* dv = (FdoDataValue*)expression.p;
        if(dv == NULL || dv->IsNull())
            return;

        switch (dpd->GetDataType())
        {
        case FdoDataType_Boolean :
                m_wrtr.WriteByte(((FdoBooleanValue*)(expression.p))->GetBoolean() ? 1 : 0);
                break;

        case FdoDataType_Byte :
                m_wrtr.WriteByte(((FdoByteValue*)(expression.p))->GetByte());
                break;

        case FdoDataType_DateTime :
                m_wrtr.WriteDateTime(((FdoDateTimeValue*)(expression.p))->GetDateTime());
                break;

        case FdoDataType_Decimal :
                m_wrtr.WriteDouble(((FdoDecimalValue*)(expression.p))->GetDecimal());
                break;

        case FdoDataType_Double :
                m_wrtr.WriteDouble(((FdoDoubleValue*)(expression.p))->GetDouble());
                break;

        case FdoDataType_Int16 :
                m_wrtr.WriteInt16(((FdoInt16Value*)(expression.p))->GetInt16());
                break;

        case FdoDataType_Int32 :
                m_wrtr.WriteInt32(((FdoInt32Value*)(expression.p))->GetInt32());
                break;

        case FdoDataType_Int64 :
                m_wrtr.WriteInt64(((FdoInt64Value*)(expression.p))->GetInt64());
                break;

        case FdoDataType_Single :
                m_wrtr.WriteSingle(((FdoSingleValue*)(expression.p))->GetSingle());
                break;

        case FdoDataType_String :
            if( ! forAssociation )
                m_wrtr.WriteRawString(((FdoStringValue*)(expression.p))->GetString());
            else
                m_wrtr.WriteString(((FdoStringValue*)(expression.p))->GetString());
                break;

        case FdoDataType_BLOB :
        case FdoDataType_CLOB :
        default:
                GWS_THROW(eGwsNotSupported);
        }
    }
    else
    {
        //we have a geometric property
        FdoPtr<FdoByteArray> byteArray = ((FdoGeometryValue*)(expression.p))->GetGeometry();

        //Note we do not need to write the length of a byte array since we know it
        //by subtracting the offsets into property values in the data record
        if (byteArray)
            m_wrtr.WriteBytes(byteArray->GetData(), byteArray->GetCount());
    }
}


bool GwsBinaryFeatureWriter::WriteAssociationProperty(FdoAssociationPropertyDefinition* apd,
                                                      FdoPropertyValueCollection* pvc)
{
    bool  errorIfSet = apd->GetIsReadOnly();
    bool  oneIdentIsNull = false;
    bool  written = false;

    FdoPtr<FdoDataPropertyDefinitionCollection> idents = apd->GetIdentityProperties();
    if( idents->GetCount() == 0 )
    {
        // Search for property values with names build from the association property name and the
        // associated class identity properties. For example if the associated class identifier is "Id" and the
        // association property name is "AssocProp", then we should search for a property value with
        // name: "AssocProp.Id". If that property value is found and set, then that means an association
        // exists between the new object(we are about to insert) and the object identified by the value
        // of the property value(AssocProp.Id)
        FdoPtr<FdoClassDefinition> cls = apd->GetAssociatedClass();
        idents = cls->GetIdentityProperties();
    }

    for(int i=0; i<idents->GetCount(); i++ )
    {
        FdoPtr<FdoDataPropertyDefinition> prop = idents->GetItem( i );
        std::wstring wstr = apd->GetName();
        wstr += L".";
        wstr += prop->GetName();
        FdoPtr<FdoPropertyValue> pv = pvc->FindItem( wstr.c_str() );
        if(pv != NULL)
        {
            if( errorIfSet )
                GWS_THROW(eGwsFailed); //cannot add readonly association property value

            if( oneIdentIsNull )
                GWS_THROW(eGwsFailed); //one of the identity properties is null

            WriteProperty(prop, pv, true);
            written = true;
            oneIdentIsNull = false;
        }
        else
        {
            if( written ) // we already written one or more identity and this one is null
                GWS_THROW(eGwsFailed);
            oneIdentIsNull = true;
        }
    }

    return written;
}


void GwsBinaryFeatureWriter::WriteAssociationProperty(FdoAssociationPropertyDefinition* apd,
                                                      FdoIFeatureReader* reader)
{
    if( apd->GetIsReadOnly() )
        return;

    FdoPtr<FdoDataPropertyDefinitionCollection> idents = apd->GetIdentityProperties();
    if( idents->GetCount() == 0 )
    {
        // Search for property values with names build from the association property name and the
        // associated class identity properties. For example if the associated class identifier is "Id" and the
        // association property name is "AssocProp", then we should search for a property value with
        // name: "AssocProp.Id". If that property value is found and set, then that means an association
        // exists between the new object(we are about to insert) and the object identified by the value
        // of the property value(AssocProp.Id)
        FdoPtr<FdoClassDefinition> cls = apd->GetAssociatedClass();
        idents = cls->GetIdentityProperties();
    }
    if( reader->IsNull( apd->GetName() ) )
        return;

    FdoPtr<FdoIFeatureReader> loc_reader = reader->GetFeatureObject( apd->GetName() );
    if( ! loc_reader->ReadNext() )
        return;
    for(int i=0; i<idents->GetCount(); i++ )
    {
        FdoPtr<FdoDataPropertyDefinition> prop = idents->GetItem( i );
        if( ! loc_reader->IsNull( prop->GetName() ) )
            WriteProperty( prop, loc_reader);
    }
}


void GwsBinaryFeatureWriter::WriteFeature(FdoClassDefinition* fc, FdoString* fcName,
                                          FdoPropertyValueCollection* pvc)
{
    FdoPtr<FdoReadOnlyPropertyDefinitionCollection> bpdc = fc->GetBaseProperties();
    FdoPtr<FdoPropertyDefinitionCollection> pdc = fc->GetProperties();
    //FdoPtr<FdoDataPropertyDefinitionCollection> idpdc = FindIDProps(fc);

    //write feature class ID
    m_wrtr.WriteString(fcName);
    int fcNameLen = m_wrtr.GetPosition();

    //find number of properties we will store into the buffer
    //we will use this number to save an offset into the beffer for each property
    //at the beginning of the buffer
    int numProps = bpdc->GetCount() + pdc->GetCount();

    //now generate the data value -- write all property values, except for the ones
    //we already wrote to the key -- we have to check if each one is in the ID prop
    //collection

    //reserve space for offsets into property values in data record
    for (int i=0; i<numProps; i++)
        m_wrtr.WriteInt32(0);

    int index = 0;

    //base properties first
    for (int i=0; i<bpdc->GetCount(); i++)
    {
        FdoPtr<FdoPropertyDefinition> pd = (FdoPropertyDefinition*)bpdc->GetItem(i);

        //save offset of property data to the reserved position at the
        //beginning of the record
        //TODO: endian
        ((int*)(m_wrtr.GetData() + fcNameLen))[index++] = m_wrtr.GetPosition();

        //if property is autogenerated, do not write
        //anything. We use the record number as autogen property value
        //so we can obtain it at read time
        //if ( (pi==NULL) || !pi->IsPropAutoGen(pd->GetName()))
        //{
        if (pvc)
        {
            if( pd->GetPropertyType() == FdoPropertyType_AssociationProperty )
                WriteAssociationProperty((FdoAssociationPropertyDefinition*)pd.p, pvc);
            else
            {
                FdoPtr<FdoPropertyValue> pv = pvc->FindItem(pd->GetName());
                if(pv != NULL)
                    WriteProperty(pd, pv);
                else
                    WriteProperty(pd, (FdoPropertyValue*)NULL);
            }
        }
        else
        {
            WriteProperty(pd, (FdoPropertyValue*)NULL);
        }
        //}
    }

    //class properties
    for (int i=0; i<pdc->GetCount(); i++)
    {
        FdoPtr<FdoPropertyDefinition> pd = (FdoPropertyDefinition*)pdc->GetItem(i);

        //save offset of property data to the reserved position at the
        //beginning of the record
        //TODO: endian
        ((int*)(m_wrtr.GetData() + fcNameLen))[index++] = m_wrtr.GetPosition();

        //if property is autogenerated, do not write
        //anything. We use the record number as autogen property value
        //so we can obtain it at read time
        //if ( (pi == NULL) || !pi->IsPropAutoGen(pd->GetName()))
        //{
        if (pvc)
        {
            if( pd->GetPropertyType() == FdoPropertyType_AssociationProperty )
                WriteAssociationProperty((FdoAssociationPropertyDefinition*)pd.p, pvc);
            else
            {
                FdoPtr<FdoPropertyValue> pv = pvc->FindItem(pd->GetName());
                if(pv != NULL)
                    WriteProperty(pd, pv);
                else
                    WriteProperty(pd, (FdoPropertyValue*)NULL);
            }
        }
        else
        {
            WriteProperty(pd, (FdoPropertyValue*)NULL);
        }
        //}
    }
}


//serializes a collection of property values into a byte array
//This function differs from WriteFeature() in that
//it will merge property values specified in the given PropertyValueCollection
//with property values in the given FeatureReader. The PropertyValueCollection
//represents properties to be updated in an old feature record represented by
//the given FeatureReader.
void GwsBinaryFeatureWriter::WriteFeature(FdoClassDefinition* fc,  FdoString* fcName,
                                          FdoPropertyValueCollection* pvc,
                                          FdoIFeatureReader* reader)
{
    FdoPtr<FdoReadOnlyPropertyDefinitionCollection> bpdc = fc->GetBaseProperties();
    FdoPtr<FdoPropertyDefinitionCollection> pdc = fc->GetProperties();

    //find number of properties we will store into the data record
    //we will use this number to save an offset into the data records for each property
    //at the beginning of the data record
    int numProps = bpdc->GetCount() + pdc->GetCount();// - idpdc->GetCount();

    //now generate the data value -- write all property values, except for the ones
    //we already wrote to the key -- we have to check if each one is in the ID prop
    //collection

    //write feature class ID
    m_wrtr.WriteString(fcName);
    int fcNameLen = m_wrtr.GetPosition();

    //reserve space for offsets into property values in data record
    for (int i=0; i<numProps; i++)
        m_wrtr.WriteInt32(0);

    int index = 0;

    //base properties first
    for (int i=0; i<bpdc->GetCount(); i++)
    {
        FdoPtr<FdoPropertyDefinition> pd = (FdoPropertyDefinition*)bpdc->GetItem(i);

        //save offset of property data to the reserved position at the
        //beginning of the record
        //TODO: endian
        ((int*)(m_wrtr.GetData() + fcNameLen))[index++] = m_wrtr.GetPosition();

        //if property is autogenerated, do not write
        //anything. We use the record number as autogen property value
        //so we can obtain it at read time
        //if (!pi->IsPropAutoGen(pd->GetName()))
        //{
        if (pvc)
        {
            if( pd->GetPropertyType() == FdoPropertyType_AssociationProperty )
            {
                if( ! WriteAssociationProperty((FdoAssociationPropertyDefinition*)pd.p, pvc) )
                    WriteAssociationProperty((FdoAssociationPropertyDefinition*)pd.p, reader);
            }
            else
            {
                FdoPtr<FdoPropertyValue> pv((FdoPropertyValue*)pvc->FindItem(pd->GetName()));
                if(pv != NULL)
                    WriteProperty(pd, pv);
                else
                    WriteProperty(pd, reader);
            }
        }
        else
        {
            WriteProperty(pd, reader);
        }
        //}
    }

    //class properties
    for (int i=0; i<pdc->GetCount(); i++)
    {
        FdoPtr<FdoPropertyDefinition> pd = (FdoPropertyDefinition*)pdc->GetItem(i);

        //save offset of property data to the reserved position at the
        //beginning of the record
        //TODO: endian
        ((int*)(m_wrtr.GetData() + fcNameLen))[index++] = m_wrtr.GetPosition();


        //if property is autogenerated, do not write
        //anything. We use the record number as autogen property value
        //so we can obtain it at read time
        //if (!pi->IsPropAutoGen(pd->GetName()))
        //{
        if (pvc)
        {
            if( pd->GetPropertyType() == FdoPropertyType_AssociationProperty )
            {
                if( ! WriteAssociationProperty((FdoAssociationPropertyDefinition*)pd.p, pvc) )
                    WriteAssociationProperty((FdoAssociationPropertyDefinition*)pd.p, reader);
            }
            else
            {

                FdoPtr<FdoPropertyValue> pv((FdoPropertyValue*)pvc->GetItem(pd->GetName()));
                if(pv != NULL)
                    WriteProperty(pd, pv);
                else
                    WriteProperty(pd, reader);
            }
        }
        else
        {
            WriteProperty(pd, reader);
        }
        //}
    }
}


FdoDataPropertyDefinitionCollection* GwsBinaryFeatureWriter::FindIDProps(FdoClassDefinition* fc)
{
    FdoPtr <FdoDataPropertyDefinitionCollection> idpdc = fc->GetIdentityProperties();
    FdoPtr<FdoClassDefinition> base = FDO_SAFE_ADDREF(fc);

    //go up class hierarchy to find base class (it has the identity properties)
    while ((base = base->GetBaseClass()) != NULL)
        idpdc = base->GetIdentityProperties();

    if (idpdc->GetCount() == 0)
        return NULL;

    return (FDO_SAFE_ADDREF (idpdc.p));
}
