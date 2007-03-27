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

#include "CgiStrings.h"

// DO NOT TRANSLATE.  THESE STRINGS ARE HTTP PROTOCOL SPECIFIC
const char* CgiStrings::ServerName = "SERVER_NAME";
const char* CgiStrings::ServerPort = "SERVER_PORT";
const char* CgiStrings::ScriptName = "SCRIPT_NAME";
const char* CgiStrings::Http = "http://";
const char* CgiStrings::QueryString = "QUERY_STRING";
const char* CgiStrings::RequestMethod = "REQUEST_METHOD";
const char* CgiStrings::HttpAuth = "HTTP_AUTHORIZATION";
const char* CgiStrings::HttpRemoteUser = "REMOTE_USER";
const char* CgiStrings::Username = "USERNAME";
const char* CgiStrings::Password = "PASSWORD";
const char* CgiStrings::PhysicalPath = "APPL_PHYSICAL_PATH";
const char* CgiStrings::ContentType = "CONTENT_TYPE";
const char* CgiStrings::UrlEncoded = "application/x-www-form-urlencoded";
const char* CgiStrings::MultiPartForm = "multipart/form-data";
const char* CgiStrings::ContentLength = "CONTENT_LENGTH";
const char* CgiStrings::BasicAuth = "Basic ";
const char* CgiStrings::WWWAuth = "WWW-Authenticate: Basic realm=\"%s\"\r\n";
const char* CgiStrings::UnauthorizedHeader = "Status: 401 Unauthorized\r\n";
const char* CgiStrings::ContentTypeHeader = "Content-type: %s%s\r\n";
const char* CgiStrings::Utf8Text = "; charset=utf-8";
const char* CgiStrings::ContentLengthHeader = "Content-Length: %d\r\n";
const char* CgiStrings::StatusHeader = "Status: %d %s\r\n";
const char* CgiStrings::StatusOkHeader = "Status: 200 OK\r\n";
const char* CgiStrings::TextPlain = "text/plain";
const char* CgiStrings::TextHtml = "text/html";
const char* CgiStrings::TextXml = "text/xml";
const char* CgiStrings::PostBoundary = "boundary=";
const char* CgiStrings::PostName = "name=\"";
const char* CgiStrings::PostContent = "Content-Type: ";
const char* CgiStrings::PostFile = "filename=";

const wchar_t* CgiStrings::WebConfig = L"../webconfig.ini";
const wchar_t* CgiStrings::FailedAuth1 = L"MgAuthenticationFailedException";
const wchar_t* CgiStrings::FailedAuth2 = L"MgUnauthorizedAccessException";
