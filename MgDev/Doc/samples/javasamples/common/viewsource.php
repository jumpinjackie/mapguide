<?php
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

	$sourceFile = ($_SERVER['REQUEST_METHOD'] == "POST")? $_POST['FILENAME']: $_GET['FILENAME'];
	
	if (DIRECTORY_SEPARATOR == '/')
		$sourceFile = str_replace('\\', '/', $sourceFile);
	else
		$sourceFile = str_replace('/', '\\', $sourceFile);
	
	$testForUtf = pack("ccc", 0xEF, 0xBB, 0xBF);
	$fileContents = file_get_contents($sourceFile);
	if (substr($fileContents, 0, 3) == $testForUtf)
		$fileContents = htmlentities(utf8_decode(substr($fileContents, 3)));
	else
		$fileContents = htmlentities($fileContents);
?>
<html>
	<head>
		<title>Source of <?= basename($sourceFile) ?></title>
	</head>
	
	<body>
		<p><strong><?= $sourceFile ?></strong></p>
		<pre style="background-color: #f8f8f8; padding: 10px; border: 2px solid black">
<code><?= $fileContents ?>
</code>
		</pre>
	</body>
</html>