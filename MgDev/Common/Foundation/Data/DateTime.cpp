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

#include <wchar.h>
#include "Foundation.h"
#include "ace/Date_Time.h"
#include <math.h>

#define MG_MAX_DATE_TIME_BUFFER_LENGTH  64
#define MG_MICRO_VALUE                  1000000

MG_IMPL_DYNCREATE(MgDateTime);

///<summary>Construct a date time value initialized to the
/// current date and time</summary>
///<return>Returns nothing.</return>
MgDateTime::MgDateTime()
{
    ACE_Date_Time now;

    m_year        = (INT16)now.year();
    m_month       = (INT8)now.month();
    m_day         = (INT8)now.day();
    m_hour        = (INT8)now.hour();
    m_minute      = (INT8)now.minute();
    m_second      = (INT8)now.second();
    m_microsecond = now.microsec();
}

///<summary>Construct a date value</summary>
/// <param name="year">Input year</param>
/// <param name="month">Input month</param>
/// <param name="day">Input day of month</param>
///<return>Returns nothing.</return>
MgDateTime::MgDateTime(INT16 year, INT8 month, INT8 day) :
    m_year(year),
    m_month(month),
    m_day(day),
    m_hour(-1),
    m_minute(-1),
    m_second(-1),
    m_microsecond(-1)
{
}

///<summary>Construct a time value</summary>
/// <param name="hour">Input hour</param>
/// <param name="minute">Input minute</param>
/// <param name="second">Input second</param>
/// <param name="microsecond">Input microsecond</param>
///<return>Returns nothing.</return>
MgDateTime::MgDateTime(INT8 hour, INT8 minute, INT8 second, INT32 microsecond) :
    m_year(-1),
    m_month(-1),
    m_day(-1),
    m_hour(hour),
    m_minute(minute),
    m_second(second),
    m_microsecond(microsecond)
{
}

///<summary>Construct a date time value</summary>
/// <param name="year">Input year</param>
/// <param name="month">Input month</param>
/// <param name="day">Input day of month</param>
/// <param name="hour">Input hour</param>
/// <param name="minute">Input minute</param>
/// <param name="second">Input second</param>
/// <param name="microsecond">Input microsecond</param>
///<return>Returns nothing.</return>
MgDateTime::MgDateTime(INT16 year, INT8 month, INT8 day,
    INT8 hour, INT8 minute, INT8 second, INT32 microsecond) :
    m_year(year),
    m_month(month),
    m_day(day),
    m_hour(hour),
    m_minute(minute),
    m_second(second),
    m_microsecond(microsecond)
{
}

///<summary>Construct a date time value</summary>
/// <param name="year">Input year</param>
/// <param name="month">Input month</param>
/// <param name="day">Input day of month</param>
/// <param name="hour">Input hour</param>
/// <param name="minute">Input minute</param>
/// <param name="second">Input second</param>
///<return>Returns nothing.</return>
MgDateTime::MgDateTime(INT16 year, INT8 month, INT8 day,
    INT8 hour, INT8 minute, float seconds) :
    m_year(year),
    m_month(month),
    m_day(day),
    m_hour(hour),
    m_minute(minute),
    m_second(-1),
    m_microsecond(-1)
{
    SplitSeconds(seconds);
}

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the MgDateTime object based on the specified FDO date time
/// string in the formats:
///     "TIMESTAMP 'CCYY-MM-DD hh:mm:ss[.sss]'"
///     "DATE 'CCYY-MM-DD'"
///     "TIME 'hh:mm:ss[.sss]'"
/// </summary>
///----------------------------------------------------------------------------

