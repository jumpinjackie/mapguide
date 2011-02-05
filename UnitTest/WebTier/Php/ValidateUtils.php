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


//Class that defines helper methods used in the validation process.

class ValidateUtils
{
    public static function RemoveStackTrace($result)
    {
        $cleanResult = $result;
        $start = strpos($result, "<h2>");
        if ($start)
        {
            $start += strlen("<h2>");
            $end = strpos($result, "</h2>");
            $cleanResult = substr($result, $start, $end-$start);
        }
        return $cleanResult;
    }

    public static function GetExtension($mimeType)
    {
        $extension="xml";

        if ($mimeType == "application/agf")
        {
            $extension="agf";
        }
        elseif ($mimeType == "application/octet-stream")
        {
            //TODO: what extension should we put for binary data
            $extension="bin";
        }
        elseif ($mimeType == "model/vnd.dwf")
        {
            $extension = "dwf";
        }
        elseif (strstr($mimeType, "ePlot") != FALSE)
        {
            $extension = "dwf";
        }
        elseif ($mimeType == "image/jpeg")
        {
            $extension = "jpg";
        }
        elseif ($mimeType == "image/png")
        {
            $extension = "png";
        }
        elseif (strstr($mimeType,"text/plain") != FALSE)
        {
            $extension = "txt";
        }
        elseif ($mimeType == "image/tiff")
        {
            $extension = "tiff";
        }
        elseif (strstr($mimeType,"text/html") != FALSE)
        {
            $extension = "html";
        }
        elseif ($mimeType == "application/x-w2d")
        {
            $extension = "dwf";
        }

        return $extension;
    }

    public static function RemoveTimeStamp($result)
    {
        $newResult=$result;

        while (substr_count($result,"<CreatedDate>")>0)
        {
            $newResult=substr($result, 0, strpos($result, "<CreatedDate>"));
            $newResult.=substr($result, strpos($result, "</ModifiedDate>")+strlen("</ModifiedDate>"));
            $result=$newResult;
        }
        return $newResult;
    }

    public static function RemoveLogEntryTimeStamp($result)
    {
        $newResult = $result;

        while (substr_count($result, "<") > 0)
        {
            $newResult = substr($result, 0, strpos($result, "<"));
            $newResult.= substr($result, strpos($result, ">")+1);
            $result = $newResult;
        }

        return $newResult;
    }

    public static function RemoveDwfSectionName($actualResult)
    {
        $newResult = substr($actualResult, strpos($actualResult, ".w2d"));
        if (0!=strpos($newResult, "EndOfDWF"))
        {
            $newResult = substr($newResult, 0, strpos($newResult, "EndOfDWF"));
        }
        return $newResult;
    }

    public static function SpecialDataHandling($operation, $resultData, $contentType)
    {
        if ( $operation == "ENUMERATERESOURCES")
        {
            $resultData = self::RemoveTimeStamp($resultData);
        }
        elseif ($operation == "GETDRAWINGLAYER")
        {
            $resultData = self::RemoveDwfSectionName($resultData);
        }
        elseif ($operation == "GETDRAWINGSECTION")
        {
            $resultData = self::RemoveDwfSectionName($resultData);
        }
        elseif ($operation == "GETLOG")
        {
            $resultData = self::RemoveLogEntryTimeStamp($resultData);
        }
        elseif ($operation == "GETMAP")
        {
            $resultData = self::GetMapHeader($resultData);
        }
        elseif ($operation == "GETLONGTRANSACTIONS")
        {
            $resultData = self::RemoveCreationDate($resultData);
        }


        if (strstr($contentType,"text/xml") != FALSE)
        {
            $doc = new DOMDocument();
            $doc->preserveWhiteSpace = true;
            if ($doc->loadXML($resultData))
            {
                $resultData = self::SortElement($doc, '');
            }
        }
        return $resultData;
    }

    //Does validation for operation that do not allow straight string comparison between expected and actual data
    public static function SpecialValidation($operation, &$actualData, $expectedData)
    {
        $success = false;

        if ($operation == "GETFEATUREPROVIDERS")
        {
            $success = self::GetFeatureProvidersValidation($actualData, $expectedData);
        }
        return $success;
    }
    //Hack...returns the part of the exception from the beginning up to and including "exception occurred"
    public static function ProcessExceptionMessage($resultData)
    {
        if (stripos($resultData, "exception occurred"))
        {
            $length = stripos($resultData, "exception occurred")+strlen("exception occurred");
            $resultData = substr($resultData, 0,  $length);
        }
        return $resultData;
    }

    //Check if we have DWF 6.01 in the header. For now this is the only way to validate maps
    public static function GetMapHeader($resultData)
    {
        if (substr_count($resultData, "(DWF V06.01)")>0)
        {
            $resultData = "(DWF V06.01)";
        }

        return $resultData;
    }

    //The expectedData is comma separated list of all providers.
    //The function checks if all the providers in the list are in the $actualData document
    //Overwrites the $actualData with an error message to provide some feedback to the user in case of failure
    public static function GetFeatureProvidersValidation(&$actualData, $expectedData)
    {
        $pass = true;
        $errMsg = "";
        $providers = explode(",", $expectedData);

        foreach ($providers as $providerName)
        {
            if (substr_count($actualData, $providerName)==0)
            {
                $pass = false;
                $errMsg .= sprintf("Provider not found: %s \n",$providerName);
            }
        }
        $actualData = $errMsg;
        return $pass;
    }

    public static function RemoveCreationDate($resultData)
    {
        $newResult=$resultData;

        while (substr_count($resultData,"<CreationDate>")>0)
        {
            $newResult=substr($resultData, 0, strpos($resultData, "<CreationDate>"));
            $newResult.=substr($resultData, strpos($resultData, "</CreationDate>")+strlen("</CreationDate>"));
            $resultData=$newResult;
        }
        return $newResult;
    }

    public static function SortElement($elem, $preText)
    {
        $elemArray = array();
        $elemString = '';

        if ($elem->hasChildNodes())
        {
            $children = $elem->childNodes;
            for($i = 0; $i < $children->length; $i++)
            {
                $child = $children->item($i);
                if ($child->nodeType == XML_ELEMENT_NODE)
                {
                    $elemValue =self::SortElement($child, $preText.'  ');
                    if (strlen($elemValue) > 0)
                    {
                        array_push($elemArray, $elemValue);
                    }
                }
                else if ($child->nodeType == XML_TEXT_NODE)
                {
                    $content = trim($child->textContent);
                    if (strlen($content) > 0)
                    {
                        array_push($elemArray,$content);
                    }
                }
            }
            sort($elemArray);
            foreach ($elemArray as $str)
            {
                $elemString .= $str;
            }
        }

        $endTag = '';
        if (sizeof($elemArray) > 1 && strlen($elemString) > 0)
        {
            $endTag = "\n".$preText;
        }
        $tagName = '';
        if (is_a($elem,'DOMDocument') == false)
        {
            $tagName = $elem->tagName;
        }
        $endTag .= "</".$tagName.">";

        if ("" != $tagName)
        {
            $elemString = "\n".$preText."<".$tagName.">".$elemString.$endTag;
        }

        return $elemString;
    }
}
?>
