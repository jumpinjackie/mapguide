/*------------------------------------------------------------------------------
 * Copyright © 2007 John Robbins -- All rights reserved. 
 -----------------------------------------------------------------------------*/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.IO;
using System.Diagnostics.CodeAnalysis;

namespace Wintellect.Paraffin
{
    internal class ParaffinArgParser : ArgParser
    {
        #region Required Creation Parameters
        /// <summary>
        /// The directory to process.
        /// </summary>
        public String StartDirectory { get; set; }

        /// <summary>
        /// The custom value to put on all components/files/dirs.
        /// </summary>
        public String CustomValue { get; set; }

        #endregion

        #region Optional Creation Parameters
        /// <summary>
        /// Override the DirectoryRef Id if you want the fragment files
        /// to go somewhere esle besides the INSTALLDIR.
        /// </summary> 
        public String DirectoryRef { get; set; }

        /// <summary>
        /// The value to replace the starting directory in the File element
        /// src attribute.
        /// </summary>
        public String Alias { get; set; }

        /// <summary>
        /// If true, all the files in a directory will be included into a 
        /// single component.
        /// </summary>
        public Boolean MultipleFilesPerComponent { get; set; }

        /// <summary>
        /// The list of extensions to skip.
        /// </summary>
        public Dictionary<String , Boolean> ExtensionList { get; set; }

        /// <summary>
        /// If true, GUIDs will be generated for all components.
        /// </summary>
        public Boolean GenerateGuids { get; set; }

        /// <summary>
        /// The amount to add to each component number to leave room for 
        /// additional component files between directories.
        /// </summary>
        public Int32 IncrementValue { get; set; }

        /// <summary>
        /// If true, does not recurse the directories.
        /// </summary>
        public Boolean NoDirectoryRecursion { get; set; }

        /// <summary>
        /// The list of directories to exclude from the processing.
        /// </summary>
        public List<String> DirectoryExcludeList { get; set; }

        /// <summary>
        /// If true, adds the Win64 attribute to components.
        /// </summary>
        public Boolean Win64 { get; set; }
        #endregion

        /// <summary>
        /// True if the user wants to update a previously created file.
        /// </summary>
        public Boolean Update { get; set; }

        /// <summary>
        /// The output filename.
        /// </summary>
        public String FileName { get; set; }

        // The private string to hold more detailed error information.
        private String errorMessage;
        // Indicates the error was found in OnDoneParse.
        private Boolean errorInOnDoneParse;
        // Indicates we've already seen the -inc switch.
        private Boolean seenIncrementSwitch;

        #region Command Line Option Constants
        private const string k_HELP_QUESTION = "?";
        private const string k_HELP_SHORT = "h";
        private const string k_HELP = "help";
        private const string k_GUIDS = "guids";
        private const string k_GUIDS_SHORT = "g";
        private const string k_ALIAS = "alias";
        private const string k_ALIAS_SHORT = "a";
        private const string k_MULTIPLE = "multiple";
        private const string k_MULTIPLE_SHORT = "m";
        private const string k_NORECURSE = "norecurse";
        private const string k_NORECURSE_SHORT = "nr";
        private const string k_DIR = "dir";
        private const string k_DIR_SHORT = "d";
        private const string k_CUSTOM = "custom";
        private const string k_CUSTOM_SHORT = "c";
        private const string k_EXT = "ext";
        private const string k_EXT_SHORT = "e";
        private const string k_DIREXCLUDE = "direXclude";
        private const string k_DIREXCLUDE_SHORT = "x";
        private const string k_INC = "inc";
        private const string k_INC_SHORT = "i";
        private const string k_UPDATE = "update";
        private const string k_UPDATE_SHORT = "u";
        private const string k_WIN64 = "win64";
        private const string k_DIRREF = "dirref";
        private const string k_DIRREF_SHORT = "dr";
        #endregion

        public ParaffinArgParser ( )
            : base ( new String [] { k_HELP_QUESTION   , k_HELP_SHORT , k_HELP ,
                                     k_GUIDS_SHORT     , k_GUIDS ,
                                     k_MULTIPLE_SHORT  , k_MULTIPLE ,
                                     k_NORECURSE_SHORT , k_NORECURSE ,
                                     k_UPDATE_SHORT    , k_UPDATE ,
                                     k_WIN64 } ,
                     new String [] { k_DIR_SHORT        , k_DIR ,
                                     k_CUSTOM_SHORT     , k_CUSTOM ,
                                     k_ALIAS_SHORT      , k_ALIAS ,
                                     k_EXT_SHORT        , k_EXT ,
                                     k_INC_SHORT        , k_INC ,
                                     k_DIREXCLUDE_SHORT , k_DIREXCLUDE ,
                                     k_DIRREF           , k_DIRREF_SHORT } , 
                     true )
        {
            // Set all the appropriate defaults.
            FileName = String.Empty;
            StartDirectory = String.Empty;
            CustomValue = String.Empty;
            Alias = String.Empty;
            DirectoryRef = String.Empty;
            MultipleFilesPerComponent = false;
            ExtensionList = new Dictionary<String , Boolean> ( );
            GenerateGuids = false;
            IncrementValue = 1;
            DirectoryExcludeList = new List<String> ( );

            errorMessage = String.Empty;
        }

