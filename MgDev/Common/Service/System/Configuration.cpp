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

#include "AceCommon.h"

// Process-wide MgConfiguration
Ptr<MgConfiguration> MgConfiguration::m_configuration = (MgConfiguration*)NULL;

/// <summary>
/// Constructor
/// </summary>
MgConfiguration::MgConfiguration(void)
{
    m_fileLoaded = false;
}

/// <summary>
/// Destructor
/// </summary>
MgConfiguration::~MgConfiguration(void)
{
}

/// <summary>
/// Self Destructor
/// </summary>
void MgConfiguration::Dispose(void)
{
    delete this;
}

/// <summary>
/// Get pointer to a process-wide MgConfiguration.
/// </summary>
MgConfiguration* MgConfiguration::GetInstance(void)
{
    MG_CONFIGURATION_TRY()

    ACE_TRACE ("MgConfiguration::GetInstance");

    if (MgConfiguration::m_configuration == NULL)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance (), 0));
        if (MgConfiguration::m_configuration == NULL)
        {
            MgConfiguration::m_configuration = new MgConfiguration;
        }
    }

    MG_CONFIGURATION_CATCH_AND_THROW(L"MgConfiguration.GetInstance")

    // To avoid overheads and maintain thread safety,
    // do not assign this returned static singleton to a Ptr object.
    return MgConfiguration::m_configuration;
}

///----------------------------------------------------------------------------
/// <summary>
/// Loads the configuration file specified
/// </summary>
///
/// <param name="fileName">
/// name of file to load
/// </param>
///
/// <returns>
/// Nothing
/// </returns>
///----------------------------------------------------------------------------

