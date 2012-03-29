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
<%@ Import Namespace="System.Web" %>
<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="System.Text" %>
<%@ Import Namespace="System.Drawing" %>
<%@ Import Namespace="OSGeo.MapGuide" %>
<%@ Import Namespace="System.Net" %>
<%@ Import Namespace="System.Drawing.Drawing2D" %>
<%@ Import Namespace="System.Globalization" %>

<!-- #Include File="common.aspx -->

<script runat="server">
string sessionID = "";
string mapName   = "";
double rotation  = 0.0;
int printDpi     = 300;
int scaleDenominator = 0;
MgPolygon captureBox = null;
MgPolygon normalizedCapture;
Size printSize = new Size(0, 0);
SizeF paperSize = new SizeF(0.0f, 0.0f);

</script>

<%
    Response.Charset = "utf-8";
    
    try
    {
        GetRequestParameters();
        GenerateMap(printSize);
    }
    catch (MgException e)
    {
        Response.ContentType = "text/html";
        Response.Write(string.Format("ERROR: {0} <br />", e.GetExceptionMessage()));
        Response.Write(string.Format("{0} <br />", e.GetStackTrace()));
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
    sessionID = ValidateSessionId(GetParameter(parameters, "session_id"));
    mapName = ValidateMapName(GetParameter(parameters, "map_name"));
    rotation = GetDoubleParameter(parameters, "rotation");
    printDpi = GetIntParameter(parameters, "print_dpi");

    scaleDenominator = GetIntParameter(parameters, "scale_denominator");

    string[] a = parameters["paper_size"].Split(',');
    paperSize  = new SizeF(float.Parse(a[0], CultureInfo.InvariantCulture), float.Parse(a[1], CultureInfo.InvariantCulture));
    printSize  = new Size((int) (paperSize.Width / 25.4 * printDpi), (int) (paperSize.Height / 25.4 * printDpi));

    a = parameters["box"].Split(',');
    captureBox = CreatePolygon(a);

    a = parameters["normalized_box"].Split(',');
    normalizedCapture = CreatePolygon(a);
}

MgPolygon CreatePolygon(string[] coordinates)
{
    MgGeometryFactory geometryFactory = new MgGeometryFactory();
    MgCoordinateCollection coordinateCollection = new MgCoordinateCollection();

    for (int i = 0; i < coordinates.Length; ++i)
    {
        coordinateCollection.Add(geometryFactory.CreateCoordinateXY(double.Parse(coordinates[i], CultureInfo.InvariantCulture), double.Parse(coordinates[++i], CultureInfo.InvariantCulture)));
    }

    coordinateCollection.Add(geometryFactory.CreateCoordinateXY(double.Parse(coordinates[0], CultureInfo.InvariantCulture), double.Parse(coordinates[1], CultureInfo.InvariantCulture)));
    MgLinearRing linearRing = geometryFactory.CreateLinearRing(coordinateCollection);

    return geometryFactory.CreatePolygon(linearRing, null);
}

