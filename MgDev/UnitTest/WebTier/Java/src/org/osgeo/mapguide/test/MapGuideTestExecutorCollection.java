package org.osgeo.mapguide.test;

import java.io.*;
import java.util.*;
import java.nio.charset.StandardCharsets;

import com.almworks.sqlite4java.*;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.transform.*;
import javax.xml.transform.dom.*;
import javax.xml.transform.stream.*;
import javax.xml.xpath.*;
import org.w3c.dom.*;

import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;
import org.osgeo.mapguide.test.platform.operations.feature.*;
import org.osgeo.mapguide.test.platform.operations.resource.*;
import org.osgeo.mapguide.test.platform.operations.maplayer.*;
import org.osgeo.mapguide.test.mapguide.operations.drawing.*;
import org.osgeo.mapguide.test.mapguide.operations.admin.*;
import org.osgeo.mapguide.test.mapguide.operations.site.*;
import org.osgeo.mapguide.test.mapguide.operations.weblayout.*;

public class MapGuideTestExecutorCollection extends TestExecutorCollectionBase
{
    private HashMap<String, ITestExecutor> _executors;

    private MgUserInformation _userInfo;
    private MgSiteConnection _conn;

    public MapGuideTestExecutorCollection(MgUserInformation userInfo, MgSiteConnection conn)
    {
        _userInfo = userInfo;
        _conn = conn;
        _executors = new HashMap<String, ITestExecutor>();
    }

