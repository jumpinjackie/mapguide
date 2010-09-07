<%--
  -Copyright (C) 2004-2010 by Autodesk, Inc.
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
<%@ page import="java.util.*" %>
<%@ page import="java.awt.*" %>
<%@ page import="java.awt.image.*" %>
<%@ page import="java.awt.geom.*" %>
<%@ page import="javax.imageio.*" %>
<%@ page import="javax.net.*" %>

<%@ include file="common.jsp" %>
<%@ page isThreadSafe="false" %>

<%!
String mapName;
String sessionId;
double rotation = 0.0;
int printDpi    = 300;
int scaleDenominator = 0;
MgPolygon captureBox = null;
MgPolygon normalizedCapture = null;
Size<Integer> printSize = null;
Size<Double> paperSize = null;
String mapAgentPath = "";
String naPath = "";
%>

<%
    
    GetRequestParameters(request);
    response.setContentType("image/png");
    ImageIO.write(GenerateMap(printSize), "png", response.getOutputStream());

%>

<%!
void GetRequestParameters(HttpServletRequest request) throws MgException
{
    sessionId = ValidateSessionId(GetParameter(request, "session_id"));
    mapName   = ValidateMapName(GetParameter(request, "map_name"));
    rotation  = GetDoubleParameter(request, "rotation");
    printDpi  = GetIntParameter(request, "print_dpi");
    scaleDenominator = GetIntParameter(request, "scale_denominator");
    
    String[] a = GetParameter(request, "paper_size").split(",");
    paperSize  = new Size<Double>(Double.parseDouble(a[0]), Double.parseDouble(a[1]));
    printSize  = new Size<Integer>((int) (paperSize.width / 25.4 * printDpi), (int) (paperSize.height / 25.4 * printDpi));
    
    a = GetParameter(request, "box").split(",");
    captureBox = CreatePolygon(a);
    
    a = GetParameter(request, "normalized_box").split(",");
    normalizedCapture = CreatePolygon(a);
    
    mapAgentPath = GetMapAgentPath(request);
    naPath = getServletContext().getRealPath("viewerfiles/quickplotnortharrow.png");
}

MgPolygon CreatePolygon(String[] coordinates) throws MgException
{
    MgGeometryFactory geometryFactory = new MgGeometryFactory();
    MgCoordinateCollection coordinateCollection = new MgCoordinateCollection();
    
    for (int i = 0; i < coordinates.length; ++i)
    {
        coordinateCollection.Add(geometryFactory.CreateCoordinateXY(Double.parseDouble(coordinates[i]), Double.parseDouble(coordinates[++i])));
    }
    
    coordinateCollection.Add(geometryFactory.CreateCoordinateXY(Double.parseDouble(coordinates[0]), Double.parseDouble(coordinates[1])));
    MgLinearRing linearRing = geometryFactory.CreateLinearRing(coordinateCollection);
    
    return geometryFactory.CreatePolygon(linearRing, null);
}

BufferedImage GenerateMap(Size<Integer> size) throws MgException, IOException
{
    MgUserInformation userInfo = new MgUserInformation(sessionId);
    MgSiteConnection siteConnection = new MgSiteConnection();
    siteConnection.Open(userInfo);
    MgResourceService resourceService = (MgResourceService) siteConnection.CreateService(MgServiceType.ResourceService);
    MgRenderingService renderingService = (MgRenderingService) siteConnection.CreateService(MgServiceType.RenderingService);
    
    MgMap map = new MgMap();
    map.Open(resourceService, mapName);
    
    MgSelection selection = new MgSelection(map);
    
    // Caculate the generated picture size
    MgEnvelope envelope = captureBox.Envelope();
    MgEnvelope normalizedE = normalizedCapture.Envelope();
    Size<Double> size1 = new Size<Double>(envelope.GetWidth(), envelope.GetHeight());
    Size<Double> size2 = new Size<Double>(normalizedE.GetWidth(), normalizedE.GetHeight());
    
    Size<Double> toSize = new Size<Double>(size1.width / size2.width * size.width, size1.height / size2.height * size.height);
    MgCoordinate center = captureBox.GetCentroid().GetCoordinate();
    
    // Get the map agent url
    // Get the correct http protocol
    StringBuilder mapAgent = new StringBuilder(mapAgentPath);
    mapAgent.append("?VERSION=1.0.0&OPERATION=GETMAPIMAGE")
            .append("&SESSION=").append(sessionId)
            .append("&MAPNAME=").append(mapName)
            .append("&FORMAT=PNG")
            .append("&SETVIEWCENTERX=").append(center.GetX())
            .append("&SETVIEWCENTERY=").append(center.GetY())
            .append("&SETVIEWSCALE=").append(scaleDenominator)
            .append("&SETDISPLAYDPI=").append(printDpi)
            .append("&SETDISPLAYWIDTH=").append(String.valueOf(toSize.width))
            .append("&SETDISPLAYHEIGHT=").append(String.valueOf(toSize.height))
            .append("&CLIP=0");
            
    BufferedImage image  = ImageIO.read(new URL(mapAgent.toString()));
    BufferedImage result = Math.abs(rotation) > Double.MIN_VALUE ? new BufferedImage(size.width, size.height, BufferedImage.TYPE_INT_RGB) : image;
    Graphics2D graphics  = result.createGraphics();
    
    if (Math.abs(rotation) > Double.MIN_VALUE)
    {
        graphics.translate(size.width / 2, size.height / 2);
        graphics.rotate(rotation * Math.PI / 180);
        graphics.drawImage(image, new AffineTransformOp(new AffineTransform(), AffineTransformOp.TYPE_BICUBIC), -image.getWidth() / 2, -image.getHeight() / 2);
    }

    graphics.dispose();
    
    DrawNorthArrow(result);
    
    return result;
}

