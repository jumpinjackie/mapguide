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

#ifndef MGDATETIME_H_
#define MGDATETIME_H_

class MgDateTime;
template class MG_FOUNDATION_API Ptr<MgDateTime>;

/// \defgroup MgDateTime MgDateTime
/// \ingroup Common_Module
/// \{

/////////////////////////////////////////////////////////////////
/// \brief
/// MgDateTime is used to store dates, times, or both.
///
/// \remarks
/// The date and/or time can be modified using the various getters or setters that this class provides.
/// MgDateTime represent date and time ranging from midnight (00:00:00), January 1, 1970 through 19:14:07 January 18, 2038, UTC.
/// \param year
/// Year in the range of 1 to 9999
/// \param month
/// Month in the range of 1 to 12 inclusive (January = 1)
/// \param day
/// Day of the month in the range of 1 to 31 inclusive
/// \param hour
/// Hour since midnight in the range of 0 to 23
/// \param minute
/// Minutes after hour in the range of 0 to 59
/// \param second
/// Seconds after minute in the range of 0 to 59
/// \param microsecond
/// Microseconds after second in the range of 0 to 999999
///
class MG_FOUNDATION_API  MgDateTime : public MgSerializable
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgDateTime)

PUBLISHED_API:
    /// \brief
    /// Construct a date time value initialized to the
    /// current date and time
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgDateTime();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgDateTime();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgDateTime();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns nothing.
    ///
    MgDateTime();

    /// \brief
    /// Construct a date value
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgDateTime(short year, short month, short day);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgDateTime(short year, short month, short day);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgDateTime(int year, int month, int day);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param year (short/int)
    /// Input year
    /// \param month (short/int)
    /// Input month
    /// \param day (short/int)
    /// Input day of month
    ///
    /// \return
    /// Returns nothing.
    ///
    MgDateTime(INT16 year, INT8 month, INT8 day);

    /// \brief
    /// Construct a time value
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgDateTime(short hour, short minute, short second, int microsecond);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgDateTime(short hour, short minute, short second, int microsecond);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgDateTime(int hour, int minute, int second, int microsecond);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param hour (short/int)
    /// Input hour
    /// \param minute (short/int)
    /// Input minute
    /// \param second (short/int)
    /// Input second
    /// \param microsecond (int)
    /// Input microsecond
    ///
    /// \return
    /// Returns nothing.
    ///
    MgDateTime(INT8 hour, INT8 minute, INT8 second, INT32 microsecond);

    /// \brief
    /// Construct a date time value
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgDateTime(short year, short month, short day, short hour, short minute, short second, int microsecond);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgDateTime(short year, short month, short day, short hour, short minute, short second, int microsecond);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgDateTime(int year, int month, int day, int hour, int minute, int second, int microsecond);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param year (short/int)
    /// Input year
    /// \param month (short/int)
    /// Input month
    /// \param day (short/int)
    /// Input day of month
    /// \param hour (short/int)
    /// Input hour
    /// \param minute (short/int)
    /// Input minute
    /// \param second (short/int)
    /// Input second
    /// \param microsecond (int)
    /// Input microsecond
    ///
    /// \return
    /// Returns nothing.
    ///
    MgDateTime(INT16 year, INT8 month, INT8 day, INT8 hour, INT8 minute, INT8 second, INT32 microsecond);

    /// \brief
    /// Gets the day component of this object
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// short GetDay();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// short GetDay();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetDay();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the day component of this object.
    ///
    INT8 GetDay();  /// __get, __set

    /// \brief
    /// Gets the month component of this object
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// short GetMonth();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// short GetMonth();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetMonth();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the month component of this object.
    ///
    INT8 GetMonth();  /// __get, __set

    /// \brief
    /// Gets the year component of this object
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// short GetYear();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// short GetYear();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetYear();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the year component of this object.
    ///
    INT16 GetYear();  /// __get, __set

    /// \brief
    /// Gets the hour component of this object
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// short GetHour();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// short GetHour();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetHour();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the hour component of this object.
    ///
    INT8 GetHour();  /// __get, __set

    /// \brief
    /// Gets the minute component of this object
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// short GetMinute();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// short GetMinute();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetMinute();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the minute component of this object.
    ///
    INT8 GetMinute();  /// __get, __set

    /// \brief
    /// Gets the second component of this object
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// short GetSecond();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// short GetSecond();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetSecond();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the second component of this object.
    ///
    INT8 GetSecond();  /// __get, __set

    /// \brief
    /// Gets the microsecond component of this object
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetMicrosecond();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetMicrosecond();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetMicrosecond();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the microsecond component of this object.
    ///
    INT32 GetMicrosecond();  /// __get, __set

    /// \brief
    /// Sets the day component of this object
    /// The Validate method should be called after the date time value has been
    /// fully specified by Set methods.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetDay(short day);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetDay(short day);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetDay(int day);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param day (short/int)
    /// Day of the month in the range of 1 to 31 inclusive
    ///
    void SetDay(INT8 day);

    /// \brief
    /// Sets the month component of this object
    /// The Validate method should be called after the date time value has been
    /// fully specified by Set methods.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetMonth(short month);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetMonth(short month);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetMonth(int month);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param month (short/int)
    /// Month in the range of 1 to 12 inclusive (January = 1)
    ///
    void SetMonth(INT8 month);

    /// \brief
    /// Sets the year component of this object
    /// The Validate method should be called after the date time value has been
    /// fully specified by Set methods.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetYear(short year);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetYear(short year);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetYear(int year);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param year (short/int)
    /// Year in the range of 1 to 9999
    ///
    void SetYear(INT16 year);

    /// \brief
    /// Sets the hour component of this object
    /// The Validate method should be called after the date time value has been
    /// fully specified by Set methods.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetHour(short hour);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetHour(short hour);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetHour(int hour);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param hour (short/int)
    /// Hour since midnight in the range of 0 to 23
    ///
    void SetHour(INT8 hour);

    /// \brief
    /// Sets the minute component of this object
    /// The Validate method should be called after the date time value has been
    /// fully specified by Set methods.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetMinute(short minute);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetMinute(short minute);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetMinute(int minute);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param minute (short/int)
    /// Minutes after hour in the range of 0 to 59
    ///
    void SetMinute(INT8 minute);

    /// \brief
    /// Sets the second component of this object
    /// The Validate method should be called after the date time value has been
    /// fully specified by Set methods.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetSecond(short second);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetSecond(short second);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetSecond(int second);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param second (short/int)
    /// Seconds after minute in the range of 0 to 59
    ///
    void SetSecond(INT8 second);

    /// \brief
    /// Sets the microsecond component of this object
    /// The Validate method should be called after the date time value has been
    /// fully specified by Set methods.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetMicrosecond(int microsecond);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetMicrosecond(int microsecond);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetMicrosecond(int microsecond);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param microsecond (int)
    /// Microseconds after second in the range of 0 to 999999
    ///
    void SetMicrosecond(INT32 microsecond);

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Validates the date time value.  This method should be called
    /// after the date time value has been fully specified by Set methods.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void Validate();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void Validate();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void Validate();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns nothing.
    ///
    void Validate();

    /// \brief
    /// Determines if this object only represents a date value.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool IsDate();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean IsDate();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool IsDate();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns true if only the date is valid; otherwise returns false.
    ///
    bool IsDate();

    /// \brief
    /// Determines if this object only represents a time value.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool IsTime();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean IsTime();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool IsTime();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns true if only the time is valid; otherwise returns false.
    ///
    bool IsTime();

    /// \brief
    /// Determines if this object represents both date and time values.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool IsDateTime();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean IsDateTime();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool IsDateTime();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns true if both the date and time are valid; otherwise returns false.
    ///
    bool IsDateTime();

