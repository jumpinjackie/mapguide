// IMake.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SimpleXmlParser.h"

enum Language
{
    unknown,
    php,
    csharp,
    java
};

static char version[] = "1.2.4";
static char EXTERNAL_API_DOCUMENTATION[] = "(NOTE: This API is not officially supported and may be subject to removal in a future release without warning. Use with caution.)";

static string module;
static string customPath;
static string target;
static string docTarget;
static string cppInline;
static string swigInline;
static string typedefs;
static string nameSpace;
static string package;
static set<string> classesWithDocs;
static map<string, string> typeReplacements;
static map<string, bool> classes;
static vector<string> headers;
static map<string, int> rootObjectMethods;
static FILE* outfile;
static FILE* docOutFile;
static char charbuf[2];
static bool translateMode;
static Language language;

#ifdef _WIN32
#define FILESEP '\\'
#else
#define FILESEP '/'
#endif

void error(string msg)
{
    fprintf(stderr, "\nError: %s", msg.c_str());
    exit(1);
}

void warning(string msg)
{
    fprintf(stderr, "\nWarning: %s", msg.c_str());
}

string parseModule(XNode* elt)
{
    LPXAttr attr = elt->GetAttr("name");
    if(attr == NULL)
        error("Module element does not have a 'name' attribute");

    return attr->value;
}

string parseCustom(XNode* elt)
{
    LPXAttr attr = elt->GetAttr("path");
    if(attr == NULL)
        error("Custom element does not have a 'path' attribute");

    return attr->value;
}

string parseTarget(XNode* elt)
{
    LPXAttr attr = elt->GetAttr("path");
    if(attr == NULL)
        error("Target element does not have a 'path' attribute");

    return attr->value;
}

string parseDocTarget(XNode* elt)
{
    LPXAttr attr = elt->GetAttr("path");
    if(attr == NULL)
        error("DocTarget element does not have a 'path' attribute");

    return attr->value;
}

string parseCppInline(XNode* elt)
{
    string text = Trim(elt->GetText());
    string uetext = XRef2Entity(text.c_str());
    return uetext;
}

string parseSwigInline(XNode* elt)
{
    string text = Trim(elt->GetText());
    string uetext = XRef2Entity(text.c_str());
    return uetext;
}

string parseTypedefs(XNode* elt)
{
    string text = Trim(elt->GetText());
    return text;
}

void parseHeaders(XNode* elt, vector<string>& headers)
{
    XNodes childs = elt->GetChilds();
    for(int i = 0 ; i < (int)childs.size(); i++)
    {
        XNode* node = childs[i];
        if(node->type != XNODE_ELEMENT)
            continue;

        LPXAttr attr = node->GetAttr("path");
        if(attr == NULL)
            error("Header element does not have a 'path' attribute");

        string path = Trim(attr->value);

        headers.push_back(path);
    }

}

void parseTypeReplacements(XNode* elt, map<string, string>& types)
{
    XNodes childs = elt->GetChilds();
    for(int i = 0 ; i < (int)childs.size(); i++)
    {
        XNode* node = childs[i];
        if(node->type != XNODE_ELEMENT)
            continue;

        LPXAttr oldTypeattr = node->GetAttr("oldtype");
        if(oldTypeattr == NULL)
            error("TypeReplacement element does not have a 'oldtype' attribute");

        LPXAttr newTypeattr = node->GetAttr("newtype");
        if(newTypeattr == NULL)
            error("TypeReplacement element does not have a 'newtype' attribute");

        string oldType = Trim(oldTypeattr->value);
        string newType = Trim(newTypeattr->value);
        if(oldType.length() == 0 || (newType.length() == 0 && !translateMode))
            error("TypeReplacement element have an empty 'oldtype' or 'newtype' attribute");

        types[oldType] = newType;
    }
}

void parseClasses(XNode* elt, map<string, bool>& classes)
{
    XNodes childs = elt->GetChilds();
    for(int i = 0 ; i < (int)childs.size(); i++)
    {
        XNode* node = childs[i];
        if(node->type != XNODE_ELEMENT)
            continue;

        LPXAttr attr = node->GetAttr("name");
        if(attr == NULL)
            error("Class element does not have a 'name' attribute");

        string className = Trim(attr->value);

        classes[className] = true;
    }
}

string parseNamespace(XNode* elt)
{
    string text = Trim(elt->GetText());
    return text;
}

string parsePackage(XNode* elt)
{
    string text = Trim(elt->GetText());
    return text;
}

void parseParameterFile(char* xmlDef)
{
    XNode xml;
    if(xml.Load(xmlDef) == NULL)
        error("XML parsing error");

    XNodes childs = xml.GetChilds();
    for(int i = 0 ; i < (int)childs.size(); i++)
    {
        XNode* node = childs[i];
        if(node->type != XNODE_ELEMENT)
            continue;

        if(node->name == "Module")
        {
            if(translateMode)
                error("Module is not a valid section in translation mode");
            module = parseModule(node);
        }
        else if(node->name == "CustomFile")
        {
            if(translateMode)
                error("Module is not a valid section in translation mode");

            customPath = parseCustom(node);
        }
        else if(node->name == "Target")
        {
            if(translateMode)
                error("Target is not a valid section in translation mode");
            target = parseTarget(node);
        }
        else if(node->name == "DocTarget")
        {
            if(translateMode)
                error("DocTarget is not a valid section in translation mode");
            docTarget = parseDocTarget(node);
        }
        else if(node->name == "CppInline")
        {
            if(translateMode)
                error("CppInLine is not a valid section in translation mode");
            cppInline = parseCppInline(node);
        }
        else if(node->name == "SwigInline" || (node->name == "Inline" && translateMode))
        {
            swigInline = parseSwigInline(node);
        }
        else if(node->name == "Typedefs")
        {
            if(translateMode)
                error("Typedefs is not a valid section in translation mode");
            typedefs = parseTypedefs(node);
        }
        else if(node->name == "TypeReplacements")
        {
            if(translateMode)
                error("TypeReplacements is not a valid section in translation mode");
            parseTypeReplacements(node, typeReplacements);
        }
        else if(node->name == "PHPTypeReplacements")
        {
            if(!translateMode)
                error("PHPTypeReplacements is not a valid section in SWIG mode");
            if(language == php)
                parseTypeReplacements(node, typeReplacements);
        }
        else if(node->name == "JavaTypeReplacements")
        {
            if(!translateMode)
                error("JavaTypeReplacements is not a valid section in SWIG mode");
            if(language == java)
                parseTypeReplacements(node, typeReplacements);
        }
        else if(node->name == "CSharpTypeReplacements")
        {
            if(!translateMode)
                error("CSharpTypeReplacements is not a valid section in SWIG mode");
            if(language == csharp)
                parseTypeReplacements(node, typeReplacements);
        }
        else if(node->name == "Headers")
        {
            parseHeaders(node, headers);
        }
        else if(node->name == "Classes")
        {
            if(!translateMode)
                error("Classes is not a valid section in SWIG generation mode");
            parseClasses(node, classes);
        }
        else if(node->name == "Namespace")
        {
            if(!translateMode)
                error("Namespace is not a valid section in SWIG generation mode");
            nameSpace = parseNamespace(node);
        }
        else if(node->name == "Package")
        {
            if(!translateMode)
                error("Package is not a valid section in SWIG generation mode");
            package = parsePackage(node);
        }
        else
        {
            error(string("Unknow element: ") + node->name);
        }
    }

}

