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

?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
    <?php
        $sessionId = $_GET['sessionId'];
        $resName = $_GET['resId'];
        $schemaName = $_GET['schemaName'];
        $className = $_GET['className'];
        $viewer = $_GET['viewer'];

        if(!$viewer)
        {
            $viewer = 'flexible';
        }

        echo '<frameset cols="45%,*" bordercolor=#a0a0a0 frameborder=1 framespacing=2>';
        echo '<frame name="srcFrame" src="displayschema.php?resId=' . $resName . '&sessionId=' . $sessionId . '&schemaName=' . $schemaName . '&className=' . $className . '&viewer=' . $viewer .'">';
        echo '<frame name="viewFrame" src="blank.php">';
        echo '</frameset>';
    ?>
</html>

