using OSGeo.MapGuide;
using OSGeo.MapGuide.Test.Common;
using SqliteDotNet;
using System;
using System.Collections.Generic;
using System.Data.SqlTypes;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Xml.Linq;

namespace OSGeo.MapGuide.Test.Web
{
    public class MapGuideTestExecutorCollection : TestExecutorCollectionBase
    {
        private Dictionary<string, ITestExecutor> _executors;

        private MgUserInformation _userInfo;
        private MgSiteConnection _conn;

        public MapGuideTestExecutorCollection(MgUserInformation userInfo, MgSiteConnection conn)
        {
            _userInfo = userInfo;
            _conn = conn;
        }

        protected override void SetupExecutors(string dbPath)
        {
            //NOTE: We can't share the SqliteVm instance among our executor objects as this messes up query results
            //we must be able to re-create a new SqliteVm for each executor, so we pass down the db path

            _executors = new Dictionary<string, ITestExecutor>();

            MgResourceService resSvc = (MgResourceService)_conn.CreateService(MgServiceType.ResourceService);
            MgFeatureService featSvc = (MgFeatureService)_conn.CreateService(MgServiceType.FeatureService);
            MgDrawingService drawSvc = (MgDrawingService)_conn.CreateService(MgServiceType.DrawingService);

            var site = _conn.GetSite();
            var admin = new MgServerAdmin();
            admin.Open(_userInfo);
            var wlCreator = new MgWebLayoutCreator(resSvc);
            var creator = new MgMapCreator(resSvc);
            var sessionCreator = new MgSessionCreator(_conn);
            var sessionApply = new MgApplySession(_userInfo);
            var session = new MgSession();

            //Resource Service
            _executors[typeof(Operations.ApplyResourcePackage).Name.ToUpper()] = new Operations.ApplyResourcePackage(resSvc, dbPath);
            _executors[typeof(Operations.ChangeResourceOwner).Name.ToUpper()] = new Operations.ChangeResourceOwner(resSvc, dbPath);
            _executors[typeof(Operations.CopyResource).Name.ToUpper()] = new Operations.CopyResource(resSvc, dbPath);
            _executors[typeof(Operations.DeleteResource).Name.ToUpper()] = new Operations.DeleteResource(resSvc, dbPath);
            _executors[typeof(Operations.DeleteResourceData).Name.ToUpper()] = new Operations.DeleteResourceData(resSvc, dbPath);
            _executors[typeof(Operations.EnumerateResourceData).Name.ToUpper()] = new Operations.EnumerateResourceData(resSvc, dbPath);
            _executors[typeof(Operations.EnumerateResourceReferences).Name.ToUpper()] = new Operations.EnumerateResourceReferences(resSvc, dbPath);
            _executors[typeof(Operations.EnumerateResources).Name.ToUpper()] = new Operations.EnumerateResources(resSvc, dbPath);
            _executors[typeof(Operations.GetRepositoryContent).Name.ToUpper()] = new Operations.GetRepositoryContent(resSvc, dbPath);
            _executors[typeof(Operations.GetRepositoryHeader).Name.ToUpper()] = new Operations.GetRepositoryHeader(resSvc, dbPath);
            _executors[typeof(Operations.GetResourceContent).Name.ToUpper()] = new Operations.GetResourceContent(resSvc, dbPath);
            _executors[typeof(Operations.GetResourceData).Name.ToUpper()] = new Operations.GetResourceData(resSvc, dbPath);
            _executors[typeof(Operations.GetResourceHeader).Name.ToUpper()] = new Operations.GetResourceHeader(resSvc, dbPath);
            _executors[typeof(Operations.InheritPermissionsFrom).Name.ToUpper()] = new Operations.InheritPermissionsFrom(resSvc, dbPath);
            _executors[typeof(Operations.MoveResource).Name.ToUpper()] = new Operations.MoveResource(resSvc, dbPath);
            _executors[typeof(Operations.RenameResourceData).Name.ToUpper()] = new Operations.RenameResourceData(resSvc, dbPath);
            _executors[typeof(Operations.SetResource).Name.ToUpper()] = new Operations.SetResource(resSvc, dbPath);
            _executors[typeof(Operations.SetResourceData).Name.ToUpper()] = new Operations.SetResourceData(resSvc, dbPath);
            _executors[typeof(Operations.UpdateRepository).Name.ToUpper()] = new Operations.UpdateRepository(resSvc, dbPath);

            //Feature Service
            _executors[typeof(Operations.DescribeFeatureSchema).Name.ToUpper()] = new Operations.DescribeFeatureSchema(featSvc, dbPath);
            _executors[typeof(Operations.ExecuteSqlQuery).Name.ToUpper()] = new Operations.ExecuteSqlQuery(featSvc, dbPath);
            _executors[typeof(Operations.GetClasses).Name.ToUpper()] = new Operations.GetClasses(featSvc, dbPath);
            _executors[typeof(Operations.GetConnectionPropertyValues).Name.ToUpper()] = new Operations.GetConnectionPropertyValues(featSvc, dbPath);
            _executors[typeof(Operations.GetFeatureProviders).Name.ToUpper()] = new Operations.GetFeatureProviders(featSvc, dbPath);
            _executors[typeof(Operations.GetLongTransactions).Name.ToUpper()] = new Operations.GetLongTransactions(featSvc, dbPath);
            _executors[typeof(Operations.GetProviderCapabilities).Name.ToUpper()] = new Operations.GetProviderCapabilities(featSvc, dbPath);
            _executors[typeof(Operations.GetSchemas).Name.ToUpper()] = new Operations.GetSchemas(featSvc, dbPath);
            _executors[typeof(Operations.GetSpatialContexts).Name.ToUpper()] = new Operations.GetSpatialContexts(featSvc, dbPath);
            _executors[typeof(Operations.SelectAggregates).Name.ToUpper()] = new Operations.SelectAggregates(featSvc, dbPath);
            _executors[typeof(Operations.SelectFeatures).Name.ToUpper()] = new Operations.SelectFeatures(featSvc, dbPath);
            _executors[typeof(Operations.SetLongTransaction).Name.ToUpper()] = new Operations.SetLongTransaction(featSvc, dbPath, sessionCreator, sessionApply);
            _executors[typeof(Operations.TestConnection).Name.ToUpper()] = new Operations.TestConnection(featSvc, dbPath);

            //Map and Layers
            _executors[typeof(Operations.AddLayerGroup).Name.ToUpper()] = new Operations.AddLayerGroup(resSvc, dbPath, creator);
            _executors[typeof(Operations.AddLayer).Name.ToUpper()] = new Operations.AddLayer(resSvc, dbPath, creator);
            _executors[typeof(Operations.GetCoordinateSystem).Name.ToUpper()] = new Operations.GetCoordinateSystem(resSvc, dbPath, creator);
            _executors[typeof(Operations.GetDataExtent).Name.ToUpper()] = new Operations.GetDataExtent(resSvc, dbPath, creator);
            _executors[typeof(Operations.GetDisplayInLegend).Name.ToUpper()] = new Operations.GetDisplayInLegend(resSvc, dbPath, creator);
            _executors[typeof(Operations.GetLayerFeatureClass).Name.ToUpper()] = new Operations.GetLayerFeatureClass(resSvc, dbPath, creator);
            _executors[typeof(Operations.GetLayerFeatureSource).Name.ToUpper()] = new Operations.GetLayerFeatureSource(resSvc, dbPath, creator);
            _executors[typeof(Operations.GetLayerDefinition).Name.ToUpper()] = new Operations.GetLayerDefinition(resSvc, dbPath, creator);
            _executors[typeof(Operations.GetGroups).Name.ToUpper()] = new Operations.GetGroups(resSvc, dbPath, creator);
            _executors[typeof(Operations.GetLayers).Name.ToUpper()] = new Operations.GetLayers(resSvc, dbPath, creator);
            _executors[typeof(Operations.GetLayerVisibility).Name.ToUpper()] = new Operations.GetLayerVisibility(resSvc, dbPath, creator);
            _executors[typeof(Operations.GetLegendLabel).Name.ToUpper()] = new Operations.GetLegendLabel(resSvc, dbPath, creator);
            _executors[typeof(Operations.GetMapExtent).Name.ToUpper()] = new Operations.GetMapExtent(resSvc, dbPath, creator);
            _executors[typeof(Operations.GetMapName).Name.ToUpper()] = new Operations.GetMapName(resSvc, dbPath, creator);
            _executors[typeof(Operations.GetViewCenter).Name.ToUpper()] = new Operations.GetViewCenter(resSvc, dbPath, creator);
            _executors[typeof(Operations.GetViewScale).Name.ToUpper()] = new Operations.GetViewScale(resSvc, dbPath, creator);
            _executors[typeof(Operations.HideGroup).Name.ToUpper()] = new Operations.HideGroup(resSvc, dbPath, creator);
            _executors[typeof(Operations.IsLayerVisible).Name.ToUpper()] = new Operations.IsLayerVisible(resSvc, dbPath, creator);
            _executors[typeof(Operations.LayerExists).Name.ToUpper()] = new Operations.LayerExists(resSvc, dbPath, creator);
            _executors[typeof(Operations.RemoveGroup).Name.ToUpper()] = new Operations.RemoveGroup(resSvc, dbPath, creator);
            _executors[typeof(Operations.ShowGroup).Name.ToUpper()] = new Operations.ShowGroup(resSvc, dbPath, creator);

            //Drawing Service
            _executors[typeof(Operations.DescribeDrawing).Name.ToUpper()] = new Operations.DescribeDrawing(drawSvc, dbPath);
            _executors[typeof(Operations.GetDrawing).Name.ToUpper()] = new Operations.GetDrawing(drawSvc, dbPath);
            _executors[typeof(Operations.EnumerateDrawingLayers).Name.ToUpper()] = new Operations.EnumerateDrawingLayers(drawSvc, dbPath);
            _executors[typeof(Operations.GetDrawingLayer).Name.ToUpper()] = new Operations.GetDrawingLayer(drawSvc, dbPath);
            _executors[typeof(Operations.GetDrawingSection).Name.ToUpper()] = new Operations.GetDrawingSection(drawSvc, dbPath);
            _executors[typeof(Operations.EnumerateDrawingSections).Name.ToUpper()] = new Operations.EnumerateDrawingSections(drawSvc, dbPath);
            _executors[typeof(Operations.EnumerateDrawingSectionResources).Name.ToUpper()] = new Operations.EnumerateDrawingSectionResources(drawSvc, dbPath);
            _executors[typeof(Operations.GetDrawingSectionResource).Name.ToUpper()] = new Operations.GetDrawingSectionResource(drawSvc, dbPath);

            //Mapping Service

            //Rendering Service

            //Server Admin
            _executors[typeof(Operations.Offline).Name.ToUpper()] = new Operations.Offline(admin, dbPath);
            _executors[typeof(Operations.Online).Name.ToUpper()] = new Operations.Online(admin, dbPath);
            _executors[typeof(Operations.GetLog).Name.ToUpper()] = new Operations.GetLog(admin, dbPath);
            _executors[typeof(Operations.GetLogByDate).Name.ToUpper()] = new Operations.GetLogByDate(admin, dbPath);
            _executors[typeof(Operations.ClearLog).Name.ToUpper()] = new Operations.ClearLog(admin, dbPath);
            _executors[typeof(Operations.DeleteLog).Name.ToUpper()] = new Operations.DeleteLog(admin, dbPath);
            _executors[typeof(Operations.RenameLog).Name.ToUpper()] = new Operations.RenameLog(admin, dbPath);
            _executors[typeof(Operations.EnumeratePackages).Name.ToUpper()] = new Operations.EnumeratePackages(admin, dbPath);
            _executors[typeof(Operations.DeletePackage).Name.ToUpper()] = new Operations.DeletePackage(admin, dbPath);
            _executors[typeof(Operations.LoadPackage).Name.ToUpper()] = new Operations.LoadPackage(admin, dbPath);
            _executors[typeof(Operations.GetPackageStatus).Name.ToUpper()] = new Operations.GetPackageStatus(admin, dbPath);
            _executors[typeof(Operations.GetPackageLog).Name.ToUpper()] = new Operations.GetPackageLog(admin, dbPath);

            //Site Service
            _executors[typeof(Operations.CreateSession).Name.ToUpper()] = new Operations.CreateSession(site, dbPath, session);
            _executors[typeof(Operations.DestroySession).Name.ToUpper()] = new Operations.DestroySession(site, dbPath);
            _executors[typeof(Operations.GetUserForSession).Name.ToUpper()] = new Operations.GetUserForSession(site, dbPath, session);
            _executors[typeof(Operations.EnumerateUsers).Name.ToUpper()] = new Operations.EnumerateUsers(site, dbPath);
            _executors[typeof(Operations.AddUser).Name.ToUpper()] = new Operations.AddUser(site, dbPath);
            _executors[typeof(Operations.UpdateUser).Name.ToUpper()] = new Operations.UpdateUser(site, dbPath);
            _executors[typeof(Operations.DeleteUsers).Name.ToUpper()] = new Operations.DeleteUsers(site, dbPath);
            _executors[typeof(Operations.GrantRoleMembershipsToUsers).Name.ToUpper()] = new Operations.GrantRoleMembershipsToUsers(site, dbPath);
            _executors[typeof(Operations.RevokeRoleMembershipsFromUsers).Name.ToUpper()] = new Operations.RevokeRoleMembershipsFromUsers(site, dbPath);
            _executors[typeof(Operations.GrantGroupMembershipsToUsers).Name.ToUpper()] = new Operations.GrantGroupMembershipsToUsers(site, dbPath);
            _executors[typeof(Operations.RevokeGroupMembershipsFromUsers).Name.ToUpper()] = new Operations.RevokeGroupMembershipsFromUsers(site, dbPath);
            _executors[typeof(Operations.EnumerateGroups).Name.ToUpper()] = new Operations.EnumerateGroups(site, dbPath);
            _executors[typeof(Operations.EnumerateGroups2).Name.ToUpper()] = new Operations.EnumerateGroups2(site, dbPath);
            _executors[typeof(Operations.EnumerateRoles2).Name.ToUpper()] = new Operations.EnumerateRoles2(site, dbPath);
            _executors[typeof(Operations.AddGroup).Name.ToUpper()] = new Operations.AddGroup(site, dbPath);
            _executors[typeof(Operations.UpdateGroup).Name.ToUpper()] = new Operations.UpdateGroup(site, dbPath);
            _executors[typeof(Operations.DeleteGroups).Name.ToUpper()] = new Operations.DeleteGroups(site, dbPath);
            _executors[typeof(Operations.GrantRoleMembershipsToGroups).Name.ToUpper()] = new Operations.GrantRoleMembershipsToGroups(site, dbPath);
            _executors[typeof(Operations.RevokeRoleMembershipsFromGroups).Name.ToUpper()] = new Operations.RevokeRoleMembershipsFromGroups(site, dbPath);
            _executors[typeof(Operations.EnumerateRoles).Name.ToUpper()] = new Operations.EnumerateRoles(site, dbPath);
            _executors[typeof(Operations.EnumerateServers).Name.ToUpper()] = new Operations.EnumerateServers(site, dbPath);
            _executors[typeof(Operations.AddServer).Name.ToUpper()] = new Operations.AddServer(site, dbPath);
            _executors[typeof(Operations.UpdateServer).Name.ToUpper()] = new Operations.UpdateServer(site, dbPath);
            _executors[typeof(Operations.RemoveServer).Name.ToUpper()] = new Operations.RemoveServer(site, dbPath);

            //Web Layout
            _executors[typeof(Operations.WL_GetTitle).Name.ToUpper()] = new Operations.WL_GetTitle(wlCreator, dbPath);
            _executors[typeof(Operations.WL_GetMapDefinition).Name.ToUpper()] = new Operations.WL_GetMapDefinition(wlCreator, dbPath);
            _executors[typeof(Operations.WL_GetScale).Name.ToUpper()] = new Operations.WL_GetScale(wlCreator, dbPath);
            _executors[typeof(Operations.WL_GetCenter).Name.ToUpper()] = new Operations.WL_GetCenter(wlCreator, dbPath);
            _executors[typeof(Operations.WL_ShowToolbar).Name.ToUpper()] = new Operations.WL_ShowToolbar(wlCreator, dbPath);
            _executors[typeof(Operations.WL_ShowStatusbar).Name.ToUpper()] = new Operations.WL_ShowStatusbar(wlCreator, dbPath);
            _executors[typeof(Operations.WL_ShowTaskpane).Name.ToUpper()] = new Operations.WL_ShowTaskpane(wlCreator, dbPath);
            _executors[typeof(Operations.WL_ShowTaskbar).Name.ToUpper()] = new Operations.WL_ShowTaskbar(wlCreator, dbPath);
            _executors[typeof(Operations.WL_ShowLegend).Name.ToUpper()] = new Operations.WL_ShowLegend(wlCreator, dbPath);
            _executors[typeof(Operations.WL_ShowProperties).Name.ToUpper()] = new Operations.WL_ShowProperties(wlCreator, dbPath);
            _executors[typeof(Operations.WL_GetTaskPaneWidth).Name.ToUpper()] = new Operations.WL_GetTaskPaneWidth(wlCreator, dbPath);
            _executors[typeof(Operations.WL_GetInformationPaneWidth).Name.ToUpper()] = new Operations.WL_GetInformationPaneWidth(wlCreator, dbPath);
            _executors[typeof(Operations.WL_GetInitialTaskUrl).Name.ToUpper()] = new Operations.WL_GetInitialTaskUrl(wlCreator, dbPath);
            _executors[typeof(Operations.WL_ShowContextMenu).Name.ToUpper()] = new Operations.WL_ShowContextMenu(wlCreator, dbPath);
            _executors[typeof(Operations.WL_TestUiItem).Name.ToUpper()] = new Operations.WL_TestUiItem(wlCreator, dbPath);
            _executors[typeof(Operations.WL_HomeTooltip).Name.ToUpper()] = new Operations.WL_HomeTooltip(wlCreator, dbPath);
            _executors[typeof(Operations.WL_HomeDescription).Name.ToUpper()] = new Operations.WL_HomeDescription(wlCreator, dbPath);
            _executors[typeof(Operations.WL_BackTooltip).Name.ToUpper()] = new Operations.WL_BackTooltip(wlCreator, dbPath);
            _executors[typeof(Operations.WL_BackDescription).Name.ToUpper()] = new Operations.WL_BackDescription(wlCreator, dbPath);
            _executors[typeof(Operations.WL_ForwardTooltip).Name.ToUpper()] = new Operations.WL_ForwardTooltip(wlCreator, dbPath);
            _executors[typeof(Operations.WL_ForwardDescription).Name.ToUpper()] = new Operations.WL_ForwardDescription(wlCreator, dbPath);
            _executors[typeof(Operations.WL_TasksName).Name.ToUpper()] = new Operations.WL_TasksName(wlCreator, dbPath);
            _executors[typeof(Operations.WL_TasksTooltip).Name.ToUpper()] = new Operations.WL_TasksTooltip(wlCreator, dbPath);
            _executors[typeof(Operations.WL_TasksDescription).Name.ToUpper()] = new Operations.WL_TasksDescription(wlCreator, dbPath);
        }

