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

#ifndef MGFOUNDATIONCONFIGPROPERTIES_H_
#define MGFOUNDATIONCONFIGPROPERTIES_H_

struct MgConfigValidationInfo;

/// \cond INTERNAL
//////////////////////////////////////////////////////////////////
/// \brief
/// Properties stored in an MgConfiguration object.  Note that the properties are
/// organized into Property Sections.  These sections may be used in the API to
/// set/get groups of configuration properties.
/// INTERNAL_ONLY:
class MG_FOUNDATION_API MgFoundationConfigProperties
{
    DECLARE_CLASSNAME(MgFoundationConfigProperties)

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// Common Configuration Validation Information Maps
    ///

    /// GENERAL PROPERTIES SECTION ---------------------------------------------------------------------------------------

    /// General application properties
    static const STRING GeneralPropertiesSection;                       /// value("GeneralProperties")

    /// Application's locale
    static const STRING GeneralPropertyLocale;                          /// value("Locale")
    static const STRING DefaultGeneralPropertyLocale;                   /// value("")

    /// Application's default message locale
    static const STRING GeneralPropertyDefaultMessageLocale;            /// value("DefaultMessageLocale")
    static const STRING DefaultGeneralPropertyDefaultMessageLocale;     /// value("en")

    /// Root folder for application's localization resources
    static const STRING GeneralPropertyResourcesPath;                   /// value("ResourcesPath")
    static const STRING DefaultGeneralPropertyResourcesPath;            /// value("Resources/")

    /// Sets the time duration (in seconds) between service registrations
    static const STRING GeneralPropertyServiceRegistrationTimerInterval;        /// value("ServiceRegistrationTimerInterval")
    static const INT32 DefaultGeneralPropertyServiceRegistrationTimerInterval;  /// value(60)

    /// Temporary file path
    static const STRING GeneralPropertyTempPath;                        /// value("TempPath")
    static const STRING DefaultGeneralPropertyTempPath;                 /// value("Temp/")

    /// TCP/IP Maximum Transmission Unit
    static const STRING GeneralPropertyTcpIpMtu;                        /// value("TcpIpMtu")
    static const INT32 DefaultGeneralPropertyTcpIpMtu;                  /// value(1460)

public:

    // Common Configuration Validation Information Maps
    static const MgConfigValidationInfo sm_cviGeneralProperties[];
};
/// \endcond

#endif
