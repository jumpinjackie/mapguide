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


//This class defines a Result object. Each Result object contains the data returned from an operation,
//content/mime type, and the httpStatusCode(http requests only)
class Result
{
    private $resultData;
    private $contentType;
    private $httpStatusCode;

    public function __construct($resultData="", $contentType="", $httpStatusCode="")
    {
        $this->resultData=$resultData;
        $this->contentType=$contentType;
        $this->httpStatusCode=$httpStatusCode;
    }

    public function SetResultData($newData)
    {
        $this->resultData=$newData;
    }

    public function SetContentType($newContentType)
    {
        $this->contentType=$newContentType;
    }

    public function SetHttpStatusCode($newHttpStatusCode)
    {
        $this->httpStatusCode=$newHttpStatusCode;
    }

    public function GetResultData()
    {
        return $this->resultData;
    }

    public function GetContentType()
    {
        return $this->contentType;
    }

    public function GetHttpStatusCode()
    {
        return $this->httpStatusCode;
    }
}
?>
