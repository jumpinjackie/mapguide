// --------------------------------------------------------------------------------------------------------------------
// <copyright file="SilverlightExtension.cs" company="Xsd2Code">
//   N/A
// </copyright>
// <summary>
//   Implements code generation extension for Silverlight 2.0
// </summary>
// <remarks>
//  Updated 2010-01-20 Deerwood McCord Jr. Cleaned CodeSnippetStatements by replacing with specific CodeDom Expressions
// </remarks>
// --------------------------------------------------------------------------------------------------------------------

namespace Xsd2Code.Library.Extensions
{
    using System.CodeDom;
    using System.IO;
    using System.IO.IsolatedStorage;
    using Helpers;

    /// <summary>
    /// Implements code generation extension for Silverlight 2.0
    /// </summary>
    [CodeExtension(TargetFramework.Silverlight)]
    public class SilverlightExtension : Net35Extension
    {
        /// <summary>
        /// Import namespaces
        /// </summary>
        /// <param name="code">Code namespace</param>
        protected override void ImportNamespaces(CodeNamespace code)
        {
            base.ImportNamespaces(code);
            if (GeneratorContext.GeneratorParams.IncludeSerializeMethod)
            {
                code.Imports.Add(new CodeNamespaceImport("System.IO.IsolatedStorage"));
            }
        }

        /// <summary>
        /// Gets the load from file CodeDOM method.
        /// </summary>
        /// <param name="type">The type CodeTypeDeclaration.</param>
        /// <returns>return the codeDom LoadFromFile method</returns>
        protected override CodeMemberMethod GetLoadFromFileMethod(CodeTypeDeclaration type)
        {
            string typeName = GeneratorContext.GeneratorParams.UseGenericBaseClass ? "T" : type.Name;

            // ---------------------------------------------
            // public static T LoadFromFile(string fileName)
            // ---------------------------------------------
            var loadFromFileMethod = new CodeMemberMethod
            {
                Attributes = MemberAttributes.Public | MemberAttributes.Static,
                Name = GeneratorContext.GeneratorParams.LoadFromFileMethodName
            };

            loadFromFileMethod.Parameters.Add(new CodeParameterDeclarationExpression(typeof(string), "fileName"));
            loadFromFileMethod.ReturnType = new CodeTypeReference(typeName);

            loadFromFileMethod.Statements.Add(
                new CodeVariableDeclarationStatement(
                        new CodeTypeReference(typeof(IsolatedStorageFile)),
                        "isoFile",
                        new CodePrimitiveExpression(null)));

            loadFromFileMethod.Statements.Add(
                new CodeVariableDeclarationStatement(
                        new CodeTypeReference(typeof(IsolatedStorageFileStream)),
                        "isoStream",
                        new CodePrimitiveExpression(null)));

            loadFromFileMethod.Statements.Add(
               new CodeVariableDeclarationStatement(
                       new CodeTypeReference(typeof(StreamReader)),
                       "sr",
                       new CodePrimitiveExpression(null)));

            var tryStatmanentsCol = new CodeStatementCollection();
            var finallyStatmanentsCol = new CodeStatementCollection();

            tryStatmanentsCol.Add(
                new CodeAssignStatement(
                new CodeVariableReferenceExpression("isoFile"),
                CodeDomHelper.GetInvokeMethod("IsolatedStorageFile", "GetUserStoreForApplication")));

            tryStatmanentsCol.Add(
                new CodeAssignStatement(
                    new CodeVariableReferenceExpression("isoStream"),
                    new CodeObjectCreateExpression(
                        typeof(IsolatedStorageFileStream),
                        new CodeExpression[]
                        {
                             new CodeArgumentReferenceExpression("fileName"),
                             CodeDomHelper.GetEnum("FileMode","Open"),
                             new CodeVariableReferenceExpression("isoFile")
                    })));

            tryStatmanentsCol.Add(
                new CodeAssignStatement(
                    new CodeVariableReferenceExpression("sr"),
                    new CodeObjectCreateExpression(
                        typeof(StreamReader),
                        new CodeExpression[]
                        {
                            new CodeVariableReferenceExpression("isoStream"),
                    })));

            // ----------------------------------
            // string xmlString = sr.ReadToEnd();
            // ----------------------------------
            var readToEndInvoke = CodeDomHelper.GetInvokeMethod("sr", "ReadToEnd");

            var xmlString = new CodeVariableDeclarationStatement(
                new CodeTypeReference(typeof(string)), "xmlString", readToEndInvoke);

            tryStatmanentsCol.Add(xmlString);
            tryStatmanentsCol.Add(CodeDomHelper.GetInvokeMethod("isoStream", "Close"));
            tryStatmanentsCol.Add(CodeDomHelper.GetInvokeMethod("sr", "Close"));

            // ------------------------------------------------------
            // return Deserialize(xmlString, out obj, out exception);
            // ------------------------------------------------------            
            var fileName = new CodeVariableReferenceExpression("xmlString");

            var deserializeInvoke =
                new CodeMethodInvokeExpression(
                    new CodeMethodReferenceExpression(null, GeneratorContext.GeneratorParams.DeserializeMethodName),
                    new CodeExpression[] { fileName });

            var rstmts = new CodeMethodReturnStatement(deserializeInvoke);
            tryStatmanentsCol.Add(rstmts);

            finallyStatmanentsCol.Add(CodeDomHelper.GetDispose("isoFile"));
            finallyStatmanentsCol.Add(CodeDomHelper.GetDispose("isoStream"));
            finallyStatmanentsCol.Add(CodeDomHelper.GetDispose("sr"));

            var tryfinally = new CodeTryCatchFinallyStatement(
                CodeDomHelper.CodeStmtColToArray(tryStatmanentsCol), new CodeCatchClause[0], CodeDomHelper.CodeStmtColToArray(finallyStatmanentsCol));

            loadFromFileMethod.Statements.Add(tryfinally);

            return loadFromFileMethod;
        }