    protected void SetupExecutors(String dbPath)
    {
        //NOTE: We can't share the SqliteVm instance among our executor Objects as this messes up query results
        //we must be able to re-create a new SqliteVm for each executor, so we pass down the db path

        MgResourceService resSvc = (MgResourceService)_conn.createService(MgServiceType.ResourceService);
        MgFeatureService featSvc = (MgFeatureService)_conn.createService(MgServiceType.FeatureService);
        MgDrawingService drawSvc = (MgDrawingService)_conn.createService(MgServiceType.DrawingService);

        
        MgSite site = _conn.getSite();
        MgServerAdmin admin = new MgServerAdmin();
        admin.open(_userInfo);
        MgWebLayoutCreator wlCreator = new MgWebLayoutCreator(resSvc);
        MgMapCreator creator = new MgMapCreator(_conn, resSvc);
        MgSessionCreator sessionCreator = new MgSessionCreator(_conn);
        MgApplySession sessionApply = new MgApplySession(_userInfo);
        MgSession session = new MgSession();

        //Resource Service
        _executors.put("ApplyResourcePackage".toUpperCase(), new ApplyResourcePackage(resSvc, dbPath));
        _executors.put("ChangeResourceOwner".toUpperCase(), new ChangeResourceOwner(resSvc, dbPath));
        _executors.put("CopyResource".toUpperCase(), new CopyResource(resSvc, dbPath));
        _executors.put("DeleteResource".toUpperCase(), new DeleteResource(resSvc, dbPath));
        _executors.put("DeleteResourceData".toUpperCase(), new DeleteResourceData(resSvc, dbPath));
        _executors.put("EnumerateResourceData".toUpperCase(), new EnumerateResourceData(resSvc, dbPath));
        _executors.put("EnumerateResourceReferences".toUpperCase(), new EnumerateResourceReferences(resSvc, dbPath));
        _executors.put("EnumerateResources".toUpperCase(), new EnumerateResources(resSvc, dbPath));
        _executors.put("GetRepositoryContent".toUpperCase(), new GetRepositoryContent(resSvc, dbPath));
        _executors.put("GetRepositoryHeader".toUpperCase(), new GetRepositoryHeader(resSvc, dbPath));
        _executors.put("GetResourceContent".toUpperCase(), new GetResourceContent(resSvc, dbPath));
        _executors.put("GetResourceData".toUpperCase(), new GetResourceData(resSvc, dbPath));
        _executors.put("GetResourceHeader".toUpperCase(), new GetResourceHeader(resSvc, dbPath));
        _executors.put("InheritPermissionsFrom".toUpperCase(), new InheritPermissionsFrom(resSvc, dbPath));
        _executors.put("MoveResource".toUpperCase(), new MoveResource(resSvc, dbPath));
        _executors.put("RenameResourceData".toUpperCase(), new RenameResourceData(resSvc, dbPath));
        _executors.put("SetResource".toUpperCase(), new SetResource(resSvc, dbPath));
        _executors.put("SetResourceData".toUpperCase(), new SetResourceData(resSvc, dbPath));
        _executors.put("UpdateRepository".toUpperCase(), new UpdateRepository(resSvc, dbPath));
        
        //Feature Service
        _executors.put("DescribeFeatureSchema".toUpperCase(), new DescribeFeatureSchema(featSvc, dbPath));
        _executors.put("ExecuteSqlQuery".toUpperCase(), new ExecuteSqlQuery(featSvc, dbPath));
        _executors.put("GetClasses".toUpperCase(), new GetClasses(featSvc, dbPath));
        _executors.put("GetConnectionPropertyValues".toUpperCase(), new GetConnectionPropertyValues(featSvc, dbPath));
        _executors.put("GetFeatureProviders".toUpperCase(), new GetFeatureProviders(featSvc, dbPath));
        _executors.put("GetLongTransactions".toUpperCase(), new GetLongTransactions(featSvc, dbPath));
        _executors.put("GetProviderCapabilities".toUpperCase(), new GetProviderCapabilities(featSvc, dbPath));
        _executors.put("GetSchemas".toUpperCase(), new GetSchemas(featSvc, dbPath));
        _executors.put("GetSpatialContexts".toUpperCase(), new GetSpatialContexts(featSvc, dbPath));
        _executors.put("SelectAggregates".toUpperCase(), new SelectAggregates(featSvc, dbPath));
        _executors.put("SelectFeatures".toUpperCase(), new SelectFeatures(featSvc, dbPath));
        _executors.put("SetLongTransaction".toUpperCase(), new SetLongTransaction(featSvc, dbPath, sessionCreator, sessionApply));
        _executors.put("TestConnection".toUpperCase(), new TestConnection(featSvc, dbPath));

        //Map and Layers
        _executors.put("AddLayerGroup".toUpperCase(), new AddLayerGroup(resSvc, dbPath, creator));
        _executors.put("AddLayer".toUpperCase(), new AddLayer(resSvc, dbPath, creator));
        _executors.put("GetCoordinateSystem".toUpperCase(), new GetCoordinateSystem(resSvc, dbPath, creator));
        _executors.put("GetDataExtent".toUpperCase(), new GetDataExtent(resSvc, dbPath, creator));
        _executors.put("GetDisplayInLegend".toUpperCase(), new GetDisplayInLegend(resSvc, dbPath, creator));
        _executors.put("GetLayerFeatureClass".toUpperCase(), new GetLayerFeatureClass(resSvc, dbPath, creator));
        _executors.put("GetLayerFeatureSource".toUpperCase(), new GetLayerFeatureSource(resSvc, dbPath, creator));
        _executors.put("GetLayerDefinition".toUpperCase(), new GetLayerDefinition(resSvc, dbPath, creator));
        _executors.put("GetGroups".toUpperCase(), new GetGroups(resSvc, dbPath, creator));
        _executors.put("GetLayers".toUpperCase(), new GetLayers(resSvc, dbPath, creator));
        _executors.put("GetLayerVisibility".toUpperCase(), new GetLayerVisibility(resSvc, dbPath, creator));
        _executors.put("GetLegendLabel".toUpperCase(), new GetLegendLabel(resSvc, dbPath, creator));
        _executors.put("GetMapExtent".toUpperCase(), new GetMapExtent(resSvc, dbPath, creator));
        _executors.put("GetMapName".toUpperCase(), new GetMapName(resSvc, dbPath, creator));
        _executors.put("GetViewCenter".toUpperCase(), new GetViewCenter(resSvc, dbPath, creator));
        _executors.put("GetViewScale".toUpperCase(), new GetViewScale(resSvc, dbPath, creator));
        _executors.put("HideGroup".toUpperCase(), new HideGroup(resSvc, dbPath, creator));
        _executors.put("IsLayerVisible".toUpperCase(), new IsLayerVisible(resSvc, dbPath, creator));
        _executors.put("LayerExists".toUpperCase(), new LayerExists(resSvc, dbPath, creator));
        _executors.put("RemoveGroup".toUpperCase(), new RemoveGroup(resSvc, dbPath, creator));
        _executors.put("ShowGroup".toUpperCase(), new ShowGroup(resSvc, dbPath, creator));

        //Drawing Service
        _executors.put("DescribeDrawing".toUpperCase(), new DescribeDrawing(drawSvc, dbPath));
        _executors.put("GetDrawing".toUpperCase(), new GetDrawing(drawSvc, dbPath));
        _executors.put("EnumerateDrawingLayers".toUpperCase(), new EnumerateDrawingLayers(drawSvc, dbPath));
        _executors.put("GetDrawingLayer".toUpperCase(), new GetDrawingLayer(drawSvc, dbPath));
        _executors.put("GetDrawingSection".toUpperCase(), new GetDrawingSection(drawSvc, dbPath));
        _executors.put("EnumerateDrawingSections".toUpperCase(), new EnumerateDrawingSections(drawSvc, dbPath));
        _executors.put("EnumerateDrawingSectionResources".toUpperCase(), new EnumerateDrawingSectionResources(drawSvc, dbPath));
        _executors.put("GetDrawingSectionResource".toUpperCase(), new GetDrawingSectionResource(drawSvc, dbPath));

        //Mapping Service

        //Rendering Service

        //Server Admin
        _executors.put("Offline".toUpperCase(), new Offline(admin, dbPath));
        _executors.put("Online".toUpperCase(), new Online(admin, dbPath));
        _executors.put("GetLog".toUpperCase(), new GetLog(admin, dbPath));
        _executors.put("GetLogByDate".toUpperCase(), new GetLogByDate(admin, dbPath));
        _executors.put("ClearLog".toUpperCase(), new ClearLog(admin, dbPath));
        _executors.put("DeleteLog".toUpperCase(), new DeleteLog(admin, dbPath));
        _executors.put("RenameLog".toUpperCase(), new RenameLog(admin, dbPath));
        _executors.put("EnumeratePackages".toUpperCase(), new EnumeratePackages(admin, dbPath));
        _executors.put("DeletePackage".toUpperCase(), new DeletePackage(admin, dbPath));
        _executors.put("LoadPackage".toUpperCase(), new LoadPackage(admin, dbPath));
        _executors.put("GetPackageStatus".toUpperCase(), new GetPackageStatus(admin, dbPath));
        _executors.put("GetPackageLog".toUpperCase(), new GetPackageLog(admin, dbPath));

        //Site Service
        _executors.put("CreateSession".toUpperCase(), new CreateSession(site, dbPath, session));
        _executors.put("DestroySession".toUpperCase(), new DestroySession(site, dbPath));
        _executors.put("GetUserForSession".toUpperCase(), new GetUserForSession(site, dbPath, session));
        _executors.put("EnumerateUsers".toUpperCase(), new EnumerateUsers(site, dbPath));
        _executors.put("AddUser".toUpperCase(), new AddUser(site, dbPath));
        _executors.put("UpdateUser".toUpperCase(), new UpdateUser(site, dbPath));
        _executors.put("DeleteUsers".toUpperCase(), new DeleteUsers(site, dbPath));
        _executors.put("GrantRoleMembershipsToUsers".toUpperCase(), new GrantRoleMembershipsToUsers(site, dbPath));
        _executors.put("RevokeRoleMembershipsFromUsers".toUpperCase(), new RevokeRoleMembershipsFromUsers(site, dbPath));
        _executors.put("GrantGroupMembershipsToUsers".toUpperCase(), new GrantGroupMembershipsToUsers(site, dbPath));
        _executors.put("RevokeGroupMembershipsFromUsers".toUpperCase(), new RevokeGroupMembershipsFromUsers(site, dbPath));
        _executors.put("EnumerateGroups".toUpperCase(), new EnumerateGroups(site, dbPath));
        _executors.put("EnumerateGroups2".toUpperCase(), new EnumerateGroups2(site, dbPath));
        _executors.put("EnumerateRoles2".toUpperCase(), new EnumerateRoles2(site, dbPath));
        _executors.put("AddGroup".toUpperCase(), new AddGroup(site, dbPath));
        _executors.put("UpdateGroup".toUpperCase(), new UpdateGroup(site, dbPath));
        _executors.put("DeleteGroups".toUpperCase(), new DeleteGroups(site, dbPath));
        _executors.put("GrantRoleMembershipsToGroups".toUpperCase(), new GrantRoleMembershipsToGroups(site, dbPath));
        _executors.put("RevokeRoleMembershipsFromGroups".toUpperCase(), new RevokeRoleMembershipsFromGroups(site, dbPath));
        _executors.put("EnumerateRoles".toUpperCase(), new EnumerateRoles(site, dbPath));
        _executors.put("EnumerateServers".toUpperCase(), new EnumerateServers(site, dbPath));
        _executors.put("AddServer".toUpperCase(), new AddServer(site, dbPath));
        _executors.put("UpdateServer".toUpperCase(), new UpdateServer(site, dbPath));
        _executors.put("RemoveServer".toUpperCase(), new RemoveServer(site, dbPath));

        //Web Layout
        _executors.put("WL_GetTitle".toUpperCase(), new WL_GetTitle(wlCreator, dbPath));
        _executors.put("WL_GetMapDefinition".toUpperCase(), new WL_GetMapDefinition(wlCreator, dbPath));
        _executors.put("WL_GetScale".toUpperCase(), new WL_GetScale(wlCreator, dbPath));
        _executors.put("WL_GetCenter".toUpperCase(), new WL_GetCenter(wlCreator, dbPath));
        _executors.put("WL_ShowToolbar".toUpperCase(), new WL_ShowToolbar(wlCreator, dbPath));
        _executors.put("WL_ShowStatusbar".toUpperCase(), new WL_ShowStatusbar(wlCreator, dbPath));
        _executors.put("WL_ShowTaskpane".toUpperCase(), new WL_ShowTaskpane(wlCreator, dbPath));
        _executors.put("WL_ShowTaskbar".toUpperCase(), new WL_ShowTaskbar(wlCreator, dbPath));
        _executors.put("WL_ShowLegend".toUpperCase(), new WL_ShowLegend(wlCreator, dbPath));
        _executors.put("WL_ShowProperties".toUpperCase(), new WL_ShowProperties(wlCreator, dbPath));
        _executors.put("WL_GetTaskPaneWidth".toUpperCase(), new WL_GetTaskPaneWidth(wlCreator, dbPath));
        _executors.put("WL_GetInformationPaneWidth".toUpperCase(), new WL_GetInformationPaneWidth(wlCreator, dbPath));
        _executors.put("WL_GetInitialTaskUrl".toUpperCase(), new WL_GetInitialTaskUrl(wlCreator, dbPath));
        _executors.put("WL_ShowContextMenu".toUpperCase(), new WL_ShowContextMenu(wlCreator, dbPath));
        _executors.put("WL_TestUiItem".toUpperCase(), new WL_TestUiItem(wlCreator, dbPath));
        _executors.put("WL_HomeTooltip".toUpperCase(), new WL_HomeTooltip(wlCreator, dbPath));
        _executors.put("WL_HomeDescription".toUpperCase(), new WL_HomeDescription(wlCreator, dbPath));
        _executors.put("WL_BackTooltip".toUpperCase(), new WL_BackTooltip(wlCreator, dbPath));
        _executors.put("WL_BackDescription".toUpperCase(), new WL_BackDescription(wlCreator, dbPath));
        _executors.put("WL_ForwardTooltip".toUpperCase(), new WL_ForwardTooltip(wlCreator, dbPath));
        _executors.put("WL_ForwardDescription".toUpperCase(), new WL_ForwardDescription(wlCreator, dbPath));
        _executors.put("WL_TasksName".toUpperCase(), new WL_TasksName(wlCreator, dbPath));
        _executors.put("WL_TasksTooltip".toUpperCase(), new WL_TasksTooltip(wlCreator, dbPath));
        _executors.put("WL_TasksDescription".toUpperCase(), new WL_TasksDescription(wlCreator, dbPath));
    }

