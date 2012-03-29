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

#ifndef MG_CONFIGURATION_HEAP_H_
#define MG_CONFIGURATION_HEAP_H_

/// \cond INTERNAL
class MgConfigurationHeap
{
    DECLARE_CLASSNAME(MgConfigurationHeap)

INTERNAL_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a new object, initializing the section list and root section
    ///
    /// \param sectionName
    /// Name of the section
    ///
    /// \return
    /// Returns nothing
    ///
    MgConfigurationHeap();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructs the object
    ///
    /// \return
    /// Returns nothing
    ///
    ~MgConfigurationHeap();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Imports a configuration from a file
    ///
    /// \param filename
    /// File containing the configuration
    ///
    /// \return
    /// True if no errors occur, false otherwise
    ///
    bool ImportConfig(CREFSTRING filename);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Exports a configuration to a file
    ///
    /// \param filename
    /// File to export the configuration to
    ///
    /// \return
    /// True if no Errors occur, false otherwise
    ///
    bool ExportConfig(CREFSTRING filename);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the section key to be that belonging to the section path.
    /// Creates a new section if it doesn't exist and create is not zero.
    ///
    /// \param sectionPath
    /// Name of the section to be retrieved
    /// \param create
    /// 1 to create the section if it doesn't exists, 0 otherwise
    /// \param key
    /// ACE_Configuration_Section_Key to be set based on the sectionPath
    ///
    /// \return
    /// True if no Errors occur, false otherwise
    ///
    bool OpenSection(CREFSTRING sectionPath, INT32 create, ACE_Configuration_Section_Key &key);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Exports a configuration to a file
    ///
    /// \param filename
    /// File to export the configuration to
    ///
    /// \return
    /// True if no Errors occur, false otherwise
    ///
    bool RemoveSection(CREFSTRING sectionPath, int recursive = 0);

    /// Functions for ACE_Configuration_Heap
    bool Open(int default_map_size = ACE_DEFAULT_CONFIG_SECTION_SIZE);
    bool EnumerateSections(const ACE_Configuration_Section_Key& key, int index, ACE_TString& name);
    bool EnumerateValues(const ACE_Configuration_Section_Key& key, int index, ACE_TString& name, ACE_Configuration::VALUETYPE& type);
    bool SetStringValue(const ACE_Configuration_Section_Key& key, const ACE_TCHAR* name, const ACE_TString& value);
    bool GetStringValue(const ACE_Configuration_Section_Key& key, const ACE_TCHAR* name, ACE_TString& value);
    bool RemoveValue(const ACE_Configuration_Section_Key& key, const ACE_TCHAR* name);

private:
    Ptr<MgConfigurationSection> m_rootSection;
    Ptr<MgConfigurationSectionCollection> m_sectionCollection;

    ACE_Configuration_Heap m_aceConfigurationHeap;
    ACE_Recursive_Thread_Mutex m_mutex;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Adds a new section to the configuration object
    ///
    /// \param sectionPath
    /// Name of the section to be retrieved
    /// \param key
    /// ACE_Configuration_Section_Key to be set based on the sectionPath
    ///
    /// \return
    /// The MgConfigurationSection object that was created
    ///
    MgConfigurationSection* AddSection(CREFSTRING sectionPath, ACE_Configuration_Section_Key& key);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Writes the section name, comments and parameters to the specified file
    ///
    /// \param section
    /// The MgConfigurationSection object to be written out to file
    /// \param outFile
    /// File stream to export the section to
    ///
    /// \return
    /// True if no Errors occur, false otherwise
    ///
    bool ExportSection(MgConfigurationSection* section, FILE* outFile);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Removes any whitespace ( ntr) from the beginning and end of given string
    ///
    /// \param line
    /// The string to remove white space from
    ///
    /// \return
    /// The string after the white space has been removed
    ///
    wstring Trim(const wstring& line);
};
/// \endcond

#endif // MG_CONFIGURATION_HEAP_H_