char* toChar(char c)
{
    charbuf[0] = c;
    charbuf[1] = '\0';
    return charbuf;
}

/// Tokenize a C input into a string array. Ignore preprocessing
/// directives and removes comments. The tokenization is very loose and
/// in no way similar to a real C scanner. For our need we just tokenize
/// identifier, and a few operators and delimiters such as '{' '}' '(' ')' ',' ';' ':'
void tokenize(string filename, vector<string>& tokens)
{
    FILE* f = fopen(filename.c_str(), "rb");
    if(f == NULL)
        error(string("Cannot open header file ") + filename);

    //load file in memory
    fseek(f, 0, SEEK_END);
    int length = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* data = new char[length + 1];
    fread(data, length, 1, f);
    data[length] = '\0';

    char* line = strtok(data, "\n");
    string comment;

    if(line != NULL)
    {
        do
        {
            string input = Trim(string(line));

            if(input.length() == 0)
                continue;

            if(input[0] == '#')
                continue;   //ignore preprocessing directive

            int i = 0;
            for(bool done = false; !done && i < (int)input.length(); )
            {
                switch(input[i])
                {
                    case ' ':
                    case '\t':
                    case '\r':
                        //skip whitespaces
                        do
                        {
                            if(isspace(input[i]))
                                break;
                        } while(i < (int)input.length());
                        if(i == (int)input.length())
                            done = true;
                        break;

                    case '/':
                        /*
                        if(!translateMode)
                        {
                            if(i < (int)input.length() - 1 && input[i + 1] == '/')
                                done = true;    //C++ comment, ignore the rest of the input
                            else
                                tokens.push_back("/");
                        }
                        else*/
                        {
                            if(input[i + 1] == '/')
                            {
                                if(input[i + 2] == '/')
                                {
                                    //documentation comment
                                    comment = "";
                                    while(i < (int)input.length())
                                    {
                                        if(input[i] == '\n')
                                            break;
                                        comment.push_back(input[i]);
                                        i++;
                                    }

                                    tokens.push_back(comment);
                                }
                                done = true;
                            }
                            else
                                tokens.push_back("/");
                        }
                        break;

                    case '(':
                        tokens.push_back("(");
                        break;

                    case ')':
                        tokens.push_back(")");
                        break;

                    case '{':
                        tokens.push_back("{");
                        break;

                    case '}':
                        tokens.push_back("}");
                        break;

                    case ',':
                        tokens.push_back(",");
                        break;

                    case ';':
                        {
                            //preserve doc comment at the end of the ; if any
                            bool isComment = false;
                            int j = i + 1;
                            while(j < (int)input.length())
                            {
                                if(input[j] == '/')
                                {
                                    if(input[j+1] == '/' && input[j+2] == '/')
                                    {
                                        isComment = true;
                                        comment = "";
                                        while(i < (int)input.length())
                                        {
                                            if(input[i] == '\n')
                                                break;
                                            comment.push_back(input[i]);
                                            i++;
                                        }
                                        tokens.push_back(comment);
                                        done = true;
                                        break;
                                    }
                                }
                                else
                                    if(!isspace(input[j]))
                                        break;
                                j++;
                            }
                            if(!isComment)
                                tokens.push_back(";");
                        }
                        break;

                    case ':':
                        tokens.push_back(":");
                        break;

                    default:
                        if(isalpha(input[i]) || input[i] == '_')
                        {
                            //identifier
                            string id = toChar(input[i]);
                            while(++ i < (int)input.length())
                            {
                                if(!isalpha(input[i]) &&
                                    !isdigit(input[i]) &&
                                    input[i] != '_')
                                    break;
                                id += toChar(input[i]);
                            }
                            tokens.push_back(id);
                        }
                        else
                        {
                            int beginStr = i;
                            if(translateMode && input[i] == '"')
                            {
                                string str;
                                while(i < (int)input.length())
                                {
                                    str.push_back(input[i]);
                                    if(input[i] == '"' && (beginStr < i && input[i-1] != '\\'))
                                        break;
                                    i++;
                                }
                                ++ i;
                                tokens.push_back(str);
                            }
                            else
                            {
                                //we don't care what is at this point of the input
                                //we store all consecutive characters as a token
                                //until we find a character known from us.
                                string token = toChar(input[i]);
                                while(++ i < (int)input.length())
                                {
                                    if(isalpha(input[i]) ||
                                        input[i] == '_' ||
                                        isspace(input[i]) ||
                                        input[i] == '(' ||
                                        input[i] == ')' ||
                                        input[i] == '{' ||
                                        input[i] == '}' ||
                                        input[i] == ',' ||
                                        input[i] == ';' ||
                                        input[i] == '/' ||
                                        input[i] == ':')
                                        break;
                                    token += toChar(input[i]);
                                }
                                tokens.push_back(token);
                            }
                        }
                        continue;


                }
                ++ i;
            }
        } while((line = strtok(NULL, "\n")) != NULL);
    }

    delete data;
    fclose(f);
}

void processClassIdSection(vector<string>& tokens, int begin, int end)
{
    fprintf(outfile, "\nprivate:\n  ");
    for(int i = begin; i <= end; i++)
    {
        string token = tokens[i];
        fprintf(outfile, "%s ", token.c_str());
    }
}

bool isRootObjectMethod(const string& symbolName)
{
    map<string, int>::const_iterator itMethod = rootObjectMethods.find(symbolName);
    return itMethod != rootObjectMethods.end();
}

bool isAllSlashes(const string& str)
{
    for (int i = 0; i < str.length(); i++) {
        if (str[i] != '/')
            return false;
    }
    return str.length() > 3; //A "///" does not count
}

bool stringReplace(string& str, const string& find, const string& replace)
{
    size_t start_pos = str.find(find);
    if(start_pos == string::npos)
        return false;
    str.replace(start_pos, find.length(), replace);
    return true;
}

void stripHtml(string& str)
{
    //NOTE: We're only stripping tags known to exist in some bits of API documentation
    stringReplace(str, "<p>", "");
    stringReplace(str, "<b>", "");
    stringReplace(str, "<c>", "");
    stringReplace(str, "</p>", "");
    stringReplace(str, "</b>", "");
    stringReplace(str, "</c>", "");
}