void MgConfiguration::LoadConfiguration(CREFSTRING fileName)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    MG_CONFIGURATION_TRY()

    m_fileLoaded = false;
    m_fileName = fileName;

    if(m_config.Open())
    {
        if(m_config.ImportConfig(fileName))
        {
            m_fileLoaded = true;
        }
        else
        {
            MgStringCollection arguments;
            arguments.Add(fileName);

            throw new MgConfigurationLoadFailedException(
                L"MgConfiguration.LoadConfiguration",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
    }
    else
    {
        ACE_ERROR((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("MgConfiguration::LoadConfiguration()")));
    }

    MG_CONFIGURATION_CATCH_AND_THROW(L"MgConfiguration.LoadConfiguration")
}

///----------------------------------------------------------------------------
/// <summary>
/// Saves the configuration data to file specified
/// </summary>
///
/// <param name="fileName">
/// name of file to save
/// </param>
///
/// <returns>
/// Nothing
/// </returns>
///----------------------------------------------------------------------------

void MgConfiguration::SaveConfiguration(CREFSTRING fileName)
{
    MG_CONFIGURATION_TRY()

    // the MgConfigurationHeap is thread-safe
    if(!m_config.ExportConfig(fileName))
    {
        MgStringCollection arguments;
        arguments.Add(fileName);

        throw new MgConfigurationSaveFailedException(
            L"MgConfiguration.SaveConfiguration",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    MG_CONFIGURATION_CATCH_AND_THROW(L"MgConfiguration.SaveConfiguration")
}

///----------------------------------------------------------------------------
/// <summary>
/// Get Integer value from a section
/// </summary>
///
/// <param name="section">
/// Section in config file
/// </param>
/// <param name="property">
/// Property in the section
/// </param>
/// <param name="value">
/// Out parameter for value to store
/// </param>
/// <param name="defaultValue">
/// Default value in case property is missing
/// </param>
///
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgInvalidPropertyValueException ???
///----------------------------------------------------------------------------

void MgConfiguration::GetIntValue(CREFSTRING section, CREFSTRING property, INT32& value, INT32 defaultValue)
{
    MG_CONFIGURATION_TRY()

    STRING tempValue;

    if (GetValue(section, property, tempValue))
    {
        value = ACE_OS::atoi(tempValue.c_str());
    }
    else
    {
        value = defaultValue;
    }

    MG_CONFIGURATION_CATCH_AND_THROW(L"MgConfiguration.GetIntValue")
}

///----------------------------------------------------------------------------
/// <summary>
/// Get bool value from a section
/// </summary>
///
/// <param name="section">
/// Section in config file
/// </param>
/// <param name="property">
/// Property in the section
/// </param>
/// <param name="value">
/// Out parameter for value to store
/// </param>
/// <param name="defaultValue">
/// Default value in case property is missing
/// </param>
///
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgInvalidPropertyValueException ???
///----------------------------------------------------------------------------

void MgConfiguration::GetBoolValue(CREFSTRING section, CREFSTRING property, bool& value, bool defaultValue)
{
    MG_CONFIGURATION_TRY()

    STRING tempValue;

    if (GetValue(section, property, tempValue))
    {
        value = (ACE_OS::atoi(tempValue.c_str()) != 0);
    }
    else
    {
        value = defaultValue;
    }

    MG_CONFIGURATION_CATCH_AND_THROW(L"MgConfiguration.GetBoolValue")
}

///----------------------------------------------------------------------------
/// <summary>
/// Get Double value from a section
/// </summary>
///
/// <param name="section">
/// Section in config file
/// </param>
/// <param name="property">
/// Property in the section
/// </param>
/// <param name="value">
/// Out parameter for value to store
/// </param>
/// <param name="defaultValue">
/// Default value in case property is missing
/// </param>
///
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgInvalidPropertyValueException ???
///----------------------------------------------------------------------------

void MgConfiguration::GetDoubleValue(CREFSTRING section, CREFSTRING property, double& value, double defaultValue)
{
    MG_CONFIGURATION_TRY()

    STRING tempValue;

    if (GetValue(section, property, tempValue))
    {
        // Assume all the settings specified in the configuration file are in
        // the C locale.
        value = ACE_OS::strtod(tempValue.c_str(), NULL);
    }
    else
    {
        value = defaultValue;
    }

    MG_CONFIGURATION_CATCH_AND_THROW(L"MgConfiguration.GetDoubleValue")
}

///----------------------------------------------------------------------------
/// <summary>
/// Get Integer value from a section
/// </summary>
///
/// <param name="section">
/// Section in config file
/// </param>
/// <param name="property">
/// Property in the section
/// </param>
/// <param name="value">
/// Out parameter for value to store
/// </param>
/// <param name="defaultValue">
/// Default value in case property is missing
/// </param>
///
/// <returns>
/// true  - Property found
/// false - Default value
/// </returns>
///----------------------------------------------------------------------------

void MgConfiguration::GetStringValue(CREFSTRING section, CREFSTRING property, REFSTRING value, CREFSTRING defaultValue)
{
    MG_CONFIGURATION_TRY()

    if (!GetValue(section, property, value))
    {
        value = defaultValue;
    }

    MG_CONFIGURATION_CATCH_AND_THROW(L"MgConfiguration.GetStringValue")
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the generic value in string.
/// </summary>
///
/// <param name="section">
/// Section in config file.
/// </param>
/// <param name="property">
/// Property in the section.
/// </param>
/// <param name="value">
/// Out parameter for value to store.
/// </param>
///
/// <returns>
/// true if successful (i.e. the property is found and its value is retrieved),
/// false otherwise.
/// </returns>
///----------------------------------------------------------------------------

bool MgConfiguration::GetValue(CREFSTRING section, CREFSTRING property, REFSTRING value)
{
    MG_CONFIGURATION_TRY()

    value.clear();
    ACE_Configuration_Section_Key sectionKey;

    // the MgConfigurationHeap is thread-safe
    if (m_config.OpenSection(section, 0, sectionKey))
    {
        ACE_TString temp;

        if (m_config.GetStringValue(sectionKey, MG_WCHAR_TO_TCHAR(property), temp))
        {
            value = MG_TCHAR_TO_WCHAR(temp.c_str());
        }
    }

    MG_CONFIGURATION_CATCH_AND_THROW(L"MgConfiguration.GetValue")

    return !value.empty();
}


STRING MgConfiguration::GetFileName()
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, L""));
    return m_fileName;
}


bool MgConfiguration::IsFileLoaded()
{
    return m_fileLoaded;
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the properties for the specified section.
/// </summary>
///
/// <param name="section">
/// Name of the Section in the config file.
/// </param>
///
/// <returns>
/// MgPropertyCollection representing the properties for the specified section.
/// </returns>
///----------------------------------------------------------------------------

MgPropertyCollection* MgConfiguration::GetProperties(CREFSTRING section)
{
    Ptr<MgPropertyCollection> pProperties;

    MG_CONFIGURATION_TRY()

    if (section.empty())
    {
        throw new MgNullArgumentException(
            L"MgConfiguration.GetProperties", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // If the section does not exist, then an empty property collection will be
    // returned.

    // the MgConfigurationHeap is thread-safe
    ACE_Configuration_Section_Key sectionKey;
    if (m_config.OpenSection(section, 0, sectionKey))
    {
        pProperties = new MgPropertyCollection();

        int nIndex = 0;
        ACE_TString propertyName;
        ACE_Configuration::VALUETYPE propertyType;

        while (m_config.EnumerateValues(sectionKey, nIndex, propertyName, propertyType))
        {
            Ptr<MgProperty> pProperty;
            STRING name = MG_TCHAR_TO_WCHAR(propertyName.c_str());

            if (ACE_Configuration::STRING == propertyType)
            {
                STRING value;

                GetStringValue(section, name, value, L"");
                pProperty = new MgStringProperty(name, value);
            }
            else
            {
                // Everything in the ini file is treated as a string so we should not be here
                throw new MgInvalidPropertyTypeException(
                    L"MgConfiguration::GetProperties", __LINE__, __WFILE__, NULL, L"", NULL);
            }

            // Add this property to the collection
            pProperties->Add(pProperty.p);
            nIndex++;
        }
    }

    MG_CONFIGURATION_CATCH_AND_THROW(L"MgConfiguration.GetProperties")

    return pProperties.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets the properties for the specified section.
/// If the section does not exist, then it will be created.
/// </summary>
///
/// <param name="section">
/// Name of the Section in the config file.
/// </param>
/// <param name="properties">
/// The MgPropertyCollection object representing the properties for the
/// specified section.
/// </param>
///
/// <returns>
/// Nothing
/// </returns>
///----------------------------------------------------------------------------

void MgConfiguration::SetProperties(CREFSTRING section,
    MgPropertyCollection* properties)
{
    MG_CONFIGURATION_TRY()

    if (section.empty())
    {
        throw new MgNullArgumentException(
            L"MgConfiguration.SetProperties", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (NULL == properties)
    {
        throw new MgNullReferenceException(
            L"MgConfiguration.SetProperties", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    else if (properties->GetCount() <= 0)
    {
        // Do nothing if the property collection is empty.
        return;
    }

    // the MgConfigurationHeap is thread-safe
    ACE_Configuration_Section_Key sectionKey;
    bool success = m_config.OpenSection(section, 1, sectionKey);

    if (success)
    {
        for (INT32 i = 0; i < properties->GetCount(); ++i)
        {
            Ptr<MgProperty> pBaseProperty = properties->GetItem(i);

            if (MgPropertyType::String == pBaseProperty->GetPropertyType())
            {
                MgStringProperty* pProperty =
                    static_cast<MgStringProperty*>(pBaseProperty.p);

                // TODO: Property Validation - Should throw an exception if value is not valid
                // Example: Filename = "" <---- This is not valid

                success = m_config.SetStringValue(sectionKey,
                    MG_WCHAR_TO_TCHAR(pProperty->GetName()),
                    MG_WCHAR_TO_TCHAR(pProperty->GetValue()));
            }
            else if (MgPropertyType::Int32 == pBaseProperty->GetPropertyType())
            {
                MgInt32Property* pProperty =
                    static_cast<MgInt32Property*>(pBaseProperty.p);

                // TODO: Property Validation - Should throw an exception if value is not valid
                // Example: Port = Hello <---- This is not valid

                success = m_config.SetIntegerValue(sectionKey,
                    MG_WCHAR_TO_TCHAR(pProperty->GetName()),
                    pProperty->GetValue());
            }
            else if (MgPropertyType::Boolean == pBaseProperty->GetPropertyType())
            {
                MgBooleanProperty* pProperty =
                    static_cast<MgBooleanProperty*>(pBaseProperty.p);

                // TODO: Property Validation - Should throw an exception if value is not valid
                // Example: Enabled = Yes <---- This is not valid

                success = m_config.SetIntegerValue(sectionKey,
                    MG_WCHAR_TO_TCHAR(pProperty->GetName()),
                    pProperty->GetValue() ? 1 : 0);
            }
            else
            {
                // TODO: Support more types?

                throw new MgInvalidPropertyTypeException(
                    L"MgConfiguration.SetProperties", __LINE__, __WFILE__, NULL, L"", NULL);
            }

            if (!success)
            {
                break;
            }
        }
    }
    else
    {
        MgStringCollection arguments;
        arguments.Add(section);

        throw new MgConfigurationException(L"MgConfiguration.SetProperties",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // The properties have been updated so we need to save the changes.
    SaveConfiguration(GetFileName());

    MG_CONFIGURATION_CATCH_AND_THROW(L"MgConfiguration.SetProperties")
}

///----------------------------------------------------------------------------
/// <summary>
/// Removes the properties from the specified section.
/// If the properties are not specified, then the entire section will be removed.
/// </summary>
///
/// <param name="section">
/// Name of the Section in the config file.
/// </param>
/// <param name="properties">
/// The MgPropertyCollection object representing the properties for the specified section.
/// </param>
///
/// <returns>
/// Nothing
/// </returns>
///----------------------------------------------------------------------------

void MgConfiguration::RemoveProperties(CREFSTRING section,
    MgPropertyCollection* properties)
{
    MG_CONFIGURATION_TRY()

    if (section.empty())
    {
        throw new MgNullArgumentException(
            L"MgConfiguration.RemoveProperties", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    bool success = true;

    if (NULL == properties)
    {
        success = m_config.RemoveSection(section, 1);
    }
    else if (properties->GetCount() > 0)
    {
        ACE_Configuration_Section_Key sectionKey;

        success = m_config.OpenSection(section, 0, sectionKey);

        if (success)
        {
            for (INT32 i = 0; i < properties->GetCount(); ++i)
            {
                Ptr<MgProperty> pProperty = properties->GetItem(i);

                success = m_config.RemoveValue(sectionKey,
                    MG_WCHAR_TO_TCHAR(pProperty->GetName()));

                if (!success)
                {
                    break;
                }
            }
        }
    }
    else
    {
        // Do nothing if the property collection is empty.
        return;
    }

    if (!success)
    {
        MgStringCollection arguments;
        arguments.Add(section);

        throw new MgConfigurationException(L"MgConfiguration.RemoveProperties",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // The properties have been updated so we need to save the changes.
    SaveConfiguration(GetFileName());

    MG_CONFIGURATION_CATCH_AND_THROW(L"MgConfiguration.RemoveProperties")
}
