//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef MGFEATUREDEFS_H
#define MGFEATUREDEFS_H

/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Operation IDs for Feature Service.
/// INTERNAL do not document.
///
class MG_PLATFORMBASE_API MgFeatureServiceOpId
{
INTERNAL_API:
    static const int GetFeatureProviders_Id         = 0x1111ED01;
    static const int GetConnectionPropertyValues_Id = 0x1111ED02;
    static const int TestConnection_Id              = 0x1111ED03;
    static const int GetCapabilities_Id             = 0x1111ED04;
    static const int DescribeSchema_Id              = 0x1111ED05;
    static const int SelectFeatures_Id              = 0x1111ED06;
    static const int SelectAggregate_Id             = 0x1111ED07;
    static const int ExecuteSqlQuery_Id             = 0x1111ED08;
    static const int ExecuteSqlNonQuery_Id          = 0x1111ED09;
    static const int GetSpatialContexts_Id          = 0x1111ED0A;
    static const int UpdateFeatures_Id              = 0x1111ED0B;
    static const int TestConnectionWithResource_Id  = 0x1111ED0C;
    static const int GetFeatures_Id                 = 0x1111ED0D;
    static const int CloseFeatureReader_Id          = 0x1111ED0E;
    static const int GetLongTransactions_Id         = 0x1111ED0F;

    static const int GetSchemas_Id                  = 0x1111ED10;
    static const int GetClasses_Id                  = 0x1111ED11;
    static const int GetClassDefinition_Id          = 0x1111ED12;
    static const int GetRaster_Id                   = 0x1111ED13;
    static const int GetSqlRows_Id                  = 0x1111ED14;
    static const int CloseSqlReader_Id              = 0x1111ED15;
    static const int GetDataRows_Id                 = 0x1111ED16;
    static const int CloseDataReader_Id             = 0x1111ED17;
    static const int SchemaToXml_Id                 = 0x1111ED18;
    static const int XmlToSchema_Id                 = 0x1111ED19;
    static const int DescribeSchemaAsXml_Id         = 0x1111ED1A;
    static const int CreateFeatureSource_Id         = 0x1111ED1B;
    static const int GetIdentityProperties_Id       = 0x1111ED1C;
    static const int DescribeWfsFeatureType_Id      = 0x1111ED1D;
    static const int GetWfsFeature_Id               = 0x1111ED1E;
    static const int CloseGwsFeatureReader_Id       = 0x1111ED1F;

    static const int EnumerateDataStores_Id         = 0x1111ED20;
    static const int GetSchemaMapping_Id            = 0x1111ED21;
    static const int SelectFeaturesWithCS_Id        = 0x1111ED22;
    static const int GetLockedFeatures_Id           = 0x1111ED23;
    static const int SetLongTransaction_Id          = 0x1111ED24;
    static const int GetFdoCacheInfo_Id             = 0x1111ED25;
    static const int GetClassDefinition2_Id         = 0x1111ED26;
};
/// \endcond

#endif