void xmlEscapeString(string& str)
{
    stringReplace(str, "&", "&amp;");
    stringReplace(str, "'", "&apos;");
    stringReplace(str, "\"", "&quot;");
    stringReplace(str, "<", "&lt;");
    stringReplace(str, ">", "&gt;");
}

string linkifyCSharpDocFragment(const string& str)
{
    // Explode the fragment into a space delimited list.
    // Go through the delimited tokens and surround any
    // token containing "Mg" with {@link <token>} 
    //
    // Re-combine each delimited token to form the linkified result
    std::vector<std::string> elems;
    std::stringstream ss(str);
    std::string item;
    while(std::getline(ss, item, ' ')) {
        //We can't process these yet, so skip them as they may
        //interfere with doxygen directive translation
        if (item == "\\link" || item == "\\endlink")
            continue;
        stripHtml(item);
        xmlEscapeString(item);
        elems.push_back(item);
    }

    std::string nspace = nameSpace;
    if (nspace.empty()) //Faulty logic if this is the case
        nspace = "OSGeo.MapGuide";

    std::string output;
    for (int i = 0; i < elems.size(); i++) {
        if (i != 0) {
            output.append(" ");
        }
        //If it contains "Mg", assume it's a MapGuide class name and link-ify it
        //TODO: Resolve :: to member links. Right now it just linkifies the Mg class name
        int idx = elems[i].find("Mg");
        if (idx != std::string::npos) {
            std::string prefix;
            std::string mgClassName;
            std::string suffix;
            //Collect characters before Mg
            if (idx > 0) {
                prefix = elems[i].substr(0, idx);
            }
            int cont = -1;
            //Collect the characters in the MapGuide class name
            for (int j = idx; j < elems[i].length(); j++) {
                if (!isalnum(elems[i][j])) {
                    cont = j;
                    break;
                } else {
                    mgClassName += elems[i][j];
                }
            }
            //Collect any characters afterwards
            for (int j = cont; j < elems[i].length(); j++) {
                suffix += elems[i][j];
            }
            output.append(prefix);
            output.append("<see cref=\"");
            output.append(nspace);
            output.append(".");
            output.append(mgClassName);
            output.append("\" />");
            output.append(suffix);
        } else {
            output.append(elems[i]);
        }
    }
    return output;
}

string linkifyJavaDocFragment(const string& str)
{
    // Explode the fragment into a space delimited list.
    // Go through the delimited tokens and surround any
    // token containing "Mg" with {@link <token>} 
    //
    // Re-combine each delimited token to form the linkified result
    std::vector<std::string> elems;
    std::stringstream ss(str);
    std::string item;
    while(std::getline(ss, item, ' ')) {
        //We can't process these yet, so skip them as they may
        //interfere with doxygen directive translation
        if (item == "\\link" || item == "\\endlink")
            continue;
        elems.push_back(item);
    }
    std::string output;
    for (int i = 0; i < elems.size(); i++) {
        if (i != 0) {
            output.append(" ");
        }
        //If it contains "Mg", assume it's a MapGuide class name and link-ify it
        //TODO: Resolve :: to member links. Right now it just linkifies the Mg class name
        int idx = elems[i].find("Mg");
        if (idx != std::string::npos) {
            std::string prefix;
            std::string mgClassName;
            std::string suffix;
            //Collect characters before Mg
            if (idx > 0) {
                prefix = elems[i].substr(0, idx);
            }
            int cont = -1;
            //Collect the characters in the MapGuide class name
            for (int j = idx; j < elems[i].length(); j++) {
                if (!isalnum(elems[i][j])) {
                    cont = j;
                    break;
                } else {
                    mgClassName += elems[i][j];
                }
            }
            //Collect any characters afterwards
            for (int j = cont; j < elems[i].length(); j++) {
                suffix += elems[i][j];
            }
            output.append(prefix);
            output.append("{@link ");
            output.append(mgClassName);
            output.append("}");
            output.append(suffix);
        } else {
            output.append(elems[i]);
        }
    }
    return output;
}

