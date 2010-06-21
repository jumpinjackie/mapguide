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

#include "WebApp.h"

///////////////////////////////////////////////////////////////////////////
// Constructor
//
MgWebInvokeUrlCommand::MgWebInvokeUrlCommand()
: m_disabledWhenSelectionEmpty(false)
{
    m_action = MgWebActions::InvokeUrl;
    m_params = new MgPropertyCollection();
    m_layers = new MgStringCollection();

    if(m_params == NULL || m_layers == NULL)
        throw new MgOutOfMemoryException(L"MgWebInvokeUrlCommand.MgWebInvokeUrlCommand", __LINE__, __WFILE__, NULL, L"", NULL);
}

///////////////////////////////////////////////////////////////////////////
// Returns the url to invoke to execute the command
//
STRING MgWebInvokeUrlCommand::GetUrl()
{
    return m_url;
}

///////////////////////////////////////////////////////////////////////////
// Returns the additional parameters to pass to the request
//
INT32 MgWebInvokeUrlCommand::GetParameterCount()
{
    return m_params->GetCount();
}

///////////////////////////////////////////////////////////////////////////
// Returns the name of parameter at the specified index in the list of parameters
//
STRING MgWebInvokeUrlCommand::GetParameterNameAt(INT32 index)
{
    Ptr<MgStringProperty> param = (MgStringProperty*)m_params->GetItem(index);
    return param->GetName();
}

///////////////////////////////////////////////////////////////////////////
// Returns the value of parameter at the specified index in the list of parameters
//
STRING MgWebInvokeUrlCommand::GetParameterValueAt(INT32 index)
{
    Ptr<MgStringProperty> param = (MgStringProperty*)m_params->GetItem(index);
    return param->GetValue();
}

///////////////////////////////////////////////////////////////////////////
// Tells if the command should be disable when the selection set is empty
//
bool MgWebInvokeUrlCommand::DisabledIfSelectionEmpty()
{
    return m_disabledWhenSelectionEmpty;
}

///////////////////////////////////////////////////////////////////////////
// Returns the number of layers in the layer constraint list
//
INT32 MgWebInvokeUrlCommand::GetLayerCount()
{
    return m_layers->GetCount();
}

///////////////////////////////////////////////////////////////////////////
// Returns the name of layers at the specified index in the list of layer constraint
//
STRING MgWebInvokeUrlCommand::GetLayerNameAt(INT32 index)
{
    return m_layers->GetItem(index);
}

///////////////////////////////////////////////////////////////////////////
// Set the url
//
void MgWebInvokeUrlCommand::SetUrl(CREFSTRING url)
{
    m_url = url;
}

///////////////////////////////////////////////////////////////////////////
// Get the parameter collection
//
MgPropertyCollection* MgWebInvokeUrlCommand::GetParameters()
{
    return SAFE_ADDREF((MgPropertyCollection*)m_params);
}

///////////////////////////////////////////////////////////////////////////
// Instructs if the command should be disabled or not when the selection set is empty
//
void MgWebInvokeUrlCommand::SetDisabledWhenSelectionEmpty(bool disable)
{
    m_disabledWhenSelectionEmpty = disable;
}

///////////////////////////////////////////////////////////////////////////
// Gets the layer constraint list
//
MgStringCollection* MgWebInvokeUrlCommand::GetLayers()
{
    return SAFE_ADDREF((MgStringCollection*)m_layers);
}

///////////////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgWebInvokeUrlCommand::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////
// Dispose this object
//
void MgWebInvokeUrlCommand::Dispose()
{
    delete this;
}
