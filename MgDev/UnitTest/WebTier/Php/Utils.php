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

//Class that defines static methods that are commonly used in the infrastructure

require_once("SQLiteEngine.php");

class Utils
{
    public static function SetCommonParams($paramSet, $db)
    {
        $vm = new SqliteVM($db);
        try
        {
            $arrayParam = array();
            $vm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"OPERATION\"");
            $arrayParam["OPERATION"]=$vm->GetString("ParamValue");

            $vm->Execute("Select ParamValue from CommonParams WHERE ParamName=\"VERSION\"");
            $arrayParam["VERSION"]=$vm->GetString("ParamValue");

            $vm->Execute("Select ParamValue from CommonParams WHERE ParamName=\"CREDENTIALS\"");
            $arrayParam["CREDENTIALS"]=$vm->GetString("ParamValue");

            $vm->Execute("Select ParamValue from CommonParams WHERE ParamName=\"LOCALE\"");
            $arrayParam["LOCALE"]=$vm->GetString("ParamValue");

            unset($vm);
            return $arrayParam;
        }

        catch (MgException $e)
        {
            print ($e->GetExceptionMessage("en"));
        }
        catch (SqliteException $s)
        {
            try
            {
                unset($vm);
                $vm = new SqliteVM($db, true);
                $vm->Execute("Select ParamValue from Params WHERE ParamName=\"VERSION\"");
                $arrayParam["VERSION"]=$vm->GetString("ParamValue");

                $vm->Execute("Select ParamValue from Params WHERE ParamName=\"CREDENTIALS\"");
                $arrayParam["CREDENTIALS"]=$vm->GetString("ParamValue");

                $vm->Execute("Select ParamValue from Params WHERE ParamName=\"LOCALE\"");
                $arrayParam["LOCALE"]=$vm->GetString("ParamValue");
                return $arrayParam;
            }
            catch (SqliteException $sq)
            {
                print ($sq->GetExceptionMessage("eng"));
            }
        }
    }

    public static function IsWindows() {
        return strtoupper(substr(PHP_OS, 0, 3)) === 'WIN';
    }

    private static function GetAbsolutePath($path, $encoding = "UTF-8") {
        // Attempt to avoid path encoding problems.
        $path = iconv($encoding, "$encoding//IGNORE//TRANSLIT", $path);
 
        // Process the components
        $parts = explode('/', $path);
        $safe = array();  
        foreach ($parts as $idx => $part) {
    
            if (($idx > 0 && trim($part)=="") || $part=='.') {
                continue;
            } elseif ('..' == $part) {
                array_pop($safe);
                continue;
            } else {
                $safe[] = $part;
            }
        }
  
        // Return the "clean" path
        $path = implode(DIRECTORY_SEPARATOR, $safe);
        return $path;
    }

    public static function GetPath($fileName)
    {
        if (strpos($fileName, "/") === 0)
        {
            //echo "Utils::GetPath($fileName) - $fileName\n";
            return self::GetAbsolutePath($fileName);
        }
        $pos = strrpos($fileName, ':');
        if($pos)
        {
            //echo "Utils::GetPath($fileName) - $fileName\n";
            return $fileName;
        }

        $relPath = __FILE__;
        $pos = strrpos($relPath, '\\');

        if ($pos == false)
        {
            $pos = strrpos($relPath, '/');
        }
        $relPath = substr_replace($relPath, $fileName, $pos+1);

        //Replace the back slashes in the path with forward slashes for Linux compatibility
        $relPath = str_replace("\\", "/", $relPath);
        $relPath = self::GetAbsolutePath($relPath);
        //echo "Utils::GetPath($fileName) - $relPath\n";
        return $relPath;
    }

    public static function MapAgentInit($webConfigIniPath)
    {
        $cfgFile = self::GetPath($webConfigIniPath);
        //echo "Initializing from $cfgFile\n\n";
        MgInitializeWebTier($cfgFile);
    }

    public static function GetResponse($byteReader)
    {
        try
        {
            $result = new Result();
            $responseContent="";
            $content = "";

            if($byteReader != null)
            {
                $result->SetContentType($byteReader->GetMimeType());
                while( ($byteReader->Read($content, 1024)) > 0)
                {
                    $responseContent .=$content;
                }
                $result->SetResultData($responseContent);
            }
            return $result;
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    //Takes a comma separated string and returns an object of MgStringCollection
    public static function StringToMgStringCollection($string)
    {
        $mgCollection = new MgStringCollection();
        $stringArray = explode(",", $string);

        for ($i=0; $i<count($stringArray); $i++)
        {
            $mgCollection->Add($stringArray[$i]);
        }
        return $mgCollection;
    }

    //Takes a MgStringCollection objects and returns a comma separated string
    public static function MgStringCollectionToString($mgCollection)
    {
        try
        {
            $string="";
            $count = $mgCollection->GetCount();
            $items = array();
            for ($i=0; $i<$count; $i++)
            {
                array_push($items, $mgCollection->GetItem($i));
            }
            sort($items);

            for ($i=0; $i<$count; $i++)
            {
                $string .= $items[$i].",";
            }

            return $string;
        }
        catch (MgException $e)
        {
            return $e->GetExceptionMessage();
        }
    }

    //Takes a boolean value and returns its string representation
    public static function BooleanToString($bval)
    {
        return $bval? "True": "False";
    }

    //Takes a MgEnvelope object and returns its string representation
    public static function MgEnvelopeToString($env)
    {
        try
        {
            $ll = $env->GetLowerLeftCoordinate();
            $ur = $env->GetUpperRightCoordinate();

            return "(" . $ll->GetX() . ":" . $ll->GetY() . ")-(" . $ur->GetX() . ":" . $ur->GetY() . ")";
        }
        catch (MgException $e)
        {
            return $e->GetExceptionMessage();
        }

    }

    //Takes a MgPoint object and returns its string representation
    public static function MgPointToString($env)
    {
        try
        {
            $coordinate = $env->GetCoordinate();
            return "(" . $coordinate->GetX() . ":" . $coordinate->GetY() . ")";
        }
        catch (MgException $e)
        {
            return $e->GetExceptionMessage();
        }


    }

    public static function GetDbPath($dumpFileName)
    {
        $db = new SqliteDB();
        $dbPath = substr($dumpFileName, 0, strpos($dumpFileName, ".dump")).".db";
        $dbName = Utils::GetPath($dbPath);
        $dumpFileName = Utils::GetPath($dumpFileName);

        //Clear the stat cache as filemtime may not work correctly
        clearstatcache();
        //If we do not have neither a dump file nor a database file we are stuck.
        if ( (!file_exists($dumpFileName)) && (!file_exists($dbName)))
        {
            printf("<b>Error: Dump file <i>%s</i> not found. Unable to create database file</b><br />", $dumpFileName);
        }
        //If the database file is missing but we have a dump file then create a new database
        elseif (!file_exists($dbName))
        {
            $db->GenerateDatabase($dumpFileName, $dbName);
        }
        //If we have both files but the database file has an older time stamp than the dump file then create a new database
        elseif ((file_exists($dumpFileName) && filemtime($dumpFileName)>filemtime($dbName)))
        {
            //Try to delete the database and if you cannot than do not create a new database as this will result in duplicate records
            if (@unlink($dbName))
            {
                $db->GenerateDatabase($dumpFileName, $dbName);
            }
            else
            {
                printf("<b>Unable to delete database file <i>%s</i>. The file is either in use or is read only.</b><br />", $dbName);
                print("<b>The database has not been updated!</b><br />");
            }
        }

        return $dbPath;
    }
    
    public static function CreateDumpFile($db, $overwrite = false)
    {
        $dbName = $db->GetName();
        $iniFileName = substr($dbName, 0, strpos($dbName, ".db")).".ini";
        $dumpFileName = substr($dbName, 0, strpos($dbName, ".db")).".dump";

        if ($overwrite)
        {
            if (file_exists($dumpFileName))
                unlink($dumpFileName);
            
            $iniFileContent =".output ".$dumpFileName;
            file_put_contents($iniFileName, $iniFileContent);
            $db->DumpDatabase($iniFileName);
            if (php_sapi_name() == 'cli')
                printf("Updated dump file: %s\n", $dumpFileName);
            else
                printf("<b>Updated dump file <i>%s</i></b>", $dumpFileName);
            unlink($iniFileName);
        }
        else
        {
            //Clear the stat cache as filemtime may not work correctly
            clearstatcache();

            //Check if the file exists and is writable before updating it
            if (!file_exists($dumpFileName) || (is_writable($dumpFileName)&&filemtime($db->GetName())>filemtime($dumpFileName)))
            {
                $iniFileContent =".output ".$dumpFileName;
                file_put_contents($iniFileName, $iniFileContent);
                $db->DumpDatabase($iniFileName);
                if (php_sapi_name() == 'cli')
                    printf("Updated dump file: %s\n", $dumpFileName);
                else
                    printf("<b>Updated dump file <i>%s</i></b>", $dumpFileName);
                unlink($iniFileName);
            }
            else
            {
                if (php_sapi_name() == 'cli')
                    printf("Dump file %s already exists and is read only or is newer than the database. File has not been updated\n", $dumpFileName);
                else
                    printf("<b>Dump file <i>%s</i> already exists and is read only or is newer than the database. File has not been updated</b>", $dumpFileName);
            }
        }
    }
}
?>
