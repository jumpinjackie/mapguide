//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#include "MapAgentStrings.h"

// DO NOT TRANSLATE.  THESE STRINGS ARE HTTP PROTOCOL SPECIFIC
const char* MapAgentStrings::ServerName = "SERVER_NAME";
const char* MapAgentStrings::ServerPort = "SERVER_PORT";
const char* MapAgentStrings::ScriptName = "SCRIPT_NAME";
const char* MapAgentStrings::Secure = "HTTPS";
const char* MapAgentStrings::Http = "http://";
const char* MapAgentStrings::Https = "https://";
const char* MapAgentStrings::QueryString = "QUERY_STRING";
const char* MapAgentStrings::RequestMethod = "REQUEST_METHOD";
const char* MapAgentStrings::HttpAuth = "HTTP_AUTHORIZATION";
const char* MapAgentStrings::HttpRemoteUser = "REMOTE_USER";
const char* MapAgentStrings::RemoteAddr = "REMOTE_ADDR";
const char* MapAgentStrings::HttpClientIp = "HTTP_CLIENT_IP";
const char* MapAgentStrings::HttpXForwardedFor = "HTTP_X_FORWARDED_FOR";
const char* MapAgentStrings::Unknown = "unknown";
const char* MapAgentStrings::Username = "USERNAME";
const char* MapAgentStrings::Password = "PASSWORD";
const char* MapAgentStrings::PhysicalPath = "APPL_PHYSICAL_PATH";
const char* MapAgentStrings::ContentType = "CONTENT_TYPE";
const char* MapAgentStrings::UrlEncoded = "application/x-www-form-urlencoded";
const char* MapAgentStrings::MultiPartForm = "multipart/form-data";
const char* MapAgentStrings::ContentLength = "CONTENT_LENGTH";
const char* MapAgentStrings::BasicAuth = "Basic ";
const char* MapAgentStrings::WWWAuth = "WWW-Authenticate: Basic realm=\"%s\"\r\n";
const char* MapAgentStrings::UnauthorizedHeader = "Status: 401 Unauthorized\r\n";
const char* MapAgentStrings::Unauthorized = "401 Unauthorized";
const char* MapAgentStrings::ContentTypeHeader = "Content-type: %s%s\r\n";
const char* MapAgentStrings::Utf8Text = "; charset=utf-8";
const char* MapAgentStrings::ContentLengthHeader = "Content-Length: %d\r\n";
const char* MapAgentStrings::StatusHeader = "Status: %d %s\r\n";
const char* MapAgentStrings::StatusOkHeader = "Status: 200 OK\r\n";
const char* MapAgentStrings::TextPlain = "text/plain";
const char* MapAgentStrings::TextHtml = "text/html";
const char* MapAgentStrings::TextXml = "text/xml";
const char* MapAgentStrings::PostBoundary = "boundary=";
const char* MapAgentStrings::PostName = "name=\"";
const char* MapAgentStrings::PostContent = "Content-Type: ";
const char* MapAgentStrings::PostFile = "filename=";
const char* MapAgentStrings::CrLf = "\r\n";
const char* MapAgentStrings::ScriptFileName = "SCRIPT_FILENAME";
const char* MapAgentStrings::StatusKey = "Status";
const char* MapAgentStrings::StatusVal200OK = "200 OK";
const char* MapAgentStrings::ContentLengthKey = "Content-Length";
const char* MapAgentStrings::ContentTypeKey = "Content-Type";
const char* MapAgentStrings::WWWAuthKey = "WWW-Authenticate";
const char* MapAgentStrings::WWWAuthVal = "Basic realm=\"%s\"";


const wchar_t* MapAgentStrings::WebConfig = L"../webconfig.ini";
const wchar_t* MapAgentStrings::FailedAuth1 = L"MgAuthenticationFailedException";
const wchar_t* MapAgentStrings::FailedAuth2 = L"MgUnauthorizedAccessException";
const wchar_t* MapAgentStrings::ProductName = L"MapGuide";

// used to indicate the creation of temporary files as part of a request
const wchar_t* MapAgentStrings::TempfileKey = L"tempfile";
