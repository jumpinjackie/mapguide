using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using Doxygen.NET;
using System.Xml;

namespace DoxyTransform
{
    class Program
    {
        /*
         
         Why does this work? How is this possible? The following reasons:
         
          1. Doxygen supports XML output, allowing the API documentation to be represented in a logically structured form
          2. .net assemblies use XML files for documentation, meaning it is easy to transform the doxygen XML to .net XML
          3. Doxygen.NET supports parsing a subset of this XML form into Object-oriented classes
          4. The MapGuide API is fortunately designed and written in a convention-based manner (when you're making bindings 
             to 3 different languages, you have to!). The convention-based design allows us to make many valid assumptions:
            a. All classes will be under a single namespace (OSGeo.MapGuide)
            b. Only classes are exported out to the public API, no enums, structs or other fancy C++ features
            d. Any word in a summary starting with the letters "Mg" can be assumed to be a class, thus we can replace them with
               cref links to said class
            e. Get/Set Methods are decorated with assorted metadata for .net. Just like IMake.exe detects __set, __get as hints to generate .net properties, we can use
               these same markers as a hint to generate a property entry in our integrated API documentation
         
         */

        static void Main(string[] args)
        {
            if (args.Length != 2 && args.Length != 3)
            {
                Usage();
                return;
            }

            if (!Directory.Exists(args[1]))
            {
                Console.WriteLine("Doxygen XML output path not found: {0}", args[1]);
                return;
            }

            string srcDir = args[1];
            string outputPath = (args.Length == 3) ? args[2] : "";

            if (args[0].ToLower() == "dotnet")
            {
                ProcessDotNet(srcDir, outputPath);
            }
            else if (args[0].ToLower() == "java")
            {
                Console.WriteLine("java not supported yet");
            }
        }

        const string DOTNET_NAMESPACE = "OSGeo.MapGuide";
        const string JAVA_PACKAGE = "org.osgeo.mapguide";

        static XmlDocument CreateDotNetDocument(string assemblyName)
        {
            var doc = new XmlDocument();
            doc.LoadXml("<doc><assembly><name>" + assemblyName + "</name></assembly></doc>");
            return doc;
        }

        static string GetDotNetClassName(string className)
        {
            return DOTNET_NAMESPACE + className;
        }

        static string FormatDotNetCtor(Class klass, Constructor ctor)
        {
            if (ctor.Parameters.Count == 0)
            {
                return "M:" + DOTNET_NAMESPACE + "." + klass.Name + ".#ctor";
            }
            else
            {
                var paramTokens = new List<string>();
                foreach (var p in ctor.Parameters)
                {
                    string clrType = MgTypeToDotNetType(p.Type);
                    if (clrType == p.Type)
                        paramTokens.Add(DOTNET_NAMESPACE + "." + p.Type);
                    else
                        paramTokens.Add(clrType);
                }
                return "M:" + DOTNET_NAMESPACE + "." + klass.Name + ".#ctor(" + string.Join(", ", paramTokens.ToArray()) + ")";
            }
        }

        static string FormatDotNetCtor(Class klass, Method ctor)
        {
            if (ctor.Parameters.Count == 0)
            {
                return "M:" + DOTNET_NAMESPACE + "." + klass.Name + ".#ctor";
            }
            else
            {
                var paramTokens = new List<string>();
                foreach (var p in ctor.Parameters)
                {
                    string clrType = MgTypeToDotNetType(p.Type);
                    if (clrType == p.Type)
                        paramTokens.Add(DOTNET_NAMESPACE + "." + p.Type);
                    else
                        paramTokens.Add(clrType);
                }
                return "M:" + DOTNET_NAMESPACE + "." + klass.Name + ".#ctor(" + string.Join(", ", paramTokens.ToArray()) + ")";
            }
        }

        static string FormatDotNetMethod(Class klass, Method m)
        {
            if (m.Parameters.Count == 0)
            {
                return "M:" + DOTNET_NAMESPACE + "." + klass.Name + "." + m.Name;
            }
            else
            {
                var paramTokens = new List<string>();
                foreach (var p in m.Parameters)
                {
                    string clrType = MgTypeToDotNetType(p.Type);
                    if (clrType == p.Type)
                        paramTokens.Add(DOTNET_NAMESPACE + "." + p.Type);
                    else
                        paramTokens.Add(clrType);
                }
                return "M:" + DOTNET_NAMESPACE + "." + klass.Name + "." + m.Name + "(" + string.Join(", ", paramTokens.ToArray()) + ")";
            }
        }

        static string MgTypeToDotNetType(string mgType)
        {
            switch (mgType)
            {
                case "bool":
                    return "System.Boolean";
                case "double":
                    return "System.Double";
                case "float":
                    return "System.Single";
                case "INT8": //These are exposed as short in .net
                case "INT16":
                    return "System.Int16";
                case "INT32":
                    return "System.Int32";
                case "INT64":
                    return "System.Int64";
                case "STRING":
                    return "System.String";
                case "CREFSTRING":
                    return "System.String";
                case "BYTE_ARRAY_IN":
                case "BYTE_ARRAY_OUT":
                    return "System.Byte[]";
                default:
                    return mgType;
            }
        }

