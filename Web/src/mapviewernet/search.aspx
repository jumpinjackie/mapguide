<%--
Copyright (C) 2004-2010 by Autodesk, Inc.
This library is free software; you can redistribute it and/or
modify it under the terms of version 2.1 of the GNU Lesser
General Public License as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
--%>

<%@ Page language="c#" %>
<%@ Import Namespace="System" %>
<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="System.Collections" %>
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="System.Runtime.Serialization" %>
<%@ Import Namespace="System.Text" %>
<%@ Import Namespace="OSGeo.MapGuide" %>

<!-- #Include File="common.aspx -->

<script runat="server">
String userInput;
String locale = "";
int target;
int popup;
String layerName;
String mapName;
String sessionId;
String filter;
ArrayList resNames;
ArrayList resProps;
int matchLimit;
String searchError;
</script>

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
    MgFeatureReader features = null;

    Response.Charset = "utf-8";

    try
    {
        MgLocalizer.SetLocalizedFilesPath(Request.ServerVariables["APPL_PHYSICAL_PATH"] + "..\\localized\\");
        GetRequestParameters();

        searchError = MgLocalizer.GetString("SEARCHERROR", locale);

        try
        {
            InitializeWebTier();

            MgUserInformation cred = new MgUserInformation(sessionId);
            cred.SetClientIp(GetClientIp(Request));
            cred.SetClientAgent(GetClientAgent());

            //connect to the site and get a feature service and a resource service instances
            MgSiteConnection site = new MgSiteConnection();
            site.Open(cred);
            MgFeatureService featureSrvc = site.CreateService(MgServiceType.FeatureService) as MgFeatureService;
            MgResourceService resourceSrvc = site.CreateService(MgServiceType.ResourceService) as MgResourceService;

            //Create a temporary map runtime object, locate the layer
            MgMap map = new MgMap();
            map.Open(resourceSrvc, mapName);
            MgLayerCollection layers = map.GetLayers();
            int i = 0;
            MgLayer layer = null;
            for (i = 0; i < layers.GetCount(); i++)
            {
                layer = (MgLayer)layers.GetItem(i);
                if (layer.GetName() == layerName)
                    break;
            }

            if (layer == null)
            {
                throw new SearchError(String.Format(MgLocalizer.GetString("SEARCHLAYERNOTFOUND", locale), new Object[] { layerName }), searchError);
            }

            //substitute the input tag with the actual user input to make up the filter
            filter = filter.Replace("$USER_VARIABLE", userInput);

            //parse the match label string, which defines what columns to be displayed
            ArrayList props = new ArrayList();

            bool displayAll = (resProps.Count == 0);

            //query the features
            MgFeatureQueryOptions opts = new MgFeatureQueryOptions();
            opts.SetFilter(filter);
            String featureClassName = layer.GetFeatureClassName();
            MgResourceIdentifier srcId = new MgResourceIdentifier(layer.GetFeatureSourceId());
            features = featureSrvc.SelectFeatures(srcId, featureClassName, opts);
            bool hasResult = features.ReadNext();

            if (hasResult)
            {
                int colCount = displayAll ? features.GetPropertyCount() : resProps.Count;

                //output the beginning of the document (head section and beginning of body section)
                String templ = MgLocalizer.Localize(LoadTemplate(Request, "../viewerfiles/search.templ"), locale, GetClientOS(Request));

                String[] vals = {
                            colCount.ToString(NumberFormatInfo.InvariantInfo),
                            target.ToString(NumberFormatInfo.InvariantInfo),
                            popup.ToString(NumberFormatInfo.InvariantInfo)
                            };
                Response.Write(Substitute(templ, vals));

                MgClassDefinition classDef = features.GetClassDefinition();
                MgPropertyDefinitionCollection classDefProps = classDef.GetIdentityProperties();
                ArrayList idPropNames = new ArrayList(classDefProps.GetCount());
                for (int j = 0; j < classDefProps.GetCount(); j++)
                {
                    MgPropertyDefinition idProp = classDefProps.GetItem(j);
                    idPropNames.Add(idProp.GetName());
                }

                //table headings
                Response.Write("<tr class=\"SearchHeading\">");
                if (displayAll)
                {
                    for (i = 0; i < colCount; i++)
                    {
                        resProps.Add(features.GetPropertyName(i));
                        Response.Write("<td class=\"SearchHeading\">&nbsp;" + (String)resProps[i] + "</td>");
                    }
                }
                else
                {
                    for (i = 0; i < colCount; i++)
                        Response.Write("<td class=\"SearchHeading\">&nbsp;" + (String)resNames[i] + "</td>");
                }
                Response.Write("</tr>");

                //output the results
                int idPropType = 0;
                int row = 0;
                do
                {
                    Response.Write("<tr>");
                    for (i = 0; i < colCount; i++)
                    {
                        String prop = (String)resProps[i];
                        int propType = features.GetPropertyType(prop);
                        String val = "";
                        if (!features.IsNull(prop))
                        {
                            switch (propType)
                            {
                                case MgPropertyType.Boolean:
                                    val = features.GetBoolean(prop) ? "True" : "False";
                                    break;
                                case MgPropertyType.Single:
                                    val = features.GetSingle(prop).ToString();
                                    break;
                                case MgPropertyType.Double:
                                    val = features.GetDouble(prop).ToString();
                                    break;
                                case MgPropertyType.Int16:
                                    val = features.GetInt16(prop).ToString();
                                    break;
                                case MgPropertyType.Int32:
                                    val = features.GetInt32(prop).ToString();
                                    break;
                                case MgPropertyType.Int64:
                                    val = features.GetInt64(prop).ToString();
                                    break;
                                case MgPropertyType.String:
                                    val = features.GetString(prop);
                                    break;
                                case MgPropertyType.DateTime:
                                    val = features.GetDateTime(prop).ToString();
                                    break;
                            }
                        }

                        // Generate XML to selection this feature
                        //
                        MgSelection sel = new MgSelection(map);
                        MgPropertyCollection idProps = new MgPropertyCollection();
                        foreach (string id in idPropNames)
                        {
                            idPropType = features.GetPropertyType(id);
                            switch (idPropType)
                            {
                                case MgPropertyType.Int32:
                                    idProps.Add(new MgInt32Property(id, features.GetInt32(id)));
                                    break;
                                case MgPropertyType.String:
                                    idProps.Add(new MgStringProperty(id, features.GetString(id)));
                                    break;
                                case MgPropertyType.Int64:
                                    idProps.Add(new MgInt64Property(id, features.GetInt64(id)));
                                    break;
                                case MgPropertyType.Double:
                                    idProps.Add(new MgDoubleProperty(id, features.GetDouble(id)));
                                    break;
                                case MgPropertyType.Single:
                                    idProps.Add(new MgSingleProperty(id, features.GetSingle(id)));
                                    break;
                                case MgPropertyType.DateTime:
                                    idProps.Add(new MgDateTimeProperty(id, features.GetDateTime(id)));
                                    break;
                                default:
                                    throw new SearchError(String.Format(MgLocalizer.GetString("SEARCHTYYPENOTSUP", locale), new Object[] { idPropType.ToString() }), searchError);
                            }
                        }
                        sel.AddFeatureIds(layer, featureClassName, idProps);
                        String selText = EscapeForHtml(sel.ToXml());

                        Response.Write(String.Format("<td class=\"{0}\" id=\"{1}:{2}\" onmousemove=\"SelectRow({3})\" onclick=\"CellClicked('{4}')\">&nbsp;{5}</td>\n", (row % 2) == 0 ? "Search" : "Search2", row, i, row, selText, val));
                    }
                    Response.Write("</tr>");
                    if (++row == matchLimit)
                        break;
                } while (features.ReadNext());
            }
            else
            {
                throw new SearchError(MgLocalizer.GetString("SEARCHNOMATCHES", locale), MgLocalizer.GetString("SEARCHREPORT", locale));
            }
        }
        catch (MgException ae)
        {
            if (features != null)
            {
                // Close the feature reader
                features.Close();
            }
            OnError(searchError, ae.GetMessage() + "<br>" + ae.GetStackTrace());
        }
        catch (SearchError exc)
        {
            if (features != null)
            {
                // Close the feature reader
                features.Close();
            }
            OnError(exc.title, exc.Message);
        }


        //terminate the html document
        Response.Write("</table></body></html>");
        Response.Flush();
    }
    finally
    {
    }
