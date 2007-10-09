<?php
/*****************************************************************************
 *
 * $Id: CreateSession.php 837 2007-09-19 20:32:05Z pspencer $
 *
 * Purpose: initialize a server-side session for GMap and return to the client
 *
 * Project: MapGuide Open Source GMap demo application
 *
 * Author: DM Solutions Group Inc
 *
 * Copyright (c) 2007 DM Solutions Group Inc.
 *****************************************************************************
 * This code shall not be copied or used without the expressed written consent
 * of DM Solutions Group Inc.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 *****************************************************************************/
include('Common.php');
include('../../common/php/Utilities.php');

try {
    $site = $siteConnection->GetSite();
    $sessionId =  $site->CreateSession();
    $user->SetMgSessionId($sessionId);

    header('Content-type: text/x-json');
    header('X-JSON: true');
    $result = null;
    $result->sessionId = $sessionId;
    $result->userName = $username;
    echo var2json($result);
    
    /* start a php session in the web tier as well, using same session id */
    session_start($sessionId);
    $_SESSION['username'] = $username;

} catch (MgException $e) {
     echo "ERROR: " . $e->GetMessage() . "n";
     echo $e->GetDetails() . "n";
     echo $e->GetStackTrace() . "n";
}