        [SuppressMessage ( "Microsoft.Maintainability" ,
                           "CA1502:AvoidExcessiveComplexity" ,
                           Justification =
              "A switch statement using strings always generates complexity." )]
        protected override SwitchStatus OnSwitch ( string switchSymbol ,
                                                   string switchValue )
        {
            SwitchStatus ss = SwitchStatus.NoError;
            switch ( switchSymbol )
            {
                case k_HELP_QUESTION:
                case k_HELP_SHORT:
                case k_HELP:
                    ss = SwitchStatus.ShowUsage;
                    break;

                case k_GUIDS_SHORT:
                case k_GUIDS:
                    GenerateGuids = true;
                    break;

                case k_MULTIPLE_SHORT:
                case k_MULTIPLE:
                    MultipleFilesPerComponent = true;
                    break;

                case k_NORECURSE_SHORT:
                case k_NORECURSE:
                    NoDirectoryRecursion = true;
                    break;

                case k_UPDATE_SHORT:
                case k_UPDATE:
                    Update = true;
                    break;

                case k_WIN64:
                    Win64 = true;
                    break;

                case k_ALIAS_SHORT:
                case k_ALIAS:
                    if ( false == String.IsNullOrEmpty ( Alias ) )
                    {
                        errorMessage = Constants.AliasMultipleSwitches;
                        ss = SwitchStatus.Error;
                    }
                    else
                    {
                        // If the alias does not end with a \, add one to help
                        // the user out.
                        if ( false == switchValue.EndsWith ( "\\" ,
                                          StringComparison.OrdinalIgnoreCase ) )
                        {
                            switchValue += "\\";
                        }
                        Alias = switchValue;
                    }
                    break;


                case k_DIRREF_SHORT:
                case k_DIRREF:
                    if ( false == String.IsNullOrEmpty ( DirectoryRef ) )
                    {
                        errorMessage = Constants.DirectoryRefMultipleSwitches;
                        ss = SwitchStatus.Error;
                    }
                    else
                    {
                        DirectoryRef = switchValue;
                    }
                    break;

                case k_DIR_SHORT:
                case k_DIR:
                    if ( false == String.IsNullOrEmpty ( StartDirectory ) )
                    {
                        errorMessage = Constants.DirectoryMultipleSwitches;
                        ss = SwitchStatus.Error;
                    }
                    else if ( true == String.IsNullOrEmpty ( switchValue ) )
                    {
                        errorMessage = Constants.DirectoryCannotBeEmpty;
                        ss = SwitchStatus.Error;
                    }
                    else
                    {
                        // If the directory does not end with a \, add one.
                        if ( false == switchValue.EndsWith ( "\\" ,
                                          StringComparison.OrdinalIgnoreCase ) )
                        {
                            switchValue += "\\";
                        }
                        StartDirectory = switchValue;
                    }
                    break;

                case k_CUSTOM_SHORT:
                case k_CUSTOM:
                    if ( false == String.IsNullOrEmpty ( CustomValue ) )
                    {
                        errorMessage = Constants.UniqueMultipleSwitches;
                        ss = SwitchStatus.Error;
                    }
                    else if ( true == String.IsNullOrEmpty ( switchValue ) )
                    {
                        errorMessage = Constants.UniqueCannotBeEmpty;
                        ss = SwitchStatus.Error;
                    }
                    else if ( switchValue.Length >= 65 )
                    {
                        errorMessage = Constants.UniqueTooLong;
                        ss = SwitchStatus.Error;
                    }
                    else
                    {
                        CustomValue = switchValue;
                    }
                    break;

                case k_EXT_SHORT:
                case k_EXT:
                    if ( true == String.IsNullOrEmpty ( switchValue ) )
                    {
                        errorMessage = Constants.ExtensionCannotBeEmpty;
                        ss = SwitchStatus.Error;
                    }
                    else
                    {
                        // Does it start with a period? If not, add one to help
                        // the user out.
                        if ( '.' != switchValue [ 0 ] )
                        {
                            switchValue = "." + switchValue;
                        }
                        // You can have as many -ext switches as you want.
                        ExtensionList.Add ( switchValue.ToUpperInvariant ( ) ,
                                            true );
                    }
                    break;

                case k_INC_SHORT:
                case k_INC:
                    if ( true == seenIncrementSwitch )
                    {
                        errorMessage = Constants.IncrementMultipleSwitches;
                        ss = SwitchStatus.Error;
                    }
                    else
                    {
                        Int32 level = 0;
                        if ( false == Int32.TryParse ( switchValue ,
                                                       out level ) )
                        {
                            errorMessage = Constants.IncrementNoParse;
                            ss = SwitchStatus.Error;
                        }
                        else if ( level <= 0 )
                        {
                            errorMessage = Constants.IncrementNotZero;
                            ss = SwitchStatus.Error;
                        }
                        else
                        {
                            IncrementValue = level;
                            seenIncrementSwitch = true;
                        }
                    }
                    break;

                case k_DIREXCLUDE_SHORT:
                case k_DIREXCLUDE:
                    DirectoryExcludeList.Add ( switchValue );
                    break;

                default:
                    {
                        errorMessage = Constants.UnknownCommandLineOption;
                        ss = SwitchStatus.Error;
                    }
                    break;
            }
            return ( ss );
        }

