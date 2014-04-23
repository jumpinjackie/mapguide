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

#include "Foundation.h"

///////////////////////////////////////////////////////////////////////////////
/// Length ranges of string properties
///
#define MG_CONFIG_MIN_FILE_NAME_LENGTH                  1
#define MG_CONFIG_MAX_FILE_NAME_LENGTH                  128
#define MG_CONFIG_FILE_NAME_RESERVED_CHARACTERS         L"\\/:*?\"<>|"

#define MG_CONFIG_MIN_FOLDER_NAME_LENGTH                1
#define MG_CONFIG_MAX_FOLDER_NAME_LENGTH                128
#define MG_CONFIG_FOLDER_NAME_RESERVED_CHARACTERS       MG_CONFIG_FILE_NAME_RESERVED_CHARACTERS

#define MG_CONFIG_MIN_LOCALE_LENGTH                     2
#define MG_CONFIG_MAX_LOCALE_LENGTH                     5

#define MG_CONFIG_MIN_OPTIONAL_STRING_LENGTH            0
#define MG_CONFIG_MAX_OPTIONAL_STRING_LENGTH            255

#define MG_CONFIG_MIN_PATH_LENGTH                       1
#define MG_CONFIG_MAX_PATH_LENGTH                       255
#define MG_CONFIG_PATH_RESERVED_CHARACTERS              L"*?\"<>|"


// Configuration file section/property names and default values

// ******************************************************************
// General Properties
// ******************************************************************
const STRING MgFoundationConfigProperties::GeneralPropertiesSection                                   = L"GeneralProperties";
const STRING MgFoundationConfigProperties::GeneralPropertyLocale                                      = L"Locale";
const STRING MgFoundationConfigProperties::DefaultGeneralPropertyLocale                               = L"";
const STRING MgFoundationConfigProperties::GeneralPropertyDefaultMessageLocale                        = L"DefaultMessageLocale";
const STRING MgFoundationConfigProperties::DefaultGeneralPropertyDefaultMessageLocale                 = L"en";      // ISO 639-1 name
const STRING MgFoundationConfigProperties::GeneralPropertyResourcesPath                               = L"ResourcesPath";
const STRING MgFoundationConfigProperties::DefaultGeneralPropertyResourcesPath                        = L"Resources/";
const STRING MgFoundationConfigProperties::GeneralPropertyTempPath                                    = L"TempPath";
const STRING MgFoundationConfigProperties::DefaultGeneralPropertyTempPath                             = L"Temp/";
const STRING MgFoundationConfigProperties::GeneralPropertyTcpIpMtu                                    = L"TcpIpMtu";
const INT32  MgFoundationConfigProperties::DefaultGeneralPropertyTcpIpMtu                             = 1460;

///////////////////////////////////////////////////////////////////////////////
/// Common Configuration Validation Information Maps
///
const MgConfigValidationInfo MgFoundationConfigProperties::sm_cviGeneralProperties[] =
{
    { MgFoundationConfigProperties::GeneralPropertyLocale           , MgPropertyType::String    , MG_CONFIG_MIN_OPTIONAL_STRING_LENGTH  , MG_CONFIG_MAX_OPTIONAL_STRING_LENGTH  , L""                                       },
    { MgFoundationConfigProperties::GeneralPropertyDefaultMessageLocale, MgPropertyType::String , MG_CONFIG_MIN_LOCALE_LENGTH           , MG_CONFIG_MAX_LOCALE_LENGTH           , L""                                       },
    { MgFoundationConfigProperties::GeneralPropertyResourcesPath    , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgFoundationConfigProperties::GeneralPropertyTempPath         , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgFoundationConfigProperties::GeneralPropertyTcpIpMtu         , MgPropertyType::Int32     , 1                                     , 65535                                 , L""                                       },
    { L""                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

bool InitializeFoundationConfig()
{
    MgConfiguration* mgConfig = MgConfiguration::GetInstance();
    if (NULL != mgConfig)
    {
        mgConfig->AppendValidationInfo(MgFoundationConfigProperties::GeneralPropertiesSection,
            (MgConfigValidationInfo*) MgFoundationConfigProperties::sm_cviGeneralProperties);
        return true;
    }
    return false;
}

static bool bFoundationConfigInit = InitializeFoundationConfig();

