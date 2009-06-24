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

#include "PlatformBase.h"
#include "ResourceDefs.h"

///////////////////////////////////////////////////////////////////////////////
/// Reserved character sets
/// INTERNAL - Do not document
///
const STRING MgReservedCharacterSet::Name           = L"\\:*?\"<>|&'%=/";
const STRING MgReservedCharacterSet::Path           = L"\\:*?\"<>|&'%=";
const STRING MgReservedCharacterSet::WhiteSpace     = L" \t\r\n\v\f";


///////////////////////////////////////////////////////////////////////////////
/// Repository types
///
const STRING MgRepositoryType::Library              = L"Library";
const STRING MgRepositoryType::Session              = L"Session";
const STRING MgRepositoryType::Site                 = L"Site";


///////////////////////////////////////////////////////////////////////////////
/// Resource types
///
const STRING MgResourceType::Map                    = L"Map";
const STRING MgResourceType::MapDefinition          = L"MapDefinition";
const STRING MgResourceType::LayerDefinition        = L"LayerDefinition";
const STRING MgResourceType::DrawingSource          = L"DrawingSource";
const STRING MgResourceType::FeatureSource          = L"FeatureSource";
const STRING MgResourceType::Folder                 = L"Folder";
const STRING MgResourceType::LoadProcedure          = L"LoadProcedure";
const STRING MgResourceType::PrintLayout            = L"PrintLayout";
const STRING MgResourceType::Selection              = L"Selection";
const STRING MgResourceType::SymbolDefinition       = L"SymbolDefinition";
const STRING MgResourceType::SymbolLibrary          = L"SymbolLibrary";
const STRING MgResourceType::WebLayout              = L"WebLayout";
const STRING MgResourceType::ApplicationDefinition  = L"ApplicationDefinition";
const STRING MgResourceType::PrintLayoutElement     = L"PrintLayoutElement";
const STRING MgResourceType::PrintLayoutElementDefinition   = L"PrintLayoutElementDefinition";


const STRING MgResourceType::User                   = L"User";
const STRING MgResourceType::Group                  = L"Group";
const STRING MgResourceType::Role                   = L"Role";


///////////////////////////////////////////////////////////////////////////////
/// Resource tags
///
const STRING MgResourceTag::LoginUsername           = L"%MG_LOGIN_USERNAME%";
const STRING MgResourceTag::LoginPassword           = L"%MG_LOGIN_PASSWORD%";
const STRING MgResourceTag::Username                = L"%MG_USERNAME%";
const STRING MgResourceTag::Password                = L"%MG_PASSWORD%";
const STRING MgResourceTag::DataFilePath            = L"%MG_DATA_FILE_PATH%";
const STRING MgResourceTag::DataPathAliasBegin      = L"%MG_DATA_PATH_ALIAS[";
const STRING MgResourceTag::DataPathAliasEnd        = L"]%";


///////////////////////////////////////////////////////////////////////////////
/// Resource data names
///
const STRING MgResourceDataName::UserCredentials    = L"MG_USER_CREDENTIALS";


///////////////////////////////////////////////////////////////////////////////
/// Resource data types
///
const STRING MgResourceDataType::File               = L"File";
const STRING MgResourceDataType::Stream             = L"Stream";
const STRING MgResourceDataType::String             = L"String";


///////////////////////////////////////////////////////////////////////////////
/// Pre-processing types
///
const STRING MgResourcePreProcessingType::Substitution = L"Substitution";


///////////////////////////////////////////////////////////////////////////////
/// Types of resource permissions
///
const STRING MgResourcePermission::NoAccess         = L"n";
const STRING MgResourcePermission::ReadOnly         = L"r";
const STRING MgResourcePermission::ReadWrite        = L"r,w";


///////////////////////////////////////////////////////////////////////////////
/// Names of resource folders
/// INTERNAL - Do not document
///
const STRING MgResourceFolder::Users                = L"Users";
const STRING MgResourceFolder::Groups               = L"Groups";
const STRING MgResourceFolder::Roles                = L"Roles";

///////////////////////////////////////////////////////////////////////////////
/// Types of unmanaged data that can be returned
///
const STRING MgResourceUnmanagedDataType::Folders         = L"Folders";
const STRING MgResourceUnmanagedDataType::Files           = L"Files";
const STRING MgResourceUnmanagedDataType::Both            = L"Both";