MgDateTime::MgDateTime(CREFSTRING fdoDateTime)
{
    Initialize();

    bool valid = false;
    int numFields = 0;
    float seconds = 0.0;

    // Parse the FDO date time string.

    if (!fdoDateTime.empty() && fdoDateTime.length() < MG_MAX_DATE_TIME_BUFFER_LENGTH)
    {
        wchar_t buf[MG_MAX_DATE_TIME_BUFFER_LENGTH];
        ::memset(buf, 0, sizeof(buf));

        bool isDate = STRING::npos != fdoDateTime.find(L"-");
        bool isTime = STRING::npos !=  fdoDateTime.find(L":");

        if (isDate && isTime)
        {
            numFields = ::swscanf(fdoDateTime.c_str(), L"%s '%d-%d-%d %d:%d:%f'",
                buf, &m_year, &m_month, &m_day, &m_hour, &m_minute, &seconds);
            valid = (7 == numFields);
        }
        else if (isDate)
        {
            numFields = ::swscanf(fdoDateTime.c_str(), L"%s '%d-%d-%d'",
                buf, &m_year, &m_month, &m_day);
            valid = (4 == numFields);
        }
        else if (isTime)
        {
            numFields = ::swscanf(fdoDateTime.c_str(), L"%s '%d:%d:%f'",
                buf, &m_hour, &m_minute, &seconds);
            valid = (4 == numFields);
        }
    }

    if (!valid)
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(fdoDateTime);

        throw new MgInvalidArgumentException(L"MgDateTime.MgDateTime",
            __LINE__, __WFILE__, &arguments, L"MgInvalidFdoDateTime", NULL);
    }

    SplitSeconds(seconds);
}

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the MgDateTime object based on the specified XML date time
/// string in the following formmats:
///     "CCYY-MM-DDThh:mm:ss[.ssssss]"
///     "CCYY-MM-DDThh:mm:ss[.ssssss]Z"
/// Currently, FDO does not support time zones, hence the following formats
/// are not supported:
///     "CCYY-MM-DDThh:mm:ss[.ssssss]+hh:mm"
///     "CCYY-MM-DDThh:mm:ss[.ssssss]-hh:mm"
/// </summary>
///----------------------------------------------------------------------------