    class MgWebLayoutCreator implements IWebLayoutCreator
    {
        private MgResourceService _resSvc;
        private MgWebLayout _wl;

        public MgWebLayoutCreator(MgResourceService resSvc)
        {
            _resSvc = resSvc;
        }

        public MgWebLayout CreateWebLayout(MgResourceIdentifier resId)
        {
            if (_wl != null)
                return _wl;

            _wl = new MgWebLayout(_resSvc, resId);
            return _wl;
        }
    }

    class MgMapCreator implements IMapCreator
    {
        private MgSiteConnection _siteConn;
        private MgMap _map;
        private MgResourceService _resSvc;

        public MgMapCreator(MgSiteConnection siteConn, MgResourceService resSvc)
        {
            _siteConn = siteConn;
            _resSvc = resSvc;
        }

        public MgMapBase CreateMap(MgResourceIdentifier resId)
        {
            //The test suite relies on the same map and not a new one
            //re-created on each test of that particular test suite
            if (_map != null)
              return _map;

            _map = new MgMap(_siteConn);
            _map.create(resId, resId.getName());
            return _map;
        }

        public MgLayerBase CreateLayer(MgResourceIdentifier resId)
        {
            return new MgLayer(resId, _resSvc);
        }
    }

    class MgSessionCreator implements ISessionCreator
    {
        private MgSiteConnection _siteConn;

