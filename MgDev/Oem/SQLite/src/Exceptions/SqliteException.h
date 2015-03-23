//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#ifndef _SQLITE_EXCEPTION_H_
#define _SQLITE_EXCEPTION_H_

#define DECLARE_EXCEPTION_ABSTRACT(className) \
    virtual INT32 GetClassId(); \
    virtual void Raise() = 0; \
    virtual STRING GetClassName() = 0; \
    protected: \
    className() throw();

#define IMPLEMENT_EXCEPTION_ABSTRACT(className) \
    className::className() throw() {} \
    INT32 className::GetClassId() { return m_cls_id; }

//////////////////////////////////////////////////////////////////
///<summary>
/// This is a base class for all Mg exceptions.
///</summary>

class SqliteException : public SqliteDisposable
{
    ///////////////////////////////////////////////////////
    /// Methods
EXTERNAL_API:

    //////////////////////////////////////////////////////////////////
    ///<summary>
    /// Retrieve the formatted exception message for the specified locale.
    /// <param name="locale">The locale identifier to format the message text.</param>
    /// </summary>
    /// <returns>
    /// Localized exception message
    /// </returns>
    virtual STRING GetExceptionMessage() throw();

INTERNAL_API:

    DECLARE_EXCEPTION_ABSTRACT(SqliteException)

INTERNAL_API:

    virtual ~SqliteException();

    SqliteException(CREFSTRING methodName, UINT32 lineNumber, CREFSTRING fileName, CREFSTRING message);

    virtual void Dispose();

private:
    wstring m_methodName;
    UINT32 m_line;
    wstring m_fileName;
    wstring m_message;

CLASS_ID:
    static const INT32 m_cls_id = SQLite_Exception;
};

#define DECLARE_EXCEPTION_DEFAULTS(className) \
    static SqliteDisposable* CreateObject(); \
    virtual INT32 GetClassId(); \
    virtual void Raise(); \
    virtual STRING GetClassName(); \
    protected: \
    className() throw();

#define IMPLEMENT_EXCEPTION_DEFAULTS(className, parentClassName) \
    className::className() throw() {} \
    INT32 className::GetClassId() { return m_cls_id; } \
    SqliteDisposable* className::CreateObject() { return new className(); } \
    void className::Raise() { throw this; } \
    STRING className::GetClassName() { return WIDEN(#className) ; }

#endif
