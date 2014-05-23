<?php

/*
 index.php
 
 This is a php-based shim to the mapagent. It uses the MgHttpRequest/MgHttpResponse APIs to replicate
 most of the functionality of the mapagent. The purpose of this script is to provide a simple mapagent
 http endpoint (in conjunction with PHP's built-in web server) in which to run the PHP-based mapagent
 test suite against
 
 Usage:
 
 php <various options> -S servername:port -t path/to/this/file.php
 
 The mapagent can then be accessed via:
 
 http://servername:port/mapguide/mapagent/mapagent.fcgi

 */

require_once dirname(__FILE__)."/../../../../Web/src/mapadmin/constants.php";

function EscapeXmlChars($str) {
    $newStr = "";
    $len = strlen($str);

    for($i = 0; $i < $len; $i++)
    {
        switch($str[$i])
        {
            case '&' :
            {
                $newStr .= "&amp;";
                break;
            }
            case '\'' :
            {
                $newStr .= "&apos;";
                break;
            }
            case '>' :
            {
                $newStr .= "&gt;";
                break;
            }
            case '<' :
            {
                $newStr .= "&lt;";
                break;
            }
            case '"' :
            {
                $newStr .= "&quot;";
                break;
            }
            default :
                $newStr .= $str[$i];
        }
    }
    return $newStr;
}

function Unauthorized() {
    //Send back 401
    header('WWW-Authenticate: Basic realm="MapGuide"');
    $e = new Exception();
    echo "You must enter a valid login ID and password to access this site\r\n";
    http_response_code(401);
    die;
}

function CheckParams($param) {
    $bValid = $param->ContainsParameter("SESSION");
    if (!$bValid)
        $bValid = $param->ContainsParameter("USERNAME");
    
    $op = $param->GetParameterValue("OPERATION");
    if ($op == "GETSITESTATUS")
        $bValid = true;

    //Invalid service parameter needs to trigger unauthorized
    if ($bValid) {
        if ($param->ContainsParameter("SERVICE")) {
            $svc = $param->GetParameterValue("SERVICE");
            $bValid = ($svc == "WFS" || $svc == "WMS");
        }
    }

    if (!$bValid)
        Unauthorized();
}

function OutputException($statusMessage, $errorMessage, $details, $phpTrace, $status = 500, $mimeType = MgMimeType::Html) {
    $errResponse = "";
    $errResponse = sprintf(
            "\r\n<html>\n<head>\n<title>%s</title>\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n</head>\n<body>\n<h2>%s</h2>\n%s\n</body>\n</html>\n",
            $statusMessage,
            $errorMessage,
            $details,
            $phpTrace);
    header($_SERVER["SERVER_PROTOCOL"]." 559 $statusMessage"); 
    header("Status: 559 $statusMessage");
    header($statusMessage, true, 559);
    echo $errResponse;
}

function OutputError($result, $mimeType = MgMimeType::Html) {
    $statusMessage = $result->GetHttpStatusMessage();
    $e = new Exception();
    if ($statusMessage === "MgAuthenticationFailedException" || $statusMessage === "MgUnauthorizedAccessException" || $statusMessage == "MgPermissionDeniedException") {
        Unauthorized();
    } else {
        header("Content-Type: $mimeType");
        OutputException($statusMessage, $result->GetErrorMessage(), $result->GetDetailedErrorMessage(), $e->getTraceAsString(), $result->GetStatusCode(), $mimeType);
    }
}

function OutputMgStringCollection($strCol, $mimeType = MgMimeType::Xml) {
    $content = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><StringCollection />";
    if ($strCol != null) {
        // MgStringCollection::ToXml() doesn't seem to be reliable in PHP (bug?), so do this manually
        $count = $strCol->GetCount();
        $content = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><StringCollection>";
        for ($i = 0; $i < $count; $i++) {
            $value = EscapeXmlChars($strCol->GetItem($i));
            $content .= "<Item>$value</Item>";
        }
        $content .= "</StringCollection>";
    }
    header("Content-Type: $mimeType");
    echo $content;
}

function OutputByteReader($byteReader, $chunkResult = false, $bPrependXmlProlog = false) {
    $mimeType = $byteReader->GetMimeType();
    header("Content-Type: $mimeType");
    $rdrLen = $byteReader->GetLength();
    if ($mimeType == MgMimeType::Xml && $bPrependXmlProlog) {
        echo "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
    }
    do
    {
        $data = str_pad("\0", 50000, "\0");
        $len = $byteReader->Read($data, 50000);
        if ($len > 0)
        {
            $str = substr($data, 0, $len);
            echo $str;
        }
    } while ($len > 0);
}

