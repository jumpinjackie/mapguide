package org.osgeo.mapguide.test;

import java.io.*;
import java.util.*;

import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;

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

      /*
      MgSite site = _conn.GetSite();
      MgServerAdmin admin = new MgServerAdmin();
      admin.Open(_userInfo);
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

      */
  }

  /*
  class MgWebLayoutCreator : IWebLayoutCreator
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

  class MgMapCreator : IMapCreator
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
          _map.Create(resId, resId.GetName());
          return _map;
      }

      public MgLayerBase CreateLayer(MgResourceIdentifier resId)
      {
          return new MgLayer(resId, _resSvc);
      }
  }

  class MgSessionCreator : ISessionCreator
  {
      private MgSiteConnection _siteConn;

      public MgSessionCreator(MgSiteConnection siteConn)
      {
          _siteConn = siteConn;
      }

      public String CreateSession()
      {
          var site = _siteConn.GetSite();
          return site.CreateSession();
      }
  }

  class MgApplySession : IApplySession
  {
      private MgUserInformation _userInfo;

      public MgApplySession(MgUserInformation userInfo)
      {
          _userInfo = userInfo;
      }

      public void SetSessionId(String sessionId)
      {
          _userInfo.SetMgSessionId(sessionId);
      }
  }

  class MgSession : IMapGuideSession
  {
      public String SessionID
      {
          get;
          set;
      }
  }
  */
  public ITestExecutor GetTestExecutor(String opName)
  {
      if (_executors.containsKey(opName))
          return _executors.get(opName);
      throw new RuntimeException("Executor for " + opName + " not found");
  }

  public int ValidateRequest(SqliteDb db, String testName, int paramSetId, String operation, TestResult actualResult, ITestLogger logger)
  {
      int exitStatus = 0;
      /*
      String outcome = "pass";
      SqliteVm vm = new SqliteVm(db, false);

      Object expectedResult = null;

      //If we have an exception we need to remove the stack trace because different line numbers will fail the test
      Object resultData = CommonUtility.RemoveStackTraceFromResult(actualResult.ResultData);
      //Get the mime type based on the content type in the result
      String mimeType = actualResult.ContentType;
      //If we have exception message we need to remove any parts that may contain system dependent information
      //Ex. file paths
      resultData = CommonUtility.ProcessExceptionMessage(resultData);
      //Get the file extension that will be used for a dump
      String actualExtension = CommonUtility.GetExtensionFromMimeType(mimeType);

      //If we have an ALWAYSPASS parameter defined for the operation then skip the whole validation process
      //This parameter should only be used for clean up operations that are no related with the tests
      if (vm.Execute("Select ParamValue from Params where ParamName=\"ALWAYSPASS\" and ParamSet={0}", paramSetId) != Sqlite.Row)
      {
          //TestName is Test_[ServiceType]
          String type = testName.SubString(testName.IndexOf("_") + 1);
          String filePath = CommonUtility.GetPath(String.Format("../../TestData/{0}/DumpFiles/{0}ApiTest", type));
          String fileName = String.Format("{0}_{1}.{2}", filePath, paramSetId, actualExtension);

          if (this.TestExecutionMode == "dump")
          {
              //File.WriteAllText(fileName, resultData);
              throw new NotImplementedException("The .net test runner does not support dumping of test results. Please use the PHP test runner for this purpose");
          }
          else
          {
              //This section is special case handling for the operations that return different data after each call
              resultData = CommonUtility.SpecialDataHandling(operation, resultData, mimeType);

              if (this.TestExecutionMode == "validate" || this.TestExecutionMode == "show")
              {   
                  String resultContent = "";

                  //Get the sample result and the expected content type from the database
                  int status = vm.Execute("Select Result, ContentType from ApiTestResults where ParamSet={0}", paramSetId);
                  String expectedContentType = vm.GetString("ContentType");
                  String expectedExtension = CommonUtility.GetExtensionFromMimeType(expectedContentType);
                  SqliteGcBlob blob = vm.GetBlob("Result");

                  byte[] b = blob.Read();
                  if (b != null)
                  {
                      if (expectedExtension == "xml" || expectedExtension == "txt" || expectedExtension == "html")
                          expectedResult = Encoding.UTF8.GetString(b);
                      else
                          expectedResult = b;
                  }
                  else
                  {
                      if (expectedExtension == "xml" || expectedExtension == "txt" || expectedExtension == "html")
                          expectedResult = String.Empty;
                      else
                          expectedResult = null;
                  }
                  String strExpectedResult = expectedResult as String;
                  //If we are validating from a file then get the contents of that file
                  //File names should be prefixed with "@@" to distinguish them from BLOB data
                  if (strExpectedResult != null && strExpectedResult.StartsWith("@@"))
                  {
                      String sampleDataFile = strExpectedResult.SubString(2);
                      sampleDataFile = CommonUtility.GetPath(sampleDataFile);
                      expectedResult = File.ReadAllText(sampleDataFile);
                  }

                  if (this.TestExecutionMode == "validate")
                  {
                      bool bEqual = false;
                      byte[] bExpected = expectedResult as byte[];
                      byte[] bActual = resultData as byte[];
                      String strResultData = resultData as String;

                      //FIXME: We're not processing DWF content properly to do this check properly. So just
                      //pass these for now
                      if (operation == "GETDRAWINGLAYER" || operation == "GETDRAWINGSECTION")
                      {
                          bEqual = true;
                      }
                      else
                      {
                          if (strExpectedResult != null && strResultData != null)
                          {
                              //Normalize line endings on LF before comparsion (in case the SQLite GUI recorded CRLFs)
                              String normStrResultData = strResultData.Replace("\r\n", "\n");
                              String normStrExpectedResult = strExpectedResult.Replace("\r\n", "\n");

                              bEqual = normStrResultData.Equals(normStrExpectedResult, StringComparison.InvariantCultureIgnoreCase);
                          }
                          else if (bExpected != null && bActual != null)
                          {
                              bEqual = CommonUtility.ByteArraysEqual(bExpected, bActual, operation, testName);
                          }
                          else
                          {
                              System.Diagnostics.Debug.WriteLine(String.Format("[MgTestRunner]: {0} - {1} - Encountered disparate data types between expected and actual results. Expecting test failure :(", testName, operation));
                          }
                      }
                      //If the results are different and special validation fails then the operation failed ->mark it red
                      if (!bEqual && !CommonUtility.SpecialValidation(operation, resultData, expectedResult))
                      {
                          outcome = "fail";
                          exitStatus = 1;

                          if (expectedExtension != "xml" && expectedExtension != "html" && expectedExtension != "txt")
                          {
                              expectedResult = "Unable to display binary data";
                          }

                          if (actualExtension != "xml" && actualExtension != "html" && actualExtension != "txt")
                          {
                              resultData = "Unable to display binary data";
                          }
                      }
                  }
                  else
                  {
                      throw new NotImplementedException("The .net test runner does not support the given test execution mode (" + this.TestExecutionMode + "). Please use the PHP test runner for this purpose");
                  }
              }
          }
      }

      if (outcome == "fail")
      {
          Console.WriteLine("****{0} {1} {2} failed.\n", testName, paramSetId, operation);
          String str = String.Format("\n****ACTUAL RESULT****\n{0}\n****EXPECTED RESULT****\n{1}\n********\n\n\n", resultData, expectedResult);
          //Console.WriteLine(str);
          //Console.WriteLine("<FAIL>");
          logger.Write(str);
      }

      vm.SqlFinalize();
      vm = null;
      */
      return exitStatus;
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