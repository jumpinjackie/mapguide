<?php
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
