<%--
Copyright (C) 2004-2011 by Autodesk, Inc.
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

<%@ Import Namespace="System" %>
<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="System.Text" %>
<%@ Import Namespace="System.Globalization" %>
<%@ Import Namespace="OSGeo.MapGuide" %>

<script language="C#" runat="server">

void InitializeWebTier()
{
    // Initialize web tier with the site configuration file.  The config
    // file should be in the same directory as this script.
    MapGuideApi.MgInitializeWebTier(Request.ServerVariables["APPL_PHYSICAL_PATH"] + "../webconfig.ini");
}

String GetDefaultLocale()
{
    return "en"; // localizable string
}

String Substitute(String templ, String[] vals)
{
    StringBuilder res = new StringBuilder();
    int index = 0, val = 0;
    bool found;
    do
    {
        found = false;
        int i = templ.IndexOf('%', index);
        if(i != -1)
        {
            found = true;
            res.Append(templ.Substring(index, i - index));
            if(i < templ.Length - 1)
            {
                if(templ[i+1] == '%')
                    res.Append('%');
                else if(templ[i+1] == 's')
                    res.Append(vals[val ++]);
                else
                    res.Append('@');    //add a character illegal in jscript so we know the template was incorrect
                index = i + 2;
            }
        }
    } while(found);
    res.Append(templ.Substring(index));
    return res.ToString();
}

bool IsParameter(NameValueCollection parameters, String name)
{
    return parameters[name] != null;
}

String GetParameter(NameValueCollection parameters, String name)
{
    String strval = parameters[name];
    if (null == strval)
        return "";

    return strval.Trim();
}

int GetIntParameter(NameValueCollection parameters, String name)
{
    String strval = GetParameter(parameters, name);
    if ("" == strval)
        return 0;
    if(System.Text.RegularExpressions.Regex.IsMatch(strval,@"^[0-9]*$"))
        return Convert.ToInt32(strval);
    else
        return 0;
}

double GetDoubleParameter(NameValueCollection parameters, String name)
{
    String strval = GetParameter(parameters, name);
    if ("" == strval)
        return 0;
    if(System.Text.RegularExpressions.Regex.IsMatch(strval,@"^([-]{0,1})(\d+)([.]{0,1})(\d*)$"))
        return Convert.ToDouble(strval, NumberFormatInfo.InvariantInfo);
    else
        return 0;

}

double GetLocalizedDoubleParameter(NameValueCollection parameters, String name, String locale)
{
    String strval = GetParameter(parameters, name);
    if ("" == strval)
        return 0;

    if(locale != null && locale.Length > 0)
    {
        //Remove thousand separators
        String thousandSeparator = MgLocalizer.GetString("THOUSANDSEPARATOR", locale);
        if(thousandSeparator != null && thousandSeparator.Length > 0)
        {
            strval = strval.Replace(thousandSeparator, "");
        }

        //Replace localized decimal separator with "."
        String decimalSeparator = MgLocalizer.GetString("DECIMALSEPARATOR", locale);
        if(decimalSeparator != null && decimalSeparator.Length > 0 && decimalSeparator != ".")
        {
            strval = strval.Replace(decimalSeparator, ".");
        }
    }

    return Convert.ToDouble(strval, NumberFormatInfo.InvariantInfo);

}

String LoadTemplate(HttpRequest request, String filePath)
{
    StreamReader sr = File.OpenText(request.ServerVariables["APPL_PHYSICAL_PATH"]+filePath);
    String template = sr.ReadToEnd();
    sr.Close();
    return template;

}

String GetSurroundVirtualPath(HttpRequest request) {
    String path = request.ServerVariables["SCRIPT_NAME"];
    return path.Substring(0, path.LastIndexOf('/') + 1);
}

String GetRootVirtualFolder(HttpRequest request) {
    String path = request.ServerVariables["URL"];
    return path.Substring(0, path.IndexOf('/', 1));
}

int GetClientOS(HttpRequest request) {
    String agent = request.Headers["User-Agent"];
    if(agent != null) {
        if(agent.IndexOf("Windows") != -1)
            return 0;
        else if(agent.IndexOf("Macintosh") != -1)
            return 1;
        else
            return 2;
    }
    return 0;
}


String EscapeForHtml(String str)
{
    str = str.Replace("'", "&#39;");
    str = str.Replace("\"", "&quot;");
    str = str.Replace("<", "&lt;");
    str = str.Replace(">", "&gt;");
    str = str.Replace("\\n", "<br>");
    return str;
}

String GetClientIp(HttpRequest request)
{
    String result = "";
    String httpClientIp = request.ServerVariables["HTTP_CLIENT_IP"];
    String httpXFF = request.ServerVariables["HTTP_X_FORWARDED_FOR"];
    String remoteAddr = request.ServerVariables["REMOTE_ADDR"];

    if (httpClientIp != null && "" != httpClientIp && String.Compare(httpClientIp, "unknown", true) != 0)
        result = httpClientIp;
    else if (httpXFF != null && "" != httpXFF && String.Compare(httpXFF, "unknown", true) != 0)
        result = httpXFF;
    else if (remoteAddr != null)
        result =  remoteAddr;

    return result;
}

String GetClientAgent()
{
    return "Ajax Viewer";
}

