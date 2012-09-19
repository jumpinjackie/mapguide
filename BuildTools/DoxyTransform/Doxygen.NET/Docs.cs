/*
 * Doxygen.NET - .NET object wrappers for Doxygen
 * Copyright 2009 - Ra-Software AS
 * This code is licensed under the LGPL version 3.
 * 
 * Authors: 
 * Thomas Hansen (thomas@ra-ajax.org)
 * Kariem Ali (kariem@ra-ajax.org)
 * 
 */

using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Xml;

namespace Doxygen.NET
{
    public class Docs
    {
        private XmlDocument _indexXmlDoc;

        public List<FileInfo> XmlFiles { get; set; }
        public DirectoryInfo XmlDirectory { get; protected set; }
        public List<Namespace> Namespaces { get; protected set; }

        public bool EagerParsing { get; set; }
                
        public Docs(string doxygenXmlOuputDirectoryPath)
        {
            if (!Directory.Exists(doxygenXmlOuputDirectoryPath))
                throw new Exception("The specified directory does not exist.");

            XmlDirectory = new DirectoryInfo(doxygenXmlOuputDirectoryPath);

           if (!File.Exists(Path.Combine(XmlDirectory.FullName, "index.xml")))
                throw new Exception("The specified directory does not contain an essential file, \"index.xml\".");

           EagerParsing = true;

            _indexXmlDoc = new XmlDocument();
            _indexXmlDoc.Load(Path.Combine(XmlDirectory.FullName, "index.xml"));

            XmlFiles = new List<FileInfo>(XmlDirectory.GetFiles("*.xml", SearchOption.AllDirectories));

            LoadNamespaces();
        }

        public Namespace GetNamespaceByName(string namespaceName)
        {
            return Namespaces.Find(delegate(Namespace n) { return n.FullName == namespaceName; });
        }

        public Type GetTypeByName(string typeFullName)
        {
            string namespaceName = typeFullName.Remove(typeFullName.LastIndexOf("."));
            Namespace nspace = GetNamespaceByName(namespaceName);

            if (nspace != null)
            {
                Type type = nspace.Types.Find(delegate(Type t) { return t.FullName == typeFullName; });
                return type;
            }

            return null;
        }

        public Type GetTypeByID(string id)
        {
            foreach (Namespace nspace in Namespaces)
            {
                Type type = nspace.Types.Find(delegate(Type t) { return t.ID == id; });
                if (type != null)
                    return type;
            }
            return null;
        }

        public List<Class> GetAllClasses()
        {
            List<Class> classes = new List<Class>();
            foreach (Namespace nspace in Namespaces)
            {
                foreach (Class c in nspace.Classes)
                {
                    classes.Add(c);
                }
            }
            return classes;
        }

        public void LoadNamespaces()
        {
            Namespaces = new List<Namespace>();
            XmlNodeList namespaceXmlNodes = _indexXmlDoc.SelectNodes("/doxygenindex/compound[@kind=\"namespace\"]");

            foreach (XmlNode namespaceXmlNode in namespaceXmlNodes)
            {
                Namespace nspace = new Namespace();
                nspace.ID = namespaceXmlNode.Attributes["refid"].Value;
                nspace.FullName = namespaceXmlNode["name"].InnerText.Replace("::", ".");

                if (EagerParsing)
                {
                    LoadTypes(nspace, false);
                }
                Namespaces.Add(nspace);
            }
        }

        private List<Type> _globalTypes = new List<Type>();

        public List<Type> GetAllGlobalTypes() { return _globalTypes; }

        public void LoadGlobalTypes(bool forceReload)
        {
            if (!forceReload && _globalTypes.Count > 0)
                return;

            _globalTypes = new List<Type>();

            XmlNodeList typesXmlNodes = _indexXmlDoc.SelectNodes(
                "/doxygenindex/compound[@kind=\"class\" or @kind=\"interface\" or @kind=\"enum\" or @kind=\"struct\" or @kind=\"delegate\"]");

            foreach (XmlNode typeXmlNode in typesXmlNodes)
            {
                string typeName = typeXmlNode["name"].InnerText.Replace("::", ".");
                if (!typeName.Contains("."))
                {
                    Type t = CreateNewType(typeXmlNode.Attributes["kind"].Value);

                    t.ID = typeXmlNode.Attributes["refid"].Value;
                    t.Kind = typeXmlNode.Attributes["kind"].Value;
                    t.FullName = typeName;

                    if (EagerParsing)
                    {
                        LoadTypesMembers(t, true);
                    }

                    _globalTypes.Add(t);
                }
            }
        }

        public void LoadTypes(Namespace nspace, bool forceReload)
        {
            if (!forceReload && nspace.Types.Count > 0)
                return;

            nspace.Types = new List<Type>();

            XmlNodeList typesXmlNodes = _indexXmlDoc.SelectNodes(
                "/doxygenindex/compound[@kind=\"class\" or @kind=\"interface\" or @kind=\"enum\" or @kind=\"struct\" or @kind=\"delegate\"]");

            foreach (XmlNode typeXmlNode in typesXmlNodes)
            {
                string typeName = typeXmlNode["name"].InnerText.Replace("::", ".");
                if (typeName.Contains(".") && typeName.Remove(typeName.LastIndexOf(".")) == nspace.FullName)
                {
                    Type t = CreateNewType(typeXmlNode.Attributes["kind"].Value);

                    t.ID = typeXmlNode.Attributes["refid"].Value;
                    t.Kind = typeXmlNode.Attributes["kind"].Value;
                    t.FullName = typeName;
                    t.Namespace = nspace;

                    if (EagerParsing)
                    {
                        LoadTypesMembers(t, false);
                    }
                    nspace.Types.Add(t);
                }
            }
        }
        
