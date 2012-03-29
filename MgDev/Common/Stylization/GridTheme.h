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

#ifndef GRIDTHEME_H_
#define GRIDTHEME_H_

#include "MdfModel.h"
#include "GridColorRule.h"
#include "Color.h"

///<summary>
/// Class GridBucket is used to store the parsed result of one filter string.
/// It includes the mathematic range info and the color which should be used on that range.
/// Moreover, it also supports the comparison with other GridBucket object.
/// Some instance of GridBucket are organized by GridTheme.
///</summary>
class GridBucket
{
public:
    ///<summary>
    /// Enum CompareResultType defined three results when compare with two
    /// GridBuckest.
    ///</summary>
    enum CompareResultType
    {
        kLess,
        kGreater,
        kOverlapped
    };

    ///<summary>
    /// Constructor.
    ///</summary>
    GridBucket();

    ///<summary>
    /// Destructor.
    ///</summary>
    ~GridBucket();

    ///<summary>
    /// Function to set the range of this GridBucket.
    ///</summary>
    ///<param name = "dBeginValue">
    /// The min value of the range.
    ///</param>
    ///<param name = "bEqualBeginValue">
    /// If bEqualBeginValue is ture, that means "dBeginValue" is in the range.
    /// Otherwise, only the value which is greater then "dBeginValue" can be
    /// in the range.
    ///</param>
    ///<param name = "dEndValue">
    /// The max value of the range.
    ///</param>
    ///<param name = "bEqualEndValue">
    /// If bEqualEndValue is ture, that means "dEndValue" is in the range.
    /// Otherwise, only the value which is less then "dEndValue" can be
    /// in the range.
    ///</param>
    void SetRange(double dBeginValue, bool bEqualBeginValue, double dEndValue, bool bEqualEndValue);

    ///<summary>
    /// Function to check whether the argument is in the bucket or not.
    ///</summary>
    ///<returns>
    /// If the pass argument value is in the range, returns true.
    /// Otherwise, returns false.
    ///</returns>
    bool IsValueInBucket(double value) const;

    ///<summary>
    /// Retrieve the color value of the GridBucket.
    ///</summary>
    const Color& GetColor() const;

    ///<summary>
    /// Set the color of the GridBucketl
    ///</summary>
    void SetColor(const Color &color);

    ///<summary>
    /// Retrieve the begin value of the range.
    ///</summary>
    double GetBeginValue() const;

    ///<summary>
    /// Retrieve whether the begin value is in the range.
    ///</summary>
    bool IsEqualToBeginValue() const;

    ///<summary>
    /// Retrieve the end value of the range.
    ///</summary>
    double GetEndValue() const;

    ///<summary>
    /// Retrieve whether the end value is in the range.
    ///</summary>
    bool IsEqualToEndValue() const;

    ///<summary>
    /// Static function to compare two buckets.
    /// If all the value of left bucket is less than right bucket, the result is CompareResultType::kLess.
    /// If all the value of left bucket is greater than right bucket, the result is CompareResultType::kGreater.
    /// Otherwise, the result is CompareResultType::kOverlapped.
    ///</summary>
    static CompareResultType CompareBuckets(const GridBucket &left, const GridBucket &right);


private:
    // Begin value of the range.
    double            m_dBeginValue;

    // Whether the begin value is in the range.
    bool              m_bEqualBeginValue;

    // End value of the range.
    double            m_dEndValue;

    // Whether the End value is in the range.
    bool              m_bEqualEndValue;

    // keeps track of comparison functions to use
    // when checking if value is in a bucket
    int               m_compareMask;

    // Color of the bucket.
    Color             m_color;

public:
    // The min value.
    static const double sm_MinDouble;

    // The max value.
    static const double sm_MaxDouble;
};

///<summary>
/// Class GridTheme is used to manage the collection of GridBucket.
/// It also supports to search the corresponding bucket or color
/// through the value.
///</summary>
class GridTheme
{
public:
    ///<summary>
    /// Constructor
    ///</summary>
    GridTheme();

