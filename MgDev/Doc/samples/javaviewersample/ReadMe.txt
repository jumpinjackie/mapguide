SETUP GUIDE
-----------

1. Open the file C:\Program Files\Autodesk\MapGuideEnterprise2008\WebServerExtensions\Apache2\conf\httpd.conf. Add the following line at the end:

   JkMount  /mapguide2008/javaviewersample/* worker1

2. Save the changes.
3. Restart the Apache2 service.
4. Copy json.jar from C:\Program Files\Autodesk\MapGuideEnterprise2008\WebServerExtensions\www\javaviewersample to C:\Program Files\Autodesk\MapGuideEnterprise2008\WebServerExtensions\www\WEB-INF\lib