void DrawNorthArrow(BufferedImage image) throws IOException
{
    // The north arrow image is created under this dpi
    int naDpi = 300;
    // The north arrow will be put at the bottom-right corner with a margin 12 MM
    int naMargin = 12;
    double scaleFactor = (double) printDpi / naDpi;

    BufferedImage na  = ImageIO.read(new File(naPath));
    Rectangle2D rec = new Rectangle2D.Double();
    rec.setRect(0.0, 0.0, na.getWidth(), na.getHeight());
    
    Path2D.Double path = new Path2D.Double();
    path.moveTo(0, 0);
    path.lineTo(na.getWidth(), 0);
    path.lineTo(na.getWidth(), na.getHeight());
    path.lineTo(0, na.getHeight());

    AffineTransform transform = new AffineTransform();
    transform.rotate(rotation * Math.PI / 180, (double) na.getWidth() / 2, (double) na.getHeight() / 2);
    transform.scale(scaleFactor, scaleFactor);
    path.transform(transform);
    
    rec = path.getBounds2D();

    BufferedImage rotatedNA = na;
    // Transform the northarrow image if it's necessary to rotate it or scale it
    if (Math.abs(rotation) > Double.MIN_VALUE || Math.abs(scaleFactor - 1.0) > Double.MIN_VALUE)
    {
        GraphicsConfiguration gc = na.createGraphics().getDeviceConfiguration();
        rotatedNA = gc.createCompatibleImage((int) Math.ceil(rec.getWidth()), (int) Math.ceil(rec.getHeight()), Transparency.TRANSLUCENT);
        Graphics2D nag = rotatedNA.createGraphics();
        nag.setComposite(AlphaComposite.Src);
        nag.translate(rec.getWidth() / 2, rec.getHeight() / 2);
        nag.rotate(rotation * Math.PI / 180);
        nag.scale(scaleFactor, scaleFactor);
        nag.drawImage(na, new AffineTransformOp(new AffineTransform(), AffineTransformOp.TYPE_BICUBIC), (int) Math.ceil(-na.getWidth() / 2), (int) Math.ceil(-na.getHeight() / 2));
        nag.dispose();
    }
    
    int x = (int) Math.ceil(image.getWidth() - rotatedNA.getWidth() - naMargin / 25.4 * printDpi);;
    int y = (int) Math.ceil(image.getHeight() - rotatedNA.getHeight() - naMargin / 25.4 * printDpi);
    
    Graphics2D graphics = image.createGraphics();
    graphics.drawImage(rotatedNA, new AffineTransformOp(new AffineTransform(), AffineTransformOp.TYPE_BICUBIC), x, y);
    graphics.dispose();
}

String GetMapAgentPath(HttpServletRequest request)
{
    // Get the map agent url
    // Get the correct http protocol
    StringBuilder mapAgent = new StringBuilder(request.getScheme());
    mapAgent.append("://");
    // Just use the 127.0.0.1 specificly to point to localhost. Because the WebExtension will
    // be always on the same server with map agent. 
    mapAgent.append("127.0.0.1").append(":");
    // Get the correct port number;
    mapAgent.append(request.getServerPort());
    // Get the correct virtual directory
    mapAgent.append(request.getContextPath());
    mapAgent.append("/mapagent/mapagent.fcgi");
    
    return mapAgent.toString();
}
%>

<%!
    class Size<T>
    {
        public T width;
        public T height;
        
        public Size(T width, T height)
        {
            this.width  = width;
            this.height = height;
        }
    }
%>