string doxygenToJavaDoc(const string& commentStr, bool isPublished)
{
    // Doxygen documentation translation overview:
    //
    // What sorcery allows us to transplant doxygen documentation to our target language of choice?
    //
    // The answer lies in the swig directives %javamethodmodifiers and %typemap(javaclassmodifiers)
    // (and its csharp equvialents %csmethodmodifiers and %typemap(csclassmodifiers))
    //
    // The official SWIG documentation says that these directives are used to modify the visibility modifier for
    // <class declaration|class method name>. Although this directive is mainly used to modify
    // class/method visibility, the content itself does not have to be public/protected/abstract/static/const/etc
    // SWIG itself does not do any validation of the content in this directive.
    //
    // Given this directive
    //
    // %javamethodmodifiers MgMyClass::MyMethod() <content>
    //
    // SWIG generates this (Java):
    //
    // public class MgMyClass {
    //    ...
    //    <content> MyMethod() {
    //    ...
    //    }
    //    ...
    // }
    //
    // <content> should generally be public/protected/abstract/static/const/etc, but SWIG does not validate or enforce this
    //
    // Therefore we (ab)use these SWIG directives to prepend arbitrary content to the generated proxy class declaration/methods. In our
    // case, the content being our doxygen commentary (collected by IMake) translated to the target-specific documentation format
    //
    // In fact, this technique is documented as a way of transplanting documentation:
    //   http://www.swig.org/Doc1.3/Java.html#javadoc_comments

    // Supported doxygen tags:
    // \brief - Converted to main javadoc body
    // \param - @param
    // \return - @return
    // \deprecated - Adds the @Deprecated annotation to the class/method declaration (after the converted javadoc)
    // \since - @since
    // \exception - @exception

    //Re-tokenize the doxygen comments
    std::vector<std::string> elems;
    std::stringstream ss(commentStr);
    std::string item;
    while(std::getline(ss, item, '\n')) {
        elems.push_back(item);
    }
    int i = 0;
    
    bool isDeprecated = false;
    std::vector<std::string> descriptionParts;
    std::vector<std::string> paramParts;
    std::vector<std::string> returnParts;
    std::string sinceVer;
    std::vector<std::string> exceptionParts;

    while(i < elems.size()) {
        if (elems[i].find("<!-- Syntax in .Net, Java, and PHP -->") != std::string::npos ||
            elems[i].find("<!-- Example (PHP) -->") != std::string::npos ||
            elems[i].find("<!-- Example (Java) -->") != std::string::npos ||
            elems[i].find("<!-- Example (C#) -->") != std::string::npos) {
            i++;
            continue;
        }

        if (elems[i].find("\\brief") != std::string::npos) {
            i++;
            //Keep going until we find the next doxygen directive or end of comments
            while (i < elems.size() && elems[i].find("\\") == std::string::npos) {
                if (elems[i].find("<!-- Syntax in .Net, Java, and PHP -->") != std::string::npos ||
                    elems[i].find("<!-- Example (PHP) -->") != std::string::npos ||
                    elems[i].find("<!-- Example (Java) -->") != std::string::npos ||
                    elems[i].find("<!-- Example (C#) -->") != std::string::npos) {
                    i++;
                    continue;
                }
                std::string token = elems[i].substr(3);
                if (!token.empty())
                    descriptionParts.push_back(token);
                i++;
            }
            
            continue;
        }
        else if (elems[i].find("\\param") != std::string::npos) {
            std::string paramPart = elems[i].substr(elems[i].find("\\param") + 6);
            paramPart.append(" ");
            i++;
            //Keep going until we find the next doxygen directive or end of comments
            while (i < elems.size() && elems[i].find("\\") == std::string::npos) {
                std::string token = elems[i].substr(3);
                if (!token.empty()) {
                    paramPart.append(token);
                    paramPart.append(" ");
                }
                i++;
            }
            paramParts.push_back(paramPart);
            continue;
        }
        else if (elems[i].find("\\return") != std::string::npos) {
            i++;
            //Keep going until we find the next doxygen directive or end of comments
            while (i < elems.size() && elems[i].find("\\") == std::string::npos) {
                if (elems[i].find("<!-- Syntax in .Net, Java, and PHP -->") != std::string::npos ||
                    elems[i].find("<!-- Example (PHP) -->") != std::string::npos ||
                    elems[i].find("<!-- Example (Java) -->") != std::string::npos ||
                    elems[i].find("<!-- Example (C#) -->") != std::string::npos) {
                    i++;
                    continue;
                }
                std::string token = elems[i].substr(3);
                if (!token.empty())
                    returnParts.push_back(token);
                i++;
            }
            continue;
        }
        else if (elems[i].find("\\deprecated") != std::string::npos) {
            i++;
            isDeprecated = true;
            continue;
        }
        else if (elems[i].find("\\since") != std::string::npos) {
            sinceVer = elems[i].substr(elems[i].find("\\since") + 6);
            i++;
            continue;
        }
        else if (elems[i].find("\\exception") != std::string::npos) {
            std::string except = elems[i].substr(elems[i].find("\\exception") + 10);
            exceptionParts.push_back(except);
            i++;
            continue;
        }
        i++;
    }

    // ---------------------- JAVADOC START ------------------------ //
    std::string javaDoc = "\n/**\n";

    if (descriptionParts.size() > 0) {
        if (!isPublished) {
            javaDoc.append(" * ");
            javaDoc.append(EXTERNAL_API_DOCUMENTATION);
            javaDoc.append("\n");
        }
        for (int i = 0; i < descriptionParts.size(); i++) {
            javaDoc.append(" *");
            javaDoc.append(linkifyJavaDocFragment(descriptionParts[i]));    
            javaDoc.append("\n");
        }
        javaDoc.append(" *\n");
    } else {
        if (!isPublished) {
            javaDoc.append(" * ");
            javaDoc.append(EXTERNAL_API_DOCUMENTATION);
            javaDoc.append("\n");
        } else {
            javaDoc.append(" * TODO: API Documentation is missing or failed to translate doxygen brief directive (message inserted by IMake.exe)\n");
        }
    }

    if (paramParts.size() > 0) {
        for (int i = 0; i < paramParts.size(); i++) {
            javaDoc.append(" * @param ");
            javaDoc.append(linkifyJavaDocFragment(paramParts[i]));
            javaDoc.append("\n");
        }
    }

    if (returnParts.size() > 0) {
        javaDoc.append(" * @return ");
        for (int i = 0; i < returnParts.size(); i++) {
            javaDoc.append(linkifyJavaDocFragment(returnParts[i]));
            if (i < returnParts.size() - 1)
                javaDoc.append("\n * ");
        }
        javaDoc.append("\n");
    }

    if (exceptionParts.size() > 0) {
        for (int i = 0; i < exceptionParts.size(); i++) {
            javaDoc.append(" * @exception ");
            javaDoc.append(exceptionParts[i]);
            javaDoc.append("\n");
        }
    }

    if (!sinceVer.empty()) {
        javaDoc.append(" * @since ");
        javaDoc.append(sinceVer);
    }

    // ---------------------- JAVADOC END ------------------------ //
    javaDoc.append(" */\n");
    if (isDeprecated)
        javaDoc.append("@Deprecated\n");
    return javaDoc;
}

