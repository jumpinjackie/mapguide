Option explicit
On Error Goto 0

' This script allows quick updates to the server version across all files that store it.
' Usage:  cscript updateversion.vbs /major:2 /minor:1 /point:0 /build:4245

' Simple four-part version... Somebody went a little overboard playing with VBS classes.
Class VersionInfo

  Private m_Major
  Private m_Minor
  Private m_Point
  Private m_Build

  Public Property Get Major
    If isEmpty(m_Major) Then
      Err.Raise 1, "Property not yet assigned", "Major"
    Else
      Major = m_Major
    End If
  End Property
  
  Public Property Let Major (iMajor)
    If ( CInt(iMajor) >= 0 ) Then
      m_Major = iMajor
    Else
      Err.Raise 2, "Invalid value", "Major: " & iMajor
    End If
  End Property

  Public Property Get Minor
    If isEmpty(m_Minor) Then
      Err.Raise 1, "Property not yet assigned", "Minor"
    Else
      Minor = m_Minor
    End If
  End Property
  
  Public Property Let Minor (iMinor)
    If ( CInt(iMinor) >= 0 ) Then
      m_Minor = iMinor
    Else
      Err.Raise 2, "Invalid value", "Minor: " & iMinor
    End If
  End Property

  Public Property Get Point
    If isEmpty(m_Point) Then
      Err.Raise 1, "Property not yet assigned", "Point"
    Else
      Point = m_Point
    End If
  End Property
  
  Public Property Let Point (iPoint)
    If ( CInt(iPoint) >= 0 ) Then
      m_Point = iPoint
    Else
      Err.Raise 2, "Invalid value", "Point: " & iPoint
    End If
  End Property

  Public Property Get Build
    If isEmpty(m_Build) Then
      Err.Raise 1, "Property not yet assigned", "Build"
    Else
      Build = m_Build
    End If
  End Property
  
  Public Property Let Build (iBuild)
    If ( CInt(iBuild) >= 0 ) Then
      m_Build = iBuild
    Else
      Err.Raise 2, "Invalid value", "Build: " & iBuild
    End If
  End Property

  Public Function GetVersion(sType)
    Select Case sType
      Case "decimal"
        GetVersion = me.Major & "." & me.Minor & "." & me.Point & "." & me.Build
      Case "comma"
        GetVersion = me.Major & "," & me.Minor & "," & me.Point & "," & me.Build
      Case "commaspace"
        GetVersion = me.Major & ", " & me.Minor & ", " & me.Point & ", " & me.Build
      Case Else 
        Err.Raise 1, "Invalid version type", "Type: " & sType
    End Select
  End Function

End Class  


' Let's make sure there are the correct number, names, and types of arguments
Sub CheckArguments(cArgs)

  Dim sUsage
  sUsage = vbCrLf & "Usage: cscript " & Wscript.ScriptName & " /major:2 /minor:1 /point:0 /build:4245"
  
  If cArgs.Count <> 4 Then
    Err.Raise 1, "Not enough arguments", sUsage
    Exit Sub
  End If
  
  If ( Not cArgs.Named.Exists("major") ) Or _
     ( Not cArgs.Named.Exists("minor") ) Or _
     ( Not cArgs.Named.Exists("point") ) Or _
     ( Not cArgs.Named.Exists("build") ) Then
    Err.Raise 1, "Incorrect arguments", sUsage
    Exit Sub
  End If
  
  If ( Not cArgs.Named.Item("major") = CStr(CInt(cArgs.Named.Item("major"))) ) Or _
     ( Not cArgs.Named.Item("minor") = CStr(CInt(cArgs.Named.Item("minor")))) Or _
     ( Not cArgs.Named.Item("point") = CStr(CInt(cArgs.Named.Item("point")))) Or _
     ( Not cArgs.Named.Item("build") = CStr(CInt(cArgs.Named.Item("build")))) Then
    Err.Raise 1, "Version numbers must be integers", sUsage
    Exit Sub
  End If

