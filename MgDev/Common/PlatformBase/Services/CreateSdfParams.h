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

#ifndef _MG_CREATE_SDF_PARAMS_H
#define _MG_CREATE_SDF_PARAMS_H

/// \defgroup MgCreateSdfParams MgCreateSdfParams
/// \ingroup Feature_Service_classes
/// \{

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// Contains the parameters used to create an SDF feature source.
/// This is a deprecated class. Please use MgFileFeatureSourceParams
/// to create the parameters used to an SDF feature source.
///
/// \remarks
/// Use MgFeatureService::DescribeSchema and
/// MgFeatureService::GetSpatialContexts to get the values needed
/// to construct and configure the MgCreateSdfParams object.
///
/// <!-- Example (PHP) -->
/// \htmlinclude PHPExampleTop.html
/// \code
/// $schemaCollection = $featureService->DescribeSchema($activeFeatSrcResId, $schemaName, NULL);
/// $schema = $schemaCollection->GetItem(0);
/// $activeOnly = true;
/// $activeSpatialContextReader = $featureService->GetSpatialContexts($activeFeatSrcResId, $activeOnly);
/// $activeSpatialContextReader->ReadNext();
/// $spatialContextName = $activeSpatialContextReader->GetName();
/// $coordSysWkt = $activeSpatialContextReader->GetCoordinateSystemWkt();
/// $createSdfParams = new MgCreateSdfParams($spatialContextName, $coordSysWkt, $schema);
/// $createSdfParams->SetSpatialContextDescription($activeSpatialContextReader->GetDescription());
/// $createSdfParams->SetXYTolerance($activeSpatialContextReader->GetXYTolerance());
/// $createSdfParams->SetZTolerance($activeSpatialContextReader->GetZTolerance());
/// \endcode
/// \htmlinclude ExampleBottom.html
///
class MG_PLATFORMBASE_API MgCreateSdfParams : public MgFileFeatureSourceParams
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgCreateSdfParams)

PUBLISHED_API:
    //////////////////////////////////////////////////
    /// \brief
    /// Constructs an empty MgCreateSdfParams object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  MgCreateSdfParams();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  MgCreateSdfParams();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  MgCreateSdfParams();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgCreateSdfParams();

    /////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgCreateSdfParams object with the supplied
    /// parameters.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  MgCreateSdfParams(string spatialContextName, string srsWkt, MgFeatureSchema featureSchema);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  MgCreateSdfParams(String spatialContextName, String srsWkt, MgFeatureSchema featureSchema);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  MgCreateSdfParams(string spatialContextName, string srsWkt, MgFeatureSchema featureSchema);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param spatialContextName (String/string)
    /// The name of the spatial context. This is
    /// a user defined spatial context for the
    /// SDF file. Only one spatial context is
    /// supported.
    /// \param srsWkt (String/string)
    /// An OGC WKT string representing the
    /// coordinate system for the SDF file.
    /// \param featureSchema (MgFeatureSchema)
    /// The schema definition for the SDF file.
    ///
    /// \return
    /// Returns nothing.
    ///
    MgCreateSdfParams(CREFSTRING spatialContextName, CREFSTRING srsWkt, MgFeatureSchema* featureSchema);

    ///////////////////////////////////////////////
    /// \brief
    /// Always throws an MgInvalidOperationException instance because we
    /// can't change FDO provider for MgCreateSdfParams.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  string SetProviderName(string name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  String SetProviderName(String name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  string SetProviderName(string name);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// The FDO provider name of the feature source.
    ///
    /// \return
    /// Returns nothing.
    ///
    virtual void SetProviderName(CREFSTRING name);

protected:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the class Id.
    ///
    /// \return
    /// Returns the integer value of the class Id.
    ///
    INT32 GetClassId();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Self destructing method.
    ///
    virtual void Dispose();

    virtual ~MgCreateSdfParams();

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_CreateSdfParams;
};
/// \}

#endif