string doxygenToCsharpDoc(const string& commentStr, bool isPublished)
{
    //See doxygenToJavaDoc for how we do this sorcery

    std::string nspace = nameSpace;
    if (nspace.empty()) //Faulty logic if this is the case
        nspace = "OSGeo.MapGuide";

    //Re-tokenize the doxygen comments
    std::vector<std::string> elems;
    std::stringstream ss(commentStr);
    std::string item;
    while(std::getline(ss, item, '\n')) {
        elems.push_back(item);
    }
    int i = 0;
    
    bool isDeprecated = false;
    std::vector<std::string> descriptionParts;
    std::vector<std::string> paramParts;
    std::vector<std::string> returnParts;
    std::vector<std::string> exceptionParts;

    while(i < elems.size()) {
        if (elems[i].find("<!-- Syntax in .Net, Java, and PHP -->") != std::string::npos ||
            elems[i].find("<!-- Example (PHP) -->") != std::string::npos ||
            elems[i].find("<!-- Example (Java) -->") != std::string::npos ||
            elems[i].find("<!-- Example (C#) -->") != std::string::npos) {
            i++;
            continue;
        }

        if (elems[i].find("\\brief") != std::string::npos) {
            i++;
            //Keep going until we find the next doxygen directive or end of comments
            while (i < elems.size() && elems[i].find("\\") == std::string::npos) {
                if (elems[i].find("<!-- Syntax in .Net, Java, and PHP -->") != std::string::npos ||
                    elems[i].find("<!-- Example (PHP) -->") != std::string::npos ||
                    elems[i].find("<!-- Example (Java) -->") != std::string::npos ||
                    elems[i].find("<!-- Example (C#) -->") != std::string::npos) {
                    i++;
                    continue;
                }
                std::string token = elems[i].substr(3);
                if (!token.empty())
                    descriptionParts.push_back(token);
                i++;
            }
            
            continue;
        }
        else if (elems[i].find("\\param") != std::string::npos) {
            std::string paramPart = elems[i].substr(elems[i].find("\\param") + 6);
            paramPart.append(" ");
            i++;
            //Keep going until we find the next doxygen directive or end of comments
            while (i < elems.size() && elems[i].find("\\") == std::string::npos) {
                std::string token = elems[i].substr(3);
                if (!token.empty()) {
                    paramPart.append(token);
                    paramPart.append(" ");
                }
                i++;
            }
            paramParts.push_back(paramPart);
            continue;
        }
        else if (elems[i].find("\\return") != std::string::npos) {
            i++;
            //Keep going until we find the next doxygen directive or end of comments
            while (i < elems.size() && elems[i].find("\\") == std::string::npos) {
                if (elems[i].find("<!-- Syntax in .Net, Java, and PHP -->") != std::string::npos ||
                    elems[i].find("<!-- Example (PHP) -->") != std::string::npos ||
                    elems[i].find("<!-- Example (Java) -->") != std::string::npos ||
                    elems[i].find("<!-- Example (C#) -->") != std::string::npos) {
                    i++;
                    continue;
                }
                std::string token = elems[i].substr(3);
                if (!token.empty())
                    returnParts.push_back(token);
                i++;
            }
            continue;
        }
        else if (elems[i].find("\\deprecated") != std::string::npos) {
            i++;
            isDeprecated = true;
            continue;
        }
        else if (elems[i].find("\\exception") != std::string::npos) {
            std::string except = elems[i].substr(elems[i].find("\\exception") + 10);
            exceptionParts.push_back(except);
            i++;
            continue;
        }
        i++;
    }

    // ---------------------- csharpDoc START ------------------------ //
    std::string csharpDoc = "\n///<summary>\n";

    if (descriptionParts.size() > 0) {
        if (!isPublished) {
            csharpDoc.append("/// ");
            csharpDoc.append(EXTERNAL_API_DOCUMENTATION);
            csharpDoc.append("\n");
        }
        for (int i = 0; i < descriptionParts.size(); i++) {
            csharpDoc.append("///");
            csharpDoc.append(linkifyCSharpDocFragment(descriptionParts[i]));    
            csharpDoc.append("\n");
        }
        csharpDoc.append("///</summary>\n");
    } else {
        if (!isPublished) {
            csharpDoc.append("/// ");
            csharpDoc.append(EXTERNAL_API_DOCUMENTATION);
            csharpDoc.append("\n");
        } else {
            csharpDoc.append("///TODO: API Documentation is missing or failed to translate doxygen brief directive (message inserted by IMake.exe)\n///</summary>\n");
        }
    }

    if (paramParts.size() > 0) {
        for (int i = 0; i < paramParts.size(); i++) {
            std::string paramPart = paramParts[i];
            stripHtml(paramPart);
            xmlEscapeString(paramPart);

            std::vector<std::string> pelems;
            std::stringstream pss(linkifyCSharpDocFragment(paramPart));
            std::string pitem;
            while(std::getline(pss, pitem, ' ')) {
                if (!pitem.empty())
                    pelems.push_back(pitem);
            }
    
            if (pelems.size() > 1) { //Should be
                csharpDoc.append("///<param name=\"");
                csharpDoc.append(pelems[0]);
                csharpDoc.append("\">");
                csharpDoc.append("\n///");
                for (int i = 1; i < pelems.size(); i++) {
                    csharpDoc.append(" ");
                    csharpDoc.append(pelems[i]);
                }
                csharpDoc.append("\n///</param>\n");
            }
        }
    }

    if (returnParts.size() > 0) {
        csharpDoc.append("///<returns>");
        for (int i = 0; i < returnParts.size(); i++) {
            string retPart = returnParts[i];
            stripHtml(retPart);
            csharpDoc.append(linkifyCSharpDocFragment(retPart));
            if (i < returnParts.size() - 1)
                csharpDoc.append("\n/// ");
        }
        csharpDoc.append("\n///</returns>\n");
    }

    if (exceptionParts.size() > 0) {
        for (int i = 0; i < exceptionParts.size(); i++) {

            std::vector<std::string> eelems;
            std::stringstream ess(exceptionParts[i]);
            std::string eitem;
            while(std::getline(ess, eitem, ' ')) {
                if (!eitem.empty())
                    eelems.push_back(eitem);
            }

            if (eelems.size() > 0)
            {
                //Skip anything that is not of the form:
                //\exception MgExceptionType Description of cases when the exception is thrown
                //
                //So the first token must start with "Mg"
                std::string t("Mg");
                if (eelems[0].compare(0, t.length(), t) == 0)
                {
                    csharpDoc.append("///<exception cref=\"");
                    csharpDoc.append(nspace);
                    csharpDoc.append(".");
                    if (eelems.size() > 1) {
                        csharpDoc.append(eelems[0]);
                        csharpDoc.append("\">");
                        for (int j = 1; j < eelems.size(); j++) {
                            csharpDoc.append(" ");
                            csharpDoc.append(eelems[j]);
                        }
                        csharpDoc.append("</exception>\n");
                    }
                    else {
                        csharpDoc.append(eelems[0]);
                        csharpDoc.append("\"></exception>\n");
                    }
                }
            }
        }
    }

    // ---------------------- csharpDoc END ------------------------ //
    //csharpDoc.append("///\n");
    if (isDeprecated)
        csharpDoc.append("[Obsolete(\"This method is deprecated\")]\n");
    return csharpDoc;
}

void outputClassDoc(const string& className, const string& commentStr)
{
    //Nothing for PHP
    if (language == php)
        return;

    string convertedDoc;
    if (language == java) {
        convertedDoc = doxygenToJavaDoc(commentStr, true); //EXTERNAL_API only applies to class members, so treat this fragment as PUBLISHED_API
        fprintf(docOutFile, "\n%%typemap(javaclassmodifiers) %s %%{%s public%%}\n", className.c_str(), convertedDoc.c_str());
    } else if(language == csharp) {
        convertedDoc = doxygenToCsharpDoc(commentStr, true); //EXTERNAL_API only applies to class members, so treat this fragment as PUBLISHED_API
        fprintf(docOutFile, "\n%%typemap(csclassmodifiers) %s %%{%s public partial%%}\n", className.c_str(), convertedDoc.c_str());
    }
}

