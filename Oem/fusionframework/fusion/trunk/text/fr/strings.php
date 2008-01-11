# Web surround localized strings
# English language
#
# Important: This file must be saved with UTF-8 encoding
#

# General

# Fonts - the 6 following entries are mandatory
@fontWindows        = Arial
@fontsizeWindows    = 8pt
@fontLinux          = Arial
@fontsizeLinux      = 8pt
@fontMacintosh      = Arial
@fontsizeMacintosh  = 8pt

# Distances
DISTANCEMILES       = Miles
DISTANCEKILOMETERS  = Kilometers
DISTANCEFEET        = Feet
DISTANCEMETERS      = Meters

# Fill Patterns/Values
FILLSOLID           = Solid
FILLNET             = Net
FILLLINE            = Line
FILLLINE45          = Line_45
FILLLINE90          = Line_90
FILLLINE135         = Line_135
FILLSQUARE          = Square
FILLBOX             = Box
FILLCROSS           = Cross
FILLDASH            = Dash
FILLDOLMIT          = Dolmit
FILLHEX             = Hex
FILLSACNCR          = Sacncr
FILLSTEEL           = Steel

# Transparency
TRANSPARENT         = transparent

# Line Styles/Values
LINESOLID           = Solid
LINEDASH            = Dash
LINEDOT             = Dot
LINEDASHDOT         = DashDot
LINEDASHDOTDOT      = DashDotDot
LINERAIL            = Rail
LINEBORDER          = Border
LINEDIVIDE          = Divide
LINEFENCELINE       = FenceLine

# Buttons
BUTTONOK            = OK
BUTTONDONE          = Done
BUTTONCANCEL        = Cancel
BUTTONREFRESH       = Refresh
BUTTONCLOSE         = Close
BUTTONCLEAR         = Clear
BUTTONSAVE          = Save

# Colors
COLORRED            = Red
COLORGREEN          = Green
COLORBLUE           = Blue

# BufferReport
BUFFERREPORTTITLE               = Create Buffer
BUFFERREPORTCREATED             = %s has been created.
BUFFERREPORTUPDATED             = %s has been updated.
BUFFERREPORTFEATURESSINGULAR    = %s buffer feature was created.
BUFFERREPORTFEATURESPLURAL      = %s buffer features were created.
BUFFERREPORTWARNINGSINGULAR     = <b>Warning:</b> %s layer was excluded from buffer computation because its coordinate system is incompatible with the coordinate system of the map, or because it has no coordinate system at all
BUFFERREPORTWARNINGPLURAL       = <b>Warning:</b> %s layers were excluded from buffer computation because their coordinate systems are incompatible with the coordinate system of the map, or because they have no coordinate system at all
BUFFERREPORTERRORTITLE          = Buffer Error

# Buffer
BUFFERCLASSDESCR                = Feature class for buffer layer
BUFFERSCHEMADESCR               = Temporary buffer schema
BUFFERDIFFARBXY                 = Cannot compute a unique buffer around features with different 'Arbitrary X-Y' coordinate systems. Please uncheck the option 'Merge buffer areas' or select only one layer

# BufferUI
BUFFERTITLE                     = Create a Buffer
BUFFERSUBTITLE                  = &nbsp;Buffer settings
BUFFERINFOTEXT                  = Select features on the image.
BUFFERDISTANCE                  = Distance around features:
BUFFERLAYERS                    = Layers to include in the buffer:
BUFFERNOLAYER                   = <No layer selected>
BUFFERNAME                      = Name for the resulting buffer layer:
BUFFERNAMETEXT                  = Buffer 1
BUFFERMERGE                     = Merge buffer areas
BUFFERFILLSTYLE                 = &nbsp;Fill style
BUFFERFILLPATTERN               = Fill pattern:
BUFFERFOREGROUNDCOLOR           = Foreground color:
BUFFERTRANSPARENCY              = Transparency:
BUFFERBACKGROUNDCOLOR           = Background color:
BUFFERBORDERSTYLE               = &nbsp;Border style
BUFFERBORDERLINEPATTERN         = Line pattern:
BUFFERBORDERLINECOLOR           = Line color:
BUFFERBORDERLINETHICKNESS       = Line thickness:
BUFFERHELP                      = <li><p align="left">Results of the buffer are put into a new layer above the top feature layer. This buffer layer has the name you enter here.</li><li><p align="left">To create multiple buffer layers, use different buffer names.</li><li><p align="left">To hide buffer layers, turn them off.</li><li><p align="left">Buffer layers are temporary and are removed when you close the viewer.</li>
BUFFERERRORZERODISTANCE         = Please enter a nonzero distance.
BUFFERERRORNOLAYERNAME          = Please enter a layer name.
BUFFERERRORNEGATIVETHICKNESS    = Please enter positive or null thickness.
BUFFERERRORNOSELECTION          = Please select feature(s) to calculate a buffer around.

# ColorPicker
COLORPICKERDLGTITLE             = Select a color
COLORPICKERTITLE                = Color palette
COLORPICKERSUBTITLE             = Specify a color
COLORPICKER100TRANSPARENCY      = 100%% transparency
COLORPICKERHEXFORMAT            = Hex format

# MeasureUI
MEASURETITLE            = Measure Distance
MEASUREINFO             = Click a start and end point.
RESUME                  = Resume
STOP                    = Stop
CLEAR                   = Clear
MILES                   = Miles
KILOMETERS              = Kilometers
MEASUREHELP             = <li>To measure multiple distances, continue clicking new points.</li><li>To finish, click &quot;Stop&quot;.</li><li>You can pan or zoom and continue measuring by clicking &quot;Resume&quot;.</li><li>To start over, click &quot;Clear&quot;.</li>
SEGMENT                 = Segment
TOTAL                   = Total
LENGTH                  = Length
MEASUREERROR            = Measure Error

