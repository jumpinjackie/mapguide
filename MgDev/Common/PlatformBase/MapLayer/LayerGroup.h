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

#ifndef _MG_LAYER_GROUP_H_
#define _MG_LAYER_GROUP_H_

/// \defgroup MgLayerGroup MgLayerGroup
/// \ingroup Maps_and_Layers_Module
/// \{

class MgLayerGroup;
class MgLayerGroupCollection;

#ifndef TEMPL_INST_LAYER_GROUP
#define TEMPL_INST_LAYER_GROUP
template class MG_PLATFORMBASE_API Ptr<MgLayerGroup>;
#endif

/////////////////////////////////////////////////////////////////
/// \brief
/// Manipulates the properties of a layer group.
/// \todo
///   * [[Job for Philip: Clarify what a layer group is.]]
///   * [[Job for Phliip: Add an example.]]
class MG_PLATFORMBASE_API MgLayerGroup : public MgNamedSerializable
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgLayerGroup)

PUBLISHED_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgLayerGroup object with the specified name.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgLayerGroup(string name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgLayerGroup(String name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgLayerGroup(string name);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// Name of the group
    ///
    MgLayerGroup(CREFSTRING name);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the group name.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetName();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the group name as a string.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See \link MgMapBase MgMapBase class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    virtual STRING GetName();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a unique identifier for this layer group.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetObjectId();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetObjectId();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetObjectId();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the unique identifier as a string.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See \link MgMapBase MgMapBase class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    /// \todo
    /// [[Job for Philip: Clarify -- is this unique with the
    /// repository? or where?]]
    ///
    virtual STRING GetObjectId();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the type of the layer group.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetLayerGroupType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetLayerGroupType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetLayerGroupType();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the type of the layer group.
    ///
    virtual INT32 GetLayerGroupType();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the group this group is organized within.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgLayerGroup GetGroup();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgLayerGroup GetGroup();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgLayerGroup GetGroup();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the parent group, or NULL if the group is at the top
    /// level of the hierarchy.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See \link MgMapBase MgMapBase class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    virtual MgLayerGroup* GetGroup();  /// __get, __set

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Changes the group this group is organized within.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetGroup(MgLayerGroup group);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetGroup(MgLayerGroup group);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetGroup(MgLayerGroup group);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param group (MgLayerGroup)
    /// Group for this group to belong to, or NULL if the
    /// group should not belong to any group.
    ///
    /// \return
    /// Returns nothing.
    ///
    virtual void SetGroup(MgLayerGroup* group);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the layer group is potentially visible.
    ///
    /// \remarks
    /// Note that this method may return true even though the group
    /// is not visible. This will occur if one of the groups this
    /// group is organized within is not visible.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool GetVisible();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean GetVisible();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool GetVisible();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns true if the layer group is potentially visible, or
    /// false otherwise.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See \link MgMapBase MgMapBase class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    virtual bool GetVisible();  /// __get, __set

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Alters the group potential visibility.
    ///
    /// \remarks
    /// Note that setting this property to true will not necessarily
    /// guarantee the group will be visible. The group will be
    /// visible only when this property is true, and all groups this
    /// group is organized within are visible.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetVisible(bool visible);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetVisible(boolean visible);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetVisible(bool visible);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param visible (boolean/bool)
    /// True if the group should be visible, or false
    /// otherwise.
    ///
    /// \return
    /// Returns nothing.
    ///
    virtual void SetVisible(bool visible);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines the actual visibility of the group.
    ///
    /// \remarks
    /// The visibility depends on the visible property of the group,
    /// and the visible property of each group this group is
    /// organized within.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool IsVisible();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean IsVisible();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool IsVisible();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns true if the layer group is visible, or false
    /// otherwise.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See \link MgMapBase MgMapBase class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    virtual bool IsVisible();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines whether an entry for the group should appear in
    /// the legend.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool GetDisplayInLegend();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean GetDisplayInLegend();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool GetDisplayInLegend();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns true if the group should appear in the legend, or
    /// false otherwise.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See \link MgMapBase MgMapBase class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    virtual bool GetDisplayInLegend();  /// __get, __set

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Specifies whether an entry for the group should appear in the
    /// legend.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetDisplayInLegend(bool displayInLegend);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetDisplayInLegend(boolean displayInLegend);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetDisplayInLegend(bool displayInLegend);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param displayInLegend (boolean/bool)
    /// True if the group should appear in the
    /// legend, or false otherwise.
    ///
    /// \return
    /// Returns nothing.
    ///
    virtual void SetDisplayInLegend(bool displayInLegend);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines whether the entry for the group should initially
    /// be expanded in the legend, if possible.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool GetExpandInLegend();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean GetExpandInLegend();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool GetExpandInLegend();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns true if the group should initially be expanded in the legend, or
    /// false otherwise.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See \link MgMapBase MgMapBase class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    virtual bool GetExpandInLegend();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the label to be displayed for the group in the legend.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetLegendLabel();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetLegendLabel();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetLegendLabel();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the legend label as a string.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See \link MgMapBase MgMapBase class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    virtual STRING GetLegendLabel();  /// __get, __set

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the label to be displayed for the group in the legend.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetLegendLabel(string legendLabel);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetLegendLabel(String legendLabel);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetLegendLabel(string legendLabel);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param legendLabel (String/string)
    /// Legend label.
    ///
    /// \return
    /// Returns nothing.
    ///
    virtual void SetLegendLabel(CREFSTRING legendLabel);

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an empty MgLayerGroup object
    ///
    MgLayerGroup();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serialize data to a stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserialize data from a stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Does it allow to set the name
    ///
    /// \return
    /// true - allows to set the name
    /// false - can not set the name
    ///
    virtual bool CanSetName();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Attaches this group to a set of groups
    ///
    virtual void SetContainer(MgLayerGroupCollection* cont);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Specifies whether the entry for the group should initially
    /// be expanded in the legend, if possible.
    ///
    virtual void SetExpandInLegend(bool expandInLegend);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the type of the layer group
    ///
    virtual void SetLayerGroupType(INT32 layerGroupType);

protected:
    /// \brief
    /// Destruct a MgLayerGroup object
    ///
    /// \return
    /// Nothing
    ///
    virtual ~MgLayerGroup();

    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
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
    static const INT32 m_cls_id = PlatformBase_MapLayer_LayerGroup;

private:
    STRING              m_name;
    STRING              m_objectId;
    INT32               m_type;
    Ptr<MgLayerGroup>   m_group;
    bool                m_visible;
    bool                m_displayInLegend;
    bool                m_expandInLegend;
    STRING              m_legendLabel;
    MgLayerGroupCollection* m_groups;   //no need for reference counting here
};
/// \}

#endif
