<%--
Copyright (C) 2004-2009 by Autodesk, Inc.
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

<script runat="server">

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
    return Convert.ToInt32(strval);
}

double GetDoubleParameter(NameValueCollection parameters, String name)
{
    String strval = GetParameter(parameters, name);
    if ("" == strval)
        return 0;
    return Convert.ToDouble(strval, NumberFormatInfo.InvariantInfo);

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
    str = str.Replace("\n", " ");
    str = str.Replace("<", "&lt;");
    str = str.Replace(">", "&gt;");
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
</script>
