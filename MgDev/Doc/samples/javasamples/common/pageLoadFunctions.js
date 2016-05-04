//  Copyright (C) 2007  Autodesk, Inc.
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
// JavaScript functions to run when the task pane loads.
// These functions temporarily disable the task pane until the map frame
// has been loaded. 

var saveBodyNodes = document.createElement('body');
window.onload = function()
{
  // If the map has initialized, no need to do anything else.
  
  if (parent.parent.mapFrame.mapInit)
  return;

  // Replace the OnMapLoaded function from the main frame. We can use this
  // method to perform any custom functions that depend
  // on the map being initialized. In this case, use it to activate the
  // task pane. Also set mapFrame.mapInit. The DWF Viewer does not use
  // this variable, but we need it for both AJAX and DWF Viewers as a
  // check for when to run this window.onload() function.
  //
  // Be sure to run the original OnMapLoaded() function; parts
  // of the Viewer depend on it.
  
  original_OnMapLoaded = parent.parent.OnMapLoaded;
  parent.parent.OnMapLoaded = function()
  {
    restoreBody();
    replaceMapName();
    parent.parent.mapFrame.mapInit = true;
    original_OnMapLoaded();
  }

  // Temporarily replace the task pane with holder text while we wait
  // for initialization to complete. It will be restored when the 
  // OnMapLoaded() function is called.
  
  replaceBody();
  
  return;

  // Functions ----------------------------------------------------
  
  function replaceBody()
  {
    // Replace the body of the task pane with inactive holder text.
    while (document.body.hasChildNodes())
    {
      saveBodyNodes.appendChild(document.body.firstChild);
    }
    var tempPara = document.createElement('p');
    var tempText = document.createTextNode('Loading...');
    tempPara.appendChild(tempText);
    document.body.appendChild(tempPara);
  }
  function restoreBody()
  {
    // Remove the holder text and restore the task pane.
    while (document.body.hasChildNodes())
    {
      document.body.removeChild(document.body.firstChild);
    }
    while (saveBodyNodes.hasChildNodes())
    {
      document.body.appendChild(saveBodyNodes.firstChild);
    }
  }
  function replaceMapName()
  {
    // Check document links for empty map name. Replace with the 
    // map name used in this layout.
    docLinks = document.links;
    mapName = parent.parent.mapFrame.GetMapName();
    for (i = 0; i < docLinks.length; i++)
    {
      newHref = docLinks[i].href.replace("MAPNAME=unknown", "MAPNAME=" + mapName);
      docLinks[i].href = newHref;
    }
  }
}
