//------------------------------------------------------------------------------
// <copyright company="Wintellect">
//    Copyright (c) John Robbins/Wintellect -- All rights reserved.
// </copyright>
// <Project>
//    Wintellect Debugging .NET Code
// </Project>
//------------------------------------------------------------------------------
/*------------------------------------------------------------------------------
 * See the following blog entries for more information about PARAFFIN:
 * 
 * http://www.wintellect.com/cs/blogs/jrobbins/archive/2007/10/18/wix-hints-for-new-users-part-1-of-3.aspx
 * http://www.wintellect.com/cs/blogs/jrobbins/archive/2007/10/19/wix-the-pain-of-wix-part-2-of-3.aspx
 * http://www.wintellect.com/cs/blogs/jrobbins/archive/2007/10/21/wix-a-better-tallow-paraffin.aspx
 * 
 * 1.00 - Initial release
 * 1.01 - Fixed a bug where directory and component names could have a dash in 
 *        them, which is not supported by WiX.
 * 1.02 - Special thanks to Darren Stone for all his input about PARAFFIN.
 *      - Added -Win64 switch, which adds Win64="yes" to all components.
 *      - Updated the Id naming to keep all values in the range [0-9a-zA-Z_] to
 *        avoid any naming problems. WiX is not consistent on exactly what can
 *        characters can be in the Id attribute.
 *      - When updating, I was previously only relying on the Directory and 
 *        File elements Name attribute to find those elements. I mistakenly 
 *        thought the short file/directory name was guaranteed to be unique.
 *        I fixed this bug by updating the Directory element searching to look 
 *        for either the matching Name attribute or LongName attribute depending
 *        if the long name is different than the short name. For File elements, 
 *        I look at both the Name and the Source attributes for the exact match.
 *      - Fixed a bug where I wasn't properly matching directory names when
 *        generating the Id attribute.
 *      - Fixed the innocuous bug where I was appending a double slash on an
 *        alias if the input directory did not end in a trailing slash.
 * 1.03 - Fixed a bug where I was assuming that the short name for a file was
 *        constant. It's really a random value. Now I only look at the Source
 *        attribute when updating a File node as there's no other way to ensure
 *        that a file is the same. This means I might have a rare conflict with 
 *        the short name for a file. The big reason for upgrading to WiX 3.0 is
 *        that you no longer need to mess with these darn short names!
 * 1.04 - Thanks to Matthew Goos, added the -dirref option to allow a custom 
 *        name for the DirectoryRef node when creating a file.
 *      - Now the -ext and direXclude command line options can also be specified 
 *        for updates in order to add additional extensions or directories to 
 *        ignore when updating a file.
 * 3.00 - Sorry for the big version jump but Paraffin now targets WiX 3.0 so I
 *        thought I'd make them the same. Now that WiX 3.0 has hit beta, it's 
 *        time to support it. Note that this version no longer will create files
 *        for use with WiX 2.0. However, it will import and convert previously
 *        created Paraffin files to WiX 3.0.
 *        Yay! No more short filenames!
 *      - When adding files, I now check if they are .DLL, .EXE, or .OCX and if
 *        so, add CheckSum='yes' attribute to the File element.
 *      - All command line switches are now case insensitive. I'd forgotten to
 *        set that in a prior version.
 -----------------------------------------------------------------------------*/