String ValidateSessionId(String proposedSessionId)
{
    // 00000000-0000-0000-0000-000000000000_aa_[aaaaaaaaaaaaa]000000000000
    // the [aaaaaaaaaaaaa] is a based64 string and in variant length
    String validSessionId = "";
    if(proposedSessionId != null && System.Text.RegularExpressions.Regex.IsMatch(proposedSessionId,
        "^[A-Fa-f0-9]{8}-[A-Fa-f0-9]{4}-[A-Fa-f0-9]{4}-[A-Fa-f0-9]{4}-[A-Fa-f0-9]{12}_[A-Za-z]{2}_\\w+[A-Fa-f0-9]{12}$"))
    {
        validSessionId = proposedSessionId;
    }
    return validSessionId;
}

String ValidateLocaleString(String proposedLocaleString)
{
    // aa or aa-aa
    String validLocaleString = GetDefaultLocale(); // Default
    if(proposedLocaleString != null && (System.Text.RegularExpressions.Regex.IsMatch(proposedLocaleString, "^[A-Za-z]{2}$") ||
        System.Text.RegularExpressions.Regex.IsMatch(proposedLocaleString, "^[A-Za-z]{2}-[A-Za-z]{2}$")))
    {
        validLocaleString = proposedLocaleString;
    }
    return validLocaleString;
}

String ValidateHyperlinkTargetValue(String proposedHyperlinkTarget)
{
    // 1, 2 or 3
    String validHyperlinkTarget = "1"; // Default
    if(proposedHyperlinkTarget != null && System.Text.RegularExpressions.Regex.IsMatch(proposedHyperlinkTarget, "^[1-3]$"))
    {
        validHyperlinkTarget = proposedHyperlinkTarget;
    }
    return validHyperlinkTarget;
}

String ValidateFrameName(String proposedFrameName)
{
    // Allowing alphanumeric characters and underscores in the frame name
    String validFrameName = "";
    if(proposedFrameName != null && System.Text.RegularExpressions.Regex.IsMatch(proposedFrameName, "^[a-zA-Z0-9_]*$"))
    {
        validFrameName = proposedFrameName;
    }
    return validFrameName;
}

String ValidateIntegerString(String proposedNumberString)
{
    // Allow numeric characters only
    String validNumberString = "";
    if(proposedNumberString != null && System.Text.RegularExpressions.Regex.IsMatch(proposedNumberString, "^[0-9]*$"))
    {
        validNumberString = proposedNumberString;
    }
    return validNumberString;
}

String ValidateResourceId(String proposedResourceId)
{
    String validResourceId = "";
    try
    {
        MgResourceIdentifier resId = new MgResourceIdentifier(proposedResourceId);
        validResourceId = resId.ToString();
    }
    catch(MgException)
    {
        validResourceId = "";
    }
    return validResourceId;
}

String ValidateMapName(String proposedMapName)
{
    String validMapName = "";
    if (proposedMapName.IndexOfAny("*:|?<'&\">=".ToCharArray()) < 0)
    {
        validMapName = proposedMapName;
    }
    return validMapName;
}

String ValidateColorString(String proposedColorString)
{
    return ValidateColorString(proposedColorString, 6);
}

String ValidateColorString(String proposedColorString, int charCount)
{
    String validColorString = "000000";
    if (proposedColorString != null &&
        System.Text.RegularExpressions.Regex.IsMatch(proposedColorString, "^[A-Fa-f0-9]{" + charCount + "}$"))
    {
        validColorString = proposedColorString;
    }
    return validColorString;
}

MgFeatureQueryOptions BuildFeatureQueryOptions(MgClassDefinition classDef)
{
    MgFeatureQueryOptions query = new MgFeatureQueryOptions();
    String geomPropName = classDef.GetDefaultGeometryPropertyName();
    MgPropertyDefinitionCollection propertyList = classDef.GetProperties();
    for (int i = 0; i < propertyList.GetCount(); i++)
    {
        MgPropertyDefinition propertyDef = propertyList.GetItem(i);
        String property = propertyDef.GetName();

        if ((property != geomPropName) && (propertyDef.GetPropertyType() == MgFeaturePropertyType.DataProperty)) 
        {
            MgDataPropertyDefinition dp = (MgDataPropertyDefinition)propertyDef;
            int propertyType = dp.GetDataType();
            switch (propertyType) {
                case MgPropertyType.Boolean:
                    query.AddFeatureProperty(property);
                    break;
                case MgPropertyType.Byte:
                    query.AddFeatureProperty(property);
                    break;
                case MgPropertyType.DateTime:
                    query.AddFeatureProperty(property);
                    break;
                case MgPropertyType.Single:
                    query.AddFeatureProperty(property);
                    break;
                case MgPropertyType.Double:
                    query.AddFeatureProperty(property);
                    break;
                case MgPropertyType.Int16:
                    query.AddFeatureProperty(property);
                    break;
                case MgPropertyType.Int32:
                    query.AddFeatureProperty(property);
                    break;
                case MgPropertyType.Int64:
                    query.AddFeatureProperty(property);
                    break;
                case MgPropertyType.String:
                    query.AddFeatureProperty(property);
                    break;
            }
        } else if (property == geomPropName) { 
            query.AddFeatureProperty(property);
        } 
    }
    return query;
}

</script>
