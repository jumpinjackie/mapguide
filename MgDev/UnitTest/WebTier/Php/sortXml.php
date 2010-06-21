<?php

//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

function SortElement($elem, $preText)
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
                $elemValue = SortElement($child, $preText.'  ');
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

    if ($tagName != "")
    {
        $elemString = "\n".$preText."<".$tagName.">".$elemString.$endTag;
    }

    return $elemString;
}

$doc = new DOMDocument();
$doc->load('D:\MgDev\UnitTest\TestData\Unicode\Sample_World_Unicode.FeatureSource');
$doc->preserveWhiteSpace = false;
$sorted = SortElement($doc, '');
//$sorted = $doc->saveXML();
print $sorted;

?>
