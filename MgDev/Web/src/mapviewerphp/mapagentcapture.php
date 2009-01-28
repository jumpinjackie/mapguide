<?php

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

// Start output buffering
ob_start();

require_once("capturerequest.php");
include 'common.php';
include 'actions.php';
include 'helper.php';

//Edit that with your test name
//First paramater - the name of your tests
//Second parameter - the name of the database where all http requests will be stored
//Third parameter - the absolute path to the location where the database file and all uploaded files(if any) should be stored
//Fourth parameter - the relative from your test script to all data files. This path will be stored in the database and, thus, will ensure that
//everyone will be able to run the captured tests
$request = new CaptureRequest("StudioTests", "StudioTests.db", "D:\\AwDev\\UnitTest\\TestData\\StudioTests\\", "..\\..\\TestData\\StudioTests\\");
$request->SaveRequest();

// turn this on if debugging needed
$DEBUG_AGENT = 0;

// amount of time in seconds to sleep/debug
$SLEEP_SECS = 20;

// Useful for debugging
if ($DEBUG_AGENT) sleep($SLEEP_SECS);

// Initialize web tier with the site configuration file.
InitializeWebTier();

// Create a request object
$hreq = new MgHttpRequest();

// Get access to the header collection
$hdr = $hreq->GetHeader();

// Get access to request parameter collection
$reqParam = $hreq->GetRequestParam();

// Get access to request metadata collection
$reqMd = $hreq->GetRequestMetadata();

// Fill all header variables
setHeader($hdr);

// Fill request parameters
setRequestParameters($reqParam);

// Fill request metadata
setRequestMetadata($reqMd);

// Custom action before executing the request
beforeExecute($hreq);

// Execute the request and return response
$hresp = $hreq->Execute();

// Custom action after execute
if(!afterExecute($hreq, $hresp))
{
    // no custom actions, send stream to client
    $statusCode = $hresp->GetStatusCode();
    if($statusCode != 200)
    {
        $shortError = $hresp->GetHttpErrorMessage();
        $longError = $hresp->GetErrorMessage();
        header("HTTP/1.1 {$statusCode} {$shortError}");
        header("Status: {$statusCode} {$shortError}");
        echo "<html>\n<body>\n";
        echo "<h2>{$shortError}</h2>\n{$longError}";
        echo "</body>\n</html>\n";
    }
    else
    {
        // Dump all headers before sending data
        $header = $hresp->GetHeader();
        if ($header != null)
        {
            $headerLines = $header->GetHeaderNames();
            // TODO: PHP collection semantics do not work on our collections.
            // We should try to address this so that the following will work
            // foreach ($headerLines as $headerLine)
            $hdrCount = $headerLines->GetCount();
            for ($hdrNum = 0; $hdrNum < $hdrCount; $hdrCount++)
            {
                $headerLine = $headerLines->GetItem($hdrNum);
                $headerValue = $header->GetHeaderValue($headerLine);
                header("{$headerLine}: {$headerValue}");
            }
        }

        // adds the content type specific to this response

        $contentType = $hresult->GetResultContentType();

        if (strlen($contentType)>0)
        {
            header("Content-type: " . $contentType);
        }

        $result = $hresp->GetResult();
        $resultObj = $result->GetResultObject();

        $byteReader = null;

        if ($resultObj instanceof MgByteReader)
        {
            $byteReader = $resultObj;
        }
        else if ($resultObj instanceof MgFeatureReader)
        {
            $byteReader = $resultObj->ToXml();
        }
        else if ($resultObj instanceof MgStringCollection)
        {
            $byteReader = $resultObj->ToXml();
        }
        else if ($resultObj instanceof MgSqlDataReader)
        {
            $byteReader = $resultObj->ToXml();
        }
        else if ($resultObj instanceof MgDataReader)
        {
            $byteReader = $resultObj->ToXml();
        }
        else if ($resultObj instanceof MgSpatialContextReader)
        {
            $byteReader = $resultObj->ToXml();
        }
        else if ($resultObj instanceof MgLongTransactionReader)
        {
            $byteReader = $resultObj->ToXml();
        }
        else if ($resultObj instanceof MgHttpPrimitiveValue)
        {
            $str = $resultObj->ToString();
            echo $str;
        }

        if ($byteReader != null)
        {
            // Set content length header
            header("Content-Length:".$byteReader->GetLength());

            $len = 0;
            $val = "";
            $len = $byteReader->Read($val, 4096);
            while ($len > 0)
            {
                echo $val;
                ob_flush();
                flush();s
                $len = $byteReader->Read($val, 4096);
            }
        }
    }
}

// Flush output buffer
ob_end_flush();
?>
