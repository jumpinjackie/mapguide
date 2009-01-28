//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

function getElementWithTag( elementName, tagName )
{
    var theElement = null;

    var elements = document.getElementsByTagName( tagName );
    if ( elements != null )
    {
        var numElements = elements.length;
        for ( var i = 0; i < numElements; i++ )
        {
            if ( elements.item(i).name == elementName )
            {
                theElement = elements.item(i);
                break;
            }
        }
    }

    return theElement;
}

function getElement( elementName )
{
    // Get element - Note that getElementsByTagName is used for compatibility with Firefox. getElementById does not
    // work under Firefox.
    var theElement = getElementWithTag( elementName, "input" );
    if ( theElement == null )
    {
        theElement = getElementWithTag( elementName, "select" );
        if ( theElement == null )
        {
            theElement = getElementWithTag( elementName, "form" );
        }
    }

    return theElement;
}

function SetElementValue( elementName, value )
{
    var theElement = getElement( elementName );
    if ( theElement != null )
        theElement.value = value;
}

function SetElementValueWithConfirmation( elementName, confMsg )
{
    var theElement = getElement( elementName );
    if ( theElement != null )
        theElement.value = window.confirm( confMsg );
}

function MoveSelectionFromAtoB( listAName, listBName )
{
    var listA = getElementWithTag( listAName, "select" );
    var listB = getElementWithTag( listBName, "select" );
    if ( listA == null || listB == null )
        return;

    var numA = listA.length;
    var numB = listB.length;

    for ( var i = 0; i < numA; i++ )
    {
        if ( listA.options[i].selected == true )
        {
            // Add to listB
            var newSelection = new Option( listA.options[i].text, listA.options[i].value );
            listB.options[ numB ] = newSelection;
            listB.options[ numB ].selected = false;
            numB++;

            // Remove from listA
            listA.options[i] = null;
            numA--;
            i--;
        }
    }
}

function SelectAllItems( selectorName )
{
    var theSelector = getElementWithTag( selectorName, "select" );
    if ( theSelector != null )
    {
        theSelector.disabled = false;

        var numItems = theSelector.length;

        for ( var i = 0; i < numItems; i++ )
        {
            theSelector.options[i].selected = true;
        }
    }
}

function RefreshPage( formName, refreshVarName )
{
    var refreshVar = getElementWithTag( refreshVarName, 'input' );
    if ( refreshVar != null )
        refreshVar.value = "true";

    var theForm = getElementWithTag( formName, 'form' );
    if ( theForm != null )
        theForm.submit();
}

function ChangeEnable( fields )
{
    if ( fields != null )
    {
        var numFields = fields.length;

        for ( var i = 0; i < numFields; i++ )
        {
            var theField = getElement( fields[i] );
            if ( theField != null )
            {
                if ( theField.disabled == false )
                    theField.disabled = true;
                else
                    theField.disabled = false;
            }
        }
    }
}

function DisableField( field )
{
    var theField = getElement( field );
    if ( theField != null )
        theField.disabled = true;
}

function EnableField( field )
{
    var theField = getElement( field );
    if ( theField != null )
        theField.disabled = false;
}

NO_PERSISTENCE_FLAG = "_DO_NOT_PERSIST";
function PersistAllInputFields()
{
    var theElement = null;

    var elements = document.getElementsByTagName( 'input' );
    if ( elements != null )
    {
        var numElements = elements.length;
        for ( var i = 0; i < numElements; i++ )
        {
            var theElement = elements.item(i);
            if ( theElement.name.indexOf( NO_PERSISTENCE_FLAG ) < 0 )
                theElement.disabled = false;
        }
    }

    var selectors = document.getElementsByTagName( 'select' );
    if ( selectors != null )
    {
        var numSelectors = selectors.length;
        for ( var i = 0; i < numSelectors; i++ )
        {
            var theSelector = selectors.item(i);
            theSelector.disabled = false;
            if ( theSelector.name.indexOf( NO_PERSISTENCE_FLAG ) < 0 )
            {
                var numItems = theSelector.length;
                for ( var j = 0; j < numItems; j++ )
                {
                    theSelector.options[j].selected = true;
                }
            }
        }
    }
}

function Logout( formName, logoutVarName )
{
    var logoutVar = getElementWithTag( logoutVarName, 'input' );
    if ( logoutVar != null )
        logoutVar.value = "true";

    var theForm = getElementWithTag( formName, 'form' );
    if ( theForm != null )
        theForm.submit();
}

function SubmitForm( formName )
{
    var theForm = getElementWithTag( formName, 'form' );
    if ( theForm != null )
    {
        PersistAllInputFields(); // This should not be necessary because our forms are created with "onSubmit=PersistAllInputFields()"
                                 // However, this does not seem to always work when the form is submitted by calling SubmitForm, which
                                 // is the case for all toolbar buttons.
        theForm.submit();
    }
}

