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

// get current path
$curPath = __FILE__;
$pos = strrpos($curPath, '\\');
if ($pos == false)
{
    $pos = strrpos($curPath, '/');
}
$curPath = substr_replace($curPath, "", $pos+1);

//Note: current path might already be the script!!
MgInitializeWebTier("../../../Web/src/webconfig.ini");

$locale = $_GET['LOCALE'];

try
{
$site = new MgSiteConnection();
$cred = new MgUserInformation();
$cred->SetMgUsernamePassword("Administrator","admin");
$cred->SetLocale($locale);
$site->Open($cred);
$svc = $site->CreateService(5);
}
catch (MgException $exc)
{
    header("HTTP/1.1 559".$exc->GetExceptionMessage());
    $hdr = "Status: 559 ".$exc->GetExceptionMessage();
    header($hdr);
    echo "<html>\n";
    echo "<body>\n";
    echo $hdr."\n";
    echo $exc->GetExceptionMessage()."\n";
    echo $exc->GetDetails()."\n";
    echo "</body>\n";
    echo "</html>\n";
    return;
}
echo "<html>\n";
echo "<body>\n";
echo "Done.\n";
echo "</body>\n";
echo "</html>\n";
?>