        /// <summary>
        /// Gets the Silverlight save to isolate storage file.
        /// </summary>
        /// <param name="type">CodeTypeDeclaration type.</param>
        /// <returns>return the save to file code DOM method statment </returns>
        protected override CodeMemberMethod GetSaveToFileMethod()
        {
            // -----------------------------------------------
            // public virtual void SaveToFile(string fileName)
            // -----------------------------------------------
            var saveToFileMethod = new CodeMemberMethod
            {
                Attributes = MemberAttributes.Public,
                Name = GeneratorContext.GeneratorParams.SaveToFileMethodName
            };

            saveToFileMethod.Parameters.Add(new CodeParameterDeclarationExpression(typeof(string), "fileName"));

            saveToFileMethod.Statements.Add(
                new CodeVariableDeclarationStatement(
                    new CodeTypeReference(typeof(StreamWriter)),
                    "streamWriter",
                    new CodePrimitiveExpression(null)));

            saveToFileMethod.Statements.Add(
                new CodeVariableDeclarationStatement(
                    new CodeTypeReference(typeof(IsolatedStorageFile)),
                    "isoFile",
                    new CodePrimitiveExpression(null)));

            saveToFileMethod.Statements.Add(
                new CodeVariableDeclarationStatement(
                    new CodeTypeReference(typeof(IsolatedStorageFileStream)),
                    "isoStream",
                    new CodePrimitiveExpression(null)));

            // ------------------------
            // try {...} finally {...}
            // -----------------------
            var tryExpression = new CodeStatementCollection();

            tryExpression.Add(
                new CodeAssignStatement(
                new CodeVariableReferenceExpression("isoFile"),
                CodeDomHelper.GetInvokeMethod("IsolatedStorageFile", "GetUserStoreForApplication")));

            tryExpression.Add(
                new CodeAssignStatement(
                    new CodeVariableReferenceExpression("isoStream"),
                    new CodeObjectCreateExpression(
                        typeof(IsolatedStorageFileStream),
                        new CodeExpression[]
                        {
                             new CodeArgumentReferenceExpression("fileName"),
                             CodeDomHelper.GetEnum("FileMode","Create"),
                             new CodeVariableReferenceExpression("isoFile")
                    })));

            tryExpression.Add(
                new CodeAssignStatement(
                    new CodeVariableReferenceExpression("streamWriter"),
                    new CodeObjectCreateExpression(
                        typeof(StreamWriter),
                        new CodeExpression[]
                        {
                            new CodeVariableReferenceExpression("isoStream"),
                    })));

            // ------------------------------
            // string xmlString = Serialize();
            // -------------------------------
            var serializeMethodInvoke = new CodeMethodInvokeExpression(
                new CodeMethodReferenceExpression(null, GeneratorContext.GeneratorParams.SerializeMethodName));

            var xmlString = new CodeVariableDeclarationStatement(
                new CodeTypeReference(typeof(string)), "xmlString", serializeMethodInvoke);

            tryExpression.Add(xmlString);

            // ----------------------------------
            // streamWriter.WriteLine(xmlString);
            // ----------------------------------
            var writeLineMethodInvoke =
                CodeDomHelper.GetInvokeMethod(
                                                "streamWriter",
                                                "WriteLine",
                                                new CodeExpression[]
                                                  {
                                                      new CodeVariableReferenceExpression("xmlString")
                                                  });

            tryExpression.Add(writeLineMethodInvoke);
            tryExpression.Add(CodeDomHelper.GetInvokeMethod("streamWriter", "Close"));
            tryExpression.Add(CodeDomHelper.GetInvokeMethod("isoStream", "Close"));

            var finallyStatmanentsCol = new CodeStatementCollection();
            finallyStatmanentsCol.Add(CodeDomHelper.GetDispose("streamWriter"));
            finallyStatmanentsCol.Add(CodeDomHelper.GetDispose("isoFile"));
            finallyStatmanentsCol.Add(CodeDomHelper.GetDispose("isoStream"));

            var trycatch = new CodeTryCatchFinallyStatement(tryExpression.ToArray(), new CodeCatchClause[0], finallyStatmanentsCol.ToArray());
            saveToFileMethod.Statements.Add(trycatch);

            return saveToFileMethod;
        }
    }
}