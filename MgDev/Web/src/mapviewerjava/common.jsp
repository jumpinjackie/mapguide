<%--
  -Copyright (C) 2004-2011 by Autodesk, Inc.
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
<%@ page import="java.util.regex.*" %>
<%@ page import="java.io.*" %>
<%@ page import="java.net.*" %>
<%@ page import="javax.servlet.jsp.*" %>
<%@ page import="javax.servlet.http.*" %>
<%@ page isThreadSafe="false" %>

<%!
void InitializeWebTier() throws MgException
{
    // Initialize web tier with the site configuration file.  The config
    // file should be in the same folder as this script.
    //
    String realpath = getServletContext().getRealPath("/");
    String configPath =  realpath + "webconfig.ini";
    MapGuideJavaApi.MgInitializeWebTier(configPath);
}

String GetDefaultLocale()
{
    return "en"; // localizable string
}

String GetParameter(HttpServletRequest request, String name)
{
    String strval = request.getParameter(name);
    if(strval == null)
        return "";
    return strval;
}

int GetIntParameter(HttpServletRequest request, String name)
{
    String strval = GetParameter(request, name);
    if(strval.equals(""))
        return 0;
    if(Pattern.matches("^[0-9]*$", strval))
        return Integer.parseInt(strval);
    else
        return 0;
}

double GetDoubleParameter(HttpServletRequest request, String name)
{
    String strval = GetParameter(request, name);
    if(strval.equals(""))
        return 0;
    if(Pattern.matches("^([-]{0,1})(\\d+)([.]{0,1})(\\d*)$", strval))
        return Double.parseDouble(strval);
    else
        return 0;
}

double GetLocalizedDoubleParameter(HttpServletRequest request, String name, String locale)
{
    String strval = GetParameter(request, name);
    if(strval.equals(""))
        return 0;

    if(locale != null && locale.length() > 0)
    {
        //Remove thousand separators
        String thousandSeparator = MgLocalizer.GetString("THOUSANDSEPARATOR", locale);
        if(thousandSeparator != null && thousandSeparator.length() > 0)
        {
            strval = strval.replace(thousandSeparator, "");
        }

        //Replace localized decimal separator with "."
        String decimalSeparator = MgLocalizer.GetString("DECIMALSEPARATOR", locale);
        if(decimalSeparator != null && decimalSeparator.length() > 0 && !decimalSeparator.equals("."))
        {
            strval = strval.replace(decimalSeparator, ".");
        }
    }

    return Double.parseDouble(strval);

}

boolean IsParameter(HttpServletRequest request, String name)
{
    return request.getParameter(name) != null;
}

String LoadTemplate(String filename) throws FileNotFoundException, IOException
{
    String realpath = getServletContext().getRealPath("/");
    String templPath =  realpath + File.separator + filename;

    File theFile = new File(templPath);
    int size = (int)theFile.length();
    FileInputStream is = new FileInputStream(theFile);
    byte[] buffer = new byte[size];
    is.read(buffer);
    is.close();
    return new String(buffer, "UTF-8");
}

String Substitute(String templ, String[] vals)
{
    StringBuffer res = new StringBuffer();
    int index = 0, val = 0;
    boolean found;
    do
    {
        found = false;
        int i = templ.indexOf('%', index);
        if(i != -1)
        {
            found = true;
            res.append(templ.substring(index, i));
            if(i < templ.length() - 1)
            {
                if(templ.charAt(i+1) == '%')
                    res.append('%');
                else if(templ.charAt(i+1) == 's')
                {
                    res.append(vals[val ++]);
                }
                else
                    res.append('@');    //add a character illegal in jscript so we know the template was incorrect
                index = i + 2;
            }
        }
    } while(found);
    res.append(templ.substring(index));
    return res.toString();
}

String GetSurroundVirtualPath(HttpServletRequest request) {
    String uri = request.getRequestURI();
    return uri.substring(0, uri.lastIndexOf('/') + 1);
}

String GetRootVirtualFolder(HttpServletRequest request) {
    return request.getContextPath();
}

int GetClientOS(HttpServletRequest request) {
    String agent = request.getHeader("User-Agent");
    if(agent != null) {
        if(agent.indexOf("Windows") != -1)
            return 0;
        else if(agent.indexOf("Macintosh") != -1)
            return 1;
        else
            return 2;
    }
    return 0;
}