    ///<summary>
    /// Destructor.
    ///</summary>
    ~GridTheme();

    ///<summary>
    /// Function to find which bucket the value is in.
    /// And it returns that bucket's defined color.
    ///</summary>
    ///<returns>
    /// Returns true if find the color.
    /// Otherwise, returns false.
    ///</returns>
    bool SearchColorByValue(Color &color, double value) const;

    ///<summary>
    /// Retrieve the band name that the theme applies on.
    ///</summary>
    const MdfModel::MdfString& GetBandName() const;

    ///<summary>
    /// Set the band name that the theme will applies on.
    ///</summary>
    void SetBandName(const MdfModel::MdfString &sBandName);

    ///<summary>
    /// Retrieve the theme type. It can be Aspect, Slope, or Height.
    ///</summary>
    const MdfModel::MdfString& GetType() const;

    ///<summary>
    /// Set the theme type.
    ///</summary>
    void SetType(const MdfModel::MdfString &sType);

    ///<summary>
    /// Retrieve the count of buckets.
    ///</summary>
    int GetBucketCount();

    ///<summary>
    /// Adopt the bucket into the current collection of the bucket.
    /// It will compare the bucket with the existing buckets and find
    /// a reasonable place to store the bucket.
    /// It returns true, this GridTheme takes ownership of pBucket.
    /// Caller cannot free the pBucket anymore.
    ///</summary>
    ///<returns>
    /// Returns true if the pBucket is adopted by the GridTheme.
    /// Otherwise, returns false.
    ///</returns>
    bool AdoptBucket(GridBucket *pBucket);

    ///<summary>
    /// Orphan the bucket at nIndex.
    ///</summary>
    ///<returns>
    /// Returns the pointer of orphan bucket.
    ///</returns>
    GridBucket* OrphanBucketAt(int nIndex);

    ///<summary>
    /// Orphan the pBucket.
    ///</summary>
    ///<returns>
    /// Returns the pointer of orphan bucket.
    ///</returns>
    GridBucket* OrphanBucket(GridBucket *pBucket);

    ///<summary>
    /// Retrieve the bucket at nIndex.
    ///</summary>
    GridBucket* GetBucketAt(int nIndex);

private:
    // Band that theme will apply on
    MdfModel::MdfString                      m_sBandName;

    // Theme type
    MdfModel::MdfString                      m_sType;

    // Grid Buckets
    MdfModel::MdfOwnerCollection<GridBucket> m_buckets;
};

///<summary>
/// Class GridThemeHashTable is used to improve the search performance of GridTheme.
/// It generates a hash table from the GridTheme's Buckets. And the caller can get the
/// same result from this class, and benefit better performance.
///</summary>
class GridThemeHashTable
{
public:
    ///<summary>
    /// Constructor
    ///</summary>
    GridThemeHashTable();

    ///<summary>
    /// Destructor.
    ///</summary>
    ~GridThemeHashTable();

    ///<summary>
    /// Function to initialze the hash table.
    ///</summary>
    ///<returns>
    /// Returns true if the hash table is generated.
    /// Otherwise, returns false.
    ///</returns>
    bool Initialize(GridTheme *pTheme);

    ///<summary>
    /// Function to search the corresponding color according
    /// to the value. It will calculate the index value first and
    /// search the color by the index value. So the performance is better
    /// than GridTheme's same funciton.
    ///</summary>
    bool SearchColorByValue(Color &color, double value) const;

    ///<summary>
    /// Clear the generated hash table.
    ///</summary>
    void Clear();

private:
    // Start value of the hash table
    double  m_dStart;

    // End value of the hash table
    double  m_dInterval;
    double m_dInvInterval;

    // The rows of hash table.
    std::vector<std::vector<GridBucket*> > m_vHashRows;
};

#endif