void outputMethodDoc(const string& className, const string& methodDecl, const string& commentStr, bool isPublished)
{
    //Nothing for PHP
    if (language == php)
        return;

    //Skip destructors
    if (methodDecl.find("~") != string::npos)
        return;

    string convertedDoc;
    string swigMethodDecl;
    swigMethodDecl = className;
    swigMethodDecl += "::";
    
    //Re-tokenize the method declaration
    //
    //We want the bits between '(' and ')' (parentheses included) and the first token before the '('
    std::vector<std::string> elems;
    std::stringstream ss(methodDecl);
    std::string item;
    while(std::getline(ss, item, ' ')) {
        elems.push_back(item);
    }
    std::string methodName;
    for (int i = 0; i < elems.size(); i++) {
        if (elems[i] == "" || elems[i] == "virtual")
            continue;

        if (elems[i] == "(") {
            if (i > 0) //Should be
                methodName += elems[i-1];
            methodName += elems[i];
            int j = i;
            //Process parameters between the ( and )
            while(j < elems.size()) {
                j++;
                if (elems[j] != ")") {
                    methodName += " ";
                    methodName += elems[j];
                } else {
                    methodName += " ";
                    methodName += elems[j];
                    break;
                }
            }
            break;
        }
    }

    if (methodName.empty())
        return;

    swigMethodDecl += methodName;

    if (language == java) {
        convertedDoc = doxygenToJavaDoc(commentStr, isPublished);
        fprintf(docOutFile, "\n%%javamethodmodifiers %s %%{%s public%%}\n", swigMethodDecl.c_str(), convertedDoc.c_str());
    } else if(language == csharp) {
        convertedDoc = doxygenToCsharpDoc(commentStr, isPublished);
        fprintf(docOutFile, "\n%%csmethodmodifiers %s %%{%s public%%}\n", swigMethodDecl.c_str(), convertedDoc.c_str());
    }
}

void processExternalApiSection(string& className, vector<string>& tokens, int begin, int end, bool isPublished)
{
    //until we find a problem with that, we output whatever we find in this section. In the
    //process we perform type substitution if required
    fprintf(outfile, "   ");
    int nesting = 0;
    bool destructor = false;
    bool firstToken = true;
    bool assignmentAdded = false;

    FILE* propertyFile = NULL;

    string commentStr;
    string methodDecl;

    //NOTE: This can get called multiple times for a given class with various offsets, so use a std::set to guard
    //against duplicate class documentation entries
    if (!translateMode && classesWithDocs.find(className) == classesWithDocs.end() && begin > 0) {
        
        int slashesCount = 0;
        for (int i = 0; i < begin; i++) {
            string token = tokens[i];
            if(token == "")
                continue;
            //pickup the doc comments for the class, if any.
            //all contiguous doc comment will be considered part of the class comment
            if(strncmp(token.c_str(), "///", 3) == 0)
            {
                if (isAllSlashes(token))
                {
                    slashesCount++;
                }

                if (slashesCount > 1) //Stop here, as this is generally the start of the documentation for the first method in the class
                    break;

                commentStr.append(token);
                commentStr.append("\n");
                continue;
            }
        }
        if (!commentStr.empty()) {
            outputClassDoc(className, commentStr);
            commentStr.clear();
            classesWithDocs.insert(className);
        } else if (language == csharp) {
            //We need to ensure the partial modifier is applied. However since we (ab)use csclassmodifiers to do this, if there's no class documentation
            //then it won't apply the required partial modifier. So we need to insert some kind of comment string here, so take this opportunity to 
            //note that this class has no documentation.
            std::string cmnt = "///\n";
            cmnt += "/// \\brief\n";
            cmnt += "/// TODO: This class has no class documentation (message inserted by IMake.exe)\n";
            cmnt += "///";
            outputClassDoc(className, cmnt);
            classesWithDocs.insert(className);
        }
    }
    for(int i = begin; i <= end; i++)
    {
        assignmentAdded = false;
        string token = tokens[i];

        if(token == "")
            continue;

        //pickup the doc comments for the class, if any.
        //all contiguous doc comment will be considered part of the class comment
        if(strncmp(token.c_str(), "///", 3) == 0)
        {
            commentStr.append(token);
            commentStr.append("\n");
            continue;
        }

        if(token[0] == '_' || isalpha(token[0]))
        {
            if(typeReplacements.find(token) != typeReplacements.end())
                token = typeReplacements[token];
        }
        if(translateMode)
        {
            if(!strncmp(token.c_str(), "///", 3))
            {
                fprintf(outfile, "%s\n   ", token.c_str());
                firstToken = true;
                continue;
            }
            if (firstToken)
            {
                if (translateMode && !commentStr.empty()) 
                {
                    string convertedDoc;
                    if (language == java) {
                        convertedDoc = doxygenToJavaDoc(commentStr, isPublished);
                        fprintf(outfile, "%s\n   ", convertedDoc.c_str());
                    } else if (language == csharp) {
                        convertedDoc = doxygenToCsharpDoc(commentStr, isPublished);
                        fprintf(outfile, "%s\n   ", convertedDoc.c_str());
                    }
                    commentStr.clear();
                }
            }

            if(firstToken && (language == java || language == csharp))
            {
                fprintf(outfile, "public ");
                if(language == csharp)
                {
                    //Evolution in this tokenizer have gone to far for the original,
                    //simplistic design. We should rewrite it at some point.
                    //Here, we lookahead to see if the variable name is hidding
                    //a known method of the class 'Object'. if it is, we want
                    //to hide the keyword 'new' to the declaration
                    for(int j= i + 1; j <= end; j++)
                    {
                        string tok = tokens[j];
                        if(tok[0] == ';' || tok[0] == '=')
                        {
                            if(isRootObjectMethod(tokens[j - 1]))
                            {
                                fprintf(outfile, "new ");
                            }
                            break;
                        }
                    }
                }
                firstToken = false;
            }
            if(token[0] == ';')
            {
                // if there is a doc comment as part of this line, check if it
                // contains the macro V(...) which indicates a string value to be assigned before the ;
                int posComment = token.find_first_of("///");
                if(posComment != string::npos)
                {
                    if(strstr(token.c_str(), "value("))
                    {
                        string comment = token.substr(posComment+3);
                        int posBeginValue = comment.find("value(");
                        int posEndValue = comment.find(")", posBeginValue + 1);
                        if(posEndValue != string::npos)
                        {
                            int strLen = posEndValue - posBeginValue - 6;
                            string expr = comment.substr(posBeginValue + 6, strLen);
                            comment = comment.substr(0, posBeginValue) + comment.substr(posEndValue + 1);

                            if(Trim(comment) == "")
                            {
                                //comment contained only the string value. remove comment at all
                                token = " = " + expr + ";";
                            }
                            else
                            {
                                // insert
                                token = " = " + expr + "; /// " + comment;
                            }
                            assignmentAdded = true;
                        }
                    }
                }
            }
        }
        else
        {
            // Doc comment may contain a directive for emitting .Net properties
            if(language == csharp && 0 != strstr(token.c_str(), "///"))
            {
                bool setProp = false;
                bool getProp = false;
                bool inherited = false;
                if (string::npos != token.find("__set")) { setProp = true; }
                if (string::npos != token.find("__get")) { getProp = true; }
                if (string::npos != token.find("__inherited")) { inherited = true; }

                size_t methodStart = string::npos;
                int j=0;
                if (setProp || getProp)
                {
                    for (j = 3; j < 6; j++)
                    {
                        methodStart = i-j>=0 ? tokens[i-j].find("Get") : string::npos;
                        if (string::npos != methodStart) break;
                    }
                }

                if (string::npos != methodStart && (setProp || getProp))
                {
                    if (NULL == propertyFile)
                    {
                        string fname = ".\\custom\\";
                        if (!customPath.empty())
                        {
                            fname = customPath;
                            if (fname[fname.size() - 1] != '\\')
                                fname.append("\\");
                        }
                        fname.append(className);
                        fname.append("Prop");
                        propertyFile = fopen(fname.c_str(),"w");
                        if (NULL == propertyFile)
                        {
                            printf("Unable to open autogen property file %s\n", fname.c_str());
                        }
                        else
                        {
                            printf("Writing autogen property file %s\n", fname.c_str());
                        }
                    }

                    if (NULL != propertyFile)
                    {
                        string propName = tokens[i-j].substr(3);
                        string propType = tokens[i-j-1];
                        if (propType == "*")
                        {
                            propType = tokens[i-j-2];
                            propType.append(tokens[i-j-1]);
                        }
                        else if (propType == "BYTE") {propType = "byte"; }
                        else if (propType == "INT8") {propType = "short"; }
                        else if (propType == "INT16") {propType = "short"; }
                        else if (propType == "INT32") {propType = "int"; }
                        else if (propType == "UINT32") {propType = "uint"; }
                        else if (propType == "INT64") {propType = "long"; }
                        else if (propType == "STRING") {propType = "string"; }

                        string::size_type pos = propType.find('*');
                        if (string::npos != pos) propType[pos] = ' ';

                        fprintf(propertyFile, "public %s%s %s\n{\n",
                                        inherited? "new ": "",
                                        propType.c_str(), propName.c_str());

                        if (setProp) { fprintf(propertyFile, "   set { Set%s(value); }\n", propName.c_str()); }
                        if (getProp) { fprintf(propertyFile, "   get { return Get%s(); }\n", propName.c_str()); }

                        fprintf(propertyFile, "}\n");\
                    }
                }
            }
        }

        if(token.length() > 0)
            fprintf(outfile, "%s ", token.c_str());

        if (token.find('~') != string::npos)
        {
            destructor = true;
        }

        methodDecl.append(" ");
        methodDecl.append(token);
        if(token[0] == ';' || assignmentAdded)
        {
            if (!translateMode) {
                outputMethodDoc(className, methodDecl, commentStr, isPublished);
            }
            commentStr.clear();
            methodDecl.clear();
            if(nesting == 0)
            {
                fprintf(outfile, "\n   ");
                firstToken = true;
            }
        }
        else if(tokens[i] == "{")
            ++ nesting;
        else if(tokens[i] == "}")
        {
            if(-- nesting == 0)
                fprintf(outfile, "\n   ");
        }
    }

    if (destructor == false && !translateMode)
    {
        fprintf(outfile, "virtual ~%s();\r\n  ", className.c_str());
    }

    if (NULL != propertyFile)
    {
        fclose(propertyFile);
    }
}