EXTERNAL_API:

    /// \brief
    /// Returns date time object as a string
    ///
    STRING ToString();

INTERNAL_API:

    MgDateTime(INT16 year, INT8 month, INT8 day, INT8 hour, INT8 minute, float seconds);
    explicit MgDateTime(CREFSTRING fdoDateTime);
    explicit MgDateTime(const string& xmlDateTime);
    explicit MgDateTime(time_t timeValue);
    explicit MgDateTime(double number);
    MgDateTime(const MgDateTime& dt);

    MgDateTime& operator=(const MgDateTime& dt);
    bool operator<(const MgDateTime& dt) const;
    bool operator<=(const MgDateTime& dt) const;
    bool operator>(const MgDateTime& dt) const;
    bool operator>=(const MgDateTime& dt) const;
    bool operator==(const MgDateTime& dt) const;
    bool operator!=(const MgDateTime& dt) const;

    void Serialize(MgStream* stream);
    void Deserialize(MgStream* stream);

    STRING ToXmlString(bool utc = true);
    string ToXmlStringUtf8(bool utc = true);
    time_t ToTimeValue();
    double ToNumber();
    double ToMilliSeconds();

protected:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// The integer value
    ///
    virtual INT32 GetClassId();

    virtual void Dispose();

private:

    int IsLeapYear(long year, char calendar) const;
    long lfloor(long a, long b);   //  assumes b positive
    long GregorianDay(int day, int month, long year, char calendar);
    void CalendarDate(long gdn, char calendar);
    void AssignTime(INT64 microSeconds);

    void Initialize();

    void GetDateTimeInfo(struct tm& timeInfo) const;
    void SetDateTimeInfo(const struct tm& timeInfo);

    void SplitSeconds(float seconds);

    void ValidateDateTime();
    void ValidateDate();
    void ValidateTime();

    // To perform the SelectAggregates operations on DateTime data, each data value
    // must be turned into an integer before the operation and restored to a DateTime
    // value after the operation.  Since there are three types of values (Dates,
    // Times and DateTimes), three ranges of integers are used to represent the
    // converted data values.  Integers are stored as double precision values.
    //
    // Note that FDO's DateTime data only operates with millisecond precision.
    // The methods related to performing SelectAggregates therefore only use
    // millisecond precision, depsite the ability of MgDateTime to store microsecond
    // values.  Using these methods on MgDateTime objects that were created from
    // sources other than FDO strings will result in a loss of data.
    //
    // The largest range of values represent DateTimes, which have millisecond
    // precision and range from January 1 in the year 1 to December 31 of 9999.
    // Each DateTime maps to an integer representing the number of milliseconds
    // before or since the beginning of the Gregorian Calendar (October 12, 1582),
    // which maps to zero.
    //
    // Because doubles provide only 14 digits of precision, this mapping may result
    // in a loss of data for DateTimes beginning in 3170.  This data loss would
    // occur in the millisecond portion of the DateTime.  DateTimes before the
    // beginning of the Gregorian Calendar are reprsented as negative integers
    // that range down almost to -50 trillion.
    //
    // Each Time values maps to an integer representing the number of milliseconds
    // since 12:00AM of the hypothetical day.  This results in 86.4 million values,
    // which range up from -60 trillion.  Time values have no data loss issues.
    //
    // Each Date value is mapped to an integer representing the number of days
    // before or after the beginning of the Gregorian Calendar.  These 3.65 million
    // values range up from -70 trillion, and also have no data loss issues.

    static const INT64 MIN_DATETIME = -50000000000000LL;
    static const INT64 TIME_ZERO    = -60000000000000LL;
    static const INT64 DATE_ZERO    = -70000000000000LL;

private:

    INT16   m_year;
    INT8    m_month;
    INT8    m_day;
    INT8    m_hour;
    INT8    m_minute;
    INT8    m_second;
    INT32   m_microsecond;

CLASS_ID:

    static const INT32 m_cls_id = Foundation_Data_DateTime;
};
/// \}

#endif
