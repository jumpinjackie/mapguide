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

#ifndef _MGWEBSEARCHCOMMAND_H_
#define _MGWEBSEARCHCOMMAND_H_

class MgWebSearchCommand;
template class MG_WEBAPP_API Ptr<MgWebSearchCommand>;

class MG_WEBAPP_API MgWebSearchCommand : public MgWebUiTargetCommand
{
PUBLISHED_API:

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the layer to search.
    /// </summary>
    /// <returns>
    /// Prompt text
    /// </returns>
    STRING GetLayer();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the string displayed to the user to prompt to enter the keyword
    /// </summary>
    /// <returns>
    /// Prompt text
    /// </returns>
    STRING GetPrompt();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the number of result columns
    /// </summary>
    /// <returns>
    /// Number of columns
    /// </returns>
    INT32 GetResultColumnCount();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the column display name at a given index
    /// </summary>
    /// <returns>
    /// Display name
    /// </returns>
    STRING GetColumnDisplayNameAt(INT32 index);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the result property name at a given index
    /// </summary>
    /// <returns>
    /// property name
    /// </returns>
    STRING GetColumnPropertyNameAt(INT32 index);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the text of filter expression to be applied
    /// </summary>
    /// <returns>
    /// Filter expression
    /// </returns>
    STRING GetFilter();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the maximum number of result to be returned by the search
    /// </summary>
    /// <returns>
    /// Maximum number of feature to be returned by the search
    /// </returns>
    INT32 GetMatchLimit();

INTERNAL_API:

    MgWebSearchCommand();

    ///////////////////////////////////////////////////////////////////////////
    // Set the layer to search
    //
    void SetLayer(CREFSTRING layer);

    ///////////////////////////////////////////////////////////////////////////
    // Set the string displayed to the user to prompt to enter the keyword
    //
    void SetPrompt(CREFSTRING prompt);

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    // Set the filter expression
    //
    void SetFilter(CREFSTRING filter);

    ///////////////////////////////////////////////////////////////////////////
    // Set the maximum number of result returned
    //
    void SetMatchLimit(INT32 matchLimit);

    ///////////////////////////////////////////////////////////////////////////
    // Get the result columns collection
    //
    MgPropertyCollection* GetResultColumns();

CLASS_ID:
    static const INT32 m_cls_id = WebApp_SearchCommand;

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
    STRING      m_layer;
    STRING      m_prompt;
    STRING      m_filter;
    INT32       m_matchLimit;
    Ptr<MgPropertyCollection> m_resultColumns;
};

#endif // _MGWEBSEARCHCOMMAND_H_