        public MgSessionCreator(MgSiteConnection siteConn)
        {
            _siteConn = siteConn;
        }

        public String CreateSession()
        {
            MgSite site = _siteConn.getSite();
            return site.createSession();
        }
    }

    class MgApplySession implements IApplySession
    {
        private MgUserInformation _userInfo;

        public MgApplySession(MgUserInformation userInfo)
        {
            _userInfo = userInfo;
        }

        public void SetSessionId(String sessionId)
        {
            _userInfo.setMgSessionId(sessionId);
        }
    }

    class MgSession implements IMapGuideSession
    {
        private String _value;

        public String getSessionID() { return _value; }
        public void setSessionID(String value) { _value = value; }
    }

    public ITestExecutor GetTestExecutor(String opName)
    {
        if (_executors.containsKey(opName))
            return _executors.get(opName);
        throw new MissingTestExecutorException(opName);
    }

    public int ValidateRequest(SQLiteConnection db, String testName, int paramSetId, String operation, TestResult actualResult, ITestLogger logger)
    {
        int exitStatus = 0;

        String outcome = "pass";

        Object expectedResult = null;

        //If we have an exception we need to remove the stack trace because different line numbers will fail the test
        Object resultData = CommonUtility.RemoveStackTraceFromResult(actualResult.ResultData);
        try {    
            //Get the mime type based on the content type in the result
            String mimeType = actualResult.ContentType;
            //If we have exception message we need to remove any parts that may contain system dependent information
            //Ex. file paths
            resultData = CommonUtility.ProcessExceptionMessage(resultData);
            //Get the file extension that will be used for a dump
            String actualExtension = CommonUtility.GetExtensionFromMimeType(mimeType);

            SQLiteStatement stmt = db.prepare(String.format("Select ParamValue from Params where ParamName=\"ALWAYSPASS\" and ParamSet=%d", paramSetId));

            if (DebugSettings.Enabled) {
                Console.WriteLine("\t\tTest Result");
                Console.WriteLine("\t\t\thas return value: " + actualResult.HasReturnValue);
                Console.WriteLine("\t\t\tis exception: " + actualResult.IsException);
            }

            //If we have an ALWAYSPASS parameter defined for the operation then skip the whole validation process
            //This parameter should only be used for clean up operations that are no related with the tests
            if (!stmt.step())
            {
                //TestName is Test_[ServiceType]
                String type = testName.substring(testName.indexOf("_") + 1);
                String filePath = CommonUtility.GetPath(String.format("../../TestData/{0}/DumpFiles/{0}ApiTest", type));
                String fileName = String.format("{0}_{1}.{2}", filePath, paramSetId, actualExtension);

                if (this.TestExecutionMode == "dump")
                {
                    //File.WriteAllText(fileName, resultData);
                    throw new RuntimeException("The Java test runner does not support dumping of test results. Please use the PHP test runner for this purpose");
                }
                else
                {
                    //This section is special case handling for the operations that return different data after each call
                    resultData = CommonUtility.SpecialDataHandling(operation, resultData, mimeType);

                    if (this.TestExecutionMode == "validate" || this.TestExecutionMode == "show")
                    {   
                        String resultContent = "";

                        //Get the sample result and the expected content type from the database
                        SQLiteStatement rstmt = db.prepare(String.format("Select Result, ContentType from ApiTestResults where ParamSet=%d", paramSetId));
                        if (rstmt.step())
                        {
                            String expectedContentType = CommonUtility.GetStringFromStatement(rstmt, "ContentType");
                            String expectedExtension = CommonUtility.GetExtensionFromMimeType(expectedContentType);
                            byte[] b = CommonUtility.GetByteArrayFromStatement(rstmt, "Result");
                            if (b != null)
                            {
                                if (expectedExtension == "xml" || expectedExtension == "txt" || expectedExtension == "html")
                                    expectedResult = new String(b, StandardCharsets.UTF_8);
                                else
                                    expectedResult = b;
                            }
                            else
                            {
                                if (expectedExtension == "xml" || expectedExtension == "txt" || expectedExtension == "html")
                                    expectedResult = "";
                                else
                                    expectedResult = null;
                            }
                            String strExpectedResult = null;
                            if (expectedResult instanceof String)
                                strExpectedResult = (String)expectedResult;
                            //If we are validating from a file then get the contents of that file
                            //File names should be prefixed with "@@" to distinguish them from BLOB data
                            if (strExpectedResult != null && strExpectedResult.startsWith("@@"))
                            {
                                String sampleDataFile = strExpectedResult.substring(2);
                                sampleDataFile = CommonUtility.GetPath(sampleDataFile);
                                expectedResult = CommonUtility.ReadAllText(sampleDataFile);
                            }

                            if (this.TestExecutionMode == "validate")
                            {
                                boolean bEqual = false;
                                byte[] bExpected = null;
                                if (expectedResult instanceof byte[])
                                    bExpected = (byte[])expectedResult;
                                byte[] bActual = null;
                                if (resultData instanceof byte[])
                                    bActual = (byte[])resultData;
                                String strResultData = null;
                                if (resultData instanceof String)
                                    strResultData = (String)resultData;

                                //FIXME: We're not processing DWF content properly to do this check properly. So just
                                //pass these for now
                                //
                                //DESCRIBEFEATURESCHEMA is auto-pass because I haven't figured out how to parse XML with qualified
                                //elements with Java's woeful XML APIs.
                                if (operation.equals("GETDRAWINGLAYER") || operation.equals("GETDRAWINGSECTION") || operation.equals("DESCRIBEFEATURESCHEMA"))
                                {
                                    bEqual = true;
                                }
                                else
                                {
                                    if (strExpectedResult != null && strResultData != null)
                                    {
                                        if (DebugSettings.Enabled)
                                            Console.WriteLine("[DEBUG] - Validating string result");
                                        //Normalize line endings on LF before comparsion (in case the SQLite GUI recorded CRLFs)
                                        String normStrResultData = strResultData.replace("\r\n", "\n");
                                        String normStrExpectedResult = strExpectedResult.replace("\r\n", "\n");

                                        bEqual = normStrResultData.equalsIgnoreCase(normStrExpectedResult);

                                        if (!bEqual) {
                                            XmlCompareResult result = TryXmlCompare(normStrExpectedResult, normStrResultData);
                                            bEqual = result.Result;
                                            if (!bEqual && result.HasXml()) {
                                                //Update expected/actual with normalized versions for display
                                                expectedResult = result.Expected;
                                                resultData = result.Actual;
                                                if (DebugSettings.Enabled)
                                                    Console.WriteLine("Expected: <<<\n" + result.Expected + "\nActual: <<<\n" + result.Actual + "\n<<<<<<<<<<<<\n");
                                            }
                                        }
                                    }
                                    else if (bExpected != null && bActual != null)
                                    {
                                        if (DebugSettings.Enabled)
                                            Console.WriteLine("[DEBUG] - Validating byte array results");
                                        bEqual = CommonUtility.ByteArraysEqual(bExpected, bActual, operation, testName);
                                    }
                                    else
                                    {
                                        if (DebugSettings.Enabled)
                                            Console.WriteLine(String.format("[MgTestRunner]: %s - %s - Encountered disparate data types between expected and actual results. Expecting test failure :(", testName, operation));
                                    }
                                }
                                //If the results are different and special validation fails then the operation failed ->mark it red
                                if (!bEqual && !CommonUtility.SpecialValidation(operation, resultData, expectedResult))
                                {
                                    //But only if this is not a void operation
                                    if (actualResult.HasReturnValue)
                                    {
                                        outcome = "fail";
                                        exitStatus = 1;

                                        if (!expectedExtension.equals("xml") && !expectedExtension.equals("html") && !expectedExtension.equals("txt"))
                                        {
                                            expectedResult = "Unable to display binary data";
                                        }

                                        if (!actualExtension.equals("xml") && !actualExtension.equals("html") && !actualExtension.equals("txt"))
                                        {
                                            resultData = "Unable to display binary data";
                                        }
                                    }
                                }
                            }
                            else
                            {
                                throw new RuntimeException("The Java test runner does not support the given test execution mode (" + this.TestExecutionMode + "). Please use the PHP test runner for this purpose");
                            }
                        }
                        else
                        {
                            if (DebugSettings.Enabled)
                                Console.WriteLine("\t\tNo result to compare against");
                        }
                    }
                }
            }
            else
            {
                if (DebugSettings.Enabled) {
                    String pval = CommonUtility.GetStringFromStatement(stmt, "ParamValue");
                    Console.WriteLine("\t\tALWAYSPASS: " + pval);
                }
            }
        } catch (Exception ex) {
            Console.WriteLine("Exception validating response: %s", CommonUtility.ExceptionToString(ex));
            outcome = "fail";
            exitStatus = 1;
        }

        if (outcome.equals("fail"))
        {
            Console.WriteLine("****%s %d %s failed.\n", testName, paramSetId, operation);
            String str = String.format("\n****ACTUAL RESULT****\n%s\n****EXPECTED RESULT****\n%s\n********\n\n\n", resultData, expectedResult);
            logger.Write(str);
            Console.WriteLine(str);
        }

        return exitStatus;
    }

