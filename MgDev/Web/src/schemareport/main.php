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

    include '../mapadmin/constants.php';
    include 'stringconstants.php';
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>

    <head>
        <title><?php echo HtmlTitles::DisplaySchema ?></title>
        <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
        <link href="displayschema.css" rel="stylesheet" type="text/css">
    </head>

    <body>

        <?php

            $username = "Administrator";
            $password = "admin";

            try
            {
                $thisFile = __FILE__;
                $pos = strrpos($thisFile, '\\');
                if ($pos == false)
                    $pos = strrpos($thisFile, '/');
                $configFilePath = substr($thisFile, 0, $pos+1) . "../webconfig.ini";
                MgInitializeWebTier ($configFilePath);

                $userInfo = new MgUserInformation($username, $password);
                $site = new MgSiteConnection();
                $site->Open($userInfo);
                $sessionId = $site->GetSite()->CreateSession();
            }
            catch (MgException $e)
            {
                echo $e->GetExceptionMessage();
            }
        ?>

        <h1><?php echo DisplayHeadings::MainHeading ?></h1>

        <form action="describeschema.php" method="get">
            <?php echo FormText::InputResourceId ?>
            <input name="resId" value="Library://Samples/Sheboygan/Data/RoadCenterLines.FeatureSource" type="text" size="70"><br><br>
            <?php echo FormText::InputSchema ?>
            <input name="schemaName" type="text" size="20"><br><br>
            <?php echo FormText::InputClass ?>
            <input name="className" type="text" size="20"><br><br>
            <input name="sessionId" type="hidden" value="<?php echo $sessionId ?>">

            <input type="submit" value="<?php echo FormText::FindButton ?>">
        </form>

    </body>

</html>
