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

#ifndef GRIDTHEMEPARSER_H_
#define GRIDTHEMEPARSER_H_

#include "MdfModel.h"
#include "GridColorRule.h"

// Forward declaration
class GridTheme;

///<summary>
/// Class GridThemeParser is used to parse the filter string of GridColorRule.
/// It extracts the tokens from the filter string and organize them to a GridTheme
/// object. In addtion, it will validate all the data that read from the filter string
/// and exclude the invalid data.
///</summary>
class GridThemeParser
{
public:
    ///<summary>
    /// Constructor.
    ///</summary>
    GridThemeParser();

    ///<summary>
    /// Destructor.
    ///</summary>
    ~GridThemeParser();

    ///<summary>
    /// Function to Parse a collectio of GridColorRule. It will create a GridTheme object
    /// in the heap and fill the extracted data into the GridTheme object.
    ///</summary>
    ///<param name = "pRules">
    /// The pointer of a collection of GridColorRule.
    ///</param>
    ///<returns>
    /// Returns a pointer to GridTheme. It stores the parsed result. Caller has the responsibilty
    /// to free it.
    ///</returns>
    GridTheme* ParseThemeColorRules(const MdfModel::RuleCollection *pRules);

    ///<summary>
    /// Function to Parse one GridColorRule. It will create a GridTheme object
    /// in the heap and fill the extracted data into the GridTheme object.
    ///<param name = "pRule">
    /// The pointer of a GridColorRule.
    ///</param>
    ///</summary>
    ///<returns>
    /// Returns a pointer to GridTheme. It stores the parsed result. Caller has the responsibilty
    /// to free it.
    ///</returns>
    GridTheme* ParseThemeColorRule(const MdfModel::GridColorRule *pRule);

private:
    ///<summary>
    /// Function to search the type token from the nStart position in
    /// the filter string. It will throw std::exception if doesn't find
    /// the match string.
    ///</summary>
    ///<param name = "type">
    /// [Out] It stores the result string.
    ///</param>
    ///<param name = "filter">
    /// The filter string that this parser object is parsing.
    ///</param>
    ///<param name = "nStart">
    /// The start position that this funtion searchs from.
    ///</param>
    ///<returns>
    /// Returns the position where the search ends.
    ///</returns>
    size_t FindType(MdfModel::MdfString       &type,
                    const MdfModel::MdfString &filter,
                    size_t                     nStart);

    ///<summary>
    /// Function to search the band name token from the nStart position in
    /// the filter string. It will throw std::exception if doesn't find
    /// the match string.
    ///</summary>
    ///<param name = "bandname">
    /// [Out] It stores the result string.
    ///</param>
    ///<param name = "filter">
    /// The filter string that this parser object is parsing.
    ///</param>
    ///<param name = "nStart">
    /// The start position that this funtion searchs from.
    ///</param>
    ///<returns>
    /// Returns the position where the search ends.
    ///</returns>
    size_t FindBandName(MdfModel::MdfString       &bandname,
                        const MdfModel::MdfString &filter,
                        size_t                     nStart);

    ///<summary>
    /// Function to search the operator token from the nStart position in
    /// the filter string. It will throw std::exception if doesn't find
    /// the match string.
    ///</summary>
    ///<param name = "soperator">
    /// [Out] It stores the result string.
    ///</param>
    ///<param name = "filter">
    /// The filter string that this parser object is parsing.
    ///</param>
    ///<param name = "nStart">
    /// The start position that this funtion searchs from.
    ///</param>
    ///<returns>
    /// Returns the position where the search ends.
    ///</returns>
    size_t FindOperator(MdfModel::MdfString       &soperator,
                        const MdfModel::MdfString &filter,
                        size_t                     nStart);

    ///<summary>
    /// Function to search the double value token from the nStart position in
    /// the filter string. It will throw std::exception if doesn't find
    /// the match double value.
    ///</summary>
    ///<param name = "value">
    /// [Out] It stores the result double value.
    ///</param>
    ///<param name = "filter">
    /// The filter string that this parser object is parsing.
    ///</param>
    ///<param name = "nStart">
    /// The start position that this funtion searchs from.
    ///</param>
    ///<returns>
    /// Returns the position where the search ends.
    ///</returns>
    size_t FindValue(double                    &value,
                     const MdfModel::MdfString &filter,
                     size_t                     nStart);

    ///<summary>
    /// Function to search the first argument string from the nStart position in
    /// the filter string. It will throw std::exception if doesn't find
    /// the match string.
    ///</summary>
    ///<param name = "soperator">
    /// The string to find.
    ///</param>
    ///<param name = "filter">
    /// The filter string that this parser object is parsing.
    ///</param>
    ///<param name = "nStart">
    /// The start position that this funtion searchs from.
    ///</param>
    ///<returns>
    /// Returns the position where the search ends.
    ///</returns>
    size_t Find(const MdfModel::MdfString &findstring,
                const MdfModel::MdfString &filter,
                size_t                     nStart);

private:
    // Stack to record the brackets around type token.
    std::vector<size_t> m_typeBracketStack;

    // Stack to record the brackets around band name token
    std::vector<size_t> m_bandNameBracketStack;

    // Stack to record the brackets around the value.
    std::vector<size_t> m_valueBracketStack;

public:
    // Const string for predefined aspect type.
    static const MdfModel::MdfString sm_AspectType;

    // Const string for predefined slope type.
    static const MdfModel::MdfString sm_SlopeType;

    // Const string for predefined height type.
    static const MdfModel::MdfString sm_HeightType;
};

#endif