%>

<script runat="server">
void GetRequestParameters()
{
    if(Request.HttpMethod == "POST")
        GetParameters(Request.Form);
    else
        GetParameters(Request.QueryString);
}

void GetParameters(NameValueCollection parameters)
{
    locale = GetParameter(parameters, "LOCALE");
    userInput = GetParameter(parameters, "USERINPUT");
    target = GetIntParameter(parameters, "TGT");
    popup = GetIntParameter(parameters, "POPUP");
    layerName = GetParameter(parameters, "LAYER");
    mapName = GetParameter(parameters, "MAPNAME");
    sessionId = GetParameter(parameters, "SESSION");
    filter = GetParameter(parameters, "FILTER");
    matchLimit = GetIntParameter(parameters, "MR");
    int colCount = GetIntParameter(parameters, "COLS");

    if(colCount > 0)
    {
        for(int i = 0; i < colCount; i++)
        {
            resNames.Add(parameters["CN" + i]);
            resProps.Add(parameters["CP" + i]);
        }
    }
}

void OnError(String title, String msg)
{
    //erase the current output, to produce a clean error report
    Response.Clear();

    String templ = MgLocalizer.Localize(LoadTemplate(Request, "../viewerfiles/errorpage.templ"), locale, GetClientOS(Request));
    String[] vals = { popup.ToString(NumberFormatInfo.InvariantInfo), title, msg };
    Response.Write(Substitute(templ, vals));
}

public class SearchError : Exception
{
public String title;

public SearchError(String message) : base(message)
{
}

public SearchError(String message, Exception innerException) : base (message, innerException)
{
}

public SearchError(String message, String title) : base (message)
{
    this.title = title;
}

protected SearchError(SerializationInfo info, StreamingContext context) : base (info, context)
{}

}
</script>