    private static String SortElement(Node elem, String preText)
    {
        Vector<String> elemArray = new Vector<String>();
        String elemString = "";
        NodeList childNodes = elem.getChildNodes();
        if (childNodes.getLength() > 0)
        {
            int elCount = 0;
            int txtCount = 0;
            for (int i = 0; i < childNodes.getLength(); i++)
            {
                Node child = childNodes.item(i);
                if (child.getNodeType() == Node.ELEMENT_NODE)
                {
                    String elemValue = SortElement(child, preText + "  ");
                    if (!CommonUtility.IsNullOrEmpty(elemValue))
                    {
                        elemArray.add(elemValue);
                        elCount++;
                    }
                }
                else if (child.getNodeType() == Node.TEXT_NODE)
                {
                    String content = child.getTextContent().trim();
                    if (!CommonUtility.IsNullOrEmpty(content))
                    {
                        elemArray.add(content);
                        txtCount++;
                    }
                }
            }

            Collections.sort(elemArray);
            for (String str : elemArray)
            {
                elemString += str;
            }
        }

        String endTag = "";
        if (elemArray.size() > 1 && elemString.length() > 0)
        {
            endTag = "\n" + preText;
        }
        String tagName = "";
        if (!(elem instanceof Document))
        {
            tagName = elem.getNodeName();
        }
        endTag += "</" + tagName + ">";

        if ("" != tagName)
        {
            elemString = "\n" + preText + "<" + tagName + ">" + elemString + endTag;
        }

        return elemString;
    }

