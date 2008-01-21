<?php
/**
 * Session
 *
 * $Id: Session.php 963 2007-10-16 15:37:30Z madair $
 *
 * Copyright (c) 2007, DM Solutions Group Inc.
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/*****************************************************************************
 * Purpose: Directory-based PHP session capability
 *****************************************************************************
 *
 * This code was taken from the php_utils cvs module maintained on 
 * cvs.maptools.org/cvs/maptools/cvsroot. Version 1.1 of this file was an 
 * exact copy of 1.29 except for the comment block at the top.
 *
 * Developer's notes:
 *
 * Include this file and call installSessionDirectoryHandler() before starting
 * a session.
 *
 * Call initializeSession() to start or restart a session.
 *
 * Session-based resources can be saved in the directory returned from
 * getSessionSavePath().
 *
 * Setting $GLOBALS['bDebug'] = true will enable outputting some debug
 * messages into the session.log file in the debug directory defined
 * by $GLOBALS['szDebugDir']
 *
 */

global $gszSessSavePath;
global $gszSessName;

$GLOBALS['bDebug'] = false;
$GLOBALS['szDebugPage'] = isset( $_SERVER['ORIG_PATH_TRANSLATED'] ) ? basename( $_SERVER['ORIG_PATH_TRANSLATED'] ) : 'not set';
$GLOBALS['szDebugDir'] = "/tmp/";

/* file-based session locking based on code contributed by
 * Andreas Hocevar ahocevar@hotmail.com
 */
if (!isset($GLOBALS['bLockSession'])) {
    $GLOBALS['bLockSession'] = false;
}

/**
 * _open Called by PHP session manager when a session is opened.
 * We just set the save path and session name into global variable.
 *
 * @param szSavePath String containing the absolute path where to
 *        save session info.
 * @param szSessionName String contqaining the session name.
 */
function _open($szSavePath, $szSessionName)
{
    if ($GLOBALS["bDebug"])
    {
      debug_msg("Opened() $szSavePath, $szSessionName");
      debug_msg("session_id() says ".session_id());
    }

    $GLOBALS['gszSessSavePath'] = $szSavePath;
    $GLOBALS['gszSessName'] = $szSessionName;
    $GLOBALS['gszSessId'] = session_id();

    $szSessionDir = $GLOBALS['gszSessSavePath']."/sess_".$GLOBALS['gszSessId'];
    $szSessionFile = $szSessionDir."/session_file";
    $szLockFile = $szSessionDir."/lock";

    clearstatcache();
    if (!file_exists($szSessionDir))
    {
        mkdir($szSessionDir);
    }

    if ( PHP_OS == "WINNT" || PHP_OS == "WIN32" || $GLOBALS['bLockSession'])
    {
        $i=0;
        while ($i < ini_get("max_execution_time") - 2 ) //wait at most 2 seconds less than the execution time for the lock
        {
            clearstatcache();
            if (!file_exists($szLockFile))
            {
                break;
            }
            if ($GLOBALS["bDebug"])
            {
                debug_msg("pausing in open()");
            }
            $i++;
            sleep(1);
        }
        $fh = fopen( $szLockFile, "w+" );
        if ($fh !== false)
        {
            fwrite( $fh, "1" );
            fclose($fh);
            if ($GLOBALS["bDebug"])
            {
                debug_msg("created lock file in open()");
            }
        }
    }

    return(true);
}

/**
 * _close Called by PHP session manager when a session is closed,
 * not destroyed. In this case we do nothing.
 */
function _close()
{
    if ($GLOBALS["bDebug"])
    {
      debug_msg("Closed()");
    }
    if ( PHP_OS == "WINNT" || PHP_OS == "WIN32" || $GLOBALS['bLockSession'])
    {
        $szLockFile = $GLOBALS['gszSessSavePath']."/sess_".$GLOBALS["gszSessId"]."/lock";
        if ($GLOBALS["bDebug"])
        {
            debug_msg("checking lock file $szLockFile");
        }
        clearstatcache();
        if ( @is_file( $szLockFile )  )
        {
            @unlink( $szLockFile );
            if ($GLOBALS["bDebug"])
            {
                debug_msg("removed lock file $szLockFile");
            }
        }
        else
        {
            if ($GLOBALS["bDebug"])
            {
                debug_msg("lock file $szLockFile is missing.");
            }

        }
    }
    return(true);
}

/**
 * _read Called by PHP session manager when the session file
 * is read. In this case we just return the file content of
 * session_file file.
 */
function _read($szId)
{
    $GLOBALS["gszSessId"] = $szId;

    if ($GLOBALS["bDebug"])
    {
      debug_msg("Read() $szId");
    }

    $szSessionDir = $GLOBALS['gszSessSavePath']."/sess_".$szId;
    $szSessionFile = $szSessionDir."/session_file";

    clearstatcache();
    if (!file_exists($szSessionDir))
    {
        mkdir($szSessionDir);
    }

    if ($fp = @fopen($szSessionFile, "r"))
    {
        $szSessionData = fread($fp, filesize($szSessionFile));
        fclose( $fp );
        return($szSessionData);
    }
    else
    {
        return(""); // Must return "" here.
    }
}