        static void CreateDotNetClassElement(XmlDocument doc, Class klass)
        {
            Console.WriteLine("Processing class: " + klass);

            //Class description
            {
                var mbr = doc.CreateElement("member");
                var sum = doc.CreateElement("summary");

                var clsName = doc.CreateAttribute("name");
                clsName.Value = "T:" + DOTNET_NAMESPACE + "." + klass.Name;
                mbr.Attributes.Append(clsName);

                sum.InnerXml = klass.Summary;

                mbr.AppendChild(sum);
                doc.DocumentElement.AppendChild(mbr);
            }

            //Class members
            foreach (var m in klass.Members)
            {
                if (m.AccessModifier != "public")
                    continue;

                var method = m as Method;
                var ctor = m as Constructor;
                var f = m as Field;
                if (method != null)
                {
                    if (method.Name == klass.Name) //Sanity check
                        ProcessDotNetClassConstructor(doc, klass, method);
                    else
                        ProcessDotNetClassMethod(doc, klass, method);
                }
                else if (ctor != null)
                {
                    ProcessDotNetClassConstructor(doc, klass, ctor);
                }
                else if (f != null)
                {
                    ProcessDotNetField(doc, klass, f);
                }
            }
        }

        private static void ProcessDotNetField(XmlDocument doc, Class klass, Field f)
        {
            Console.WriteLine("Processing constant: " + klass.Name + "::" + f.Name);

            var mbr = doc.CreateElement("member");
            var sum = doc.CreateElement("summary");

            var dotNetName = doc.CreateAttribute("name");

            dotNetName.Value = "F:" + DOTNET_NAMESPACE + "." + klass.Name + "." + f.Name;

            mbr.Attributes.Append(dotNetName);
            sum.InnerText = f.Summary;

            mbr.AppendChild(sum);
            doc.DocumentElement.AppendChild(mbr);
        }

        private static void ProcessDotNetClassConstructor(XmlDocument doc, Class klass, Method ctor)
        {
            Console.WriteLine("Processing ctor: " + klass.Name);

            var mbr = doc.CreateElement("member");
            var sum = doc.CreateElement("summary");

            var dotNetName = doc.CreateAttribute("name");

            dotNetName.Value = FormatDotNetCtor(klass, ctor);

            mbr.Attributes.Append(dotNetName);
            sum.InnerText = ctor.Summary;

            if (ctor.Parameters.Count > 0)
            {
                foreach (var p in ctor.Parameters)
                {
                    var pel = doc.CreateElement("param");
                    var pName = doc.CreateAttribute("name");
                    pName.Value = p.Name.Replace("*", ""); //Strip pointer
                    pel.Attributes.Append(pName);
                    mbr.AppendChild(pel);
                }
            }

            mbr.AppendChild(sum);
            doc.DocumentElement.AppendChild(mbr);
        }

        private static void ProcessDotNetClassConstructor(XmlDocument doc, Class klass, Constructor ctor)
        {
            Console.WriteLine("Processing ctor: " + klass.Name);

            var mbr = doc.CreateElement("member");
            var sum = doc.CreateElement("summary");

            var dotNetName = doc.CreateAttribute("name");

            dotNetName.Value = FormatDotNetCtor(klass, ctor);

            mbr.Attributes.Append(dotNetName);
            sum.InnerText = ctor.Summary;

            if (ctor.Parameters.Count > 0)
            {
                foreach (var p in ctor.Parameters)
                {
                    var pel = doc.CreateElement("param");
                    var pName = doc.CreateAttribute("name");
                    pName.Value = p.Name.Replace("*", ""); //Strip pointer
                    pel.Attributes.Append(pName);
                    mbr.AppendChild(pel);
                }
            }

            mbr.AppendChild(sum);
            doc.DocumentElement.AppendChild(mbr);
        }

        private static void ProcessDotNetClassMethod(XmlDocument doc, Class klass, Method method)
        {
            //Destructors should not be exposed, but nevertheless if they are leaking out here
            //ignore them
            if (method.Name.StartsWith("~"))
                return;

            Console.WriteLine("Processing method: " + klass + "::" + method.Name);

            var mbr = doc.CreateElement("member");
            var sum = doc.CreateElement("summary");

            var dotNetName = doc.CreateAttribute("name");

            dotNetName.Value = FormatDotNetMethod(klass, method);

            mbr.Attributes.Append(dotNetName);
            //This may be decorated with markers for IMake.exe, so strip them
            var methodSum = method.Summary.Replace("__inherited", "");
            //But before we strip __set or __get we too are interested as this
            //means a .net property was made for it
            if (method.Summary.Contains("__set") || method.Summary.Contains("__get"))
            {
                ProcessDotNetProperty(doc, klass, GetDotNetPropertyName(method.Name));
            }
            //Now strip them
            methodSum = methodSum.Replace("__set", "").Replace("__get", "");
            //Only set summary if it wasn't just these markers
            if (methodSum.Trim() != ",")
                sum.InnerXml = methodSum;

            if (method.Parameters.Count > 0)
            {
                foreach (var p in method.Parameters)
                {
                    var pel = doc.CreateElement("param");
                    var pName = doc.CreateAttribute("name");
                    pName.Value = p.Name.Replace("*", "")   //Strip pointer
                                        .Replace("=0", ""); //Strip default parameter values
                    pel.Attributes.Append(pName);
                    mbr.AppendChild(pel);
                }
            }

            mbr.AppendChild(sum);
            doc.DocumentElement.AppendChild(mbr);
        }

