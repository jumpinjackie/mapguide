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

#ifndef _MGWEBINVOKEURLCOMMAND_H_
#define _MGWEBINVOKEURLCOMMAND_H_

class MgWebInvokeUrlCommand;
template class MG_WEBAPP_API Ptr<MgWebInvokeUrlCommand>;

class MG_WEBAPP_API MgWebInvokeUrlCommand : public MgWebUiTargetCommand
{
PUBLISHED_API:

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the url to invoke to execute the command
    /// </summary>
    /// <returns>
    /// Url to invoke to execute the command
    /// </returns>
    STRING GetUrl();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the number of parameters to include to the URL for this command
    /// </summary>
    /// <returns>
    /// Number of parameters to include to the command URL
    /// </returns>
    INT32 GetParameterCount();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the name of parameter at the specified index in the list of parameters
    /// </summary>
    /// <returns>
    /// Parameter name
    /// </returns>
    /// <remarks>
    /// Throws an MgIndexOutOfRangeException if index is out of range
    /// </remarks>
    STRING GetParameterNameAt(INT32 index);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the value of parameter at the specified index in the list of parameters
    /// </summary>
    /// <returns>
    /// Parameter value
    /// </returns>
    /// <remarks>
    /// Throws an MgIndexOutOfRangeException if index is out of range
    /// </remarks>
    STRING GetParameterValueAt(INT32 index);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Tells if the command should be disable when the selection set is empty
    /// </summary>
    /// <returns>
    /// True if if the command should be disabled when the selection set is empty, false otherwise.
    /// </returns>
    bool DisabledIfSelectionEmpty();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the number of layers in the layer constraint list
    /// </summary>
    /// <returns>
    /// Number of layers in the layer constraint list
    /// </returns>
    INT32 GetLayerCount();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the name of layers at the specified index in the list of layer constraint
    /// </summary>
    /// <returns>
    /// Layer name
    /// </returns>
    /// <remarks>
    /// Throws an MgIndexOutOfRangeException if index is out of range
    /// </remarks>
    STRING GetLayerNameAt(INT32 index);


INTERNAL_API:

    MgWebInvokeUrlCommand();

    ///////////////////////////////////////////////////////////////////////////
    // Set the url
    //
    void SetUrl(CREFSTRING url);

    ///////////////////////////////////////////////////////////////////////////
    // Get the parameter collection
    //
    MgPropertyCollection* GetParameters();

    ///////////////////////////////////////////////////////////////////////////
    // Instructs if the command should be disabled or not when the selection set is empty
    //
    void SetDisabledWhenSelectionEmpty(bool disable);

    ///////////////////////////////////////////////////////////////////////////
    // Gets the layer constraint list
    //
    MgStringCollection* GetLayers();

CLASS_ID:
    static const INT32 m_cls_id = WebApp_InvokeUrlCommand;

protected:

    ///////////////////////////////////////////////////////////////////////////
    // Get the unique identifier for the class
    //
    virtual INT32 GetClassId();

    ///////////////////////////////////////////////////////////////////////////
    // Dispose this object
    //
    virtual void Dispose();

protected:
    STRING      m_url;
    Ptr<MgPropertyCollection> m_params;
    Ptr<MgStringCollection>   m_layers;
    bool        m_disabledWhenSelectionEmpty;
    STRING      m_layerConstraints;
};

#endif // _MGWEBINVOKEURLCOMMAND_H_
