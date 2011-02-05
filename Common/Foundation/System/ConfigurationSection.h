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

#ifndef _MG_CONFIGURATION_SECTION_H
#define _MG_CONFIGURATION_SECTION_H

/// \cond INTERNAL
class MgConfigurationSection : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgConfigurationSection)

INTERNAL_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a new object setting the key to the one given
    ///
    /// \param sectionName
    /// Name of the section
    ///
    /// \return
    /// Returns nothing
    ///
    MgConfigurationSection();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a new object with the name and key given
    ///
    /// \param sectionName
    /// Name of the section
    ///
    /// \return
    /// Returns nothing
    ///
    MgConfigurationSection(STRING sectionName);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructs the object
    ///
    /// \return
    /// Returns nothing
    ///
    virtual ~MgConfigurationSection();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name of the section
    ///
    /// \return
    /// The name of the section
    ///
    STRING GetSectionPath();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Adds a comment to the section object
    ///
    /// \param line
    /// The comment line to add
    ///
    /// \return
    /// Returns nothing
    ///
    void AddComment(CREFSTRING line);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the number of comment lines in the section
    ///
    /// \return
    /// Returns the number of comment lines in the section
    ///
    INT32 GetCommentCount();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the comment line associated with the line number given
    ///
    /// \param lineNum
    /// The comment line to be retrieved
    ///
    /// \return
    /// Returns the comment line associated with the line number given
    ///
    STRING GetComment(INT32 lineNum);

protected:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Disposes the object.
    ///
    void Dispose();

private:
    STRING m_sectionPath;
    Ptr<MgStringCollection> m_sectionComment;
};
/// \endcond

#endif // _MG_CONFIGURATION_SECTION_H