void GenerateMap(Size size)
{
    InitializeWebTier();
    
    MgUserInformation userInfo = new MgUserInformation(sessionID);
    MgSiteConnection siteConnection = new MgSiteConnection();
    siteConnection.Open(userInfo);
    MgResourceService resourceService = siteConnection.CreateService(MgServiceType.ResourceService) as MgResourceService;
    MgRenderingService renderingService = siteConnection.CreateService(MgServiceType.RenderingService) as MgRenderingService;

    MgMap map = new MgMap();
    map.Open(resourceService, mapName);

    MgSelection selection = new MgSelection(map);

    // Calculate the generated picture size
    MgEnvelope envelope = captureBox.Envelope();
    MgEnvelope normalizedE = normalizedCapture.Envelope();
    SizeF size1 = new SizeF((float) envelope.GetWidth(), (float) envelope.GetHeight());
    SizeF size2 = new SizeF((float) normalizedE.GetWidth(), (float) normalizedE.GetHeight());

    SizeF toSize = new SizeF(size1.Width / size2.Width * size.Width, size1.Height / size2.Height * size.Height);
    MgCoordinate center = captureBox.GetCentroid().GetCoordinate();

    map.SetDisplayDpi(printDpi);
    string colorString = map.GetBackgroundColor();
    // The returned color string is in AARRGGBB format. But the constructor of MgColor needs a string in RRGGBBAA format
    colorString = string.Format("{0}{1}", colorString.Substring(2, 6), colorString.Substring(0, 2));
    MgColor color = new MgColor(colorString);

    MgByteReader mgReader = renderingService.RenderMap(map,
                                                        selection,
                                                        center,
                                                        scaleDenominator,
                                                        (int) toSize.Width,
                                                        (int) toSize.Height,
                                                        color,
                                                        "PNG",
                                                        false);

    string tempImage = Path.GetTempFileName();
    mgReader.ToFile(tempImage);

    using (System.Drawing.Image image = Bitmap.FromFile(tempImage))
    {
        using (System.Drawing.Image result = Math.Abs(rotation) > double.Epsilon ? new Bitmap(size.Width, size.Height) : image)
        {
            using (Graphics graphics = Graphics.FromImage(result))
            {
                if (Math.Abs(rotation) > double.Epsilon)
                {
                    graphics.TranslateTransform(size.Width / 2, size.Height / 2);
                    graphics.RotateTransform((float)rotation);
                    graphics.DrawImage(image, -image.Width / 2, -image.Height / 2);
                }
            }

            DrawNorthArrow(result);

            using (MemoryStream stream = new MemoryStream())
            {
                result.Save(stream, System.Drawing.Imaging.ImageFormat.Png);
                Response.ContentType = "image/png";
                Response.BinaryWrite(stream.ToArray());
            }
        }
    }
        
    File.Delete(tempImage);
}

void DrawNorthArrow(System.Drawing.Image image)
{
    // The north arrow image is created under this dpi
    int naDpi = 300;
    // The north arrow will be put at the bottom-right corner with a margin 12 MM
    int naMargin = 12;
    string naPath = Server.MapPath("../viewerfiles/quickplotnortharrow.png");
    using (Graphics graphics = Graphics.FromImage(image))
    {
        using (System.Drawing.Image na = Bitmap.FromFile(naPath))
        {
            float scaleFactor = (float) printDpi / naDpi;
            RectangleF rec = new RectangleF(new PointF(0.0f, 0.0f), new SizeF(na.Width * scaleFactor, na.Height * scaleFactor));

            if (Math.Abs(rotation) > double.Epsilon)
            {
                GraphicsUnit pixelUnit = GraphicsUnit.Pixel;
                Region region = new Region(na.GetBounds(ref pixelUnit));
                Matrix matrix = new Matrix();
                matrix.RotateAt((float) rotation, new PointF((float)na.Width / 2, (float)na.Height / 2));
                matrix.Scale(scaleFactor, scaleFactor);
                region.Transform(matrix);
                rec = region.GetBounds(graphics);
            }

            using (System.Drawing.Image rotatedNA = new Bitmap((int) Math.Ceiling(rec.Width), (int) Math.Ceiling(rec.Height)))
            {
                using (Graphics nag = Graphics.FromImage(rotatedNA))
                {
                    nag.TranslateTransform((float)rotatedNA.Width / 2, (float)rotatedNA.Height / 2);
                    nag.RotateTransform((float) rotation);
                    nag.ScaleTransform(scaleFactor, scaleFactor);
                    nag.DrawImage(na, -na.Width / 2, -na.Height / 2);
                }

                double x = image.Width - rotatedNA.Width - naMargin / 25.4 * printDpi;
                double y = image.Height - rotatedNA.Height - naMargin / 25.4 * printDpi;
                graphics.DrawImage(rotatedNA, new PointF((float)x, (float)y));
            }
        }
    }
}

</script>
