<?php
/*****************************************************************************
 * $Id: CreateSession.php 837 2007-09-19 20:32:05Z pspencer $
 * Purpose: Create a session
 * Project: Fusion
 * Author:  DM Solutions Group Inc
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
 
 /* initialize a session and return the session id to the caller */
include(dirname(__FILE__).'/Common.php');
include('../../common/php/Utilities.php');

initializeSession( "sid", "", "" );
$sessionId = session_id();

header('Content-type: text/x-json');
header('X-JSON: true');
$result = null;
$result->sessionId = $sessionId;
$result->userName = '';
echo var2json($result);
?>
