<%--
  -Copyright (C) 2004-2008 by Autodesk, Inc.
  -This library is free software; you can redistribute it and/or
  -modify it under the terms of version 2.1 of the GNU Lesser
  -General Public License as published by the Free Software Foundation.

  -This library is distributed in the hope that it will be useful,
  -but WITHOUT ANY WARRANTY; without even the implied warranty of
  -MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  -Lesser General Public License for more details.

  -You should have received a copy of the GNU Lesser General Public
  -License along with this library; if not, write to the Free Software
  -Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
--%>

<%@ page contentType="text/html; charset=UTF-8" pageEncoding="UTF-8" %>
<%@ page import="org.osgeo.mapguide.*" %>
<%@ page import="java.util.*" %>
<%@ page import="java.io.*" %>
<%@ page import="java.text.*" %>
<%@ page import="javax.servlet.jsp.*" %>
<%@ page import="javax.servlet.http.*" %>
<%@ include file="common.jsp" %>

<%!
class SearchError extends Exception
{
    public SearchError(String msg, String title)
    {
        super(msg);
        this.title = title;
    }

    public String title;
}

String userInput;
int target;
int popup;
String layerName;
String mapName;
String sessionId;
String filter;
ArrayList resNames;
ArrayList resProps;
int matchLimit;
String locale;
String searchError;
%>