String EscapeForHtml(String str)
{
    str = str.replaceAll("'", "&#39;");
    str = str.replaceAll("\"", "&quot;");
    str = str.replaceAll("<", "&lt;");
    str = str.replaceAll(">", "&gt;");
    str = str.replaceAll("\\\\n", "<br>");
    return str;
}

String GetClientIp(HttpServletRequest request)
{
    String result = "";
    String httpClientIp = request.getHeader("HTTP_CLIENT_IP");
    String httpXFF = request.getHeader("HTTP_X_FORWARDED_FOR");
    String remoteAddr = request.getRemoteAddr();

    if(httpClientIp != null && !httpClientIp.equals("") && !httpClientIp.equalsIgnoreCase("unknown"))
        result = httpClientIp;
    else if(httpXFF != null && !httpXFF.equals("") && !httpXFF.equalsIgnoreCase("unknown"))
        result = httpXFF;
    else if (remoteAddr != null)
        result = remoteAddr;

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
    if(proposedSessionId != null &&
        Pattern.matches("^[A-Fa-f0-9]{8}-[A-Fa-f0-9]{4}-[A-Fa-f0-9]{4}-[A-Fa-f0-9]{4}-[A-Fa-f0-9]{12}_[A-Za-z]{2}_\\w+[A-Fa-f0-9]{12}$", proposedSessionId))
    {
        validSessionId = proposedSessionId;
    }
    return validSessionId;
}

String ValidateLocaleString(String proposedLocaleString)
{
    // aa or aa-aa
    String validLocaleString = GetDefaultLocale(); // Default
    if(proposedLocaleString != null &&
        (Pattern.matches("^[A-Za-z]{2}$", proposedLocaleString) || Pattern.matches("^[A-Za-z]{2}-[A-Za-z]{2}$", proposedLocaleString)))
    {
        validLocaleString = proposedLocaleString;
    }
    return validLocaleString;
}

String ValidateHyperlinkTargetValue(String proposedHyperlinkTarget)
{
    // 1, 2 or 3
    String validHyperlinkTarget = "1"; // Default
    if(proposedHyperlinkTarget != null && Pattern.matches("^[1-3]$", proposedHyperlinkTarget))
    {
        validHyperlinkTarget = proposedHyperlinkTarget;
    }
    return validHyperlinkTarget;
}

String ValidateFrameName(String proposedFrameName)
{
    // Allowing alphanumeric characters and underscores in the frame name
    String validFrameName = "";
    if(proposedFrameName != null && Pattern.matches("^[a-zA-Z0-9_]*$", proposedFrameName))
    {
        validFrameName = proposedFrameName;
    }
    return validFrameName;
}

String ValidateIntegerString(String proposedNumberString)
{
    // Allow numeric characters only
    String validNumberString = "";
    if(proposedNumberString != null && Pattern.matches("^[0-9]*$", proposedNumberString))
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
    catch(MgException e)
    {
        validResourceId = "";
    }
    return validResourceId;
}

String ValidateMapName(String proposedMapName)
{
    String validMapName = "";
    if (proposedMapName != null && Pattern.matches("^[^\\*:|\\?<'&\">=]*$", proposedMapName))
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
        Pattern.matches("^[A-Fa-f0-9]{" + charCount + "}$", proposedColorString))
    {
        validColorString = proposedColorString;
    }
    return validColorString;
}

MgFeatureQueryOptions BuildFeatureQueryOptions(MgClassDefinition classDef) throws MgException
{
    MgFeatureQueryOptions query = new MgFeatureQueryOptions();
    String geomPropName = classDef.GetDefaultGeometryPropertyName();
    MgPropertyDefinitionCollection propertyList = classDef.GetProperties();
    for (int i = 0; i < propertyList.GetCount(); i++)
    {
        MgPropertyDefinition propertyDef = propertyList.GetItem(i);
        String property = propertyDef.GetName();

        if ((!property.equals(geomPropName)) && (propertyDef.GetPropertyType() == MgFeaturePropertyType.DataProperty)) 
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
        } else if (property.equals(geomPropName)) { 
            query.AddFeatureProperty(property);
        } 
    }
    return query;
}

%>