        class MgWebLayoutCreator : Operations.IWebLayoutCreator
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

        class MgMapCreator : Operations.IMapCreator
        {
            private MgResourceService _resSvc;
            private MgMap _map;

            public MgMapCreator(MgResourceService resSvc)
            {
                _resSvc = resSvc;
            }

            public MgMapBase CreateMap(MgResourceIdentifier resId)
            {
                //The test suite relies on the same map and not a new one
                //re-created on each test of that particular test suite
                if (_map != null)
                    return _map;

                _map = new MgMap();
                _map.Create(_resSvc, resId, resId.GetName());
                return _map;
            }

            public MgLayerBase CreateLayer(MgResourceIdentifier resId)
            {
                return new MgLayer(resId, _resSvc);
            }
        }

        class MgSessionCreator : Operations.ISessionCreator
        {
            private MgSiteConnection _siteConn;

            public MgSessionCreator(MgSiteConnection siteConn)
            {
                _siteConn = siteConn;
            }

            public string CreateSession()
            {
                var site = _siteConn.GetSite();
                return site.CreateSession();
            }
        }

        class MgApplySession : Operations.IApplySession
        {
            private MgUserInformation _userInfo;

            public MgApplySession(MgUserInformation userInfo)
            {
                _userInfo = userInfo;
            }