End Sub


Sub FixRcFile(sFileName, oVersion)

  Const ForReading = 1
  Const ForWriting = 2
  
  Dim oFS
  Dim oFile
  Dim sContents
  Dim oRE
  
  Set oFS = CreateObject("Scripting.FileSystemObject")
  
  Set oFile = oFS.OpenTextFile(sFileName, ForReading)
  sContents = oFile.ReadAll
  oFile.Close
  
  Set oRE = CreateObject("VBScript.RegExp")
  
  oRE.MultiLine = true
  oRE.Global = true

  oRE.Pattern = "^(.*)(FILEVERSION |PRODUCTVERSION ).+$"
  sContents = oRE.Replace (sContents, "$1$2" & oVersion.GetVersion("comma") & vbCr)

  oRE.Pattern = "^(.*)VALUE ""(FileVersion|ProductVersion)"",.+$"
  sContents = oRE.Replace (sContents, "$1VALUE ""$2"", """ & oVersion.GetVersion("commaspace") & """" & vbCr)

  If Right(sContents, 2) = vbCrLf Then
    sContents = Left(sContents, Len(sContents) - 2)
  End If
  
  Set oFile = oFS.OpenTextFile(sFileName, ForWriting)
  oFile.WriteLine sContents
  oFile.Close

End Sub

Sub FixVersionFile (sFileName, sPattern, sReplacement)

  Const ForReading = 1
  Const ForWriting = 2
  
  Dim oFS
  Dim oFile
  Dim sContents
  Dim oRE
  
  Set oFS = CreateObject("Scripting.FileSystemObject")
  
  Set oFile = oFS.OpenTextFile(sFileName, ForReading)
  sContents = oFile.ReadAll
  oFile.Close
  
  Set oRE = CreateObject("VBScript.RegExp")
  
  ' MultiLine RE processing is required so that we can match line start/end
  oRE.MultiLine = true
  oRE.Global = true

  oRE.Pattern = sPattern
  sContents = oRE.Replace (sContents, sReplacement)

  If Right(sContents, 2) = vbCrLf Then
    sContents = Left(sContents, Len(sContents) - 2)
  End If
  
  Set oFile = oFS.OpenTextFile(sFileName, ForWriting)
  oFile.WriteLine sContents
  oFile.Close

End Sub

' Run through array of File, Pattern, Replace for single-value replacement
Sub FixVersionFiles (oVersion)

  Dim aVersionFiles
  aVersionFiles = Array ( _
    Array ( _ 
          "/Common/ProductVersion.h", _
          "^(const STRING ProductVersion = L"").+$", _
          "$1" & oVersion.GetVersion("decimal") & """;" & vbCr _
    ), _
    Array ( _ 
          "/Web/src/mapviewerjava/ajaxviewerabout.jsp", _
          "^(String viewerVersion = "").+$", _
          "$1" & oVersion.GetVersion("decimal") & """;" & vbCr _
    ), _
    Array ( _ 
          "/Web/src/mapviewernet/ajaxviewerabout.aspx", _
          "^(String viewerVersion = "").+$", _
          "$1" & oVersion.GetVersion("decimal") & """;" & vbCr _
    ), _
    Array ( _ 
          "/Web/src/mapviewerphp/ajaxviewerabout.php", _
          "^(.*'VIEWER_VERSION', "").+$", _
          "$1" & oVersion.GetVersion("decimal") & """ );" & vbCr _
    ), _
    Array ( _ 
          "/Web/src/mapadmin/resizableadmin.php", _
          "^(.*'SITE_ADMINISTRATOR_VERSION', "").+$", _
          "$1" & oVersion.GetVersion("decimal") & """ );" & vbCr _
    ) _
  )

  Dim sItem
  For Each sItem In aVersionFiles
    FixVersionFile "." & sItem(0) , sItem(1), sItem(2)
  Next