function ConditionalSubmitForm( formName, conditionMsg, conditionFlag, conditionVal )
{
    if ( window.confirm( conditionMsg ) )
    {
        SetElementValue( conditionFlag, conditionVal );
        SubmitForm( formName );
    }
}

function ServiceConditionalDeleteButton( deleteFlagName, okFlag, deleteNotPermittedStr, confirmationID, confirmationStr, formName )
{
    deleteFlag = getElement( deleteFlagName );
    if ( deleteFlag == null )
        return;

    if ( deleteFlag.value != okFlag )
        window.alert( deleteNotPermittedStr );
    else
    if ( confirmationID != null && window.confirm( confirmationStr ) )
    {
        SetElementValue( confirmationID, "true" );
        SubmitForm( formName );
    }
}

function DisplayServerStatus( serverToMonitorID )
{
    var serverToMonitorElement = getElement( serverToMonitorID );
    if ( serverToMonitorElement == null )
        return;

    var destination = "viewserverstatus.php?serverToMonitor="+serverToMonitorElement.value;
    statusWindow = window.open( destination, "ServerStatusWindow", "width=600, height=640, scrollbars, resizable, menubar=yes, toolbar=yes" );
}

function DisplayPackageLog( selectedPackageName )
{
    var packageNameElement = getElement( selectedPackageName );
    if ( packageNameElement == null )
        return;

    var windowNum = Math.round( Math.random() * 100000 );
    var logWindowName = "PackageLog"+windowNum;
    var destination = "displaypackagelog.php?selectedPackageID="+packageNameElement.value;

    logWindow = window.open( destination, logWindowName, "width=800, height=600, scrollbars, resizable" );
}

function DownloadLogFile( logFileID, logFileName )
{
    var windowNum = Math.round( Math.random() * 100000 );
    var logWindowName = "LogFile"+windowNum;
    var destination = "downloadlogfile.php?"+logFileID+"="+logFileName;

    logWindow = window.open( destination, logWindowName, "width=800, height=800, scrollbars, resizable, menubar=yes, toolbar=yes" );
}

function ViewCurrentLogFile( logTypeID, logType )
{
    var windowNum = Math.round( Math.random() * 100000 );
    var logWindowName = "LogFile"+windowNum;
    var destination = "viewcurrentlog.php?"+logTypeID+"="+logType;

    logWindow = window.open( destination, logWindowName, "width=800, height=800, scrollbars, resizable, menubar=yes, toolbar=yes" );
}

function DisableOption( selector, disabledOption, disabledMsg )
{
    var theSelector = getElementWithTag( selector, "select" );
    if ( theSelector != null )
    {
        var numItems = theSelector.length;
        for ( var i = 0; i < numItems; i++ )
        {
            var theOption = theSelector.options[i];
            if ( theOption.value == disabledOption && theOption.selected == true )
            {
                window.alert( disabledMsg );
                theOption.selected = false;
            }
        }
    }
}

function ScrollToBottomOfPage()
{
    var scrollWidth = 0;
    var scrollHeight = 0;
    if ( document.body.scrollHeight )
    {
        scrollWidth = document.body.scrollWidth;
        scrollHeight = document.body.scrollHeight;
    }
    window.self.scrollBy( scrollWidth, scrollHeight );
}

function ClosePopups( popups )
{
    if ( popups == null )
        return;

    var numPopups = popups.length;
    for ( var i = 0; i < numPopups; i++ )
    {
        var windowRef = window.open( "", popups[i], "" );
        if ( windowRef != null && !windowRef.closed )
            windowRef.close();
    }
}

function TrimDashes( theString )
{
    var start = 0;
    var end = theString.length;

    while ( start < end )
    {
        var c = theString.charAt( start );
        if ( c != '-' )
            break;
        start++;
    }
    while ( start < end )
    {
        var c = theString.charAt( end - 1 );
        if ( c != '-' )
            break;
        end--;
    }

    if ( start >= end )
        return new String( "" );
    else
        return theString.substring( start, end );
}

function SuggestPackageName( suggestedNameFormat, folderName, packageNameElement )
{
    if ( folderName.length == 0 )
        return;

    // Remove spaces and slashes
    var folderNameStr = folderName.replace( /Library/, '' );
    folderNameStr = folderNameStr.replace( /\:/, '' );
    folderNameStr = folderNameStr.replace( /\/\//, '' );
    folderNameStr = folderNameStr.replace( /\//g, '-' );
    folderNameStr = TrimDashes( folderNameStr );

    if ( folderNameStr.length <= 0 )
        folderNameStr = "Library";
    packageName = suggestedNameFormat.replace( /<FOLDER_NAME>/, folderNameStr );

    SetElementValue( packageNameElement, packageName );
}