/**
 * _write Called by PHP session manager when session should be
 * saved.
 *
 * @param szId String containing the unique identifier of current
 *             session.
 * @param szSessionData String containig the session file content
 *                      to be saved.
 */
function _write($szId, $szSessionData)
{
    $result = false;
    if ($GLOBALS["bDebug"])
    {
      debug_msg("Write() $szId $szSessionData");
    }

    $szSessionFile = $GLOBALS['gszSessSavePath']."/sess_".$szId.
                     "/session_file";

    // touch command don't works under windows for directory.
    // since it is only needed for unix (for now) I'll test
    // the platform and bypass this call is windows platform.
    if ( !(PHP_OS == "WINNT" || PHP_OS == "WIN32") )
        @touch($GLOBALS['gszSessSavePath']."/sess_".$szId);

    if ($fp = @fopen($szSessionFile, "w"))
    {
        $result = fwrite($fp, $szSessionData);
        fclose($fp);

    }
    return($result);
}

/**
 * _destroy Called by PHP session manager when it should be explicitly
 * destroyed now.
 */
function _destroy($szId)
{
    if ($GLOBALS["bDebug"])
    {
      debug_msg("Destroy $szId");
    }
    if ( (PHP_OS == "WINNT" || PHP_OS == "WIN32"|| $GLOBALS['bLockSession']) )
    {
        $szLockFile = $GLOBALS['gszSessSavePath']."/sess_".$GLOBALS["gszSessId"]."/lock";

        if ( @is_file( $szLockFile )  )
        {
            @unlink( $szLockFile );
        }
    }
  /*
    if ($GLOBALS['gszGarbageColectionCallBackFunction'] != "")
    {
        if (function_exists($GLOBALS['gszGarbageColectionCallBackFunction']))
            eval($GLOBALS['gszGarbageColectionCallBackFunction']);
    }

    $bReturn = true;

    if (!$szDir = @opendir($GLOBALS['gszSessSavePath']))
    {
        return false;
    }

    while($szFile = readdir($szDir))
    {
        if (!strstr($szFile,'sess_'.$szId))
            continue;

        $bReturn = (deleteDirectory($GLOBALS['gszSessSavePath']."/".
                                    $szFile)) ? $bReturn : false;
    }
    closedir($szDir);

    return $bReturn;
  */
  return true;
}

/**
 * _gc Called by PHP session manager when a session is started or
 * register (not all the time) depending og session.gc_probability
 */