# Measure
MEASUREFEATURECLASS     = Feature class for measure layer
MEASURESCHEMADESCR      = Temporary measure schema
MEASUREPARTIAL          = Partial
MEASURETOTAL            = total
MEASURELAYER            = Measure

# HtmlViewerAbout
HTMLABOUTTITLE          = About %s
HTMLABOUTTITLEBAR       = MapGuide Viewer
HTMLABOUTSERVERTITLE    = MapGuide Server
HTMLABOUTVIEWERTITLE    = MapGuide Viewer
HTMLABOUTSERVERVERSION  = %s (Version %s)
HTMLABOUTVIEWERVERSION  = %s (Version %s)
HTMLABOUTTEXT           = <br>This product is made available subject to the terms of GNU Lesser General Public License Version 2.1 ("LGPL").  A copy of the LGPL, as well as additional copyright notices and license terms applicable to portions of this product are set forth in the <a href="../localized/license_en.htm" target="blank">license</a> file.<br><br>All trademarks and registered trademarks mentioned herein are the property of their respective owners.<br><br>Copyright &copy; 2007 Autodesk, Inc.<br><br>
HTMLABOUTLICENSE        = license file
HTMLABOUTLICENSEFILE    = license_en.htm

# LegendUI
LEGENDLISTSEPARATOR     = ,

# PrintablePageUi
PRINTTITLE              = Get Printable Page
PRINTELEMENTS           = Select the elements to include in the print layout.
PRINTPAGETITLE          = Page title:
PRINTLEGEND             = Legend
PRINTNORTHARROW         = North arrow
PRINTCREATEPAGE         = Create Page

# PropertyCtrl
PROPERTIESNONE          = None Selected
PROPERTIESNAME          = Name
PROPERTIESVALUE         = Value
PROPERTIESITEMSEL       = Items selected: {0}

# Search
SEARCHDLGTITLE          = Search features
SEARCHTITLE             = Search results
SEARCHERROR             = Search error
SEARCHREPORT            = Search report
SEARCHLAYERNOTFOUND     = Layer {0} not found!
SEARCHNOMULTIPROP       = Multi-properties identity not supported yet
SEARCHTYYPENOTSUP       = Identity property type not supported yet ({0})
SEARCHNOMATCHES         = No matches.

# SearchPrompt
SEARCHPROMPTFIND        = Find

# SelectWithinUi
SELECTWITHINTITLE       = Select Within
SELECTWITHINAREAS       = Select areas on the image.
SELECTWITHINRESTRICTION = Restrict results to selected layers:
SELECTWITHINTEXT        = <li>If you continue to select areas on the image, click &quot;Refresh&quot; to update the layer list.</li><li>To finish and select all features within the highlighted areas, click &quot;Done&quot;.</li><li>To start over, click &quot;Clear&quot;.</li>

# Error/Report
REPORTTITLE             = Report

# Legend UI
REFRESH                 = Refresh
EXPANDALL               = Expand
COLLAPSEALL             = Collapse All
DISPLAYALLINGROUP       = Display All in Group
HIDEALLINGROUP          = Hide All in Group
SHOWLONGTHEME           = Show Long Theme

# Task Bar
TASKS                   = Tasks

# Viewer Options
OPTIONSTITLE            = Viewer Options
OPTIONSINFOTEXT         = Select viewing options for status bar and tool tips.
OPTIONSDISTANCE         = Display distances in
OPTIONSMETRIC           = Metric (Kilometers, Meters)
OPTIONSUSENGLISH        = US/English (Miles, Feet)
OPTIONSPOSITION         = Display cursor position in
OPTIONSLATLON           = Latitude, Longitude (Degrees)
OPTIONSMAPUNITS         = Map Coordinate System (X,Y)

# AJAX Viewer
VIEWERLAYERS            = Layers
VIEWERPROPS             = Properties
VIEWERPANNORTH          = Pan North
VIEWERPANWEST           = Pan West
VIEWERPANSOUTH          = Pan South
VIEWERPANEAST           = Pan East
VIEWERZOOMIN            = Zoom In
VIEWERZOOMOUT           = Zoom Out
VIEWERMOVESLIDER        = Drag to move slider
VIEWERDRAGZOOM          = Drag to zoom
ENDSEL                  = CTRL + click to end
ENDSELSAFARI            = SHIFT + click to end
DECIMALSEPARATOR        = .
THOUSANDSEPARATOR       = ,

# Main Frame
NEEDLOGIN               = You must enter a valid login ID and password to access this site
ALREADYINMEASURE        = Only one measure command is allowed in a web layout
ACCESSDENIED            = Access Denied   
LATLONCURPOS            = Lat: {0}, Lon: {1}
MAPUNITSCURPOS          = X: {0}, Y: {1}
FEATURESSELECTED        = {0} features selected
FEATURESELECTED         = {0} feature selected
MI                      = mi
FT                      = ft
IN                      = in
KM                      = km
MT                      = m
CM                      = cm
DIGALREADYRUNNING       = Cannot execute measure: A digitization command is in progress.
MEASALREADYRUNNING      = Cannot execute digitization: A measure is in progress.

# Localized Icon Files
POWEREDBYICON           = PoweredBy_en.gif