End Sub

' Do Multi-line replacement in special case of .rc files
Sub FixRcFiles (oVersion)

  Dim aRcFiles

  aRcFiles = Array ( _
  "/Common/Foundation/Foundation.rc", _
  "/Common/Geometry/Geometry.rc", _
  "/Common/MapGuideCommon/MapGuideCommon.rc", _
  "/Common/MdfModel/MdfModel.rc", _
  "/Common/MdfParser/MdfParser.rc", _
  "/Common/PlatformBase/PlatformBase.rc", _
  "/Common/Renderers/Renderers.rc", _
  "/Common/Stylization/Stylization.rc", _
  "/Server/src/Common/Base/ServerBase.rc", _
  "/Server/src/Common/Cache/ServerCache.rc", _
  "/Server/src/Common/Manager/ServerManager.rc", _
  "/Server/src/Common/Thread/ServerThread.rc", _
  "/Server/src/Core/ServerCore.rc", _
  "/Server/src/Gws/GwsCommon/GwsCommon.rc", _
  "/Server/src/Gws/GwsQueryEngine/GwsQueryEngine.rc", _
  "/Server/src/Gws/GwsResource/GwsResource.rc", _
  "/Server/src/Services/Drawing/ServerDrawingService.rc", _
  "/Server/src/Services/Feature/ServerFeatureService.rc", _
  "/Server/src/Services/Kml/ServerKmlService.rc", _
  "/Server/src/Services/Mapping/ServerMappingService.rc", _
  "/Server/src/Services/Rendering/ServerRenderingService.rc", _
  "/Server/src/Services/Resource/ServerResourceService.rc", _
  "/Server/src/Services/ServerAdmin/ServerAdminService.rc", _
  "/Server/src/Services/Site/ServerSiteService.rc", _
  "/Server/src/Services/Tile/ServerTileService.rc", _
  "/Server/src/UnitTesting/UnitTesting.rc", _
  "/UnitTest/Common/FoundationTest/DotNetUnmanagedApi/DotNetUnmanagedApi.rc", _
  "/Web/src/ApacheAgent/ApacheAgent.rc", _
  "/Web/src/CgiAgent/CgiAgent.rc", _
  "/Web/src/DotNetUnmanagedApi/Foundation/FoundationApi.rc", _
  "/Web/src/DotNetUnmanagedApi/Geometry/GeometryApi.rc", _
  "/Web/src/DotNetUnmanagedApi/MapGuideCommon/MapGuideCommonApi.rc", _
  "/Web/src/DotNetUnmanagedApi/PlatformBase/PlatformBaseApi.rc", _
  "/Web/src/DotNetUnmanagedApi/Web/WebApi.rc", _
  "/Web/src/HttpHandler/HttpHandler.rc", _
  "/Web/src/IsapiAgent/IsapiAgent.rc", _
  "/Web/src/JavaApi/JavaApi.rc", _
  "/Web/src/PhpApi/PhpApi.rc", _
  "/Web/src/WebApp/WebApp.rc" _
  )

  ' Update the version number in the user-defined RC files
  Dim sFile
  For Each sFile In aRcFiles
    FixRcFile "." & sFile , oVersion
  Next

End Sub

' Main

' Check to make sure that the user passed some sane values
CheckArguments(WScript.Arguments)

' Set up an object to hold the version info...
Dim oVersionInfo
Set oVersionInfo = new VersionInfo

' Pull the version info from the command line and svn revision # mod maxint
With oVersionInfo
  .Major = WScript.Arguments.Named.Item("major")
  .Minor = WScript.Arguments.Named.Item("minor")
  .Point = WScript.Arguments.Named.Item("point")
  .Build = WScript.Arguments.Named.Item("build") Mod 32767
End With

FixVersionFiles (oVersionInfo)

FixRcFiles (oVersionInfo)
