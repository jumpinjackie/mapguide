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

#ifndef _MG_LAYER_H_
#define _MG_LAYER_H_

class MgLayer;
template class MG_MAPGUIDE_API Ptr<MgLayer>;

#ifdef WIN32
#pragma warning(disable:4251)
#endif

/// \defgroup MgLayer MgLayer
/// \ingroup Mapping_Service_Module
/// \{

/////////////////////////////////////////////////////////////////
/// \brief
/// Gets and sets properties of a layer.
/// \todo
class MG_MAPGUIDE_API MgLayer : public MgLayerBase
{
    DECLARE_CREATE_OBJECT()
    DECLARE_CLASSNAME(MgLayer)

PUBLISHED_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a MgLayer object from a layer definition.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgLayer(MgResourceIdentifier layerDefinition, MgResourceService resourceService);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgLayer(MgResourceIdentifier layerDefinition, MgResourceService resourceService);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgLayer(MgResourceIdentifier layerDefinition, MgResourceService resourceService);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param layerDefinition (MgResourceIdentifier)
    /// Identifier of the layer definition in a
    /// repository.
    /// \param resourceService (MgResourceService)
    /// An MgResourceService that can be used to
    /// retrieve the layer definition.
    ///
    /// \todo
    ///   * [[Job for Philip: I need a code sample here.]]
    ///
    MgLayer(MgResourceIdentifier* layerDefinition, MgResourceService* resourceService);

  
INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates an empty MgLayer object
    ///
    MgLayer();

    //////////////////////////////////////////////////////////////////
    /// Parse the layer definition XML and extracts scale ranges,
    /// feature source and feature class from it
    void GetLayerInfoFromDefinition(MgResourceService* resourceService);

protected:
    /// \brief
    /// Destruct a MgLayer object
    ///
    /// \return
    /// Nothing
    ///
    virtual ~MgLayer();

    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifier.
    ///
    virtual INT32 GetClassId()
    {
        return m_cls_id;
    }
    

    /// \brief
    /// Dispose this object.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual void Dispose()
    {
        delete this;
    }

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_MapLayer_Layer;
    
};
/// \}

#endif