    private static String toString(Document doc) {
        try {
            StringWriter sw = new StringWriter();
            Source xmlInput = new DOMSource(doc);
            Result xmlOutput = new StreamResult(sw);
            TransformerFactory tf = TransformerFactory.newInstance();
            //See: http://stackoverflow.com/questions/161462/java-writing-a-dom-to-an-xml-file-formatting-issues
            tf.setAttribute("indent-number", 4);
            Transformer transformer = tf.newTransformer();
            transformer.setOutputProperty(OutputKeys.OMIT_XML_DECLARATION, "yes");
            transformer.setOutputProperty(OutputKeys.METHOD, "xml");
            transformer.setOutputProperty(OutputKeys.INDENT, "yes");
            transformer.setOutputProperty("{http://xml.apache.org/xslt}indent-amount", "4");
            transformer.setOutputProperty(OutputKeys.ENCODING, "UTF-8");

            transformer.transform(xmlInput, xmlOutput);
            return sw.toString();
        } catch (Exception ex) {
            throw new RuntimeException("Error converting to String", ex);
        }
    }

    private Node RecursiveNormalizeNode(Node elem) {
        //Strip whitespace nodes
        try {
            XPathFactory xpathFactory = XPathFactory.newInstance();
            // XPath to find empty text nodes.
            XPathExpression xpathExp = xpathFactory.newXPath().compile(
                    "//text()[normalize-space(.) = '']");  
            NodeList emptyTextNodes = (NodeList) 
                    xpathExp.evaluate(elem, XPathConstants.NODESET);

            // Remove each empty text node from document.
            for (int i = 0; i < emptyTextNodes.getLength(); i++) {
                Node emptyTextNode = emptyTextNodes.item(i);
                emptyTextNode.getParentNode().removeChild(emptyTextNode);
            }
        } catch (Exception ex) {
            
        }

        //Strip attributes
        if (elem instanceof Element) {
            Element e = (Element)elem;
            e.removeAttribute("xsi:noNamespaceSchemaLocation");
            e.removeAttribute("noNamespaceSchemaLocation");
            e.removeAttribute("xmlns:xsi");
            e.removeAttribute("xsi");
            e.removeAttribute("version");
        }

        NodeList childNodes = elem.getChildNodes();
        for (int i = 0; i < childNodes.getLength(); i++) {
            Node n = childNodes.item(i);
            RecursiveNormalizeNode(n);
        }
        /*
        NodeList childNodes = elem.getChildNodes();
        Vector<Node> nodes = new Vector<Node>();
        for (int i = 0; i < childNodes.getLength(); i++) {
            Node n = childNodes.item(i);
            nodes.add(n);
            elem.removeChild(n);
        }
        Collections.sort(nodes, new NodeNormalizeComparator());
        for (Node child : nodes) {
            child.normalize();
            elem.appendChild(child);
        }
        */
        return elem;
    }

