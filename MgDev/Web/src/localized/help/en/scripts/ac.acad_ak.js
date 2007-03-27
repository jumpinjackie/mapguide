// acad.js - JavaScript functions for ACAD Help Files
// 
// v.0.32: 29aug03 - updatd var release value to 19
// v.0.31: 14jul03 - added CreateSecwinObject
// v.0.30: 27may03 - added the ShowCHM to launch a CHM (use jumpHTM to open an HTML file)
// v.0.29: 16aug02 - added getcurpath2 function
// v.0.28: 14aug02 - added popGloss function
// v.0.27: 17may02 - fixed ShowAll so CSS styles with a leading "noshow" are ignored
//                   and IMG tags are ignored.
// v.0.26: 16apr02 - updated getcurpath function, it wasn't returning the proper path 
//		     when an "ms-its" protocol was used.
// v.0.25: 27mar02 - moved string constants to the ac.strings.js file
// v.0.24: 13mar02 - fixed ShowAll so it doesn't show "noshow" class items 
// v.0.23: 28feb02 - added: getcurfilename, getcurchmname, and getcurpath functions
// 		     used by AC.CHM-UPDATE.JS
// v.0.22: 05feb02 - strip hash and all that follows in doComments function
// v.0.21: 31jan02 - remove question marks from title in doComments function
// v.0.20: 07dec01 - added IE/Netscape browser check
// v.0.19: 18oct01 - added strnextpage and strlastpage
//		   - fixed showAll so it can handle a file with a #
// v.0.18: 25jul01 - fixed DoComments so it works with "webhelp" also
// v.0.17: 24jan01 - added CheckForUpdates function
// v.0.16: 12jan01 - added unescape to jumpHTM function
// v.0.15: 05jan01 - updated showinfo, added strings
// v.0.14: 14dec00 - added showinfo
// v.0.13: 13nov00 - added nextpage and prevpage functions (not working yet)
// v.0.12: 24may00 - added chmpath to doComments function
// v.0.11: 04may00 - fixed "display/collapse" all hidden text message so 
//		     it changes when clicked
// v.0.10: 03may00 - adjusted the height of the Comments dialog
// v.0.09: 26apr00 - added control of 'click4more' image in toggleLeadin
//		     added control of 'click4more' image in toggleProc
//		     deleted csJump function
//		     added localizable constants
// v.0.08: 06apr00 - updated autoExpand so it really works!
// v.0.07: 27mar00 - added doComments function
// v.0.06: 22mar00 - added build_hhrun_object
// v.0.05: 01mar00 - added extra "/" to jumpHTM
// v.0.04: 29feb00 - updated jumpCHM
// v.0.03: 09dec99 - added autoExpand
// v.0.02: 08dec99 - added jumpHTM and jumpCHM
// v.0.01: 30nov99 - the beginning
//
//===========================================================================
// Constants:  *** Now pulled from ac.strings.js ***

//var strclick4more   = "Click for more...";
//var strclick2hide   = "Click to hide...";
//var strcollapsetext = "Collapse all hidden text on this page.";
//var strprevpage     = "Previous page.";
//var strnextpage     = "Next page.";
//var strlastpage     = "Last page.";
//var strfirstpage    = "First page.";
//var strtopicupdate  = "The following information superceeds that found in this topic:";
//var hh_classid      = "clsid:adb880a6-d8ff-11cf-9377-00aa003b7a11";
//var hh_codebase     = "hhctrl.ocx#Version=4,74,8793,0";

//===========================================================================

// the build, lang, and prod variables are defined in ac_common.js
//var build  = "U070";
//var lang   = "ENU";
//var prod   = "ACAD";
var release = "19";
//var script = "http://www.autodesk.com/cgi-bin/achelp15.pl?type=UPDATES";
// ** achelpscript is defined in ac.strings.js **
var script = achelpscript + "?type=UPDATES";

var isIE = 0;
if(navigator.appName == "Microsoft Internet Explorer") { isIE = 1; }

function CheckForUpdates () {
  var updateurl = script+"&build="+build+"&lang="+lang+"&prod="+prod+"&rel="+release;
  location.href = updateurl;
}

