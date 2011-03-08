﻿namespace Xsd2Code.Library
{
    /// <summary>
    /// Static generation context 
    /// </summary>
    /// <remarks>
    /// Revision history:
    /// 
    ///     Modified 2009-02-20 by Ruslan Urban
    ///     Added CodeBaseTag and GenerateCloneMethodTag constants
    /// 
    /// </remarks>
    public static class GeneratorContext
    {
        /// <summary>
        /// Enable serialize/deserialize generation method
        /// </summary>
        public const string CODEBASETAG = "CodeBaseTag";

        /// <summary>
        /// Indicate if use base class for all methods
        /// </summary>
        public const string USEGENERICBASECLASSTAG = "UseBaseClass";       

        /// <summary>
        /// Language tag
        /// </summary>
        public const string CODETYPETAG = "codeType";

        /// <summary>
        /// Language tag
        /// </summary>
        public const string BASECLASSNAMETAG = "BaseClassName";

        /// <summary>
        /// Base generic type for collections when CollectionType is DefinedType
        /// </summary>
        public const string COLLECTIONBASETAG = "CollectionBase";

        /// <summary>
        /// Collection tag
        /// </summary>
        public const string COLLECTIONTAG = "Collection";

        /// <summary>
        /// List of custom using/import types (, separated)
        /// </summary>
        public const string CUSTOMUSINGSTAG = "CustomUsings";

        /// <summary>
        /// Name of Deserialize method
        /// </summary>
        public const string DESERIALIZEMETHODNAMETAG = "DeserializeMethodName";

        /// <summary>
        /// Disable debug in genetated code.
        /// </summary>
        public const string DISABLEDEBUGTAG = "DisableDebug";

        /// <summary>
        /// Indicate if generate automatic properties.
        /// </summary>
        public const string AUTOMATICPROPERTIESTAG = "AutomaticProperties";

        /// <summary>
        /// Enable data binding tag
        /// </summary>
        public const string ENABLEDATABINDINGTAG = "EnableDataBinding";


        /// <summary>
        /// Enable lasy pattern
        /// </summary>
        public const string ENABLELASYLOADINGTAG = "EnableLasyLoading";

        /// <summary>
        /// Generate summary documentation
        /// </summary>
        public const string ENABLESUMMARYCOMMENTTAG = "EnableSummaryComment";

        /// <summary>
        /// Hide private fields in IDE
        /// </summary>
        public const string HIDEPRIVATEFIELDTAG = "HidePrivateFieldInIDE";

        /// <summary>
        /// Enable generation of the Clone method
        /// </summary>
        public const string GENERATECLONEMETHODTAG = "GenerateCloneMethod";

        /// <summary>
        /// Enable serialize/deserialize generation method
        /// </summary>
        public const string INCLUDESERIALIZEMETHODTAG = "IncludeSerializeMethod";

        /// <summary>
        /// Name of Deserialize method
        /// </summary>
        public const string LOADFROMFILEMETHODNAMETAG = "LoadFromFileMethodName";

        /// <summary>
        /// namespace tag
        /// </summary>
        public const string NAMESPACETAG = "NameSpace";

        /// <summary>
        /// Name of Serialize method
        /// </summary>
        public const string SAVETOFILEMETHODNAMETAG = "SaveToFileMethodName";

        /// <summary>
        /// Name of Serialize method
        /// </summary>
        public const string SERIALIZEMETHODNAMETAG = "SerializeMethodName";

        /// <summary>
        /// Generate data contracts tag
        /// </summary>
        public const string GENERATEDATACONTRACTSTAG = "GenerateDataContracts";

        /// <summary>
        /// Indicate if generate NET 2.0 serialization attributes.
        /// </summary>
        public const string GENERATEXMLATTRIBUTESTAG = "GenerateXMLAttributes";

        /// <summary>
        /// Indicating to exclude class generation types includes/imported into schema.
        /// </summary>
        public const string EXCLUDEINCLUDEDTYPESTAG = "ExcludeIncludedTypes";

        /// <summary>
        ///  Enable/Disable Global initialisation of the fields in both Constructors, Lazy Properties. Maximum override
        /// </summary>
        public const string ENABLEINITIALIZEFIELDSTAG =  "EnableInitializeFields"; 

        #region Fields

        /// <summary>
        /// static instance of GeneratorParams
        /// </summary>
        private static GeneratorParams generatorParams = new GeneratorParams();

        #endregion

        #region property

        /// <summary>
        /// Gets or sets static instance of generator parameters, accessible from within any class in the application domain
        /// </summary>
        public static GeneratorParams GeneratorParams
        {
            get { return generatorParams; }
            set { generatorParams = value; }
        }

        #endregion
    }
}