MgDateTime::MgDateTime(const string& xmlDateTime)
{
    Initialize();

    bool utc = false;
    int numFields = 0;
    int year, month, day, hour, minute;
    year = month = day = hour = minute = -1;
    float seconds = 0.0;

    // Parse the XML date time string.

    if (!xmlDateTime.empty())
    {
        if (string::npos != xmlDateTime.find("Z"))
        {
            utc = true;
        }

        // Note that using "%hd-%c-%cT%c:%c:%f" with data members does not work so well.
        numFields = ::sscanf(xmlDateTime.c_str(), "%d-%d-%dT%d:%d:%f",
            &year, &month, &day, &hour, &minute, &seconds);
    }

    if (6 != numFields)
    {
        STRING buffer;
        MgUtil::MultiByteToWideChar(xmlDateTime, buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgDateTime.MgDateTime",
            __LINE__, __WFILE__, &arguments, L"MgInvalidXmlDateTime", NULL);
    }

    m_year   = year;
    m_month  = (INT8)month;
    m_day    = (INT8)day;
    m_hour   = (INT8)hour;
    m_minute = (INT8)minute;

    SplitSeconds(seconds);

    // If the time is UTC, then convert it to the local time.

    if (utc)
    {
        time_t timeValue = ToTimeValue();
        INT32 microsecond = m_microsecond;

        time_t localTime;
        ACE_OS::time(&localTime);

        struct tm gmTime = *ACE_OS::gmtime(&localTime);
        gmTime.tm_isdst = -1; // Make the GMT time local.
        double timeOffset = ACE_OS::difftime(localTime, ACE_OS::mktime(&gmTime));

        timeValue += (time_t)timeOffset;
        SetDateTimeInfo(*ACE_OS::localtime(&timeValue));
        m_microsecond = microsecond;
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the MgDateTime object based on the specified time value
/// (the number of seconds elapsed since midnight (00:00:00), January 1, 1970
/// to 19:14:07, January 18, 2038, UTC).
/// </summary>
///----------------------------------------------------------------------------

MgDateTime::MgDateTime(time_t timeValue)
{
    // Get the current system time for negative time values. Note that these
    // invalid values cause runtime date/time functions to throw exceptions.

    if (timeValue < 0)
    {
        ACE_OS::time(&timeValue);
    }

    // Convert the time value to the local time.

    SetDateTimeInfo(*ACE_OS::localtime(&timeValue));
}

///----------------------------------------------------------------------------
/// <summary>
/// Constructs an MgDateTime object based on a specified number that was
/// previously generated by the ToNumber method.  For a description of these
/// numbers, see DateTime.h.
/// </summary>
///----------------------------------------------------------------------------

MgDateTime::MgDateTime(double number)
{
    Initialize();

    // DateTime
    if (number >= MIN_DATETIME)
    {
        INT64 days = (INT64)number / (24 * 60 * 60 * 1000);
        CalendarDate((long)days, 'G');
        INT64 milliSeconds = (INT64)number - days * (24 * 60 * 60 * 1000);
        AssignTime(milliSeconds);
    }
    // Time
    else if (number >= TIME_ZERO)
    {
        INT64 milliSeconds = (INT64)number - TIME_ZERO;
        AssignTime(milliSeconds);
    }
    // Date
    else
    {
        INT64 days = (INT64)number - DATE_ZERO;
        CalendarDate((long)days, 'G');
    }
}

MgDateTime::MgDateTime(const MgDateTime& dt)
{
    *this = dt;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgDateTime::GetClassId()
{
    return m_cls_id;
}

void MgDateTime::Dispose()
{
    delete this;
}
///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Returns true if only the date is valid, false otherwise.
/// </summary>
bool MgDateTime::IsDate()
{
    return (-1 != m_year && -1 == m_hour);
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Returns true if only the time is valid, false otherwise.
/// </summary>
bool MgDateTime::IsTime()
{
    return (-1 == m_year && -1 != m_hour);
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Returns true if both the date and time are valid, false otherwise.
/// </summary>
bool MgDateTime::IsDateTime()
{
    return (-1 != m_year && -1 != m_hour);
}

///<summary>Gets the day component of this object</summary>
///<returns>Returns the day component of this object.</returns>
INT8 MgDateTime::GetDay()
{
    return m_day;
}

///<summary>Gets the month component of this object</summary>
///<returns>Returns the month component of this object.</returns>
INT8 MgDateTime::GetMonth()
{
    return m_month;
}

///<summary>Gets the year component of this object</summary>
///<returns>Returns the year component of this object.</returns>
INT16 MgDateTime::GetYear()
{
    return m_year;
}

///<summary>Gets the hour component of this object</summary>
///<returns>Returns the hour component of this object.</returns>
INT8 MgDateTime::GetHour()
{
    return m_hour;
}

///<summary>Gets the minute component of this object</summary>
///<returns>Returns the minute component of this object.</returns>
INT8 MgDateTime::GetMinute()
{
    return m_minute;
}

///<summary>Gets the second component of this object</summary>
///<returns>Returns the second component of this object.</returns>
INT8 MgDateTime::GetSecond()
{
    return m_second;
}

///<summary>Gets the microsecond component of this object</summary>
///<returns>Returns the microsecond component of this object.</returns>
INT32 MgDateTime::GetMicrosecond()
{
    return m_microsecond;
}

///<summary>Sets the day component of this object</summary>
///<param name="day">Day of the month in the range of 1 to 31 inclusive</param>
void MgDateTime::SetDay(INT8 day)
{
    if (day < 1 || day > 31)
    {
        STRING buffer;
        MgUtil::Int32ToString(day, buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgDateTime.SetDay",
            __LINE__, __WFILE__, &arguments, L"MgInvalidDay", NULL);
    }

    m_day = day;
}

///<summary>Sets the month component of this object</summary>
///<param name="month">Month in the range of 1 to 12 inclusive (January = 1)</param>
void MgDateTime::SetMonth(INT8 month)
{
    if (month < 1 || month > 12)
    {
        STRING buffer;
        MgUtil::Int32ToString(month, buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgDateTime.SetMonth",
            __LINE__, __WFILE__, &arguments, L"MgInvalidMonth", NULL);
    }

    m_month = month;
}

///<summary>Sets the year component of this object</summary>
///<param name="year">Year in the range of 1 to 9999</param>
void MgDateTime::SetYear(INT16 year)
{
    if (year < 1 || year > 9999)
    {
        STRING buffer;
        MgUtil::Int32ToString(year, buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgDateTime.SetYear",
            __LINE__, __WFILE__, &arguments, L"MgInvalidYear", NULL);
    }

    m_year = year;
}

///<summary>Sets the hour component of this object</summary>
///<param name="hour">Hour since midnight in the range of 0 to 23</param>
void MgDateTime::SetHour(INT8 hour)
{
    if (hour < 0 || hour > 23)
    {
        STRING buffer;
        MgUtil::Int32ToString(hour, buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgDateTime.SetHour",
            __LINE__, __WFILE__, &arguments, L"MgInvalidHour", NULL);
    }

    m_hour = hour;
}

///<summary>Sets the minute component of this object</summary>
///<param name="minute">Minutes after hour in the range of 0 to 59</param>
void MgDateTime::SetMinute(INT8 minute)
{
    if (minute < 0 || minute > 59)
    {
        STRING buffer;
        MgUtil::Int32ToString(minute, buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgDateTime.SetMinute",
            __LINE__, __WFILE__, &arguments, L"MgInvalidMinute", NULL);
    }

    m_minute = minute;
}

///<summary>Sets the second component of this object</summary>
///<param name="second">Seconds after minute in the range of 0 to 59</param>
void MgDateTime::SetSecond(INT8 second)
{
    if (second < 0 || second > 59)
    {
        STRING buffer;
        MgUtil::Int32ToString(second, buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgDateTime.SetSecond",
            __LINE__, __WFILE__, &arguments, L"MgInvalidSecond", NULL);
    }

    m_second = second;
}

///<summary>Sets the microsecond component of this object</summary>
///<param name="microsecond">Microsecond after second in the range of 0 to 999999</param>
void MgDateTime::SetMicrosecond(INT32 microsecond)
{
    if (microsecond < 0 || microsecond > 999999)
    {
        STRING buffer;
        MgUtil::Int32ToString(microsecond, buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgDateTime.SetMicrosecond",
            __LINE__, __WFILE__, &arguments, L"MgInvalidMicroSecond", NULL);
    }

    m_microsecond = microsecond;
}

MgDateTime& MgDateTime::operator=(const MgDateTime& dt)
{
    if (&dt != this)
    {
        m_year        = dt.m_year;
        m_month       = dt.m_month;
        m_day         = dt.m_day;
        m_hour        = dt.m_hour;
        m_minute      = dt.m_minute;
        m_second      = dt.m_second;
        m_microsecond = dt.m_microsecond;
    }

    return *this;
}

bool MgDateTime::operator<(const MgDateTime& dt) const
{
    if ((m_year <  dt.m_year)
     || (m_year == dt.m_year && m_month <  dt.m_month)
     || (m_year == dt.m_year && m_month == dt.m_month && m_day <  dt.m_day)
     || (m_year == dt.m_year && m_month == dt.m_month && m_day == dt.m_day && m_hour <  dt.m_hour)
     || (m_year == dt.m_year && m_month == dt.m_month && m_day == dt.m_day && m_hour == dt.m_hour && m_minute <  dt.m_minute)
     || (m_year == dt.m_year && m_month == dt.m_month && m_day == dt.m_day && m_hour == dt.m_hour && m_minute == dt.m_minute && m_second <  dt.m_second)
     || (m_year == dt.m_year && m_month == dt.m_month && m_day == dt.m_day && m_hour == dt.m_hour && m_minute == dt.m_minute && m_second == dt.m_second && m_microsecond < dt.m_microsecond))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool MgDateTime::operator<=(const MgDateTime& dt) const
{
    if (*this > dt)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool MgDateTime::operator>(const MgDateTime& dt) const
{
    if ((m_year >  dt.m_year)
     || (m_year == dt.m_year && m_month >  dt.m_month)
     || (m_year == dt.m_year && m_month == dt.m_month && m_day >  dt.m_day)
     || (m_year == dt.m_year && m_month == dt.m_month && m_day == dt.m_day && m_hour >  dt.m_hour)
     || (m_year == dt.m_year && m_month == dt.m_month && m_day == dt.m_day && m_hour == dt.m_hour && m_minute >  dt.m_minute)
     || (m_year == dt.m_year && m_month == dt.m_month && m_day == dt.m_day && m_hour == dt.m_hour && m_minute == dt.m_minute && m_second >  dt.m_second)
     || (m_year == dt.m_year && m_month == dt.m_month && m_day == dt.m_day && m_hour == dt.m_hour && m_minute == dt.m_minute && m_second == dt.m_second && m_microsecond > dt.m_microsecond))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool MgDateTime::operator>=(const MgDateTime& dt) const
{
    if (*this < dt)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool MgDateTime::operator==(const MgDateTime& dt) const
{
    if (m_year        == dt.m_year
     && m_month       == dt.m_month
     && m_day         == dt.m_day
     && m_hour        == dt.m_hour
     && m_minute      == dt.m_minute
     && m_second      == dt.m_second
     && m_microsecond == dt.m_microsecond)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool MgDateTime::operator!=(const MgDateTime& dt) const
{
    return !(dt == *this);
}

void MgDateTime::Serialize(MgStream* stream)
{
    Ptr<MgStreamHelper> helper = stream->GetStreamHelper();
    helper->WriteUINT16(m_year);
    helper->WriteUINT8((BYTE)m_month);
    helper->WriteUINT8((BYTE)m_day);
    helper->WriteUINT8((BYTE)m_hour);
    helper->WriteUINT8((BYTE)m_minute);
    helper->WriteUINT8((BYTE)m_second);
    helper->WriteUINT32(m_microsecond);
}

void MgDateTime::Deserialize(MgStream* stream)
{
    BYTE ch;
    Ptr<MgStreamHelper> helper = stream->GetStreamHelper();

    UINT16 val;
    helper->GetUINT16(val);
    m_year = (INT16) val;

    helper->GetUINT8(ch);
    m_month = INT8(ch);

    helper->GetUINT8(ch);
    m_day = (INT8)ch;

    helper->GetUINT8(ch);
    m_hour = (INT8)ch;

    helper->GetUINT8(ch);
    m_minute = (INT8)ch;

    helper->GetUINT8(ch);
    m_second = (INT8)ch;

    UINT32 val2;
    helper->GetUINT32(val2);
    m_microsecond = (INT32) val2;
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns an FDO date time string in one of these formats:
///     "TIMESTAMP 'CCYY-MM-DD hh:mm:ss[.sss]'"
///     "DATE 'CCYY-MM-DD'"
///     "TIME 'hh:mm:ss[.sss]'"
/// </summary>
///----------------------------------------------------------------------------

STRING MgDateTime::ToString()
{
    // Format the FDO date time string.

    string fdoDateTime;
    char buf[MG_MAX_DATE_TIME_BUFFER_LENGTH];

    ::memset(buf, 0, sizeof(buf));

    if (IsDateTime())
    {
        if (0 == m_microsecond)
        {
            ::sprintf(buf, "TIMESTAMP '%04d-%02d-%02d %02d:%02d:%02d'",
                m_year, m_month, m_day, m_hour, m_minute, m_second);

            fdoDateTime = buf;
        }
        else
        {
            if (m_microsecond % 1000 == 0) // only report milliseconds for FDO
                ::sprintf(buf, "TIMESTAMP '%04d-%02d-%02d %02d:%02d:%02d.%03d",
                    m_year, m_month, m_day, m_hour, m_minute, m_second, m_microsecond / 1000);
            else
                ::sprintf(buf, "TIMESTAMP '%04d-%02d-%02d %02d:%02d:%02d.%06d",
                    m_year, m_month, m_day, m_hour, m_minute, m_second, m_microsecond);

            fdoDateTime = buf;
            MgUtil::TrimEndingZeros(fdoDateTime);
            fdoDateTime += "'";
        }
    }
    else if (IsDate())
    {
        ::sprintf(buf, "DATE '%04d-%02d-%02d'",
            m_year, m_month, m_day);

        fdoDateTime = buf;
    }
    else if (IsTime())
    {
        if (0 == m_microsecond)
        {
            ::sprintf(buf, "TIME '%02d:%02d:%02d'",
                m_hour, m_minute, m_second);

            fdoDateTime = buf;
        }
        else
        {
            if (m_microsecond % 1000 == 0) // only report milliseconds for FDO
                ::sprintf(buf, "TIME '%02d:%02d:%02d.%03d",
                    m_hour, m_minute, m_second, m_microsecond / 1000);
            else
                ::sprintf(buf, "TIME '%02d:%02d:%02d.%06d",
                    m_hour, m_minute, m_second, m_microsecond);

            fdoDateTime = buf;
            MgUtil::TrimEndingZeros(fdoDateTime);
            fdoDateTime += "'";
        }
    }
    else
    {
        throw new MgDateTimeException(L"MgDateTime.ToString",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return MgUtil::MultiByteToWideChar(fdoDateTime);
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the XML date time string in the following formats:
///     "CCYY-MM-DDThh:mm:ss[.ssssss]"
///     "CCYY-MM-DDThh:mm:ss[.ssssss]Z"
/// </summary>
///----------------------------------------------------------------------------

STRING MgDateTime::ToXmlString(bool utc)
{
    return MgUtil::MultiByteToWideChar(ToXmlStringUtf8(utc));
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the XML date time string UTF8 in the following formats:
///     "CCYY-MM-DDThh:mm:ss[.ssssss]"
///     "CCYY-MM-DDThh:mm:ss[.ssssss]Z"
/// </summary>
///----------------------------------------------------------------------------

string MgDateTime::ToXmlStringUtf8(bool utc)
{
    // Convert the local time to the UTC time if required.

    time_t timeValue = ToTimeValue();
    struct tm timeInfo;

    if (utc)
    {
        timeInfo = *::ACE_OS::gmtime(&timeValue);
    }
    else
    {
        timeInfo = *::ACE_OS::localtime(&timeValue);
    }

    // Format the XML date time string.

    string xmlDateTime;
    char buf[MG_MAX_DATE_TIME_BUFFER_LENGTH];

    ::memset(buf, 0, sizeof(buf));

    if (0 == m_microsecond)
    {
        ACE_OS::strftime(buf, MG_MAX_DATE_TIME_BUFFER_LENGTH,
            "%Y-%m-%dT%H:%M:%S", &timeInfo);

        xmlDateTime = buf;
    }
    else
    {
        ::sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d.%06d",
            timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday,
            timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec, m_microsecond);

        xmlDateTime = buf;
        MgUtil::TrimEndingZeros(xmlDateTime);
    }

    if (utc)
    {
        xmlDateTime += "Z";
    }

    return xmlDateTime;
}

///----------------------------------------------------------------------------
/// <summary>
/// Converts the date into a time_t value to allow time functions to be
/// performed on this object.  Loses the microsecond precision.
/// Date and time must be from midnight (00:00:00), January 1, 1970
/// to 19:14:07 January 18, 2038, UTC.
/// </summary>
///----------------------------------------------------------------------------

time_t MgDateTime::ToTimeValue()
{
    // Perform the conversion.
    struct tm timeInfo;

    GetDateTimeInfo(timeInfo);

    time_t timeValue = ACE_OS::mktime(&timeInfo);

    if (timeValue < 0)
    {
        throw new MgOutOfRangeException(L"MgDateTime.ToTimeValue",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return timeValue;
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns a unique number representing this Date, Time or DateTime.
/// The algorithm for determining this number is described in DateTime.h.
/// </summary>
///----------------------------------------------------------------------------
double MgDateTime::ToNumber()
{
    double val;
    if (IsDate())
    {
        INT64 gdn = GregorianDay(m_day, m_month, m_year, 'G');
        val = (double)gdn + DATE_ZERO;
    }
    else if (IsTime())
    {
        INT64 milliSeconds = (INT64)m_hour   * 60 * 60 * 1000 +
                             (INT64)m_minute * 60 * 1000 +
                             (INT64)m_second * 1000 +
                             (INT64)m_microsecond / 1000;
        val = (double)milliSeconds + TIME_ZERO;
    }
    else // if (IsDateTime())
    {
        val = (double)ToMilliSeconds();
    }
    return val;
}

// Converts this DateTime to the number of milliseconds
// before or after the beginning of the Gregorian calendar.
double MgDateTime::ToMilliSeconds()
{
    INT64 day = GregorianDay(m_day, m_month, m_year, 'G');
    INT64 milliSeconds = (INT64)m_hour   * 60 * 60 * 1000 +
                         (INT64)m_minute * 60 * 1000 +
                         (INT64)m_second * 1000 +
                         (INT64)m_microsecond / 1000;
    if (day < 0)
        milliSeconds *= -1;

    return (day * 24.0 * 60.0 * 60.0 * 1000.0 + milliSeconds);
}

int MgDateTime::IsLeapYear(long year, char calendar) const
{
    calendar = (char)toupper((int)calendar);

    if ( !calendar )
    {
        calendar = 'G';
    }

    if ( year%4 != 0L )  // not evenly divisible by 4
    {
        return 0;
    }
    else
    {
        if ( calendar == 'J' )
        {
            return 1;
        }
        else  // either divisible by 400 or not divisible by 100
        {
            return ( (year%100 != 0L || year%400 == 0L) ? 1 : 0 );
        }
    }

} // end: IsLeapYear()


long MgDateTime::lfloor(long a, long b)   //  assumes b positive
{
    return ( a >= 0L ? a/b : ( a%b == 0L ) - 1 - labs(a)/b );
}


long MgDateTime::GregorianDay(int day, int month, long year, char calendar)
{
    long gdn;
    int month_length[14] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 0};

    calendar = (char)toupper((int)calendar);
    if ( !calendar )
        calendar = 'G';

    month_length[2] = 28 + IsLeapYear(year, calendar);

    if ( month < 1  || month > 12 || day < 1  || day > month_length[month] )
        return 0;

    // calculate the number of days before or after
    // October 15, 1582 (Gregorian)

    gdn = (year-1)*365 + lfloor(year-1,4L);
    if ( calendar == 'G' )
        gdn += lfloor(year-1,400L) - lfloor(year-1,100L);

    while (--month)
        gdn += month_length[month];
    gdn += day - 577736L - 2*(calendar=='J');

    month_length[2] = 28;
    return gdn;
}

///----------------------------------------------------------------------------
///
/// CalendarDate - inverse of the above method.
///
///----------------------------------------------------------------------------

void MgDateTime::CalendarDate(long gdn, char calendar)
{
    int month_length[14] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 0};
    int month, i, exception;
    long year, y4, y100, y400;

    calendar = (char)toupper((int)calendar);
    if ( !calendar )
        calendar = 'G';

    gdn += 577735L + 2*(calendar=='J');

    y400 = 146100L - 3*(calendar=='G');
    y100 =  36525L -   (calendar=='G');
    y4   =   1461L;

    exception = false;
    year = 400*lfloor(gdn,y400);        //  400-year periods
    gdn -= y400*lfloor(gdn,y400);
    if ( gdn > 0L )
    {
        year += 100*lfloor(gdn,y100);   //  100-year periods
        gdn -= y100*lfloor(gdn,y100);
        exception = ( gdn == 0L && calendar == 'G' );
        if ( gdn > 0L )
        {
            year += 4*lfloor(gdn,y4);   //  4-year periods
            gdn -= y4*lfloor(gdn,y4);
            if ( gdn > 0L )
            {
                i = 0;
                while ( gdn > 365 && ++i < 4 )
                {
                    year++;
                    gdn -= 365L;
                }
            }
        }
    }

    if ( exception )
        gdn = 366L;
        //  occurs once every hundred years with Gregorian calendar
    else
    {
        year++;
        gdn++;
    }

    month_length[2] = 28 + IsLeapYear(year, calendar);
    month = 1;
    while ( month < 13 && gdn > month_length[month] )
        gdn -= month_length[month++];

    if ( month == 13 )
    {
        month = 1;
        year++;
    }

    month_length[2] = 28;

    m_year = (INT16)year;
    m_month = (INT8)month;
    m_day = (INT8)gdn;
}

// populates the hours, minutes, seconds and microseconds members
// of this MgDateTime using the provided number of milliseconds from
// the beginning of this day.
void MgDateTime::AssignTime(INT64 milliSeconds)
{
    INT64 hours = milliSeconds / (60 * 60 * 1000);
    milliSeconds = milliSeconds - hours * (60 * 60 * 1000);
    INT64 minutes = milliSeconds / (60 * 1000);
    milliSeconds = milliSeconds - minutes * (60 * 1000);
    INT64 seconds = milliSeconds / 1000;
    milliSeconds = milliSeconds - seconds * 1000;
    INT64 microseconds = milliSeconds * 1000;

    m_hour = (INT8)hours;
    m_minute = (INT8)minutes;
    m_second = (INT8)seconds;
    m_microsecond = (INT32)microseconds;
}

void MgDateTime::Initialize()
{
    m_year        = -1;
    m_month       = -1;
    m_day         = -1;
    m_hour        = -1;
    m_minute      = -1;
    m_second      = -1;
    m_microsecond = -1;
}

void MgDateTime::GetDateTimeInfo(struct tm& timeInfo) const
{
    timeInfo.tm_year  = m_year - 1900;
    timeInfo.tm_mon   = m_month - 1;
    timeInfo.tm_mday  = m_day;
    timeInfo.tm_hour  = m_hour;
    timeInfo.tm_min   = m_minute;
    timeInfo.tm_sec   = m_second;
    timeInfo.tm_wday  = -1;
    timeInfo.tm_yday  = -1;
    timeInfo.tm_isdst = -1;
}

void MgDateTime::SetDateTimeInfo(const struct tm& timeInfo)
{
    m_year        = timeInfo.tm_year + 1900;
    m_month       = timeInfo.tm_mon + 1;
    m_day         = timeInfo.tm_mday;
    m_hour        = timeInfo.tm_hour;
    m_minute      = timeInfo.tm_min;
    m_second      = timeInfo.tm_sec;
    m_microsecond = 0;
}

void MgDateTime::SplitSeconds(float seconds)
{
    float intPart = 0.0, fractPart = 0.0;
    fractPart = ::modff(seconds, &intPart);

    m_second = (INT8)intPart;
    m_microsecond = (INT32)((fractPart * MG_MICRO_VALUE) + 0.5);

    if (m_microsecond > 999999)
    {
        m_microsecond = 999999;
    }
}

void MgDateTime::ValidateDateTime()
{
    ValidateDate();
    ValidateTime();
}

void MgDateTime::ValidateDate()
{
    if (m_year < 1 || m_year > 9999)
    {
        STRING buffer;
        MgUtil::Int32ToString(m_year, buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgDateTime.ValidateDate",
            __LINE__, __WFILE__, &arguments, L"MgInvalidYear", NULL);
    }

    if (m_month < 1 || m_month > 12)
    {
        STRING buffer;
        MgUtil::Int32ToString(m_month, buffer);

        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgDateTime.ValidateDate",
            __LINE__, __WFILE__, &arguments, L"MgInvalidMonth", NULL);
    }

    int month_length[14] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 0};

    month_length[2] = 28 + IsLeapYear(m_year, 'G');

    if (m_day < 1 || m_day > month_length[m_month])
    {
        STRING buffer;
        MgUtil::Int32ToString(m_day, buffer);

        MgStringCollection arguments;
        arguments.Add(L"3");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgDateTime.ValidateDate",
            __LINE__, __WFILE__, &arguments, L"MgInvalidDay", NULL);
    }
}

void MgDateTime::ValidateTime()
{
    if (m_hour < 0 || m_hour > 23)
    {
        STRING buffer;
        MgUtil::Int32ToString(m_hour, buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgDateTime.ValidateTime",
            __LINE__, __WFILE__, &arguments, L"MgInvalidHour", NULL);
    }

    if (m_minute < 0 || m_minute > 59)
    {
        STRING buffer;
        MgUtil::Int32ToString(m_minute, buffer);

        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgDateTime.ValidateTime",
            __LINE__, __WFILE__, &arguments, L"MgInvalidMinute", NULL);
    }

    if (m_second < 0 || m_second > 59)
    {
        STRING buffer;
        MgUtil::Int32ToString(m_second, buffer);

        MgStringCollection arguments;
        arguments.Add(L"3");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgDateTime.ValidateTime",
            __LINE__, __WFILE__, &arguments, L"MgInvalidSecond", NULL);
    }

    if (m_microsecond < 0 || m_microsecond > 999999)
    {
        STRING buffer;
        MgUtil::Int32ToString(m_microsecond, buffer);

        MgStringCollection arguments;
        arguments.Add(L"4");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgDateTime.ValidateTime",
            __LINE__, __WFILE__, &arguments, L"MgInvalidMicroSecond", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Validates the date time value.  This method should be called
/// after the date time value has been fully specified by Set methods.
/// </summary>
void MgDateTime::Validate()
{
    if (IsDateTime())
    {
        ValidateDateTime();
    }
    else if (IsDate())
    {
        ValidateDate();
    }
    else if (IsTime())
    {
        ValidateTime();
    }
    else
    {
        throw new MgDateTimeException(L"MgDateTime.Validate",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }
}
