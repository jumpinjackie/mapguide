<?php

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

//This class sends Http requests using curl. For more information on curl see www.php.net

require_once("Result.php");

class HttpRequest
{
    private $ch;
    public $cookieJar = "UnitTestCookieJar";

    public function __construct()
    {
        $this->ch = curl_init();
    }

    public function __destruct()
    {
        curl_close($this->ch );
    }

    public function SendRequest($httpRequest, $arrayParam=null, $requestType="GET")
    {
        curl_setopt($this->ch, CURLOPT_HTTPGET, true);
        if ("POST" == $requestType)
        {
            curl_setopt($this->ch, CURLOPT_POST,1);
            curl_setopt($this->ch, CURLOPT_POSTFIELDS, $arrayParam);
        }
        else
        {
            //form the http request for GET requests
            //encode all non-alphanumeric characters except -_
            if (isset($arrayParam))
            {
                $httpRequest .= "?";
                foreach ($arrayParam as $param => $value)
                {
                    $httpRequest .= $param."=".urlencode($value)."&";
                }
            }
        }

        curl_setopt($this->ch, CURLOPT_URL, $httpRequest);
        curl_setopt($this->ch, CURLOPT_HEADER, true);
        curl_setopt($this->ch, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($this->ch, CURLOPT_USERPWD, "Administrator:admin");

        //curl_setopt($this->ch, CURLOPT_COOKIEFILE, $this->cookieJar);
        //curl_setopt($this->ch, CURLOPT_COOKIEJAR, $this->cookieJar);

        //Execute
        $response = curl_exec($this->ch);

        if (curl_errno($this->ch))
        {
            $response = "Curl error: ".curl_error($this->ch)." Code: ".curl_errno($this->ch);
            $result = new Result($response, "text/plain", 559);
        }
        else if (self::GetHttpStatusCode()==559)
        {
            $response = self::GetHttpErrorMessage($response);
            $result = new Result($response, "text/plain", 559);
        }
        //If there was an 100 response we need to remove 2 headers
        else if (strpos($response, "100 Continue"))
        {
            $response = self::RemoveHeader($response);
            $response = self::RemoveHeader($response);
            $result = new Result($response, self::GetContentType(), self::GetHttpStatusCode());
        }
        else
        {
            $response = self::RemoveHeader($response);
            $result = new Result($response, self::GetContentType(), self::GetHttpStatusCode());
        }

        return $result;
    }

    function GetContentType()
    {
        return curl_getinfo($this->ch, CURLINFO_CONTENT_TYPE);
    }

    function GetHttpStatusCode()
    {
        return curl_getinfo($this->ch, CURLINFO_HTTP_CODE);
    }

    //Find the separator between the header and the body and remove the header
    function RemoveHeader($response)
    {
        if (strpos($response, "\r\n\r\n"))
        {
            $response = substr($response, strpos($response, "\r\n\r\n")+4);
        }
        else
        {
            $response = "";
        }
        return $response;
    }

    //Extract the error message after the HTTP status code
    function GetHttpErrorMessage($response)
    {
        $start = strpos($response, "HTTP/1.1 559 ")+strlen("HTTP/1.1 559 ");
        $end   = strpos($response, "Exception\r\n")+strlen("Exception\r\n");
        return substr($response, $start, $end - $start);
    }

    function GetCookie()
    {
        $cookie = "";
        if (file_exists($this->cookieJar))
        {
            $cookies = file_get_contents($this->cookieJar);
            if (strstr($cookies, "MGSESSION") != false)
            {
                //TODO: Determine file format and parse out the cookie
                $cookie = $cookies;
            }
        }
        return $cookie;
    }
}

?>
