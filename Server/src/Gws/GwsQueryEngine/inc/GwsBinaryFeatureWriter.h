// Copyright (C) 2004-2009 by Autodesk, Inc.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of version 2.1 of the GNU Lesser
// General Public License as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef GWS_BINARYFEATUREWRITER_H
#define GWS_BINARYFEATUREWRITER_H

#include "GwsBinaryWriter.h"

///////////////////////////////////////////////////////////////////////////////
class GwsBinaryFeatureWriter
{
public:
    GwsBinaryFeatureWriter() : m_wrtr(256) {}
    ~GwsBinaryFeatureWriter() {}

    //get the buffer
    unsigned char* ToBuffer(int& bufLen);
    void Reset() { m_wrtr.Reset(); }

    //write property value
    void WriteProperty(FdoPropertyDefinition* pd, FdoPropertyValue* pv,
                       bool forAssociation = false );
    void WriteProperty(FdoPropertyDefinition* pd, FdoIFeatureReader* reader);

    //write associtation property value
    bool WriteAssociationProperty(FdoAssociationPropertyDefinition* apd,
                                  FdoPropertyValueCollection* pvc);
    void WriteAssociationProperty(FdoAssociationPropertyDefinition* apd,
                                  FdoIFeatureReader* reader);
    //write the feature
    void WriteFeature(FdoClassDefinition* fc, FdoString* fcName,
                      FdoPropertyValueCollection* pvc);
    void WriteFeature(FdoClassDefinition* fc, FdoString* fcName,
                      FdoPropertyValueCollection* pvc,
                      FdoIFeatureReader* reader);

protected:
    FdoDataPropertyDefinitionCollection* FindIDProps(FdoClassDefinition* fc);

private:
    BinaryWriter m_wrtr;
};

#endif //GWS_BINARYFEATUREWRITER_H
