function Test() {
   alert("AcMap.js");
}

function Highlight(Elem, Color) {
   // Applies hand pointer, underline, and color (optional) to Elem.
   // Elem   Element to be highlighted
   // Color  0 = No color change 
   //        Any other integer or omitted = Color change
   // Works with UnHighlight function.
   Elem.style.cursor = "hand";
   if (Color != 0) {
      Elem.style.color = "#33c";
   }
   Elem.style.textDecoration = "underline";
}

function UnHighlight(Elem, Color) {
   // Removes hand pointer, underline, and color (optional) from Elem, but  
   // only if they were set programatically, and restores these settings to 
   // underlying values set by stylesheet or Style attribute. 
   // Elem   Element to be un-highlighted
   // Color  0 = No color change 
   //        Any other integer or omitted = Color change
   // Works with UnHighlight function.
   Elem.style.cursor = "";
   if (Color != 0) {
      Elem.style.color = "";
   }
   Elem.style.textDecoration = "none";
}

function MakeVisible(Context, Head, Body) {
   // On event in Head, makes Body visible, makes all other Bodies 
   // invisible, undoes color change and underline in all other Heads. 
   // Action restricted to scope of Context.
   // Context  ID of Div to which the action of this function is restricted
   // Head     ID of the trigger element, or "this"
   // Body     ID of the element to be made visible
   // 
   // For each element in Context
   for (i = 0; i < Context.all.length; i++) {
      with (Context.all[i].style) {
         ThisId = Context.all[i].id + ""
         // If it's a head
         if (ThisId.charAt(0) == "A") {
            textDecoration = "none";
            color="#306";
         }
         // If it's a body
         if (ThisId.charAt(0) == "B") { 
            visibility = "hidden";
         }
      }
   }
   with (Head.style) {
      textDecoration = "underline";
      color = "#39f";
   }
   Body.style.visibility = "visible";
}

function Display(Context, Head, Body, Image) {
   // On click in Head, makes Body display, makes all other Bodies 
   // not display, undoes color change and underline in all other Heads. 
   // Action restricted to scope of Context.
   // Context  ID of Div to which the action of this function is restricted
   // Head     ID of the trigger element, or "this"
   // Body     ID of the element to display
   // Image    ID of the "open/closed" image element associated with Head
   // You can call this function without the last parameter if there is 
   // no "open/closed" image element.
   if (Body.style.display == "block") {
      Body.style.display = "none";
      Image.src = "Closed.gif";
   } 
   else {
      // For each element in Context
      for (i = 0; i < Context.all.length; i++) {
         with (Context.all[i]) {
            ThisId = Context.all[i].id + ""
            // If it's a head
            if (ThisId.charAt(0) == "M") {
               // Do nothing
            }
            // If it's a body
            if (ThisId.charAt(0) == "N") { 
               style.display = "none";
            }
            // If it's an open-closed image
            if (ThisId.charAt(0) == "i") { 
               src = "Closed.gif";
            }
         }
      }
      Body.style.display = "block";
      Image.src = "Open.gif";
   }
}

function DisplayComment(Context, Head, Body, Image) {
   // On mouse-over in Head, makes Body display, makes all other Bodies 
   // not display, undoes color change and underline in all other Heads. 
   // Action restricted to scope of Context.
   // Context  ID of Div to which the action of this function is restricted
   // Head     ID of the trigger element, or "this"
   // Body     ID of the element to display
   // Image    ID of the "open/closed" image element associated with Head
   // You can call this function without the last parameter if there is 
   // no "open/closed" image element.
   //
   // Reset each element in Context
   for (i = 0; i < Context.all.length; i++) {
      with (Context.all[i]) {
         ThisId = Context.all[i].id + ""
         // If it's a head
         if (ThisId.charAt(0) == "A") {
            // Do nothing
         }
         // If it's a body
         if (ThisId.charAt(0) == "B") { 
            style.display = "none";
         }
         // If it's an open-closed image
         if (ThisId.charAt(0) == "i") { 
            src = "Closed.gif";
         }
      }
   }
   Body.style.display = "block";
   Image.src = "Open.gif";
}

var newWin;  // New window defined by the OpenWindow function
function OpenWindow(Url) 
// Opens a file in a new window with parameters hard-coded within the function.
// Url     URL of the file to open in the new window
{
    newWin = window.open(url,"CodeWin","scrollbars,resizable,width=550,height=400"); 
    newWin.focus();
}