function TryFillCredentials($param) {
    if (array_key_exists("SESSION", $_REQUEST)) {
        $param->AddParameter("SESSION", $_REQUEST["SESSION"]);
    } else {
        $username = null;
        $password = "";
        // Username/password extraction logic ripped from PHP implementation of the MapGuide AJAX viewer
        
        // No session, no credentials explicitely passed. Check for HTTP Auth user/passwd.  Under Apache CGI, the
        // PHP_AUTH_USER and PHP_AUTH_PW are not set.  However, the Apache admin may
        // have rewritten the authentication information to REMOTE_USER.  This is a
        // suggested approach from the Php.net website.

        // Has REMOTE_USER been rewritten?
        if (!isset($_SERVER['PHP_AUTH_USER']) && isset($_SERVER['REMOTE_USER']) &&
        preg_match('/Basic +(.*)$/i', $_SERVER['REMOTE_USER'], $matches))
        {
            list($name, $password) = explode(':', base64_decode($matches[1]));
            $_SERVER['PHP_AUTH_USER'] = strip_tags($name);
            $_SERVER['PHP_AUTH_PW']    = strip_tags($password);
        }


        // REMOTE_USER may also appear as REDIRECT_REMOTE_USER depending on CGI setup.
        //  Check for this as well.
        if (!isset($_SERVER['PHP_AUTH_USER']) && isset($_SERVER['REDIRECT_REMOTE_USER']) &&
        preg_match('/Basic (.*)$/i', $_SERVER['REDIRECT_REMOTE_USER'], $matches))
        {
            list($name, $password) = explode(':', base64_decode($matches[1]));
            $_SERVER['PHP_AUTH_USER'] = strip_tags($name);
            $_SERVER['PHP_AUTH_PW'] = strip_tags($password);
        }

        // Finally, PHP_AUTH_USER may actually be defined correctly.  If it is set, or
        // has been pulled from REMOTE_USER rewriting then set our USERNAME and PASSWORD
        // parameters.
        if (isset($_SERVER['PHP_AUTH_USER']) && strlen($_SERVER['PHP_AUTH_USER']) > 0)
        {
            $username = $_SERVER['PHP_AUTH_USER'];
            if (isset($_SERVER['PHP_AUTH_PW']) && strlen($_SERVER['PHP_AUTH_PW']) > 0)
                $password = $_SERVER['PHP_AUTH_PW'];
        }

        //If we have everything we need, put it into the MgHttpRequestParam
        if ($username != null) {
            $param->AddParameter("USERNAME", $username);
            if ($password !== "") {
                $param->AddParameter("PASSWORD", $password);
            }
        } else {
            Unauthorized();
        }
    }
}

$iniFile = dirname(__FILE__)."/../../../webconfig.ini";
try {
    if (array_key_exists("WEBCONFIGINI", $_SERVER)) {
        $iniFile = $_SERVER["WEBCONFIGINI"];
    }
    if (array_key_exists("WEBCONFIGINI", $_ENV)) {
        $iniFile = $_ENV["WEBCONFIGINI"];
    }
    MgInitializeWebTier($iniFile);
    $method = $_SERVER["REQUEST_METHOD"];
    if (strpos($_SERVER["REQUEST_URI"], '/mapguide/mapagent/mapagent.fcgi') !== FALSE) {
        if ($method == "GET" || $method == "POST") {
            $uri = "http://".$_SERVER["SERVER_NAME"].$_SERVER["SERVER_PORT"].$_SERVER["REQUEST_URI"];
            $req = new MgHttpRequest($uri);
            $param = $req->GetRequestParam();
            
            TryFillCredentials($param);
            
            if ($method == "GET") {
                foreach ($_GET as $key => $value) {
                    $param->AddParameter($key, $value);
                }
            } else { //POST
                foreach ($_POST as $key => $value) {
                    $param->AddParameter($key, $value);
                }
                foreach ($_FILES as $key => $value) {
                    if (array_key_exists("error", $value) && $value["error"] == 1) {
                        echo "Error: The file ".$value["name"]." for field $key exceeds the maximum file upload size of ".ini_get("upload_max_filesize");
                        die;
                    }
                    $param->AddParameter($key, $value["tmp_name"]);
                    $param->SetParameterType($key, "tempfile");
                }
            }
            
            CheckParams($param);
            
            $response = $req->Execute();
            $result = $response->GetResult();

            $status = $result->GetStatusCode();
            if ($status == 200) {
                $resultObj = $result->GetResultObject();
                if ($resultObj != null) {
                    header("Content-Type: ".$result->GetResultContentType());
                    if ($resultObj instanceof MgByteReader) {
                        OutputByteReader($resultObj);
                    } else if ($resultObj instanceof MgStringCollection) {
                        //OutputMgStringCollection($resultObj, $param->GetParameterValue("FORMAT"));
                        $byteReader = $resultObj->ToXml();
                        OutputByteReader($byteReader);
                    } else if ($resultObj instanceof MgHttpPrimitiveValue) {
                        echo $resultObj->ToString();
                    } else if (method_exists($resultObj, "ToXml")) {
                        $byteReader = $resultObj->ToXml();
                        OutputByteReader($byteReader);
                    } else {
                        throw new Exception("Could not determine how to output: ".$resultObj->ToString()); //TODO: Localize
                    }
                }
            } else {
                $format = $param->GetParameterValue("FORMAT");
                if ($format != "") {
                    OutputError($result, $format);
                } else {
                    OutputError($result);
                }
                //throw new Exception("Error executing operation: ".$param->GetParameterValue("OPERATION").". The status code is: $status"); //TODO: Localize
            }
            http_response_code($status);
            
        } else {
            echo "Unsupported method";
            http_response_code(405);
        }
    } else {
        return false;
    }
}
catch (MgException $ex) {
    OutputException(get_class($ex), $ex->GetExceptionMessage(), $ex->GetDetails(), $e->getTraceAsString(), 559);
    die;
}

?>