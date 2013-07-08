#if DEBUG
#define DEBUG_BINARY_COMPARISON
#endif
using SqliteDotNet;
using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace OSGeo.MapGuide.Test.Common
{
    /// <summary>
    /// Extension methods for SqliteVm
    /// </summary>
    public static class SqliteDbExtensions
    {
        public static int Prepare(this SqliteVm vm, string sql, params object[] args)
        {
            string formattedSql = string.Format(sql, args);
            return vm.Prepare(formattedSql);
        }

        public static int Execute(this SqliteVm vm, string sql, params object[] args)
        {
            string formattedSql = string.Format(sql, args);
            return vm.Execute(formattedSql);
        }

        public static bool ReadParameterValue(this SqliteVm vm, int paramSetId, string paramName, NameValueCollection result, bool bIsPath = false)
        {
            int stat = vm.Execute("Select ParamValue from Params WHERE ParamSet={0} AND ParamName=\"{1}\"", paramSetId, paramName);
            if (stat == Sqlite.Row)
            {
                string str = vm.GetString("ParamValue");
                if (bIsPath)
                {
                    str = CommonUtility.GetPath(str);
                }
                result.Add(paramName, str);
                return true;
            }
            return false;
        }

        public static bool ReadParameterValue(this SqliteVm vm, string paramName, NameValueCollection result)
        {
            int stat = vm.Execute("Select ParamValue from Params WHERE ParamName=\"{0}\"", paramName);
            if (stat == Sqlite.Row)
            {
                result.Add(paramName, vm.GetString("ParamValue"));
                return true;
            }
            return false;
        }

        public static bool ReadCommonParameterValue(this SqliteVm vm, string paramName, NameValueCollection result)
        {
            int stat = vm.Execute("Select ParamValue from CommonParams WHERE ParamName=\"{0}\"", paramName);
            if (stat == Sqlite.Row)
            {
                result.Add(paramName, vm.GetString("ParamValue"));
                return true;
            }
            return false;
        }
    }

    /// <summary>
    /// Common utility methods
    /// </summary>
    public class CommonUtility
    {
        public static NameValueCollection SetCommonParams(int paramSet, SqliteDb db)
        {
            NameValueCollection result = null;
            var vm = new SqliteVm(db, false);
            try
            {
                result = new NameValueCollection();

                vm.ReadParameterValue(paramSet, "OPERATION", result);
                vm.ReadCommonParameterValue("VERSION", result);
                vm.ReadCommonParameterValue("CREDENTIALS", result);
                vm.ReadCommonParameterValue("LOCALE", result);

                vm = null;
            }
            catch (MgException ex)
            {
                throw new UnitTestException(string.Format("Exception from MapGuide:\n{0}", ex.GetDetails()));
            }
            catch (Exception ex)
            {
                try
                {
                    vm = null;
                    vm = new SqliteVm(db, true);
                    vm.ReadParameterValue("VERSION", result);
                    vm.ReadParameterValue("CREDENTIALS", result);
                    vm.ReadParameterValue("LOCALE", result);
                }
                catch (Exception ex2)
                {

                }
            }
            return result;
        }

        public static MgByteReader GetByteReaderFromPath(string path, bool bCheck = true)
        {
            if (bCheck)
            {
                if (File.Exists(path))
                {
                    MgByteSource source = new MgByteSource(path);
                    MgByteReader reader = source.GetReader();
                    return reader;
                }
                return null;
            }
            else
            {
                MgByteSource source = new MgByteSource(path);
                MgByteReader reader = source.GetReader();
                return reader;
            }
        }

        public static string GetDbPath(string dumpFileName)
        {
            var db = new SqliteDb();
            var dbPath = dumpFileName.Replace(".dump", ".db");
            var dbName = CommonUtility.GetPath(dbPath);

            if (!File.Exists(dumpFileName) && !File.Exists(dbName))
            {
                throw new UnitTestException(string.Format("Error: Dump file {0} not found. Unable to create database file", dumpFileName));
            }
            else if (!File.Exists(dbName))
            {
                db.GenerateDatabase(dumpFileName, dbName);
            }
            else if (File.Exists(dumpFileName) && File.GetLastWriteTimeUtc(dumpFileName) > File.GetLastWriteTimeUtc(dbName))
            {
                try
                {
                    File.Delete(dbName);
                    db.GenerateDatabase(dumpFileName, dbName);
                }
                catch
                {
                    throw new UnitTestException(string.Format("Unable to delete database file {0}. The file is either in use or is read-only. The database has not been updated", dbName));
                }
            }

            return dbPath;
        }

        public static string GetPath(string dbPath)
        {
            if (Path.IsPathRooted(dbPath))
                return dbPath.Replace("\\", "/");
            else
                return Path.Combine(GetAssemblyPath(), dbPath).Replace("\\", "/");
        }

        private static string GetAssemblyPath()
        {
            return Path.GetDirectoryName(new Uri(Assembly.GetExecutingAssembly().CodeBase).LocalPath);
        }

        public static MgStringCollection StringToMgStringCollection(string str)
        {
            try
            {
                MgStringCollection coll = new MgStringCollection();
                string[] tokens = str.Split(',');
                foreach (var token in tokens)
                {
                    coll.Add(token);
                }
                return coll;
            }
            catch (MgException ex)
            {
                throw new UnitTestException(string.Format("Exception from MapGuide:\n{0}", ex.GetDetails()));
            }
        }

        public static string MgStringCollectionToString(MgStringCollection coll)
        {
            try
            {
                //Sigh, we're too smart for our own good. Yes, the trailing comma
                //should be there!
                StringBuilder sb = new StringBuilder();
                for (int i = 0; i < coll.GetCount(); i++)
                {
                    sb.Append(coll.GetItem(i));
                    sb.Append(",");
                }
                return sb.ToString();

                /*
                List<string> items = new List<string>();
                for (int i = 0; i < coll.GetCount(); i++)
                {
                    items.Add(coll.GetItem(i));
                }
                return string.Join(",", items.ToArray());
                 */
            }
            catch (MgException ex)
            {
                throw new UnitTestException(string.Format("Exception from MapGuide:\n{0}", ex.GetDetails()));
            }
        }

        public static string BooleanToString(bool b)
        {
            return b ? "True" : "False";
        }

        public static string MgEnvelopeToString(MgEnvelope env)
        {
            try
            {
                var ll = env.GetLowerLeftCoordinate();
                var ur = env.GetUpperRightCoordinate();
                return string.Format(CultureInfo.InvariantCulture,
                    "({0}:{1})-({2}:{3})", 
                    ll.GetX(), 
                    ll.GetY(), 
                    ur.GetX(), 
                    ur.GetY());
            }
            catch (MgException ex)
            {
                throw new UnitTestException(string.Format("Exception from MapGuide:\n{0}", ex.GetDetails()));
            }
        }

        public static string MgPointToString(MgPoint pt)
        {
            try
            {
                var coord = pt.GetCoordinate();
                return string.Format(CultureInfo.InvariantCulture,
                    "({0}:{1})",
                    coord.GetX(),
                    coord.GetY());
            }
            catch (MgException ex)
            {
                throw new UnitTestException(string.Format("Exception from MapGuide:\n{0}", ex.GetDetails()));
            }
        }

        public static string GetExtension(string name)
        {
            if (name.LastIndexOf(".") >= 0)
            {
                return name.Substring(name.LastIndexOf(".") + 1);
            }
            else
            {
                if (name == "MG_USER_CREDENTIALS")
                    return "txt";
                else
                    return "bin";
            }
        }

        public static string GetMimeType(string extension)
        {
            switch (extension)
            {
                case "agf":
                    return "application/agf";
                case "bin":
                    return "application/octet-stream";
                case "dwf":
                    return "model/vnd.dwf";
                case "jpg":
                case "jpeg":
                    return "image/jpeg";
                case "png":
                    return "image/png";
                case "tif":
                case "tiff":
                    return "image/tiff";
                case "html":
                    return "text/html";
                case "txt":
                    return "text/plain";
                case "xml":
                    return "text/xml";
                default:
                    return "application/octet-stream";
            }
        }

        public static object SpecialDataHandling(string operation, object resultData, string mimeType)
        {
            object res = resultData;
            switch (operation)
            {
                case "ENUMERATERESOURCES":
                    res = RemoveTimeStamp(resultData.ToString());
                    break;
                case "GETDRAWINGLAYER":
                    res = RemoveDwfSectionName(resultData, Encoding.UTF8);
                    break;
                case "GETDRAWINGSECTION":
                    res = RemoveDwfSectionName(resultData, Encoding.UTF8);
                    break;
                case "GETLOG":
                    res = RemoveLogEntryTimeStamp(resultData.ToString());
                    break;
                case "GETMAP":
                    res = GetMapHeader(resultData.ToString());
                    break;
                case "GETLONGTRANSACTIONS":
                    res = RemoveCreationDate(resultData.ToString());
                    break;
            }

            string strRes = res as string;
            if (strRes != null && mimeType == "text/xml")
            {
                var doc = new XmlDocument();
                doc.LoadXml(strRes);
                res = SortElement(doc, "");
            }
            return res;
        }

        class InvertedComparer<T> : IComparer<T>
        {
            private IComparer<T> _comp;

            public InvertedComparer(IComparer<T> comp) { _comp = comp; }

            public int Compare(T x, T y)
            {
                int res = _comp.Compare(x, y);
                //Invert the non-zero results
                if (res > 0)
                    return -1;
                else if (res < 0)
                    return 1;
                else
                    return 0;
            }
        }

        private static string SortElement(XmlNode elem, string preText)
        {
            var elemArray = new List<string>();
            string elemString = "";
            if (elem.ChildNodes.Count > 0)
            {
                int elCount = 0;
                int txtCount = 0;
                //foreach (XmlNode child in elem.ChildNodes)
                for (int i = 0; i < elem.ChildNodes.Count; i++)
                {
                    var child = elem.ChildNodes[i];
                    if (child.NodeType == XmlNodeType.Element)
                    {
                        var elemValue = SortElement(child, preText + "  ");
                        if (!string.IsNullOrEmpty(elemValue))
                        {
                            elemArray.Add(elemValue);
                            elCount++;
                        }
                    }
                    else if (child.NodeType == XmlNodeType.Text)
                    {
                        string content = child.InnerText.Trim();
                        if (!string.IsNullOrEmpty(content))
                        {
                            elemArray.Add(content);
                            txtCount++;
                        }
                    }
                }

                //We have to ordinal compare to match the sort behaviour of
                //sort() in PHP
                elemArray.Sort((s1, s2) =>
                {
                    return string.CompareOrdinal(s1, s2);
                });
                foreach (string str in elemArray)
                {
                    elemString += str;
                }
            }

            string endTag = "";
            if (elemArray.Count > 1 && elemString.Length > 0)
            {
                endTag = "\n" + preText;
            }
            string tagName = "";
            if (!(elem is XmlDocument))
            {
                tagName = elem.Name;
            }
            endTag += "</" + tagName + ">";

            if ("" != tagName)
            {
                elemString = "\n" + preText + "<" + tagName + ">" + elemString + endTag;
            }

            return elemString;
        }

        private static string RemoveTimeStamp(string resultData)
        {
            string result = resultData;
            string newResult = result;
            while (result.IndexOf("<CreatedDate>") >= 0)
            {
                newResult = result.Substring(0, result.IndexOf("<CreatedDate>"));
                newResult += result.Substring(result.IndexOf("</ModifiedDate>") + "</ModifiedDate>".Length);
                result = newResult;
            }
            return newResult;
        }

        private static object RemoveDwfSectionName(object resultData, Encoding enc)
        {
            bool bFromByteArray = false;
            byte[] bResultData = resultData as byte[];
            string strResultData = resultData as string;
            /*
            if (strResultData == null)
            {
                if (bResultData != null)
                {
                    strResultData = enc.GetString(bResultData);
                    bFromByteArray = true;
                }
            }*/

            if (strResultData != null)
            {
                //Console.WriteLine("RemoveDwfSectionName: length = {0}", strResultData.Length);
                int idx = strResultData.IndexOf(".w2d");
                //Console.WriteLine("RemoveDwfSectionName: widx = {0}", idx);
                if (idx >= 0)
                {
                    string newResult = strResultData.Substring(idx);
                    int eidx = newResult.IndexOf("EndOfDWF");
                    //Console.WriteLine("RemoveDwfSectionName: eidx = {0}", eidx);
                    if (0 != eidx)
                    {
                        newResult = newResult.Substring(0, eidx);
                        //Console.WriteLine("RemoveDwfSectionName: newlength = {0}", newResult.Length);
                    }
                    if (bFromByteArray)
                        return enc.GetBytes(newResult);
                    else
                        return newResult;
                }
            }
            else if (bResultData != null)
            {
                byte[] bW2d = enc.GetBytes(".w2d");
                byte[] bEOF = enc.GetBytes("EndOfDWF");

                int widx = -1;
                int eidx = -1;

                int wMatches = 0;
                int eMatches = 0;

                int i = 0;
                while(i < bResultData.Length)
                {
                    //Haven't found .w2d sequence
                    if (widx < 0 && wMatches == 0) 
                    {
                        //We've found a "."
                        if (bResultData[i] == bW2d[0])
                        {
                            wMatches++;
                            i++;

                            //Now try to follow through this sequence to see if it is ".w2d"
                            while (wMatches < bW2d.Length)
                            {
                                //End of array. Abort
                                if (i >= bResultData.Length)
                                    break;

                                //Next byte in sequence matches. Advance
                                if (bResultData[i] == bW2d[wMatches])
                                {
                                    //Increment matches
                                    wMatches++;
                                    
                                    //Check if full sequence matches
                                    if (wMatches == bW2d.Length)
                                    {
                                        //Match. Record index which is current position minus the # of consecutive matches
                                        widx = i - wMatches;
                                        break;
                                    }
                                }
                                else //Incomplete sequence. Break this loop
                                {
                                    wMatches = 0; //Reset
                                    break;
                                }
                            }
                        }
                    }
                    //Haven't found EndOfDWF sequence
                    else if (eidx < 0 && eMatches == 0)
                    {
                        //We've found a "E"
                        if (bResultData[i] == bEOF[0])
                        {
                            eMatches++;
                            i++;

                            //Now try to follow through this sequence to see if it is "EndOfDWF"
                            while (eMatches < bEOF.Length)
                            {
                                //End of array. Abort
                                if (i >= bResultData.Length)
                                    break;

                                //Next byte in sequence matches. Advance
                                if (bResultData[i] == bEOF[eMatches])
                                {
                                    //Increment matches
                                    eMatches++;

                                    //Check if full sequence matches
                                    if (eMatches == bEOF.Length)
                                    {
                                        //Match. Record index which is current position minus the # of consecutive matches
                                        eidx = i - eMatches;
                                        break;
                                    }
                                }
                                else //Incomplete sequence. Break this loop
                                {
                                    eMatches = 0; //Reset
                                    break;
                                }
                            }
                        }
                    }

                    //Found both offsets. We're done
                    if (widx > 0 && eidx > widx)
                        break;

                    i++;
                }

                if (widx > 0 && eidx > widx)
                {
                    byte[] newResult = new byte[eidx - widx];
                    int off = 0;
                    for (int j = widx; j <= eidx; j++)
                    {
                        newResult[off] = bResultData[j];
                    }
                }
            }
            return resultData;
        }

        private static string RemoveLogEntryTimeStamp(string resultData)
        {
            string result = resultData;
            string newResult = result;
            while (result.IndexOf("<") >= 0)
            {
                newResult = result.Substring(0, result.IndexOf("<"));
                newResult += result.Substring(result.IndexOf(">") + 1);
                result = newResult;
            }
            return newResult;
        }

        private static string GetMapHeader(string resultData)
        {
            if (resultData.IndexOf("(DWF V06.01)") >= 0)
                resultData = "(DWF V06.01)";
            return resultData;
        }

        private static string RemoveCreationDate(string resultData)
        {
            string newResult = resultData;
            while (resultData.IndexOf("<CreationDate>") >= 0)
            {
                newResult = resultData.Substring(0, resultData.IndexOf("<CreationDate>"));
                newResult += resultData.Substring(resultData.IndexOf("</CreationDate>") + "</CreationDate>".Length);
                resultData = newResult;
            }
            return newResult;
        }

        public static object ProcessExceptionMessage(object resultData)
        {
            string strResultData = resultData as string;
            if (strResultData != null)
            {
                string text = "exception occurred";
                if (strResultData.Contains(text))
                {
                    strResultData = strResultData.Substring(0, strResultData.IndexOf(text) + text.Length);
                }
                return strResultData;
            }
            return resultData;
        }

        public static bool SpecialValidation(string operation, object resultData, object expectedResult)
        {
            if (operation == "GETFEATUREPROVIDERS") 
            {
                //We expect both to be strings here
                return GetFeatureProvidersValidation(resultData.ToString(), expectedResult.ToString());
            }
            return false;
        }

        private static bool GetFeatureProvidersValidation(string resultData, string expectedResult)
        {
            throw new NotImplementedException();
        }

        public static object RemoveStackTraceFromResult(object result)
        {
            var strResult = result as string;
            //TODO: Clean out stack trace
            return result;
        }

        public static string GetExtensionFromMimeType(string mimeType)
        {
            string extension = "xml";
            if (mimeType.Contains("ePlot"))
                return "dwf";
            if (mimeType.Contains("text/plain"))
                return "txt";
            if (mimeType.Contains("text/html"))
                return "html";

            switch (mimeType)
            { 
                case "application/agf":
                    return "agf";
                case "application/octet-stream":
                    return "bin";
                case "model/vnd.dwf":
                    return "dwf";
                case "image/jpeg":
                    return "jpg";
                case "image/png":
                    return "png";
                case "image/tiff":
                    return "tiff";
                case "application/x-w2d":
                    return "dwf";
            }

            return extension;
        }

        public static bool ByteArraysEqual(byte[] bExpected, byte[] bActual, string operation, string testName)
        {
            if (bExpected == null && bActual != null)
                return false;

            if (bExpected != null && bActual == null)
                return false;

            bool bRet = true;
#if DEBUG_BINARY_COMPARISON
            bool bLogged = false;
            Guid guid = Guid.NewGuid();
            using (StreamWriter sw1 = new StreamWriter(guid.ToString() + "_" + operation + "_" + testName + "_expected.txt", false))
            using (StreamWriter sw2 = new StreamWriter(guid.ToString() + "_" + operation + "_" + testName + "_actual.txt", false))
            {
#endif
            for (int i = 0; i < bExpected.Length; i++)
            {
                if (i >= bExpected.Length ||
                    i >= bActual.Length)
                {
                    break;
                }

                byte b1 = bExpected[i];
                byte b2 = bActual[i];

#if DEBUG_BINARY_COMPARISON
                sw1.WriteLine("{0} {1}", b1, Convert.ToChar(b1));
                sw2.WriteLine("{0} {1}", b2, Convert.ToChar(b2));
#endif

                if (b1 != b2)
                {
#if DEBUG_BINARY_COMPARISON
                    bRet = false;
                    if (!bLogged)
                    {
                        System.Diagnostics.Debug.WriteLine(string.Format("[MgTestRunner]: Comparison {0} returned false. See logged text files", guid.ToString()));
                        bLogged = true;
                    }
#else
                    return false;
#endif
                }
            }
#if DEBUG_BINARY_COMPARISON
            }

            if (bRet)
            {
                File.Delete(guid.ToString() + "_" + operation + "_" + testName + "_expected.txt");
                File.Delete(guid.ToString() + "_" + operation + "_" + testName + "_actual.txt");
            }
#endif

            System.Diagnostics.Debug.WriteLine(string.Format("[MgTestRunner]: {0} - {1} - COMPARE: {2} with {3} = {4}", testName, operation, bExpected.Length, bActual.Length, (bRet ? 0 : 1)));
            return bRet;
        }
    }
}