void processHeaderFile(string header)
{
    vector<string> tokens;

    tokenize(header, tokens);

    if(!translateMode)
    {
        //short banner about this file
        fprintf(outfile, "\n// Definitions from file %s\n//\n", header.c_str());
    }

    //ignore every token outside of a class definition
    for(int i = 0; i < (int)tokens.size(); i++)
    {
        if(tokens[i] != "class")
            continue;

        //look for the first '{' (class definition) or ';' (incomplete definition)
        bool incompleteDef = false;
        int colonPos = -1;
        int j = ++i;
        for(; j < (int)tokens.size(); j++)
        {
            if(tokens[j][0] == ';')
            {
                incompleteDef = true;
                break;
            }
            else if(tokens[j] == "{")
                break;
            else if(tokens[j] == ":")
                colonPos = j;

        }

        if(incompleteDef)
            continue;

        //get class name
        string className = colonPos == -1? tokens[j - 1]: tokens[colonPos - 1];

        //in translation mode, filters out clases which don't belong to the class list
        bool ignore = translateMode && classes.find(className) == classes.end();
        //printf("Processing header: %s\n", className.c_str());
        if(!ignore)
        {
            if(translateMode)
            {
                if(language == java)
                {
                    string javaFile = target;
                    char end = javaFile[javaFile.length() - 1];
                    if(end != FILESEP && end != '/')
                        javaFile.push_back(FILESEP);
                    javaFile += className + ".java";
                    outfile = fopen(javaFile.c_str(), "w");
                    if(outfile == NULL)
                        error(string("Cannot create java file ") + javaFile);

                    if(package != "")
                        fprintf(outfile, "package %s;\n\n", package.c_str());
                }

                //pickup the doc comments for the class, if any.
                //all contiguous doc comment will be considered part of the class comment
                string commentStr;
                int commentStart = i - 2;
                for(; commentStart >= 0; )
                {
                    const char* thisTok = tokens[commentStart].c_str();
                    if(strncmp(tokens[commentStart].c_str(), "///", 3))
                        break;
                    commentStart--;
                }
                if(++commentStart < i - 2)
                {
                    for(commentStart = commentStart < 0? commentStart+1: commentStart; commentStart <= i - 2; commentStart++)
                    {
                        const char* thisTok = tokens[commentStart].c_str();
                        commentStr.append(thisTok);
                        commentStr.append("\n");
                    }
                }

                if (!commentStr.empty()) 
                {
                    string convertedDoc;
                    if (language == java) {
                        convertedDoc = doxygenToJavaDoc(commentStr, true);
                        fprintf(outfile, "%s", convertedDoc.c_str());
                    } else if (language == csharp) {
                        convertedDoc = doxygenToCsharpDoc(commentStr, true);
                        fprintf(outfile, "%s", convertedDoc.c_str());
                    }
                }
            }

            //output the class header
            if(translateMode && (language == java || language == csharp))
                fprintf(outfile, "public ");

            fprintf(outfile, "class %s", className.c_str());
            if(colonPos != -1)
            {
                fprintf(outfile, " : ");
                for(int k = colonPos + 1; k < j; k++)
                    fprintf(outfile, "%s ", tokens[k].c_str());
            }
            if(!translateMode)
                fprintf(outfile, "\n{\npublic:\n");
            else
                fprintf(outfile, "\n{\n");
        }

        //collect pointers to sections.
        vector<int> sections;

        ++j;
        for (int nesting = 0; j < (int)tokens.size(); j ++)
        {
            if(tokens[j] == ":")
            {
                if(nesting == 0 && !ignore)
                {
                    string sectionName = tokens[j - 1];
                    if(sectionName == "EXTERNAL_API" || sectionName == "INTERNAL_API" || sectionName == "CLASS_ID" ||
                        sectionName == "PUBLISHED_API" || sectionName == "public" || sectionName == "protected" || sectionName == "private")
                        sections.push_back(j);
                }
            }
            else if(tokens[j] == "{")
                ++ nesting;
            else if(tokens[j] == "}")
            {
                if(nesting > 0)
                    nesting --;
                else
                    break;
            }

        }
        sections.push_back(j);

        // process EXTERNAL_API and CLASS_ID sections
        for(int k = 0; k < (int)sections.size() - 1; k++)
        {
            string sectionName = tokens[sections[k] - 1];
            if(sectionName == "EXTERNAL_API" || sectionName == "PUBLISHED_API")
                processExternalApiSection(className, tokens, sections[k] + 1, sections[k + 1] - (k < (int)sections.size() - 2? 2: 1), (sectionName == "PUBLISHED_API"));
            else if(sectionName == "CLASS_ID" && !translateMode)
                processClassIdSection(tokens, sections[k] + 1, sections[k + 1] - (k < (int)sections.size() - 2? 2: 1));
        }

        //end of class
        if(!ignore)
        {
            fprintf(outfile, "\n}");
            if(!translateMode) {
                fprintf(outfile, ";\n\n");
            }
            else {
                fprintf(outfile, "\n\n");
            }
        }

    }
}