namespace Wintellect.Paraffin
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.IO;
    using System.Xml.Linq;
    using System.Runtime.InteropServices;
    using System.Globalization;
    using System.Diagnostics;
    using System.Collections;
    using System.Text.RegularExpressions;

    /// <summary>
    /// The main program.
    /// </summary>
    internal class Program
    {
        #region Comment Options Elements
        // All the elements for the data stored in the comment.
        private const String CMDLINEOPTIONSELEM = "CommandLineOptions";
        private const String PRODUCEDBYELEM = "Producer";
        private const String WARNINGELEM = "WARNING";
        private const String DATECREATE = "CreatedOn";
        private const String DIRECTORYELEM = "Directory";
        private const String CUSTOMELEM = "Custom";
        private const String ALIASELEM = "DirAlias";
        private const String INCREMENTELEM = "Increment";
        private const String GUIDSELEM = "Guids";
        private const String MULTIPLEELEM = "Multiple";
        private const String NORECURSELEM = "Norecurse";
        private const String WIN64ELEM = "Win64";
        private const String EXTEXCLUDEELEM = "ExtensionExcludes";
        private const String EXTELEM = "Ext";
        private const String DIREEXCLUDEELEM = "DirExcludes";
        private const String DIREXT = "Dir";
        private const String NEXTDIRECTORYNUMELEM = "NextDirectoryNumber";
        private const String NEXTCOMPONENTNUMBER = "NextComponentNumber";
        #endregion

        // The WiX 3.0 namespace.
        private static XNamespace nsWiX3 =
                                     "http://schemas.microsoft.com/wix/2006/wi";
        private static XNamespace nsWiX2 =
                                  "http://schemas.microsoft.com/wix/2003/01/wi";

        // The argument values used across all the methods.
        private static ParaffinArgParser argValues;

        // The current directory number.
        private static Int32 directoryNumber;

        // The starting directory name. I use this to build up unique 
        // Directory Id values.
        private static String baseDirectoryName;

        // The full starting directory. If the user wants aliases, I'll replace
        // this with the alias.
        private static String fullStartDirectory;

        // The current component number
        private static Int32 componentNumber;

        // The error message.
        private static String errorMessage;

        // The input file namespace.
        private static XNamespace inputNameSpace = nsWiX3;

        // The PE file extensions.
        private static String [] peFileExtension = { ".DLL" , ".EXE" , ".OCX" };

        internal static Int32 Main ( string [] args )
        {
            LoadProcessFeatureMap();

            directoryNumber = 0;
            componentNumber = 0;
            errorMessage = String.Empty;

            // Think positive that everything will run completely.
            Int32 returnValue = 0;
            argValues = new ParaffinArgParser ( );
            if ( args.Length > 0 )
            {
                Boolean parsed = argValues.Parse ( args );
                if ( true == parsed )
                {
                    if ( true == argValues.Update )
                    {
                        returnValue = UpdateExistingFile ( );
                    }
                    else
                    {
                        returnValue = CreateNewFile ( );
                    }
                }
            }
            else
            {
                argValues.OnUsage ( String.Empty );
                returnValue = 1;
            }

            if ( false == String.IsNullOrEmpty ( errorMessage ) )
            {
                Console.WriteLine ( errorMessage );
            }

            return ( returnValue );
        }

        /// <summary>
        /// Creates a brand new .WXS file for the specified directory and 
        /// options. Any previous file of this name is overwritten.
        /// </summary>
        /// <returns>
        /// Zero if the file was all properly written.
        /// </returns>
        private static int CreateNewFile ( )
        {
            // Create the XML document.
            XDocument doc = new XDocument ( );

            // Add the WiX and Fragment nodes.
            XElement root = new XElement ( nsWiX3 + "Wix" );
            doc.Add ( root );
            XElement fragment = new XElement ( nsWiX3 + "Fragment" );
            root.Add ( fragment );

            // Add the DirectoryRef node.
            XElement directoryRef;
            if ( true == String.IsNullOrEmpty ( argValues.DirectoryRef ) )
            {
                directoryRef = new XElement ( nsWiX3 + "DirectoryRef" ,
                                          new XAttribute ( "Id" ,
                                                           "INSTALLDIR" ) );
            }
            else
            {
                directoryRef = new XElement ( nsWiX3 + "DirectoryRef" ,
                                           new XAttribute ( "Id" ,
                                                     argValues.DirectoryRef ) );
            }

            fragment.Add ( directoryRef );

            // Get the starting directories initialized.
            InitializeDirectoryValues ( );

            // Now start the grind.
            RecurseDirectoriesForNewFile ( directoryRef , fullStartDirectory );

            // Add the Component group node.
            AddComponentGroup ( fragment );

            // Add the comment with all the command line options.
            AddCommandLineOptionsComment ( root );

            ProcessFeatureMap(doc, Path.GetFileNameWithoutExtension(argValues.FileName) );

            // We're done, save it!
            doc.Save ( argValues.FileName );
            return ( 0 );
        }

        private static Dictionary<string, string> _featureMap = new Dictionary<string, string>();

        private static void LoadProcessFeatureMap()
        {
            if (File.Exists("Features.xml"))
            {
                var doc = XDocument.Load("Features.xml");
                var mappings = from item in doc.Descendants("DirectorySearch")
                               select new
                               {
                                   Name = item.Attribute("Name").Value,
                                   Suffix = item.Attribute("Suffix").Value
                               };
                foreach (var m in mappings)
                    _featureMap[m.Name] = m.Suffix;
            }
        }

        /// <summary>
        /// Performs a search through the given XDocument for Directory fragments
        /// whose name matches the specified items in the FeatureMap, for each matching
        /// directory, all of its child elements will be assigned to the specified feature
        /// </summary>
        /// <param name="doc"></param>
        private static void ProcessFeatureMap(XDocument doc, string name)
        {
            Dictionary<string, HashSet<string>> componentMaps = new Dictionary<string, HashSet<string>>();

            foreach (var dirName in _featureMap.Keys)
            {
                var suffix = _featureMap[dirName];
                if (!componentMaps.ContainsKey(suffix))
                    componentMaps[suffix] = new HashSet<string>();
                Console.WriteLine("Checking if any Directory elements match: " + dirName);
                var matches = doc.Descendants("{" + nsWiX3 + "}Directory").Where(x => x.Attribute("Name").Value == dirName);
                foreach (var dir in matches)
                {
                    Console.WriteLine("\tProcessing Directory (Id=" + dir.Attribute("Id") + ")");
                    foreach (var comp in dir.Descendants("{" + nsWiX3 + "}Component"))
                    {
                        //Console.WriteLine("\t\tSet Component Feature (Id=" + comp.Attribute("Id") + ") to use feature (" + suffix + ")");
                        componentMaps[suffix].Add(comp.Attribute("Id").Value);
                    }
                }
            }

            //Remove these collected ids from the existing component group
            var currentGroup = doc.Descendants("{" + nsWiX3 + "}ComponentGroup").First();
            var remove = new List<XElement>();

            int moved = 0;
            //Now write out the component group
            foreach(var suffix in _featureMap.Values)
            {
                if (componentMaps[suffix].Count > 0)
                {
                    var groupId = name + suffix;

                    var grp = new XElement("{" + nsWiX3 + "}ComponentGroup");
                    grp.SetAttributeValue("Id", groupId);

                    foreach (var cid in componentMaps[suffix])
                    {
                        var matches = currentGroup.Descendants("{" + nsWiX3 + "}ComponentRef").Where(x => x.Attribute("Id").Value == cid);
                        remove.AddRange(matches);

                        var el = new XElement("{" + nsWiX3 + "}ComponentRef");
                        el.SetAttributeValue("Id", cid);

                        grp.Add(el);
                        moved++;
                    }

                    //Prepend component group to the fragment
                    doc.Descendants("{" + nsWiX3 + "}Fragment").First().AddFirst(grp);
                }
            }

            //Remove moved componentrefs from original group
            foreach (var el in remove)
            {
                el.Remove();
            }
            Console.WriteLine("Moved {0} elements to new ComponentGroup. Removed {1} elements from original ComponentGroup", moved, remove.Count);
        }

        /// <summary>
        /// Takes an existing .WXS file and generates an updated version, which
        /// is saved to a .PARAFFIN extension.
        /// </summary>
        /// <returns>
        /// 0 - The .PARAFFIN file was created.
        /// 2 - The input file does not have the special comment in the 
        /// appropriate location.
        /// </returns>
        private static int UpdateExistingFile ( )
        {
            int returnValue = 0;

            // Load the XML document. Any loading problems go right
            // to an exception for the user.
            XDocument inputDoc = XDocument.Load ( argValues.FileName );

            XAttribute ns = inputDoc.Root.Attribute ( "xmlns" );
            if ( 0 == String.Compare ( nsWiX2.ToString ( ) ,
                                       ns.Value ,
                                       StringComparison.OrdinalIgnoreCase ) )
            {
                inputNameSpace = nsWiX2;
            }

            // The output filename.
            String outputFile = Path.ChangeExtension ( argValues.FileName ,
                                                       ".PARAFFIN" );

            // The first node has to be comment I put there when 
            // the file was created.
            XComment options = inputDoc.Root.FirstNode as XComment;
            if ( null != options )
            {
                // It's a comment node, so set all the arguments from that 
                // section.
                InitializeArgumentsFromFile ( options.Value );

                // Create the new output file.
                XDocument outputDoc = new XDocument ( );

                // Add the WiX and Fragment nodes.
                XElement outputRoot = new XElement ( nsWiX3 + "Wix" );
                outputDoc.Add ( outputRoot );
                XElement outputFragment = new XElement ( nsWiX3 + "Fragment" );
                outputRoot.Add ( outputFragment );

                // Find the directory ref of the input file.
                XElement inputDirRef = inputDoc.Descendants (
                                            inputNameSpace +
                                                     "DirectoryRef" ).First ( );
                String idValue =
                                inputDirRef.Attributes ( "Id" ).First ( ).Value;

                // Build a DirectoryRef for the output file.
                XElement outputDirRef = new XElement ( nsWiX3 + "DirectoryRef" ,
                                            new XAttribute ( "Id" , idValue ) );

                // Add the directory ref to the output file.
                outputFragment.Add ( outputDirRef );

                // Get the starting directory values ready to go.
                InitializeDirectoryValues ( );

                // Recurse through the input file and the directories 
                // themselves.
                RecurseDirectoriesForExistingFile ( inputDirRef ,
                                                    outputDirRef ,
                                                    fullStartDirectory );

                // Add the Component group node.
                AddComponentGroup ( outputFragment );

                // Add the comment with all the command line options.
                AddCommandLineOptionsComment ( outputRoot );

                ProcessFeatureMap(outputDoc, Path.GetFileNameWithoutExtension(outputFile));

                // All OK, Jumpmaster!
                outputDoc.Save ( outputFile );
            }
            else
            {
                // This does not look like a file this tool previously 
                // generated.
                errorMessage = Constants.UnknownFileType;
                returnValue = 2;
            }

            return ( returnValue );
        }

        /// <summary>
        /// Does the work of recursing both the file system directories and the 
        /// original .WXS file to produce an updated XML document.
        /// </summary>
        /// <param name="currInputElement">
        /// The current element in the input .WXS file.
        /// </param>
        /// <param name="currOutputElement">
        /// The current element in the output .PARAFFIN file.
        /// </param>
        /// <param name="directory">
        /// The directory to process. This has to be the full directory value.
        /// </param>
        /// <remarks>
        /// As you can guess, this is called recursively.
        /// </remarks>
        private static void
                RecurseDirectoriesForExistingFile ( XElement currInputElement ,
                                                    XElement currOutputElement ,
                                                    String directory )
        {
            // If the currInputElement is null, I'm processing a brand new 
            // directory that isn't in the original file. Thus, I can treat 
            // adding this directory just like it's a new file and add this 
            // directory, plus all under it.
            if ( null == currInputElement )
            {
                RecurseDirectoriesForNewFile ( currOutputElement , directory );
            }
            else
            {
                // The directory element I'm going to be building up.
                XElement outputDirElement;

                // Get the directory info in order to get just the name.
                DirectoryInfo info = new DirectoryInfo ( directory );
                String name = info.Name;

                String matchAttrib = "Name";
                // If this is a WiX 2.0 input file, I need to match on LongName
                // instead if the name is more than 8 characters.
                if ( ( inputNameSpace == nsWiX2 ) && ( name.Length > 8 ) ) 
                {
                    matchAttrib = "LongName";
                }

                // Does this directory already exist in the input file? 
                var qFindDirectory = from elem in currInputElement.Elements ( )
                                     where
                           ( (string)elem.Attribute ( matchAttrib ) == name )
                                     select elem;

                XElement inputDirElement = null;
                int fileCount = qFindDirectory.Count ( );
                Debug.Assert ( fileCount <= 1 , "fileCount <= 1" );
                if ( fileCount > 1 )
                {
                    // We've got a serious problem. :( You can't have multiple
                    // directories with the same name.
                    String err = String.Format ( CultureInfo.CurrentCulture ,
                                        Constants.InvalidFileNameCountFmt ,
                                                 name );
                    throw new InvalidOperationException ( err );
                }
                else if ( 0 == fileCount )
                {
                    // This is a new directory.
                    outputDirElement = CreateDirectoryElement ( directory );
                }
                else
                {
                    // We've got one element so grab it.
                    inputDirElement = qFindDirectory.First ( );

                    if ( inputNameSpace == nsWiX2 )
                    {
                        // In case the input file was created with the 
                        // version of Paraffin that supported WiX 2.0, I 
                        // want to strip off the LongName attribute 
                        // as it is no longer needed.
                        inputDirElement.SetAttributeValue ( "LongName" , null );
                        // Since WiX3 uses the Name attribute, I willl change
                        // the existing Name attribute to the long name so
                        // it gets copied below. This is the long name now.
                        inputDirElement.SetAttributeValue ( "Name" , name );
                    }

                    // This directory was in the previous file so copy it's 
                    // attributes over to the new file.
                    outputDirElement = new XElement ( nsWiX3 + "Directory" );
                    foreach ( var attrib in inputDirElement.Attributes ( ) )
                    {
                        outputDirElement.SetAttributeValue ( attrib.Name ,
                                                             attrib.Value );
                    }

                }

                // Add this element to the output element.
                currOutputElement.Add ( outputDirElement );

                // Process all the files in this directory as compared to the 
                // input file.
                UpdateFilesInDirectoryNode ( directory ,
                                             inputDirElement ,
                                             outputDirElement );

                // Recurse directories if the original file had that set.
                if ( false == argValues.NoDirectoryRecursion )
                {
                    String [] dirs = Directory.GetDirectories ( directory );
                    foreach ( var item in dirs )
                    {
                        // Is this a directory the user wanted to skip?
                        Boolean skipDirectory = IsDirectoryExcluded ( item );
                        if ( false == skipDirectory )
                        {
                            RecurseDirectoriesForExistingFile ( 
                                                            inputDirElement ,
                                                            outputDirElement ,
                                                            item );
                        }
                    }
                }
            }
        }

        /// <summary>
        /// Looks at the input .WXS for the files in this directory and compare
        /// it to the files on disk. If the file is the same or is a new file,
        /// add the files to the output. If it's no longer present on disk, but
        /// is in the input .WXS, skip adding the file to the output.
        /// </summary>
        /// <param name="directory">The disk directory to scan.</param>
        /// <param name="inputDir">The Directory element from the .WXS file that
        /// maps to <see cref="directory"/>.</param>
        /// <param name="outputDir">The Directory element for the output 
        /// .PARAFFIN file.</param>
        /// <exception cref="InvalidOperationException">
        /// Thrown if there's multiple files with the same name in the <paramref
        /// name="outputDir"/> child elements.
        /// </exception>
        private static void UpdateFilesInDirectoryNode ( String directory ,
                                                         XElement inputDir ,
                                                         XElement outputDir )
        {
            // If the inputDir element is null, just treat this as if I was 
            // creating a new file.
            if ( null == inputDir )
            {
                AddNewFilesToDirectoryNode ( directory , outputDir );
            }
            else
            {
                // The .WXS file had files for this directory so I need to 
                // match them all up with the existing files.

                // Start by getting the files in this directory.
                String [] files = Directory.GetFiles ( directory );

                // Skip all those that have extension the user does not want.
                var filesQuery = from file in files
                                 where false == argValues.ExtensionList.
                                        ContainsKey ( Path.GetExtension ( file )
                                                         .ToUpperInvariant ( ) )
                                 select file;

                // If there's no files in this directory, there's nothing else 
                // to do.
                if ( 0 != filesQuery.Count ( ) )
                {
                    // I'll add to the output .WXS file current Directory node 
                    // by default.
                    XElement addToElement = outputDir;

                    // Are we doing multiple files per component?
                    if ( true == argValues.MultipleFilesPerComponent )
                    {
                        // Copy over the one component node. If you just add 
                        // the node directly, that copies over all the child 
                        // nodes as well so I need duplicate just the component
                        // node itself.
                        XElement intputCompElem =
                                      inputDir.Element (
                                                 inputNameSpace + "Component" );
                        XElement outputCompElem =
                                          new XElement ( nsWiX3 + "Component" );
                        foreach ( var attrib in intputCompElem.Attributes ( ) )
                        {
                            outputCompElem.SetAttributeValue ( attrib.Name ,
                                                               attrib.Value );
                        }

                        // Add the Component node on. 
                        outputDir.Add ( outputCompElem );

                        // Point to the component where I'll be adding all the
                        // file nodes.
                        addToElement = outputCompElem;
                    }

                    // First get the child component(s) from this Directory.
                    var comps = inputDir.Elements (
                                                 inputNameSpace + "Component" );

                    // Now get all the files from just these Component 
                    // elements.
                    var inputFiles = comps.Descendants (
                                                      inputNameSpace + "File" );

                    // Loop through all the files on disk.
                    foreach ( var file in filesQuery )
                    {
                        // Holds the element I'm going to add to the current
                        // output XML document.
                        XElement newOutputElem;

                        // Get the aliased value for this file.
                        String aliasedName = BuildAliasedFilename ( file );

                        // See if I can find that file in the input .WXS file 
                        // by checking the aliased Source names.
                        var inputFileQuery = from fileNode in inputFiles
                                             where
                          ( ( (string)fileNode.Attribute ( "Source" ) )
                                                               == aliasedName )
                                             select fileNode;
                        int fileCount = inputFileQuery.Count ( );
                        Debug.Assert ( fileCount <= 1 , "fileCount <= 1" );
                        if ( 0 == fileCount )
                        {
                            XElement comp = null;
                            if ( false == argValues.MultipleFilesPerComponent )
                            {
                                // Put this file element in it's own Component.
                                // The component always has to be created first.
                                // so that the component and file are using the
                                // same unique number.
                                comp = CreateComponentElement ( );
                            }

                            // This is a new file that wasn't in the input .WXS
                            // file so just add it. First create a new File 
                            // element.
                            newOutputElem = CreateFileElement ( file );

                            // Did I create a component for this file?
                            if ( null != comp )
                            {
                                // Add the file to this component.
                                comp.Add ( newOutputElem );

                                // Point at the component to add.
                                newOutputElem = comp;
                            }
                        }
                        else if ( 1 == fileCount )
                        {
                            newOutputElem = inputFileQuery.First ( );

                            // In case the input file was created with the 
                            // version of Paraffin that supported WiX 2.0, I 
                            // want to strip off the Name and LongName 
                            // attributes as they are no longer needed.
                            newOutputElem.SetAttributeValue ( "Name" , null );
                            newOutputElem.SetAttributeValue ( "LongName" ,
                                                              null );


                            // Is it one file per component? 
                            if ( false == argValues.MultipleFilesPerComponent )
                            {
                                // If this is a WiX 3.0 .WXS file I can simply
                                // cheat and make the newOutputElem actually
                                // point  to the Component element. That way I 
                                // just add the Component and the child File
                                // element in one swoop.
                                if ( inputNameSpace == nsWiX3 )
                                {
                                    newOutputElem = newOutputElem.Parent;
                                }
                                else
                                {
                                    // This is a fragment from WiX 2.0 being 
                                    // updated to 3.0. Because of the namespace
                                    // being different, I need to copy the 
                                    // attributes manually.
                                    XElement comp = new XElement (
                                                         nsWiX3 + "Component" );
                                    foreach ( var attrib in
                                           newOutputElem.Parent.Attributes ( ) )
                                    {
                                        comp.SetAttributeValue ( attrib.Name ,
                                                                 attrib.Value );
                                    }

                                    // Create the WiX 3 version of this WiX 2 
                                    // file node.
                                    XElement newFile = 
                                        CreateWiX3FileFromWix2File ( 
                                                                newOutputElem );
                                    // Add the file to the component.
                                    comp.Add ( newFile );
                                    // Set the output element to the component.
                                    newOutputElem = comp;
                                }
                            }
                            else if ( inputNameSpace == nsWiX2 )
                            {
                                // It's multiple files per component and this is
                                // a WiX 2.0 input file. Copy the attributes
                                // over manually to keep the namespaces
                                // straight. Now newOutput points to the WiX 3.0
                                // element.
                                newOutputElem = CreateWiX3FileFromWix2File ( 
                                                                newOutputElem );
                            }
                        }
                        else
                        {
                            // There's multiple files with the same name in this
                            // particular node. That's bad. :(
                            String err = String.Format (
                                        CultureInfo.CurrentCulture ,
                                        Constants.InvalidFileNameCountFmt ,
                                        file );
                            throw new InvalidOperationException ( err );
                        }

                        // Add the file element to the parent node.
                        addToElement.Add ( newOutputElem );
                    }
                }
            }
        }

        /// <summary>
        /// Called when processing a new file or a directory that's wasn't seen
        /// in the existing .WXS when updating.
        /// </summary>
        /// <param name="currElement">
        /// The current element in the output XML document.
        /// </param>
        /// <param name="directory">
        /// The disk directory to recurse.
        /// </param>
        private static void RecurseDirectoriesForNewFile ( XElement currElement ,
                                                          String directory )
        {
            // It's new so create a Directory element.
            XElement directoryNode = CreateDirectoryElement ( directory );

            // Add the current directory to the passed in element
            currElement.Add ( directoryNode );

            // Add the files to this directory node.
            AddNewFilesToDirectoryNode ( directory , directoryNode );

            // Recurse the directories if I'm supposed to do so.
            if ( false == argValues.NoDirectoryRecursion )
            {
                String [] dirs = Directory.GetDirectories ( directory );
                foreach ( var item in dirs )
                {
                    Boolean skipDirectory = IsDirectoryExcluded ( item );
                    if ( false == skipDirectory )
                    {
                        RecurseDirectoriesForNewFile ( directoryNode , item );
                    }
                }
            }
        }

        /// <summary>
        /// For new directories when creating new files or when adding new
        /// directories when processing an existing .WXS, adds the files
        /// to the <see cref="directoryElem"/> element.
        /// </summary>
        /// <param name="directory">
        /// The directory to get the files from.
        /// </param>
        /// <param name="directoryElem">
        /// The Director element to add the new Component/File elements to.
        /// </param>
        private static void AddNewFilesToDirectoryNode ( String directory ,
                                                        XElement directoryElem )
        {
            // Get the files in this directory.
            String [] files = Directory.GetFiles ( directory );

            // Only do the work if there are some files in the directory.
            if ( files.Length > 0 )
            {
                // Skip all those that have extensions the user does not want.
                var filesQuery = from file in files
                                 where false == argValues.ExtensionList.
                                        ContainsKey ( Path.GetExtension ( file )
                                                         .ToUpperInvariant ( ) )
                                 select file;

                // Create the first Component element. Only add this node to the
                // directory node if the user wants multiple files per component 
                // node. 
                XElement currentComponent = CreateComponentElement ( );
                if ( true == argValues.MultipleFilesPerComponent )
                {
                    directoryElem.Add ( currentComponent );
                }

                // For each file on disk.
                foreach ( var file in filesQuery )
                {
                    // Create the File element and add it to the current
                    // Component element.
                    XElement fileElement = CreateFileElement ( file );
                    currentComponent.Add ( fileElement );
                    if ( false == argValues.MultipleFilesPerComponent )
                    {
                        directoryElem.Add ( currentComponent );
                        currentComponent = CreateComponentElement ( );
                    }
                }

                // I'm done with this directory so bump up the component and 
                // directory count if the user asked for that to happen.
                componentNumber += ( argValues.IncrementValue - 2 );
            }
        }

        /// <summary>
        /// Initializes the <see cref="ParaffinArgParser"/> with all the
        /// settings from the first comment block. Used when reading in a .WXS
        /// to compare to the files on the disk.
        /// </summary>
        /// <param name="inputXml">
        /// The XML string to process.
        /// </param>
        private static void InitializeArgumentsFromFile ( string inputXml )
        {
            XElement options = XElement.Parse ( inputXml );

            // Save off the settings from the command line.
            ParaffinArgParser originalArgs = argValues;

            // Start the arguments from the comment section.
            argValues = new ParaffinArgParser ( );

            // Get all the easy values out.
            argValues.CustomValue =
                           options.Descendants ( CUSTOMELEM ).First ( ).Value;

            argValues.Alias =
                            options.Descendants ( ALIASELEM ).First ( ).Value;

            argValues.StartDirectory =
                        options.Descendants ( DIRECTORYELEM ).First ( ).Value;

            argValues.IncrementValue = Convert.ToInt32 (
                      options.Descendants ( INCREMENTELEM ).First ( ).Value ,
                                            CultureInfo.InvariantCulture );

            argValues.GenerateGuids = Convert.ToBoolean (
                          options.Descendants ( GUIDSELEM ).First ( ).Value ,
                                                 CultureInfo.InvariantCulture );

            argValues.MultipleFilesPerComponent = Convert.ToBoolean (
                       options.Descendants ( MULTIPLEELEM ).First ( ).Value ,
                                             CultureInfo.InvariantCulture );

            argValues.NoDirectoryRecursion = Convert.ToBoolean (
                       options.Descendants ( NORECURSELEM ).First ( ).Value ,
                                            CultureInfo.InvariantCulture );

            directoryNumber = Convert.ToInt32 (
               options.Descendants ( NEXTDIRECTORYNUMELEM ).First ( ).Value ,
                                     CultureInfo.InvariantCulture );

            componentNumber = Convert.ToInt32 (
                options.Descendants ( NEXTCOMPONENTNUMBER ).First ( ).Value ,
                                      CultureInfo.InvariantCulture );

            var extNode = options.Descendants ( EXTEXCLUDEELEM );
            foreach ( var item in extNode.Descendants ( ) )
            {
                argValues.ExtensionList.Add ( item.Value , true );
            }

            var dirEx = options.Descendants ( DIREEXCLUDEELEM );
            foreach ( var item in dirEx.Descendants ( ) )
            {
                argValues.DirectoryExcludeList.Add ( item.Value );
            }

            // After releasing 1.0, I've added a few command line options. 
            // Since I don't want to break existing PARAFFIN generated files,
            // I'll not require the following options to be in existing files.
            // If they are cool, but no sense crashing out if they aren't.
            var win64Elems = options.Descendants ( WIN64ELEM );
            if ( win64Elems.Count ( ) == 1 )
            {
                // Grab the value.
                argValues.Win64 = Convert.ToBoolean (
                                                 win64Elems.First ( ).Value ,
                                                 CultureInfo.InvariantCulture );
            }
            else
            {
                // Assume false.
                argValues.Win64 = false;
            }

            // Now that everything is read out of the original options block,
            // add in any additional -ext and -dirExclude options specified on 
            // the command line.
            foreach ( var cmdLineExt in originalArgs.ExtensionList.Keys )
            {
                if ( false ==
                            argValues.ExtensionList.ContainsKey ( cmdLineExt ) )
                {
                    argValues.ExtensionList.Add ( cmdLineExt , true );
                }
            }
            foreach ( var dirExclude in originalArgs.DirectoryExcludeList )
            {
                if ( false ==
                        argValues.DirectoryExcludeList.Contains ( dirExclude ) )
                {
                    argValues.DirectoryExcludeList.Add ( dirExclude );
                }
            }
        }

        /// <summary>
        /// Adds the command line options as the first comment element under
        /// the WiX element.
        /// </summary>
        /// <param name="wixElement">
        /// The WiX element to add to.
        /// </param>
        private static void AddCommandLineOptionsComment ( XElement wixElement )
        {
            // Create the XML data for the easy stuff.
            XElement initOptions =
                new XElement ( CMDLINEOPTIONSELEM ,
                    new XElement ( PRODUCEDBYELEM ,
                                   Constants.CommentProducer ) ,
                    new XElement ( WARNINGELEM , Constants.CommentWarning ) ,
                    new XElement ( DATECREATE ,
                                   DateTime.Now.ToString ( "g" ,
                                                CultureInfo.CurrentCulture ) ) ,
                    new XElement ( DIRECTORYELEM ,
                                   argValues.StartDirectory ) ,
                    new XElement ( CUSTOMELEM , argValues.CustomValue ) ,
                    new XElement ( ALIASELEM , argValues.Alias ) ,
                    new XElement ( INCREMENTELEM ,
                                   argValues.IncrementValue ) ,
                    new XElement ( GUIDSELEM , argValues.GenerateGuids ) ,
                    new XElement ( WIN64ELEM , argValues.Win64 ) ,
                    new XElement ( MULTIPLEELEM ,
                                   argValues.MultipleFilesPerComponent ) ,
                    new XElement ( NORECURSELEM ,
                                   argValues.NoDirectoryRecursion ) );

            // Add the file extension exclusions.
            XElement extList = new XElement ( EXTEXCLUDEELEM );
            foreach ( var item in argValues.ExtensionList )
            {
                extList.Add ( new XElement ( EXTELEM , item.Key ) );
            }

            initOptions.Add ( extList );

            // Add the directory exclusions.
            XElement dirExList = new XElement ( DIREEXCLUDEELEM );
            foreach ( var item in argValues.DirectoryExcludeList )
            {
                dirExList.Add ( new XElement ( DIREXT , item.ToString ( ) ) );
            }

            initOptions.Add ( dirExList );

            // Add the next directory number and component number so we don't
            // overwrite an existing value.
            XElement dirNum = new XElement ( NEXTDIRECTORYNUMELEM ,
                                             directoryNumber );
            initOptions.Add ( dirNum );
            XElement compNum = new XElement ( NEXTCOMPONENTNUMBER ,
                                              componentNumber );
            initOptions.Add ( compNum );

            // Add the XML comment.
            XComment comment = new XComment ( initOptions.ToString ( ) );
            wixElement.AddFirst ( comment );
        }

        /// <summary>
        /// Adds the ComponentGroup as the first child to the Fragment element.
        /// </summary>
        /// <param name="fragment">
        /// The Fragment element.
        /// </param>
        private static void AddComponentGroup ( XElement fragment )
        {
            // Grab all the Component elements and sort them by the ID 
            // attribute.
            var compNodes = from node in fragment.
                                            Descendants ( nsWiX3 + "Component" )
                            select node;

            // Sort them in logical order because I'm a little bit obsessive.
            compNodes = compNodes.OrderBy (
                                       n => (string)n.Attribute ( "Id" ).Value ,
                                       new LogicalStringComparer ( ) );

            StringBuilder sb = new StringBuilder ( 70 );
            sb.AppendFormat ( "group_{0}" , argValues.CustomValue );

            // Ensure that all invalid characters are stripped from the ID.
            String id = RemoveInvalidIdCharacters ( sb.ToString ( ) );

            XElement groupNode = new XElement ( nsWiX3 + "ComponentGroup" ,
                                                new XAttribute ( "Id" , id ) );
            foreach ( var component in compNodes )
            {
                XElement refNode = new XElement ( nsWiX3 + "ComponentRef" ,
                                        new XAttribute ( "Id" ,
                                         component.Attribute ( "Id" ).Value ) );
                groupNode.Add ( refNode );
            }

            // Add the group node as the first child of the outputFragment only
            // if there are some components. It's perfectly reasonable to have
            // a fragment made up of nothing but directories.
            if ( 0 != compNodes.Count ( ) )
            {
                fragment.AddFirst ( groupNode );
            }
        }

        /// <summary>
        /// Gets the directory values initialized so the code can handle aliases
        /// and the individual directories.
        /// </summary>
        private static void InitializeDirectoryValues ( )
        {
            fullStartDirectory = Path.GetFullPath ( argValues.StartDirectory );
            DirectoryInfo info = new DirectoryInfo ( fullStartDirectory );
            baseDirectoryName = info.Name;
        }

        /// <summary>
        /// MSI only accepts IDs that are 72 characters long so I need to ensure
        /// that the strings I use are within that limit.
        /// </summary>
        /// <param name="start">
        /// The initial part of the string.
        /// </param>
        /// <param name="main">
        /// The main part of the string.
        /// </param>
        /// <param name="uniqueId">
        /// The unique value to append to this string.
        /// </param>
        /// <returns>
        /// A unique string that is only 70 characters long and has all invalid
        /// characters stripped.
        /// </returns>
        private static String CreateSeventyCharIdString ( String start ,
                                                          String main ,
                                                          int uniqueId )
        {
            const String FormatStr = "{0}_{1}_{2}";
            const Int32 MaxLen = 70;

            String uniqueStr = String.Format ( CultureInfo.InvariantCulture ,
                                              "{0}" ,
                                              uniqueId );
            StringBuilder sb = new StringBuilder ( 100 );
            sb.AppendFormat ( FormatStr , start , main , uniqueStr );
            if ( sb.Length > MaxLen )
            {
                sb.Length = 0;
                int idLen = uniqueStr.Length;
                int startLen = start.Length;
                int len = Math.Min ( main.Length ,
                                     MaxLen - ( idLen + startLen ) );
                String sub = main.Substring ( 0 , len );
                sb.AppendFormat ( FormatStr ,
                                  start ,
                                  sub ,
                                  uniqueStr );
            }

            // Turns out id strings in WiX cannot have dashes in them so 
            // convert them to underscores.
            String retVal = RemoveInvalidIdCharacters ( sb.ToString ( ) );
            return ( retVal );
        }

        /// <summary>
        /// Looks through the list of directory exclusions and returns true
        /// if this directory is supposed to be excluded.
        /// </summary>
        /// <param name="directory">
        /// The directory to check if it's got any excluded value in it.
        /// </param>
        /// <returns>
        /// True  - Supposed to exclude and skip.
        /// False - Process this directory.
        /// </returns>
        private static Boolean IsDirectoryExcluded ( String directory )
        {
            Boolean skipDirectory = false;

            // If the user wanted to skip some directories, check to see
            // if this happens to be one.
            for ( int i = 0 ; i < argValues.DirectoryExcludeList.Count ; i++ )
            {
                if ( true == directory.Contains (
                                argValues.DirectoryExcludeList [ i ] ) )
                {
                    skipDirectory = true;
                    break;
                }
            }

            return ( skipDirectory );
        }

        /// <summary>
        /// Returns a unique name for the Directory Id attribute.
        /// </summary>
        /// <param name="directory">
        /// The full directory to process.
        /// </param>
        /// <returns>
        /// A string that encapsulates the directory name with a unique value
        /// appended.
        /// </returns>
        private static String GenerateUniqueDirectoryIdName ( String directory )
        {
            // To make the Id a bit easier to read, I'm going to use a naming 
            // scheme of "dir_<Path>_<Num>". That will put some uniqueness on 
            // the names so they don't conflict across large installs.

            // Suffix the directory with \ to ensure I find the exact match 
            // when looking the base directory.
            if ( false == directory.EndsWith ( "\\" ,
                                          StringComparison.OrdinalIgnoreCase ) )
            {
                directory += "\\";
            }

            // Figure out where the base directory name is in this string and
            // create a unique name for the Id attribute.
            String exactBaseDirectory =
                        String.Format ( CultureInfo.InvariantCulture ,
                                        "\\{0}\\" ,
                                        baseDirectoryName );
            Int32 startBaseDir = directory.IndexOf ( exactBaseDirectory ,
                                           StringComparison.OrdinalIgnoreCase );

            // Get the real value and skip the preceding \\ used to find the
            // exact base.
            String dirIdString = directory.Substring ( startBaseDir + 1 );
            if ( '\\' == dirIdString [ dirIdString.Length - 1 ] )
            {
                dirIdString = dirIdString.Substring ( 0 ,
                                                       dirIdString.Length - 1 );
            }

            dirIdString = dirIdString.Replace ( '\\' , '.' );
            dirIdString = CreateSeventyCharIdString ( "dir" ,
                                                    dirIdString ,
                                                    directoryNumber );

            // Since I've used this directoryNumber, time to bump it up.
            directoryNumber += argValues.IncrementValue;

            return ( dirIdString );
        }

        /// <summary>
        /// Creates a new Directory element.
        /// </summary>
        /// <param name="directory">
        /// The file directory for this element.
        /// </param>
        /// <returns>
        /// A constructed <see cref="XElement"/>.
        /// </returns>
        private static XElement CreateDirectoryElement ( String directory )
        {
            // Each directory element needs a unique value.
            String uniqueDirID = GenerateUniqueDirectoryIdName ( directory );

            // Get the long and short names for this directory.
            DirectoryInfo info = new DirectoryInfo ( directory );

            // I've got enough to create the Directory node.
            XElement directoryNode = new XElement ( nsWiX3 + "Directory" ,
                                        new XAttribute ( "Id" , uniqueDirID ) ,
                                        new XAttribute ( "Name" , info.Name ) );
            return ( directoryNode );
        }

        /// <summary>
        /// Creates a File element for the file in <paramref name="fileName"/>.
        /// </summary>
        /// <param name="fileName">
        /// The full filename to process.
        /// </param>
        /// <returns>
        /// A valid <see cref="XElement"/> for the File element.
        /// </returns>
        private static XElement CreateFileElement ( String fileName )
        {
            // Create a unique filename. In a one file per component run, this
            // will mean that the file and it's parent component will have the 
            // same number.
            String fileId = CreateSeventyCharIdString ( "file" ,
                                                     argValues.CustomValue ,
                                                     componentNumber - 1 );

            // If the user wanted to group all the files into a component, I
            // need to bump up the componentNumber to keep everything straight.
            if ( true == argValues.MultipleFilesPerComponent )
            {
                componentNumber++;
            }

            XElement file = new XElement ( nsWiX3 + "File" ,
                                           new XAttribute ( "Id" , fileId ) );
            if ( true == IsPEFile ( fileName ) )
            {
                file.Add ( new XAttribute ( "Checksum" , "yes" ) );
            }
            fileName = BuildAliasedFilename ( fileName );
            file.Add ( new XAttribute ( "Source" , fileName ) );
            return ( file );
        }

        /// <summary>
        /// Creates a WiX 3 File Element from a WiX 2 File Element.
        /// </summary>
        /// <param name="input">
        /// The WiX 2.0 file element.
        /// </param>
        /// <returns>
        /// The WiX 3.0 file element.
        /// </returns>
        private static XElement CreateWiX3FileFromWix2File ( XElement input )
        {

            XElement newFile = new XElement ( nsWiX3 + "File" );
            foreach ( var attrib in input.Attributes ( ) )
            {
                newFile.SetAttributeValue ( attrib.Name , attrib.Value );
            }

            // If it's a PE binary file, add on the Checksum attribute.
            if ( true == IsPEFile ( newFile.Attribute ( "Source" ).Value ) )
            {
                newFile.SetAttributeValue ( "Checksum" ,
                                            "yes" );
            }
            return ( newFile );
        }

        /// <summary>
        /// Creates a standard Component element.
        /// </summary>
        /// <returns>
        /// A newly created and unique Component elements.
        /// </returns>
        private static XElement CreateComponentElement ( )
        {
            // Make sure the Id field is less than or equal to 70 characters.
            String componentId = CreateSeventyCharIdString ( "comp" ,
                                                     argValues.CustomValue ,
                                                     componentNumber );

            // Increment since I just used that number.
            componentNumber++;

            String guidString = "PUT-GUID-HERE";
            if ( true == argValues.GenerateGuids )
            {
                Guid g = Guid.NewGuid ( );
                guidString = g.ToString ( ).ToUpperInvariant ( );
            }

            XElement comp = new XElement ( nsWiX3 + "Component" ,
                                   new XAttribute ( "Id" , componentId ) ,
                                   new XAttribute ( "DiskId" , "1" ) ,
                                   new XAttribute ( "KeyPath" , "yes" ) ,
                                   new XAttribute ( "Guid" , guidString ) );

            // Does the user want the Win64 attribute?
            if ( true == argValues.Win64 )
            {
                comp.SetAttributeValue ( "Win64" , "yes" );
            }

            return ( comp );
        }

        private static String BuildAliasedFilename ( String fileName )
        {
            // Does the user want an alias for the base directory name?
            if ( false == String.IsNullOrEmpty ( argValues.Alias ) )
            {
                fileName = fileName.Replace ( fullStartDirectory ,
                                              argValues.Alias );
            }

            return ( fileName );
        }

        private static Boolean IsPEFile ( String fileName )
        {
            String ext = Path.GetExtension ( fileName );
            ext = ext.ToUpper ( CultureInfo.CurrentCulture );

            for ( int i = 0 ; i < peFileExtension.Length ; i++ )
            {
                if ( 0 == String.Compare ( ext ,
                                           peFileExtension[i] ,
                                           true ,
                                           CultureInfo.CurrentCulture ) )
                {
                    return ( true );
                }
            }
            return ( false );
        }

        private static String RemoveInvalidIdCharacters ( String input )
        {
            // WiX 2.0 does not document the actual valid characters in an Id
            // attribute. This is especially true in Directory elements. What
            // I'll do here is replace everything that's not in the range
            // [0-9a-zA-Z_] with underscores. While that might make some of the
            // Ids harder to read, I'm assured CANDLE.EXE will compile the
            // fragment.
            return ( Regex.Replace ( input , "[^0-9a-zA-Z_]" , "_" ) );
        }

        private static class NativeMethods
        {
            private const int MaxPath = 255;

            [DllImport ( "shlwapi.dll" ,
                         CharSet = CharSet.Unicode ,
                         ExactSpelling = true )]
            internal static extern int StrCmpLogicalW ( String x , String y );

        }

        /// <summary>
        ///  Used to sort values in strings in logical order.
        /// </summary>
        private class LogicalStringComparer : IComparer<String>
        {
            /// <summary>
            /// Calls the native logical string compare method.
            /// </summary>
            /// <param name="x">
            /// The first string to compare.
            /// </param>
            /// <param name="y">
            /// The second string to compare.
            /// </param>
            /// <returns>
            /// Zero if the strings are identical, 1 if 
            /// <paramref name="x"/> is greater, -1 if 
            /// <paramref name="y"/> is greater.
            /// </returns>
            public int Compare ( string x , string y )
            {
                return ( NativeMethods.StrCmpLogicalW ( x , y ) );
            }
        }
    }
}