            public void SetSessionId(string sessionId)
            {
                _userInfo.SetMgSessionId(sessionId);
            }
        }

        class MgSession : Operations.IMapGuideSession
        {
            public string SessionID
            {
                get;
                set;
            }
        }

        public override ITestExecutor GetTestExecutor(string opName)
        {
            if (_executors.ContainsKey(opName))
                return _executors[opName];
            throw new Exception("Executor for " + opName + " not found");
        }

        public override int ValidateRequest(SqliteDotNet.SqliteDb db, string testName, int paramSetId, string operation, TestResult actualResult, ITestLogger logger)
        {
            int exitStatus = 0;
            string outcome = "pass";
            SqliteVm vm = new SqliteVm(db, false);

            object expectedResult = null;

            //If we have an exception we need to remove the stack trace because different line numbers will fail the test
            object resultData = CommonUtility.RemoveStackTraceFromResult(actualResult.ResultData);
            //Get the mime type based on the content type in the result
            string mimeType = actualResult.ContentType;
            //If we have exception message we need to remove any parts that may contain system dependent information
            //Ex. file paths
            resultData = CommonUtility.ProcessExceptionMessage(resultData);
            //Get the file extension that will be used for a dump
            string actualExtension = CommonUtility.GetExtensionFromMimeType(mimeType);

            //If we have an ALWAYSPASS parameter defined for the operation then skip the whole validation process
            //This parameter should only be used for clean up operations that are no related with the tests
            if (vm.Execute("Select ParamValue from Params where ParamName=\"ALWAYSPASS\" and ParamSet={0}", paramSetId) != Sqlite.Row)
            {
                //TestName is Test_[ServiceType]
                string type = testName.Substring(testName.IndexOf("_") + 1);
                string filePath = CommonUtility.GetPath(string.Format("../../TestData/{0}/DumpFiles/{0}ApiTest", type));
                string fileName = string.Format("{0}_{1}.{2}", filePath, paramSetId, actualExtension);

                if (this.TestExecutionMode == "dump")
                {
                    //File.WriteAllText(fileName, resultData);
                    throw new NotImplementedException("The .net test runner does not support dumping of test results. Please use the PHP test runner for this purpose");
                }
                else
                {
                    //This section is special case handling for the operations that return different data after each call
                    resultData = CommonUtility.SpecialDataHandling(operation, resultData, mimeType);

                    if (this.TestExecutionMode == "generate")
                    {
                        throw new NotImplementedException("The .net test runner does not support test update/generation. Please use the PHP test runner for this purpose");
                        /*
                        //Get the sample result that is stored in the database. If we are using file on disk for validation
                        //then do not overwrite the filename in the database
                        //To distinguish between sample data and filename all filenames should be prefixed with "@@"
                        int status = vm.Execute("Select Result from ApiTestResults where ParamSet={0}", paramSetId);
                        string sampleResult = vm.GetString("Result");

                        if (!sampleResult.StartsWith("@@"))
                        {
                            //Insert the sample data as a BLOB
                            //Update the row for that param set or create a new row if we do not have it yet

                            string responseBody = "";
                            if (status == Sqlite.Row)
                            {
                                vm.Prepare("update ApiTestResults set Result = :blob where ParamSet={0}", paramSetId);
                            }
                            else
                            {
                                Console.WriteLine("A new row has been created in ApiTestResults table to store the result for operation {0}", paramSetId);
                                Console.WriteLine("Please update the description field for that row later");
                                vm.Prepare("INSERT INTO ApiTestResults(ParamSet, Result) VALUES({0}, :blob)", paramSetId);
                            }

                            byte[] bytes = Encoding.UTF8.GetBytes(resultData);
                            vm.BindBlob(":blob", bytes);
                            vm.Execute();

                            if (mimeType != null)
                            {
                                vm.Execute("UPDATE ApiTestResults SET ContentType=\"{0}\" WHERE ParamSet={1}", mimeType, paramSetId);
                            }

                            File.WriteAllText(fileName, resultData);
                        }
                         */
                    }
                    else if (this.TestExecutionMode == "validate" || this.TestExecutionMode == "show")
                    {   
                        string resultContent = "";

                        //Get the sample result and the expected content type from the database
                        int status = vm.Execute("Select Result, ContentType from ApiTestResults where ParamSet={0}", paramSetId);
                        string expectedContentType = vm.GetString("ContentType");
                        string expectedExtension = CommonUtility.GetExtensionFromMimeType(expectedContentType);
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
                                expectedResult = string.Empty;
                            else
                                expectedResult = null;
                        }
                        string strExpectedResult = expectedResult as string;
                        //If we are validating from a file then get the contents of that file
                        //File names should be prefixed with "@@" to distinguish them from BLOB data
                        if (strExpectedResult != null && strExpectedResult.StartsWith("@@"))
                        {
                            string sampleDataFile = strExpectedResult.Substring(2);
                            sampleDataFile = CommonUtility.GetPath(sampleDataFile);
                            expectedResult = File.ReadAllText(sampleDataFile);
                        }

                        if (this.TestExecutionMode == "validate")
                        {
                            bool bEqual = false;
                            byte[] bExpected = expectedResult as byte[];
                            byte[] bActual = resultData as byte[];
                            string strResultData = resultData as string;

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
                                    bEqual = strResultData.Equals(strExpectedResult, StringComparison.InvariantCultureIgnoreCase);
                                }
                                else if (bExpected != null && bActual != null)
                                {
                                    bEqual = CommonUtility.ByteArraysEqual(bExpected, bActual, operation, testName);
                                }
                                else
                                {
                                    System.Diagnostics.Debug.WriteLine(string.Format("[MgTestRunner]: {0} - {1} - Encountered disparate data types between expected and actual results. Expecting test failure :(", testName, operation));
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
                            /*
                            type = testName.Substring(testName.IndexOf("_") + 1);
                            string showPath = CommonUtility.GetPath(string.Format("../../TestData/{0}/ShowFiles/{0}ApiTest", type));
                            string showName = string.Format("{0}_{1}.{2}", showPath, paramSetId, actualExtension);
                            File.WriteAllText(showName, expectedResult);
                             */
                        }
                    }
                }
            }

            if (outcome == "fail")
            {
                Console.WriteLine("****{0} {1} {2} failed.\n", testName, paramSetId, operation);
                string str = string.Format("\n****ACTUAL RESULT****\n{0}\n****EXPECTED RESULT****\n{1}\n********\n\n\n", resultData, expectedResult);
                //Console.WriteLine(str);
                //Console.WriteLine("<FAIL>");
                logger.Write(str);
            }

            vm.SqlFinalize();
            vm = null;

            return exitStatus;
        }

        public override void Cleanup()
        {
            foreach (var exec in _executors.Values)
            {
                exec.Dispose();
            }
            _executors.Clear();
        }

        public override string ApiType
        {
            get { return ApiTypes.Platform; }
        }
    }
}