function ShowHide(Elem) {
// Simple show-hide as with inline popups
// Elem   ID of element to show or hide
   if (Elem.style.display == "block") {
      Elem.style.display = "none";
   }
   else {
   Elem.style.display = "block";
   }
}

// Builds a ShortCut object to launch a CHM (with local path)
// build_hhrun_object - 
//
function build_hhrun_object(file) {
  var path = location.pathname;
  var i = path.lastIndexOf("\\");
  var j = path.indexOf(":", 3);
  path = path.substring(j+1,i+1);
  document.writeln("<OBJECT id=hhrun type='application/x-oleobject'");
  document.writeln("  classid='clsid:adb880a6-d8ff-11cf-9377-00aa003b7a11'");
  document.writeln("  codebase='hhctrl.ocx#Version=4,73,8561,0'>");
  document.writeln(" <PARAM name='Command' value='ShortCut'>");
  document.writeln(" <PARAM name='Item1' value=',hh.exe," + path + file + "'>");
  document.writeln(" <PARAM name='Window'  value='bogus.html'>");
  document.writeln("</OBJECT>");
}

function set_browse_buttons(show) {

// if the current record contains a show code 
// (three digits, each 0  or 1, specifying if 
// the corresponding browse button is on or off) 
if (show != "")
   {
      // make browse button group visible 
      document.all.head_nav.style.visibility = "visible";
      // show or hide buttons according to their show codes
      if (show.charAt(0) != 1) 
         document.all.nav_prev.outerHTML = "";
         // document.all.nav_prev.outerHTML = "<img src='graphics/nav-prev-xp-o.gif' border='0'>";
         // document.all.nav_prev.style.visibility = "hidden";
      if (show.charAt(1) != 1) 
         document.all.nav_up.outerHTML = "";
         // document.all.nav_up.outerHTML = "<img src='graphics/nav-up-xp-o-2.gif' border='0'>";
         // document.all.nav_up.style.visibility = "hidden";
      if (show.charAt(2) != 1) 
         document.all.nav_next.outerHTML = "";
         // document.all.nav_next.outerHTML = "<img src='graphics/nav-next-xp-o.gif' border='0'>";
         // document.all.nav_next.style.visibility = "hidden";
   }
}


// data-binding function for ref topics 
function ChangeRecord(LinkTextObj, ListObj) {
   var FilterExpr = "";
   FilterExpr = "Command=" + LinkTextObj.id;
   commands.object.Filter = FilterExpr;
   commands.Reset();
   for (var i = 0; i < ListObj.all.length; i++) {
      ListObj.all[i].style.color = "#57b";
   }
   LinkTextObj.style.color = "#137";
   LinkTextObj.style.textDecoration = "none";
}



//-------------------------------------------
// Added by IntelliArts
//-------------------------------------------
function swapImage(theImgObj, theImage1, theImage2) {
	if (theImgObj.src.search(theImage1) > 0) {
		theImgObj.src = theImage2
	}
	else {
		theImgObj.src = theImage1
	}
}

function showHide(theId) {
	if (document.getElementById) { // DOM3 = IE5, NS6
		el = document.getElementById(theId)
		if (el.style.display == 'none') {
			showElement(el)
		}
		else {
			hideElement(el)
		}
	}
}

function hideElement(theEl) {
	theEl.style.display = 'none';
}
function showElement(theEl) {
	theEl.style.display = '';
}

function linkParser(fn) {
	var X, Y, sl, a, ra, link;
	ra = /:/;
	a = location.href.search(ra);
	if (a == 2)
	X = 14;
	else
	X = 7;
	sl = "\\";
	Y = location.href.lastIndexOf(sl) + 1;
	var tmpRes = location.href.substring(X, Y) + fn;
	if (!(location.href.search('file:///') >= 0))
		tmpRes = 'file:///' + tmpRes;
	return tmpRes;
}

function linkHelpFile(fn, tn) {
	var fileUrl = linkParser(fn);
	var tmpRes = 'mk:@MSITStore:' + fileUrl;
	if (tn != null)
		tmpRes += '::/' + tn;
	return tmpRes;
}
//-------------------------------------------
