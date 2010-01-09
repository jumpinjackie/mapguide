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

    try
    {
        include '../mapadmin/constants.php';

        $locale = "en"; // localizable string
        $errorMsg = "";

        // Initialize web tier with the site configuration file.  The config
        // file should be in the directory above as this script.
        $webConfigFile = __FILE__;
        $pos = strrpos($webConfigFile, '\\');
        if ($pos == false)
            $pos = strrpos($webConfigFile, '/');

        $relativeLocation = '../'.MgConfigProperties::DefaultConfigurationFilename;
        $webConfigFile = substr_replace($webConfigFile, $relativeLocation, $pos+1);
        MgInitializeWebTier($webConfigFile);
    }
    catch ( MgException $e )
    {
        $errorMsg = $e->GetMessage();
        echo $errorMsg;
    }
    catch ( Exception $e )
    {
        $errorMsg = $e->getMessage();
        echo $errorMsg;
    }

?>
