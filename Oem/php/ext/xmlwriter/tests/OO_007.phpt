--TEST--
XMLWriter: libxml2 XML Writer, Elements & Attributes 
--SKIPIF--
<?php 
if (!extension_loaded("xmlwriter")) die("skip"); 
if (!function_exists("xmlwriter_start_attribute_ns")) die("skip: libxml2 2.6.17+ required");
?>
--FILE--
<?php 
/* $Id: OO_007.phpt,v 1.1.2.3 2005/12/12 21:21:11 tony2001 Exp $ */

$xw = new XMLWriter();
$xw->openMemory();
$xw->setIndent(TRUE);
$xw->setIndentString('   ');
$xw->startDocument('1.0', "UTF-8");
$xw->startElement('root');
$xw->startElementNS('ns1', 'child1', 'urn:ns1');
$xw->startAttributeNS('ns1', 'att1', 'urn:ns1');
$xw->text('a&b');
$xw->endAttribute();
$xw->writeAttribute('att2', "double\" single'");
$xw->startAttributeNS('ns1', 'att2', 'urn:ns1');
$xw->text("<>\"'&");
$xw->endAttribute();
$xw->writeElement('chars', "special characters: <>\"'&");
$xw->endElement();
$xw->endDocument();
// Force to write and empty the buffer
$output = $xw->flush(true);
print $output;
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<root>
   <ns1:child1 xmlns:ns1="urn:ns1" ns1:att1="a&amp;b" xmlns:ns1="urn:ns1" att2="double&quot; single'" ns1:att2="&lt;&gt;&quot;'&amp;" xmlns:ns1="urn:ns1">
      <chars>special characters: &lt;&gt;&quot;'&amp;</chars>
   </ns1:child1>
</root>
