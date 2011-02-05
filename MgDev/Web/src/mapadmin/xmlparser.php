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

    $currKey = "";

    // Below is a parser which is used for data which may contain Unicode chars.  It makes use of the mbstring functions built
    // into PHP.
    function getUnicodeValue( $tag, $text )
    {
        $valStart = mb_strpos( $text, '<'.$tag.'>' );
        if ( $valStart === FALSE )
            return "";
        $valStart += (strlen( $tag ) + 2);

        $valStop = mb_strpos( $text, '</'.$tag.'>', $valStart );
        if ( $valStop === FALSE )
            return "";

        $val = mb_substr( $text, $valStart, $valStop - $valStart );
        return $val;
    }

    function parseUnicodeData( $buffer, &$table, $recTag, $recParser )
    {
        global $currKey;

        // Initialize
        $table = array();

        // Parse
        $recStartTag = '<'.$recTag.'>';
        $recEndTag = '</'.$recTag.'>';
        $recEndTagLen = mb_strlen( $recEndTag );
        $recStart = mb_strpos( $buffer, $recStartTag );
        while ( $recStart !== FALSE )
        {
            $newRec = NULL;

            $recEndTagPos = mb_strpos( $buffer, $recEndTag, $recStart );
            if ( $recEndTagPos !== FALSE )
            {
                $recLen = ( $recEndTagPos + $recEndTagLen ) - $recStart;
                $newRec = $recParser( mb_substr( $buffer, $recStart, $recLen ) );
            }

            if ( !empty( $currKey ) && $newRec != NULL )
                $table[ $currKey ] = $newRec;

            $recStart = mb_strpos( $buffer, $recStartTag, $recStart + 1 );
        }
    }
?>