function doComments(path) {
  var stitle = document.title;
  stitle = stitle.replace( /[\?]/g, "_" );  // remove question marks from title (others?)
  var surl   = unescape(location.href);
  surl = surl.replace( /#.*$/, "" );  // strip hash and all that follows
  var i = surl.indexOf("::");
  if( i == -1 ) {
    i = surl.lastIndexOf("/"); 
    chmpath = surl.substring(0,i+1);
  } else {
    chmpath = surl.substring(0,i+3);
  }
  window.open(chmpath+path+"#"+stitle+" ["+surl+"]",null,"height=450,width=450,resizable=yes,directories=no,location=no,menubar=no,status=no,toolbar=no" );
}

var itext = "";
function showinfo(n) {
  if( itext == "" ) { itext = infoline.innerText; }
  if( n == 1 ) { infoline.style.visibility = "visible"; }
  else if( n == 2 ) {
    infoline.innerText = strprevpage;
    infoline.style.visibility = "visible";
  }
  else if( n == 3 ) {
    infoline.innerText = strnextpage;
    infoline.style.visibility = "visible";
  }
  else if( n == 4 ) {
    infoline.innerText = strlastpage;
    infoline.style.visibility = "visible";
  }
  else if( n == 5 ) {
    infoline.innerText = strfirstpage;
    infoline.style.visibility = "visible";
  }
  else { 
    infoline.style.visibility = "hidden"; 
    infoline.innerText = itext; 
  }
}

// Jump to local non-compiled HTML file from a CHM
// jumpHTM('drivers.htm');
function jumpHTM(file) {
  var path = unescape(location.pathname);
  var i = path.lastIndexOf("\\");
  var j = path.indexOf(":", 3);
  path = path.substring(j+1,i+1);
  location.href = "file:///" + path + file;
}

// Jump to a local CHM file from a non-compiled HTML file
// jumpCHM('acad_acr.chm::/acr_l30.html');
function jumpCHM(file_topic) {
  var path = location.pathname;
  var i = path.lastIndexOf("\\");
  path = path.substring(1,i+1);
  location.href = "ms-its:" + path + file_topic;
}

// Builds a ShortCut object to launch a CHM (with local path)
// build_hhrun_object - 
//
function build_hhrun_object(file) {
  //var path = location.pathname;
  var path = unescape(location.pathname);
  var i = path.lastIndexOf("\\");
  var j = path.indexOf(":", 3);
  path = path.substring(j+1,i+1);
  document.writeln("<OBJECT id=hhrun type='application/x-oleobject'");
  document.writeln("  classid='"+hh_classid+"'");
  document.writeln("  codebase='"+hh_codebase+"'>");
  document.writeln(" <PARAM name='Command' value='ShortCut'>");
  document.writeln(" <PARAM name='Item1' value=',hh.exe," + path + file + "'>");
  document.writeln(" <PARAM name='Window'  value='bogus.html'>");
  document.writeln("</OBJECT>");
}

function getpath() {
  var path = location.pathname;
  var i = path.lastIndexOf("\\");
  var j = path.indexOf(":", 3);
  path = path.substring(j+1,i+1);
  return path;
}

function MakeArray(n) {
  this.length = n;
  for (var i = 1; i <= n; i++) { 
    this[i] = 0;
  }
  return this;
}

function toggleLeadin() {
  if( leadin.style.display == "" ) {
    leadin.style.display = "none";
    click4more.src = "ac.chickletred.gif";
    click4more.title = strclick4more;
  }
  else {
    leadin.style.display="";
    click4more.src = "ac.chickletred2.gif";
    click4more.title = strclick2hide;
  }
}

var lastspan = new MakeArray(5);
var lastlink = new MakeArray(5);
function togglespan( level, spanname, linkname ) {
  if( spanname.style.display == "" ) {
    spanname.style.display = "none";
    lastspan[level] = 0;
    lastlink[level] = 0;
    if( linkname != null ) {
      linkname.style.fontWeight = "";
    }
  }
  else {
    spanname.style.display="";
    if( lastspan[level] != 0 ) { 
      lastspan[level].style.display = "none"; 
      lastlink[level].style.fontWeight = "";
    }
    lastspan[level] = spanname;
    if( linkname != null ) {
      lastlink[level] = linkname;
      linkname.style.fontWeight = "bold";
    }
  }
}

var allon = 0;
//var showall1 = 0;
function showAll() {
  if( allon == 1 ) {
    var loc = document.location.href;
    var i1 = loc.indexOf("#");
    if( i1 != -1 ) { loc = loc.substring(0,i1); }
    document.location.href = loc;
  }
  else {
    for ( i=0; i < document.all.length; i++ ) {
      var itm = document.all.item(i);
      if( itm.className.substring(0,6) != "noshow" ) {  // CSS styles with a leading "noshow" are ignored
      //if( document.all.item(i).className != "noshow" ) {
        if( itm.tagName != "IMG" ) {
          document.all.item(i).style.display = "";
        }
      }
    }
    allon = 1;
    //if( showall1 != 0 ) {
      document.all.showall1.innerText = strcollapsetext;
    //  showall2.innerText = "";
    //}
  }
}

function autoExpand( exp ) {
  if( exp == 1 ) { showAll(); }
}

function toggleproc( procname, imgname, anchor ){
  if( isIE == 1 ) {
    var i = document.location.href.lastIndexOf("#");
    var cur_href = "";
    eval("var linkid = "+procname.id+"link");
    if( i > 0 ) { cur_href = document.location.href.substring(0,i); }
    if( procname.style.display == "" ) {
      procname.style.display = "none";
      imgname.src = "ac.right.gif";
      linkid.title = strclick4more;
    }
    else {
      procname.style.display="";
      imgname.src = "ac.down.gif";
      linkid.title = strclick2hide;
    }
  }
}

function getcurfilename () {
  var surl = unescape(location.href);
  var i = surl.indexOf("::");
  if( i == -1 ) { // NOT A CHM
    i = surl.lastIndexOf("/"); 
    filename = surl.substring(i+1,surl.length);
  } else {
    filename = surl.substring(i+3,surl.length);
  }
  var j = filename.indexOf("#");
  if( j != -1 ) { filename = filename.substring(0,j); }
  j = filename.indexOf("?");
  if( j != -1 ) { filename = filename.substring(0,j); }
  return filename;
}

function getcurchmname () {
  var surl = unescape(location.href);
  var chmname = "";
  var i = surl.indexOf("::");
  if( i != -1 ) {
    chmname = surl.substring(0,i);
    var j = chmname.lastIndexOf("\\"); 
    if( j != -1 ) { chmname = chmname.substring(j+1,chmname.length); }
  }
  return chmname;
}

function getcurpath () {
  var path = unescape(location.pathname);
  var i = path.lastIndexOf("\\");
  var j = 0;
  if( path.indexOf("::") != -1 ) { // it's a CHM
    if( path.charAt(0) == "@" ) { // this is a @MSITStore: .. chop this off
      j = path.indexOf(":") + 1;
    }
    else { // this is an ms-its: .. path starts at the beginning of string
      // do nothing
    }
  }
  else { j++; }
  path = path.substring(j,i+1);
  return path;
}

function getcurpath2 () {
  var path = unescape(location.pathname);
  if( path.indexOf("@MSITStore:") == 0 ) {
    path = path.substring(11,path.length);
  }
  if( path.indexOf("/") == 0 ) {
    path = path.substring(1,path.length);
  }
  var i = path.lastIndexOf("\\");
  var j = 0;
  path = path.substring(j,i+1);
  return path;
}

var ispop = 0;
var popid = null;
function popGloss(gloss_text) {
  var gloss_file = gloss_text.replace(/ +/g, "_");
  var top  = event.screenY + 10;
  var left = event.screenX - 150;
  //alert("X:"+left+" Y:"+top);
  if( ispop == 1 ) { popid.close(); }
  popid = window.showModelessDialog("gloss_"+gloss_file+".html","Dialog Arguments Value","dialogHeight: 250px; dialogWidth: 300px; dialogTop: "+top+"; dialogLeft: "+left+"; edge: Raised; center: Yes; help: No; resizable: Yes; status: No; unadorned: yes; ");
  ispop = 1;
}

// ShowCHM - Launches a CHM in a new window, path is relative to the current CHM's folder
// 	ShowCHM('acad_nfw.chm::/1acad.html');
//
function ShowCHM (file) {
  var filepath = "mk:@MSITStore:" + getcurpath() + file;
  window.showHelp(filepath);
}

// CreateSecwinObject - creates a secondary window object
//
function CreateSecwinObject(id, winname, file) {
  var surl = unescape(location.href);
  var chmname = "";
  var i = surl.indexOf("::");
  if( i != -1 ) {
    chmname = surl.substring(0,i);
    var j = chmname.lastIndexOf("\\");
    if( j != -1 ) { chmname = chmname.substring(j+1,chmname.length); }
  }

  document.writeln("<OBJECT id="+id+" type=\"application/x-oleobject\" classid=\"clsid:adb880a6-d8ff-11cf-9377-00aa003b7a11\" codebase=\"hhctrl.ocx#Version=4,74,8793,0\">");
  document.writeln("    <PARAM name=\"Command\" value=\"Related Topics\">");
  document.writeln("    <PARAM name=\"Window\" value=\""+winname+"\">");
  // Note that "Help" in the following line is supposed to be the window title, but it 
  // is really overridden by the title defined in the HHP (I think)
  document.writeln("    <PARAM name=\"Item1\" value=\"Help;"+chmname+"::/"+file+"\">");
  document.writeln("</OBJECT>");

}