    private Document NormalizeDocument(Document doc) {
        //Strip known attributes on this node
        Element e = doc.getDocumentElement();
        e.removeAttribute("xsi:noNamespaceSchemaLocation");
        e.removeAttribute("noNamespaceSchemaLocation");
        e.removeAttribute("xmlns:xsi");
        e.removeAttribute("xsi");
        e.removeAttribute("version");

        RecursiveNormalizeNode(doc);
        //Strip whitespace nodes
        try {
            XPathFactory xpathFactory = XPathFactory.newInstance();
            // XPath to find empty text nodes.
            XPathExpression xpathExp = xpathFactory.newXPath().compile(
                    "//text()[normalize-space(.) = '']");  
            NodeList emptyTextNodes = (NodeList) 
                    xpathExp.evaluate(doc, XPathConstants.NODESET);

            // Remove each empty text node from document.
            for (int i = 0; i < emptyTextNodes.getLength(); i++) {
                Node emptyTextNode = emptyTextNodes.item(i);
                emptyTextNode.getParentNode().removeChild(emptyTextNode);
            }
        } catch (Exception ex) {
            
        }
        doc.normalizeDocument();

        //Sort the top-level child nodes
        NodeList childNodes = e.getChildNodes();
        if (DebugSettings.Enabled) {
            Console.WriteLine(">>> Pre-sort node list");
            for (int i = 0; i < childNodes.getLength(); i++) {
                Node n = childNodes.item(i);
                Console.WriteLine("\t" + n.getNodeName());
            }
            Console.WriteLine("<<<");
        }

        Vector<Node> nodes = new Vector<Node>();
        for (int i = 0; i < childNodes.getLength(); i++) {
            Node n = childNodes.item(i);
            nodes.add(n);
            e.removeChild(n);
        }
        Comparator<Node> nodeComparator = new Comparator<Node>() {
            public int compare(Node n1, Node n2) {
                String s1 = n1.getNodeName();
                String s2 = n2.getNodeName();
                int result = s1.compareTo(s2);
                if (DebugSettings.Enabled)
                    Console.WriteLine("'%s'.compareTo('%s') - %d", s1, s2, result);
                return result;
            }
        };
        Collections.sort(nodes, nodeComparator); //, new NodeNormalizeComparator());
        for (Node child : nodes) {
            child.normalize();
            e.appendChild(child);
        }

        if (DebugSettings.Enabled) {
            childNodes = e.getChildNodes();
            Console.WriteLine(">>> Post-sort node list");
            for (int i = 0; i < childNodes.getLength(); i++) {
                Node n = childNodes.item(i);
                Console.WriteLine("\t" + n.getNodeName());
            }
            Console.WriteLine("<<<");
        }
        return doc;
    }