<%
    // Initialize variables for this request
    //
    userInput = "";
    target = 0;
    popup = 0;
    layerName = "";
    mapName = "";
    sessionId = "";
    filter = "";
    resNames = new ArrayList();
    resProps = new ArrayList();
    matchLimit = 0;
    locale = "";

    response.setContentType("text/html; charset=UTF-8");
    request.setCharacterEncoding("UTF-8");

    PrintWriter outStream = response.getWriter();

    MgLocalizer.SetLocalizedFilesPath(getServletContext().getRealPath("/") + "localized/");
    searchError = MgLocalizer.GetString("SEARCHERROR", locale);

    GetRequestParameters(request);
    
    try
    {
        InitializeWebTier();

        MgUserInformation cred = new MgUserInformation(sessionId);

        //connect to the site and get a feature service and a resource service instances
        MgSiteConnection site = new MgSiteConnection();
        site.Open(cred);
        MgFeatureService featureSrvc = (MgFeatureService)site.CreateService(MgServiceType.FeatureService);
        MgResourceService resourceSrvc = (MgResourceService)site.CreateService(MgServiceType.ResourceService);

        //Create a temporary map runtime object, locate the layer
        //
        MgMap map = new MgMap();
        map.Open(resourceSrvc, mapName);
        MgLayerCollection layers = map.GetLayers();
        int i, j;
        MgLayer layer = null;
        for(i = 0; i < layers.GetCount(); i++)
        {
            layer = (MgLayer)layers.GetItem(i);
            if(layer.GetName().equals(layerName))
                break;
        }

        if(layer == null)
            throw new SearchError(MessageFormat.format(MgLocalizer.GetString("SEARCHLAYERNOTFOUND", locale), new Object[] { layerName }), searchError);

        //substitute the input tag with the actual user input to make up the filter
        int varIndex = filter.indexOf("$USER_VARIABLE");
        if(varIndex != -1)
            filter = filter.substring(0, varIndex) + userInput + filter.substring(varIndex + 14);

        //parse the match label string, which defines what columns to be displayed
        boolean displayAll = (resProps.size() == 0);

        //query the features
        MgFeatureQueryOptions opts = new MgFeatureQueryOptions();
        opts.SetFilter(filter);
        String featureClassName = layer.GetFeatureClassName();
        MgResourceIdentifier srcId = new MgResourceIdentifier(layer.GetFeatureSourceId());
        MgFeatureReader features = featureSrvc.SelectFeatures(srcId, featureClassName, opts);
        boolean hasResult = features.ReadNext();

        if(hasResult)
        {
            int colCount = displayAll ? features.GetPropertyCount(): resProps.size();

            //output the beginning of the document (head section and beginning of body section)
            String templ = MgLocalizer.Localize(LoadTemplate("/viewerfiles/search.templ"), locale, GetClientOS(request));
            String[] vals = {
                            String.valueOf(colCount),
                            String.valueOf(target),
                            String.valueOf(popup)
                            };
            outStream.write(Substitute(templ, vals));

            MgClassDefinition classDef = features.GetClassDefinition();
            MgPropertyDefinitionCollection idProps = classDef.GetIdentityProperties();
            boolean multiIds = idProps.GetCount() > 1;
            ArrayList idPropNames = new ArrayList(idProps.GetCount());
            String idPropName = "";
            String idPropTypes[];
            for(j = 0; j < idProps.GetCount(); j++)
            {
                MgPropertyDefinition idProp = idProps.GetItem(j);
                if(multiIds)
                    idPropNames.add(idProp.GetName());
                else
                    idPropName = idProp.GetName();
            }

            //table headings
            outStream.write("<tr class=\"SearchHeading\">");
            if(displayAll)
            {
                for(i = 0; i < colCount; i++)
                {
                    resProps.add(features.GetPropertyName(i));
                    outStream.write("<td class=\"SearchHeading\">&nbsp;" + (String)resProps.get(i) + "</td>");
                }
            }
            else
            {
                for(i = 0; i < colCount; i++)
                    outStream.write("<td class=\"SearchHeading\">&nbsp;" + (String)resNames.get(i) + "</td>");
            }
            outStream.write("</tr>");

            //output the results
            int row = 0;
            int idPropType = 0;
            do
            {
                outStream.write("<tr>");
                for(i = 0; i < colCount; i++)
                {
                    String prop = (String)resProps.get(i);
                    int propType = features.GetPropertyType(prop);
                    String val = "";
                    //TODO: to use the constants from MgPropertyType, make them constants...
                    switch(propType)
                    {
                        case /*MgPropertyType.Boolean*/ 1:
                            val = features.GetBoolean(prop)? "True": "False";
                            break;
                        case /*MgPropertyType.Single*/ 4:
                            val = String.valueOf(features.GetSingle(prop));
                            break;
                        case /*MgPropertyType.Double*/ 5:
                            val = String.valueOf(features.GetDouble(prop));
                            break;
                        case /*MgPropertyType.Int16*/ 6:
                            val = String.valueOf(features.GetInt16(prop));
                            break;
                        case /*MgPropertyType.Int32*/ 7:
                            val = String.valueOf(features.GetInt32(prop));
                            break;
                        case /*MgPropertyType.Int64*/ 8:
                            val = String.valueOf(features.GetInt64(prop));
                            break;
                        case /*MgPropertyType.String*/ 9:
                            val = features.GetString(prop);
                            break;
                    }

                    // Generate XML to select this feature
                    //
                    MgSelection sel = new MgSelection(map);
                    if(multiIds)
                        throw new SearchError(MgLocalizer.GetString("SEARCHNOMULTIPROP", locale), searchError);  //TODO is it still unsupported in MgSelection?
                    else
                    {
                        if(i == 0)
                            idPropType = features.GetPropertyType(idPropName);
                        switch(idPropType)
                        {
                            case /*MgPropertyType.Int32*/ 7:
                                sel.AddFeatureIdInt32(layer, featureClassName, features.GetInt32(idPropName));
                                break;
                            case /*MgPropertyType.String*/ 9:
                                sel.AddFeatureIdString(layer, featureClassName, features.GetString(idPropName));
                                break;
                            case /*MgPropertyType.Int64*/ 8:
                                sel.AddFeatureIdInt64(layer, featureClassName, features.GetInt64(idPropName));
                                break;
                            case /*MgPropertyType.Double*/ 5:
                                sel.AddFeatureIdDouble(layer, featureClassName, features.GetDouble(idPropName));
                                break;
                            default:
                                throw new SearchError(MessageFormat.format(MgLocalizer.GetString("SEARCHTYYPENOTSUP", locale), new Object[] {String.valueOf(idPropType)}), searchError);
                        }
                    }
                    String selText = EscapeForHtml(sel.ToXml());

                    outStream.write("<td class=\"" + ((row%2)==0 ? "Search" : "Search2") + "\" id=\"" + row +  ":" + i + "\" onmousemove=\"SelectRow(" + row + ")\" onclick=\"CellClicked('" + selText + "')\">&nbsp;" + val + "</td>\n");
                }
                outStream.write("</tr>");
                if(++ row == matchLimit)
                    break;
            } while(features.ReadNext());
        }
        else
        {
            throw new SearchError(MgLocalizer.GetString("SEARCHNOMATCHES", locale), MgLocalizer.GetString("SEARCHREPORT", locale));
        }
    }
    catch(MgException ae)
    {
        OnError(searchError, ae.GetMessage() + "<br>" + ae.GetStackTrace(), outStream, request);
    }
    catch(SearchError exc)
    {
        OnError(exc.title, exc.getMessage(), outStream, request);
    }

    //terminate the html document
    outStream.write("</table></body></html>");
%>

<%!
void GetRequestParameters(HttpServletRequest request)
{
    userInput = GetParameter(request, "USERINPUT");
    target = GetIntParameter(request, "TGT");
    popup = GetIntParameter(request, "POPUP");
    layerName = GetParameter(request, "LAYER");
    mapName = GetParameter(request, "MAPNAME");
    sessionId = GetParameter(request, "SESSION");
    filter = GetParameter(request, "FILTER");
    matchLimit = GetIntParameter(request, "MR");
    int colCount = GetIntParameter(request, "COLS");
    locale = GetParameter(request, "LOCALE");

    if(colCount > 0)
    {
        for(int i = 0; i < colCount; i++)
        {
            resNames.add(GetParameter(request, "CN" + i));
            resProps.add(GetParameter(request, "CP" + i));
        }
    }
}

void OnError(String title, String msg, PrintWriter outStream, HttpServletRequest request) throws FileNotFoundException, IOException
{
    String templ = MgLocalizer.Localize(LoadTemplate("/viewerfiles/errorpage.templ"), locale, GetClientOS(request));
    String[] vals = { String.valueOf(popup), title, msg };
    outStream.write(Substitute(templ, vals));
}

%>