        private static string GetDotNetPropertyName(string methodName)
        {
            if (methodName.Length > 1)
            {
                //Assuming a camel-cased method name, the property name will be the substring
                //starting from the second capitalized letter of this method name

                for (int i = 1; i < methodName.Length; i++)
                {
                    if (Char.IsUpper(methodName, i))
                        return methodName.Substring(i);
                }
            }
            return methodName;
        }

        private static void ProcessDotNetProperty(XmlDocument doc, Class klass, string propertyName)
        {
            var mbr = doc.CreateElement("member");
            var sum = doc.CreateElement("summary");

            var dotNetName = doc.CreateAttribute("name");

            dotNetName.Value = "P:" + DOTNET_NAMESPACE + "." + klass.Name + "." + propertyName;

            mbr.Attributes.Append(dotNetName);

            mbr.AppendChild(sum);
            doc.DocumentElement.AppendChild(mbr);
        }

        static XmlDocument foundationDoc = null;
        static XmlDocument geometryDoc = null;
        static XmlDocument platformBaseDoc = null;
        static XmlDocument mapguideCommonDoc = null;
        static XmlDocument webDoc = null;

        private static XmlDocument ResolveDocument(string clsHeaderLocation)
        {
            if (clsHeaderLocation == null)
                return null;

            if (clsHeaderLocation.Contains("/Common/Foundation/"))
                return foundationDoc;
            else if (clsHeaderLocation.Contains("/Common/Geometry/"))
                return geometryDoc;
            else if (clsHeaderLocation.Contains("/Common/PlatformBase"))
                return platformBaseDoc;
            else if (clsHeaderLocation.Contains("/Common/MapGuideCommon"))
                return mapguideCommonDoc;
            else if (clsHeaderLocation.Contains("/Web/src/WebApp"))
                return webDoc;

            return null;
        }

        private static void ProcessDotNet(string srcDir, string outDir)
        {
            foundationDoc = CreateDotNetDocument("OSGeo.MapGuide.Foundation");
            geometryDoc = CreateDotNetDocument("OSGeo.MapGuide.Geometry");
            platformBaseDoc = CreateDotNetDocument("OSGeo.MapGuide.PlatformBase");
            mapguideCommonDoc = CreateDotNetDocument("OSGeo.MapGuide.MapGuideCommon");
            webDoc = CreateDotNetDocument("OSGeo.MapGuide.Web");
            
            var docs = new Docs(srcDir);
            docs.LoadGlobalTypes(true);
            var globTypes = docs.GetAllGlobalTypes();
            foreach (var type in globTypes)
            {
                var cls = type as Class;
                var doc = ResolveDocument(type.Location);
                if (cls != null && doc != null)
                    CreateDotNetClassElement(doc, cls);
            }

            if (!string.IsNullOrEmpty(outDir))
            {
                if (!Directory.Exists(outDir))
                    Directory.CreateDirectory(outDir);

                foundationDoc.Save(Path.Combine(outDir, "OSGeo.MapGuide.Foundation.xml"));
                geometryDoc.Save(Path.Combine(outDir, "OSGeo.MapGuide.Geometry.xml"));
                platformBaseDoc.Save(Path.Combine(outDir, "OSGeo.MapGuide.PlatformBase.xml"));
                mapguideCommonDoc.Save(Path.Combine(outDir, "OSGeo.MapGuide.MapGuideCommon.xml"));
                webDoc.Save(Path.Combine(outDir, "OSGeo.MapGuide.Web.xml"));
            }
            else
            {
                foundationDoc.Save("OSGeo.MapGuide.Foundation.xml");
                geometryDoc.Save("OSGeo.MapGuide.Geometry.xml");
                platformBaseDoc.Save("OSGeo.MapGuide.PlatformBase.xml");
                mapguideCommonDoc.Save("OSGeo.MapGuide.MapGuideCommon.xml");
                webDoc.Save("OSGeo.MapGuide.Web.xml");
            }
        }

        static void Usage()
        {
            Console.WriteLine("DoxyTransform.exe <dotnet|java> <doxygen xml path> [output path]");
        }
    }
}