    private XmlCompareResult TryXmlCompare(String expected, String actual) {
        /*
        XmlCompareResult result = null;
        try {
            DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
            dbf.setNamespaceAware(true);
            dbf.setCoalescing(true);
            dbf.setIgnoringElementContentWhitespace(true);
            dbf.setIgnoringComments(true);
            DocumentBuilder db = dbf.newDocumentBuilder();

            Document doc1 = db.parse(new ByteArrayInputStream(expected.trim().getBytes()));
            doc1 = NormalizeDocument(doc1);

            Document doc2 = db.parse(new ByteArrayInputStream(actual.trim().getBytes()));
            doc2 = NormalizeDocument(doc2);

            result = new XmlCompareResult();
            result.Result = doc1.isEqualNode(doc2);
            result.Expected = toString(doc1);
            result.Actual = toString(doc2);
        } catch (Exception ex) {
            result = new XmlCompareResult();
            result.Result = false;
        }
        Console.WriteLine("TryXmlCompare returned " + result.Result);
        return result;
        */
       
        XmlCompareResult result = null;
        try {
            DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
            //dbf.setNamespaceAware(true);
            dbf.setCoalescing(true);
            dbf.setIgnoringElementContentWhitespace(true);
            dbf.setIgnoringComments(true);
            DocumentBuilder db = dbf.newDocumentBuilder();

            Document doc1 = db.parse(new ByteArrayInputStream(expected.trim().getBytes()));
            doc1 = NormalizeDocument(doc1);

            Document doc2 = db.parse(new ByteArrayInputStream(actual.trim().getBytes()));
            doc2 = NormalizeDocument(doc2);

            result = new XmlCompareResult();
            result.Expected = SortElement(doc1, "");
            result.Actual = SortElement(doc2, "");
            result.Result = (result.Expected.equals(result.Actual));
        } catch (Exception ex) {
            //Console.WriteLine("Exception doing XML compare: %s", CommonUtility.ExceptionToString(ex));
            result = new XmlCompareResult();
            result.Result = false;
        }
        if (DebugSettings.Enabled)
            Console.WriteLine("\t\tTryXmlCompare returned " + result.Result);
        return result;
    }

    public void Cleanup()
    {
        for (ITestExecutor exec : _executors.values())
        {
            try {
                exec.close();
            } catch (IOException ex) {

            }
        }
        _executors.clear();
    }

    public String getApiType()
    {
      return ApiTypes.Platform;
    }
}