function _gc($nMaxLifeTime)
{
   if ($GLOBALS["bDebug"])
    {
      debug_msg("_gc called");
    }
    if ($GLOBALS['gszGarbageColectionCallBackFunction'] != "")
    {
        if (function_exists($GLOBALS['gszGarbageColectionCallBackFunction']))
            eval($GLOBALS['gszGarbageColectionCallBackFunction']);
    }

    if ( !(PHP_OS == "WINNT" || PHP_OS == "WIN32") )
    {
      @touch($GLOBALS['gszSessSavePath']."/deleteme", time()-$nMaxLifeTime*60);
      system("find ".$GLOBALS['gszSessSavePath']." -name sess_* ! -newer ".
             $GLOBALS['gszSessSavePath']."/deleteme -exec rm -rf {} \; ");

      return true;
    }

    $bReturn = true;

    if (!$hDir = @opendir($GLOBALS['gszSessSavePath']))
    {
        return false;
    }

    while($szFile = readdir($hDir))
    {
        if (!strstr($szFile,'sess_'))
            continue;

        if (strpos($szFile,'sess_') != 0)
            continue;

        $szSessionDir = $GLOBALS['gszSessSavePath']."/".$szFile;
        $szSessionFile = $szSessionDir."/session_file";

        if (!($mtime = @filemtime($szSessionFile)))
        {
            $bReturn=false;
            continue;
        }

        if (time() > $mtime + $nMaxLifeTime)
        {
            $bReturn = (deleteDirectory($szSessionDir)) ? $bReturn : false;
        }
        closedir($hDir);

        return $bReturn;
    }
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

function deleteDirectory($szFile)
{
    if (PHP_OS != "WINNT" && PHP_OS != "WIN32")
        chmod($szFile,0777);

    if (is_dir($szFile))
    {
        $handle = opendir($szFile);
        while($szFileName = readdir($handle))
        {
            if ($szFileName != "." && $szFileName != "..")
            {
                deleteDirectory($szFile."/".$szFileName);
            }
        }
        closedir($handle);
        rmdir($szFile);
    }
    else
    {
        unlink($szFile);
    }
}

function installSessionDirectoryHandler($szGCCallBack="")
{
    $GLOBALS['gszGarbageColectionCallBackFunction'] = $szGCCallBack;

    // Set handler functions
    session_set_save_handler("_open",
                             "_close",
                             "_read",
                             "_write",
                             "_destroy",
                             "_gc");
}

function initializeSession( $szSessName="sid", $szSessSavePath="", $szSessionID="" )
{
    
    if ($GLOBALS["bDebug"])
    {
      debug_msg("initializeSession( $szSessName, $szSessSavePath, $szSessionID )");
    }

    
    //if session was run already don't execute again
    if (isset($GLOBALS['session_started']))
    {
        return true;   
    }
    if ($szSessName == "")
    {
        echo "<FONT color=#FF0000>FATAL ERROR: Sessionname not specified</FONT>";
        exit;
    }
    else
        ini_set("session.name", $szSessName);

    if ($szSessSavePath != "")
    {
        ini_set("session.save_path", $szSessSavePath);
    }

    clearstatcache();
    // Check if save path is writable
    if (!(file_exists(ini_get("session.save_path")) &&
          is_writable(ini_get("session.save_path"))))
    {
        echo "<FONT COLOR=#DD0000>FATAL ERROR: Session save path (".ini_get("session.save_path").") doesn't exist or is not writable</FONT>";
        exit;
    }

    //turn off cookies for propagating session ids
    ini_set( "session.use_cookies", "0" );

    // turn off tranparent SID (becuase of buffer problem)
    ini_set( "session.use_trans_sid", "0" );

    // intialize tmp id
    $szTmpID = "";

    // check both get and post variables
    if ( isset($GLOBALS['_GET'][ini_get('session.name')]) )
        $szTmpID = $GLOBALS['_GET'][ini_get('session.name')];
    elseif (isset($GLOBALS['_POST'][ini_get('session.name')]))
        $szTmpID = $GLOBALS['_POST'][ini_get('session.name')];

    // create new if necessary
    if ( strlen( $szTmpID ) <= 0 )
    {
        if ($GLOBALS["bDebug"])
        {
            debug_msg("creating a new session because .$szTmpID. has zero characters ");
        }
        // create new and set IP flag
        if ( strlen( $szSessionID ) > 0 )
        {
            $szTmpID = $szSessionID;
        }
        else            
        {
            $szTmpID = uniqid("");
        }
        $bNewSession = true;
        if ($GLOBALS["bDebug"])
        {
            debug_msg("creating a new session with id ");
        }
   }
    else
        $bNewSession = false;

    // initialize flag variable
    $bSessionOK = true;

    // set the session ID
    session_id( $szTmpID );

    // Check if session is expired
    if (!$bNewSession)
    {
        $szSavePath = getSessionSavePath();
        $szSessionFile = $szSavePath."/session_file";

        if (file_exists($szSessionFile))
            if ($atime=@filemtime($szSessionFile))
                if (time() > $atime + ini_get("session.gc_maxlifetime"))
                {
                    $szTmpID = uniqid("");

                    // reset the session ID
                    session_id( $szTmpID );

                    $bNewSession = true;
                    $bSessionOK = false;
                }
    }

    //start the session
    session_start();
    register_shutdown_function( "session_write_close" );

  
    // set IP if a new session
    if ( $bNewSession ) $_SESSION["gszRemoteAdd"] = $_SERVER["REMOTE_ADDR"];

/* ============================================================================
 * Check IP to see if it is the same
 * ========================================================================= */

    // check if the IP has been set and validate
    if ( isset( $_SESSION["gszRemoteAdd"] ) &&
                                   strlen(trim($_SESSION["gszRemoteAdd"])) > 0 )
    {
        // check if IP matches current client
        if ( trim( $_SESSION["gszRemoteAdd"] ) !=
                                               trim( $_SERVER["REMOTE_ADDR"] ) )
        {
            // possible security breach void session
            /* if the session address is the loopback interface then it is
             * likely that the application was configured to use an external
             * address but someone is trying to test locally using localhost
             */
            if ($_SESSION['gszRemoteAdd'] != '127.0.0.1')
            {
                $bSessionOK = false;
            }
        }
    }
    else
    {
        // possible security breach void session
        $bSessionOK = false;
    }
    
    // return success or failure and set global so we
    // know session has been inited.
    if ($bSessionOK)
    {
        $GLOBALS['session_started'] = true;
    }
    
        
    return $bSessionOK;

// end intializeSession() function
}

function getSessionSavePath()
{
    $szReturn  = ini_get("session.save_path")."/sess_".session_id()."/";
    $szReturn = str_replace( "\\", "/", $szReturn );
    return $szReturn;
}

function debug_msg( $szMsg )
{
    list($usec, $sec) = explode(" ",microtime()); 
    $ts = sprintf( "%s.%4d", date( "H:s", $sec), round( 10000 * $usec )); 
    $fh = fopen($GLOBALS['szDebugDir']."session.log", "a+");
    fwrite($fh, "$ts : ".$GLOBALS['szDebugPage']." : $szMsg\n");
    fclose($fh);
}
?>
