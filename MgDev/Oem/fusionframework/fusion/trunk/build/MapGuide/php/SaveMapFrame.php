<?php
/*****************************************************************************
 *
 * $Id: SaveMapFrame.php 593 2007-05-09 20:49:46Z pspencer $
 *
 * Purpose: 
 *
 * Project: Fusion
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
 *****************************************************************************/
?><!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">

<html lang="en">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    <script type="text/javascript" charset="utf-8">
    function Save(){
        setTimeout(function(){
            window.frames['theImage'].document.execCommand('SaveAs', true, "<?php echo $_REQUEST['mapname'].'.'.$REQUEST['format']?>");
        },2500);
    };
    </script>
</head>
<body onload='Save()'>
<H4>Preparing Image for download...</H4>
<?php
include('MGCommon.php');
echo "<iframe style='visibility: hidden;' id='theImage' src='SaveMap.php?format=".$_REQUEST['format'].
                            "&mapname=".$_REQUEST['mapname'].
                            "&session=".$_REQUEST['session'].
                            "'></iframe>";
?>
</body>
</html>