void createSWGInterfaceFile()
{
    printf("\n\nGenerating interface file %s...\n", target.c_str());

    //validate our mandatory sections
    if(module.length() == 0)
        error("Module section is missing");
    if(target.length() == 0)
        error("Target section is missing");

    if(headers.size() == 0)
    {
        warning("No header files to process, no class generated.");
        return;
    }

    if(!translateMode || language != java)
    {
        outfile = fopen(target.c_str(), "w");
        if(outfile == NULL)
            error(string("Cannot create target file ") + target);
        docOutFile = fopen(docTarget.c_str(), "w");
        if(docOutFile == NULL)
            error(string("Cannot create doctarget file ") + docTarget);
    }

    time_t now = time(NULL);

    //write the banner
    fprintf(outfile, "//======================================================\n");
    fprintf(outfile, "// Generated with IMake version %s\n", version);
    fprintf(outfile, "// %s\n", asctime(localtime(&now)));
    fprintf(outfile, "//\n");

    //write the module
    fprintf(outfile, "%%module %s\n", module.c_str());

    if(!translateMode)
    {
        //write the banner
        fprintf(docOutFile, "//======================================================\n");
        fprintf(docOutFile, "// Generated with IMake version %s\n", version);
        fprintf(docOutFile, "// %s\n", asctime(localtime(&now)));
        fprintf(docOutFile, "//\n");
    }

    if(!translateMode || language != java)
    {
        //write the C++ inline code
        fprintf(outfile, "%%{\n%s\n%%}\n\n", cppInline.c_str());

        //write the typedefs
        fprintf(outfile, "%s\n", typedefs.c_str());

        //write the SWIG inline code
        fprintf(outfile, "%s\n", swigInline.c_str());
    }

    //process the headers
    for(vector<string>::const_iterator it = headers.begin(); it != headers.end(); it++)
        processHeaderFile(*it);

    if(!translateMode || language != java)
    {
        fclose(outfile);
        if (docOutFile != NULL) {
            fclose(docOutFile);
        }
    }
}

void createNativeFile()
{
    if(target.length() == 0)
        error("Target section is missing");

    if (!translateMode && docTarget.length() == 0)
        error("DocTarget section is missing");

    if(language == unknown)
        error("Unknown language");

    if(language != java)
    {
        outfile = fopen(target.c_str(), "w");
        if(outfile == NULL)
            error(string("Cannot create file ") + target);
        if (!translateMode) {
            docOutFile = fopen(docTarget.c_str(), "w");
            if (docOutFile == NULL)
                error(string("Cannot create file ") + docTarget);
        }
    }

    if(language == php)
        fprintf(outfile, "<?php\n\n");
    else
    {
        if(nameSpace != "")
        {
            if(language == csharp)
                fprintf(outfile, "namespace %s {\n\n", nameSpace.c_str());
        }
    }

    if(language != java)
    {
        //write the inline code
        fprintf(outfile, "%s\n\n", swigInline.c_str());
    }

    //process the headers
    for(vector<string>::const_iterator it = headers.begin(); it != headers.end(); it++)
        processHeaderFile(*it);

    if(language == php)
        fprintf(outfile, "?>");
    else
    {
        if(nameSpace != "")
        {
            if(language == csharp)
                fprintf(outfile, "}\n");
        }
    }

    if(language != java)
        fclose(outfile);
    if (docOutFile != NULL)
        fclose(docOutFile);
}

void createInterfaceFile(char* paramFile)
{
    FILE* file = fopen(paramFile, "r");
    if(file == NULL)
    {
        error(string("Cannot open parameter file ") + paramFile);
    }

    fseek(file, 0, SEEK_END);
    int length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* data = new char[length + 1];
    memset(data, 255, length + 1);
    fread(data, length, 1, file);
    char* end = strchr(data, 255);
    *end = '\0';

    parseParameterFile(data);

    if(!translateMode)
        createSWGInterfaceFile();
    else
        createNativeFile();

}

void usage()
{
    printf("\nUsage:");
    printf("\nIMake parameterFile lang [generation_path_or_folder]");
    printf("\n      parameterFile: XML description of generation parameters\n");
    printf("\n      lang: Target language (PHP, C# or Java).\n");
    printf("\n      generation_path_or_folder: Do not generate SWIG.  Generate constant definitions.");
    printf("\n                                 For PHP and C#, pathname of the constant file.");
    printf("\n                                 For Java, folder where the constant files are created");
    exit(1);
}

int main(int argc, char* argv[])
{
    printf("\nIMake - SWIG Interface generator");
    printf("\nVersion %s", version);

    if(argc < 3)
        usage();

    if (argc > 4)
        usage();

    translateMode = false;
    language = unknown;

    if(!strcmp(argv[2], "PHP"))
        language = php;
    else if(!strcmp(argv[2], "C#"))
    {
        language = csharp;
        rootObjectMethods["Equals"] = 1;
        rootObjectMethods["GetHashCode"] = 1;
        rootObjectMethods["GetType"] = 1;
        rootObjectMethods["ReferenceEquals"] = 1;
        rootObjectMethods["ToString"] = 1;
    }
    else if(!strcmp(argv[2], "Java"))
        language = java;
    else
        usage();

    if(argc == 4)
    {
        translateMode = true;
        target = argv[3];
    }

    createInterfaceFile(argv[1]);

    return 0;
}