        public void LoadTypesMembers(Type t, bool forceReload)
        {
            if (!forceReload && t.Members.Count > 0)
                return;

            FileInfo typeXmlFile = XmlFiles.Find(delegate(FileInfo file) 
                {
                    //Normalize bits
                    string fullPath = file.FullName.Replace('\\', Path.DirectorySeparatorChar).Replace('/', Path.DirectorySeparatorChar);
                    string find = (t.ID + ".xml").Replace('\\', Path.DirectorySeparatorChar).Replace('/', Path.DirectorySeparatorChar);
                    return fullPath.EndsWith(find);
                    //return file.Name.Remove(file.Name.LastIndexOf(file.Extension)) == t.ID; 
                });

            if (typeXmlFile == null || !typeXmlFile.Exists)
                return;

            XmlDocument typeDoc = new XmlDocument();
            using (var sr = new StreamReader(typeXmlFile.FullName, Encoding.UTF8))
            {
                typeDoc.Load(sr);
            }

            t.Summary = StripXml(System.Web.HttpUtility.HtmlDecode(typeDoc.SelectSingleNode("/doxygen/compounddef/briefdescription").InnerXml)); //MapGuide-specific
            t.Description = typeDoc.SelectSingleNode("/doxygen/compounddef/detaileddescription").InnerXml.Replace("preformatted", "pre");

            XmlNodeList baseTypes = typeDoc.SelectNodes("/doxygen/compounddef/basecompoundref");
            
            if (baseTypes != null)
            {
                foreach (XmlNode baseType in baseTypes)
                {
                    t.BaseTypes.Add(baseType.Attributes["refid"].Value);
                }
            }

            XmlNodeList members = typeDoc.SelectNodes("/doxygen/compounddef/sectiondef/memberdef");
            
            foreach (XmlNode member in members)
            {
                string kind = member.Attributes["kind"].Value;
                string name = member["name"].InnerText;
                string args = string.Empty;
                if (member["argsstring"] != null) {
                    //This is to strip any const and pure virtual modifiers
                    string str = member["argsstring"].InnerText;
                    if (str.LastIndexOf("(") >= 0 && str.LastIndexOf(")") >= 0)
                        str = str.Substring(str.LastIndexOf("("), str.LastIndexOf(")") - str.LastIndexOf("("));
                    args = str.Replace("(", "").Replace(")", "").Trim();
                }

                List<Parameter> parameters = new List<Parameter>();
                
                //TODO: parse out the <parameterlist> elements as they contain parameter descriptions and exceptions to
                //watch out for
                if (!string.IsNullOrEmpty(args) && kind == "function")
                {
                    string[] argsSplits = args.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                    foreach (string arg in argsSplits)
                    {
                        string[] argParts = arg.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
                        if (argParts.Length != 2)
                            continue;

                        Parameter p = new Parameter();
                        p.Type = argParts[0].Trim();
                        p.Name = argParts[1].Trim();
                        parameters.Add(p);
                    }
                }
                
                if (kind == "function" && name == t.Name)
                    kind = "ctor";

                Member m = CreateNewMember(kind);

                if (parameters != null && parameters.Count > 0)
                    (m as Method).Parameters = parameters;

                //MapGuide-specific: We want to capture the file path so we know which .net assembly doc to assign
                //to. MapGuide follows a one-class-per-file design for public classes, so sampling the first instance
                //of a <location> node is enough
                if (member["location"] != null && string.IsNullOrEmpty(t.Location))
                {
                    t.Location = member["location"].Attributes["file"].Value;
                }

                m.ID = member.Attributes["id"].Value;
                m.FullName = string.Format("{0}.{1}", t.FullName, name);
                m.Name = name;
                m.Kind = kind;
                m.Summary = StripXml(System.Web.HttpUtility.HtmlDecode(member["briefdescription"].InnerXml)); //MapGuide-specifc
                m.Description = member["detaileddescription"].InnerXml.Replace("preformatted", "pre");
                m.AccessModifier = member.Attributes["prot"].Value;
                m.Parent = t;
                m.ReturnType = member["type"] != null ?
                    member["type"].InnerText : string.Empty;
                t.Members.Add(m);
            }
        }

        private static string StripXml(string source)
        {
            char[] buffer = new char[source.Length];
            int bufferIndex = 0;
            bool inside = false;

            for (int i = 0; i < source.Length; i++)
            {
                char let = source[i];
                if (let == '<')
                {
                    inside = true;
                    continue;
                }
                if (let == '>')
                {
                    inside = false;
                    continue;
                }
                if (!inside)
                {
                    buffer[bufferIndex] = let;
                    bufferIndex++;
                }
            }
            return new string(buffer, 0, bufferIndex);
        }

        private Type CreateNewType(string kind)
        {
            switch (kind)
            {
                case "class":
                    return new Class();
                case "interface":
                    return new Interface();
                case "delegate":
                    return new Delegate();
                case "enum":
                    return new Enum();
                case "struct":
                    return new Struct();
            }
            return new Type();
        }

        private Member CreateNewMember(string kind)
        {
            switch (kind)
            {
                case "property":
                    return new Property();
                case "event":
                    return new Event();
                case "function":
                    return new Method();
                case "variable":
                    return new Field();
                case "ctor":
                    return new Constructor();
                case "memberdelegates":
                    return new MemberDelegate();
            }
            return new Member();
        }
    }
}