        protected override SwitchStatus OnNonSwitch ( string value )
        {
            SwitchStatus ss = SwitchStatus.NoError;
            if ( false == String.IsNullOrEmpty ( FileName ) )
            {
                errorMessage = Constants.OutputAlreadySpecified;
                ss = SwitchStatus.Error;
            }
            else if ( true == String.IsNullOrEmpty ( value ) )
            {
                errorMessage = Constants.OutputCannotBeEmpty;
                ss = SwitchStatus.Error;
            }
            else
            {
                FileName = value;
            }

            // There are no non switches allowed.
            errorMessage = Constants.UnknownCommandLineOption;
            return ( ss );
        }

        protected override SwitchStatus OnDoneParse ( )
        {
            SwitchStatus ss = SwitchStatus.NoError;
            // The output file can never be null.
            if ( true == string.IsNullOrEmpty ( FileName ) )
            {
                errorMessage = Constants.OutputCannotBeEmpty;
                ss = SwitchStatus.Error;
                errorInOnDoneParse = true;
            }
            else if ( false == Update )
            {
                // Check that I at least have a directory and prefix. Everything
                // else is optional when updating files.
                if ( true == String.IsNullOrEmpty ( StartDirectory ) )
                {
                    errorMessage = Constants.DirectoryCannotBeEmpty;
                    ss = SwitchStatus.Error;
                    errorInOnDoneParse = true;
                }
                else if ( false == Directory.Exists ( StartDirectory ) )
                {
                    errorMessage = Constants.DirectoryDoesNotExist;
                    ss = SwitchStatus.Error;
                    errorInOnDoneParse = true;
                }
                else if ( true == String.IsNullOrEmpty ( CustomValue ) )
                {
                    errorMessage = Constants.UniqueCannotBeEmpty;
                    ss = SwitchStatus.Error;
                    errorInOnDoneParse = true;
                }
            }
            else
            {
                // The user is asking to update.
                // Check that they didn't also specify creation options.
                if ( ( false == String.IsNullOrEmpty ( StartDirectory ) ||
                     ( false == String.IsNullOrEmpty ( CustomValue ) ) ) )
                {
                    errorMessage = Constants.MutuallyExclusiveOptions;
                    ss = SwitchStatus.Error;
                    errorInOnDoneParse = true;
                }
                // Check to at least see if the file exists.
                if ( false == File.Exists ( FileName ) )
                {
                    errorMessage = Constants.UpdateFileMustExist;
                    ss = SwitchStatus.Error;
                    errorInOnDoneParse = true;
                }
            }
            return ( ss );
        }

        public override void OnUsage ( string errorInfo )
        {
            ProcessModule exe = Process.GetCurrentProcess ( ).Modules [ 0 ];
            Console.WriteLine ( Constants.UsageString ,
                                exe.FileVersionInfo.FileVersion );
            if ( ( false == errorInOnDoneParse ) &&
                 ( false == String.IsNullOrEmpty ( errorInfo ) ) )
            {
                Console.WriteLine ( );
                Console.WriteLine ( Constants.ErrorSwitch , errorInfo );
            }
            if ( false == String.IsNullOrEmpty ( errorMessage ) )
            {
                Console.WriteLine ( );
                Console.WriteLine ( errorMessage );
            }
        }
    }
}
