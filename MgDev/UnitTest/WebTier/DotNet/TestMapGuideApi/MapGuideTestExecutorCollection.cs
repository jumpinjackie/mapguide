using OSGeo.MapGuide;
using OSGeo.MapGuide.Test.Common;
using SqliteDotNet;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

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

            var creator = new MgMapCreator(resSvc);
            var sessionCreator = new MgSessionCreator(_conn);
            var sessionApply = new MgApplySession(_userInfo);

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

            //Site Service

            //Web Layout
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

            string expectedResult = "";

            //If we have an exception we need to remove the stack trace because different line numbers will fail the test
            string resultData = CommonUtility.RemoveStackTraceFromResult(actualResult.ResultData);
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
                    File.WriteAllText(fileName, resultData);
                }
                else
                {
                    //This section is special case handling for the operations that return different data after each call
                    resultData = CommonUtility.SpecialDataHandling(operation, resultData, mimeType);

                    if (this.TestExecutionMode == "generate")
                    {
                        throw new NotImplementedException("The .net test runner does not support test update/generation. Please use the PHP test runner");
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
                            expectedResult = Encoding.UTF8.GetString(b);

                        //If we are validating from a file then get the contents of that file
                        //File names should be prefixed with "@@" to distinguish them from BLOB data
                        if (expectedResult.StartsWith("@@"))
                        {
                            string sampleDataFile = expectedResult.Substring(2);
                            sampleDataFile = CommonUtility.GetPath(sampleDataFile);
                            expectedResult = File.ReadAllText(sampleDataFile);
                        }

                        if (this.TestExecutionMode == "validate")
                        {
                            //If the results are different and special validation fails then the operation failed ->mark it red
                            if (!resultData.Equals(expectedResult, StringComparison.InvariantCultureIgnoreCase) && !CommonUtility.SpecialValidation(operation, resultData, expectedResult))
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
                            type = testName.Substring(testName.IndexOf("_") + 1);
                            string showPath = CommonUtility.GetPath(string.Format("../../TestData/{0}/ShowFiles/{0}ApiTest", type));
                            string showName = string.Format("{0}_{1}.{2}", showPath, paramSetId, actualExtension);
                            File.WriteAllText(showName, expectedResult);
                        }
                    }
                }
            }

            if (outcome == "fail")
            {
                Console.WriteLine("****{0} {1} {2} failed.\n", testName, paramSetId, operation);
                string str = string.Format("\n****ACTUAL RESULT****\n{0}\n****EXPECTED RESULT****\n{1}\n********\n\n\n", resultData, expectedResult);
                Console.WriteLine(str);
                logger.Write(str);
            }

            vm.SqlFinalize();
            vm = null;

            return exitStatus;
        }

        public override void Cleanup()
        {
            
        }

        public override string ApiType
        {
            get { return ApiTypes.Platform; }
        }
    }
}
