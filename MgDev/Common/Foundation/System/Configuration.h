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

#ifndef MG_CONFIGURATION_H_
#define MG_CONFIGURATION_H_

#include <list>

///////////////////////////////////////////////////////////////////////////////
/// MapGuide Configuration try/catch/throw macros.
///
#define MG_CONFIGURATION_TRY()                                                \
    MG_TRY()                                                                  \

#define MG_CONFIGURATION_CATCH(methodName)                                    \
    MG_CATCH(methodName)                                                      \

#define MG_CONFIGURATION_THROW()                                              \
    MG_THROW()                                                                \

#define MG_CONFIGURATION_CATCH_AND_THROW(methodName)                          \
    MG_CATCH_AND_THROW(methodName)                                            \

/// \cond INTERNAL
//////////////////////////////////////////////////////////////////
/// Validation information on configuration properties.
///
struct MgConfigValidationInfo
{
    STRING m_propertyName; // Name of the configuration property
    INT32 m_propertyType;  // Type of the configuration property
    double m_minimumValue; // Minimum value of the numeric property or minimum value of the length of the string property
    double m_maximumValue; // Maximum value of the numeric property or maximum value of the length of the string property
    STRING m_reservedCharacters; // Reserved characters for the property value.

    STRING GetMinimumValue() const;
    STRING GetMaximumValue() const;
};
/// \endcond

/// \cond INTERNAL
class MG_FOUNDATION_API MgConfiguration : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgConfiguration)

INTERNAL_API:
    /// \brief
    /// Constructor
    ///

    MgConfiguration();

    /// \brief
    /// Destructor
    ///

    virtual ~MgConfiguration();

    /// \brief
    /// Self Destructor
    ///

    virtual void Dispose();

    /// \brief
    /// Get pointer to a process-wide MgConfiguration.
    ///

    static MgConfiguration* GetInstance();

    /// \brief
    /// Loads the configuration file specified
    ///
    /// \param fileName
    /// name of file to load
    ///
    /// \return
    /// Nothing
    ///

    void LoadConfiguration(CREFSTRING fileName);

    /// \brief
    /// Saves the configuration data to file specified
    ///
    /// \param fileName
    /// name of file to save
    ///
    /// \return
    /// Nothing
    ///

    void SaveConfiguration(CREFSTRING fileName);

    /// \brief
    /// Appends validation information to the internal validation map.
    ///
    /// \param section
    /// Section in config file
    /// \param validationInfo
    /// Validation information structure to append
    ///
    /// \return
    /// Nothing
    ///

    void AppendValidationInfo(CREFSTRING section, MgConfigValidationInfo* validationInfo);

    /// \brief
    /// Get Integer value from a section
    ///
    /// \param section
    /// Section in config file
    /// \param property
    /// Property in the section
    /// \param value
    /// Out parameter for value to store
    /// \param defaultValue
    /// Default value in case property is missing
    ///
    /// \return
    /// Nothing
    ///

    void GetIntValue(CREFSTRING section, CREFSTRING property, INT32& value, INT32 defaultValue);

    /// \brief
    /// Get bool value from a section
    ///
    /// \param section
    /// Section in config file
    /// \param property
    /// Property in the section
    /// \param value
    /// Out parameter for value to store
    /// \param defaultValue
    /// Default value in case property is missing
    ///
    /// \return
    /// Nothing
    ///

    void GetBoolValue(CREFSTRING section, CREFSTRING property, bool& value, bool defaultValue);

    /// \brief
    /// Get Double value from a section
    ///
    /// \param section
    /// Section in config file
    /// \param property
    /// Property in the section
    /// \param value
    /// Out parameter for value to store
    /// \param defaultValue
    /// Default value in case property is missing
    ///
    /// \return
    /// Nothing
    ///

    void GetDoubleValue(CREFSTRING section, CREFSTRING property, double& value, double defaultValue);

    /// \brief
    /// Get Integer value from a section
    ///
    /// \param section
    /// Section in config file
    /// \param property
    /// Property in the section
    /// \param value
    /// Out parameter for value to store
    /// \param defaultValue
    /// Default value in case property is missing
    ///
    /// \return
    /// Nothing
    ///

    void GetStringValue(CREFSTRING section, CREFSTRING property, REFSTRING value, CREFSTRING defaultValue);

    /// \brief
    /// Determines whether file was loaded successfully
    ///
    /// \return
    /// true  - file was loaded
    /// false - file was not loaded
    ///

    bool IsFileLoaded();

    /// \brief
    /// Get the file name which is loaded by this instance
    ///
    /// \return
    /// Name of file
    ///

    STRING GetFileName();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the properties for the specified section.
    ///
    /// \param section
    /// Name of the Section in the config file.
    ///
    /// \return
    /// MgPropertyCollection representing the properties for the specified section.
    ///
    ///

    MgPropertyCollection* GetProperties(CREFSTRING section);

    /// \brief
    /// Sets the properties for the specified section.
    ///
    /// \param section
    /// Name of the Section in the config file.
    /// \param properties
    /// The MgPropertyCollection object representing the properties for the specified section.
    ///
    /// \return
    /// Nothing
    ///
    ///

    void SetProperties(CREFSTRING section, MgPropertyCollection* properties);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Removes the properties from the specified section.
    /// If the properties are not specified, then the entire section will be removed.
    ///
    /// \param section
    /// Name of the Section in the config file.
    /// \param properties
    /// The MgPropertyCollection object representing the properties for the specified section.
    ///
    /// \return
    /// Nothing
    ///
    ///

    void RemoveProperties(CREFSTRING section, MgPropertyCollection* properties);

private:

    /// Get the generic value in string
    bool GetValue(CREFSTRING section, CREFSTRING property, REFSTRING value);

    const MgConfigValidationInfo* GetConfigValidationInfo(
        const MgConfigValidationInfo* validationInfoList,
        CREFSTRING property) const;
    const MgConfigValidationInfo* GetConfigValidationInfo(CREFSTRING section,
        CREFSTRING property) const;
    void ValidateValue(CREFSTRING section, CREFSTRING property,
        CREFSTRING value);

private:

    /// Pointer to a process-wide singleton.
    static Ptr<MgConfiguration> sm_configuration;

    static const STRING sm_reservedCharacters;

    /// Needed for thread-safety
    ACE_Recursive_Thread_Mutex m_mutex;

    /// Ace member
    MgConfigurationHeap m_config;

    /// Store the file name loaded
    STRING m_fileName;
    bool m_fileLoaded;

    struct ValidationEntry
    {
        wstring section;
        MgConfigValidationInfo* validationInfo;
    };

    /// Structure for storing validation info
    typedef std::list<ValidationEntry> ValidationList;
    ValidationList m_validationInfo;
};
/// \endcond

#endif // MG_CONFIGURATION_H_
