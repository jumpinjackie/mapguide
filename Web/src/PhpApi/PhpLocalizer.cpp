static string localizationPath;
static string english = "en";
typedef map<string, string> STRBUNDLE;
typedef STRBUNDLE* PSTRBUNDLE;
static map<string, PSTRBUNDLE> languages;
typedef char* CHAR_PTR_NOCOPY;

extern "C" {
#include "zend_API.h"
}

#include <algorithm>
#define MAX_LOC_LEN     4096

static string trim(string source)
{
    string cs = "\t \r\n";
    string s = source.erase(0, source.find_first_not_of(cs));
    return s.erase(s.find_last_not_of(cs) + 1) ;
}

static void strlower(string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

static void SetLocalizedFilesPath(const char*path)
{
    localizationPath = path;
}

PSTRBUNDLE GetStringBundle(string& locale_)
{
    string locale = locale_;
    if (locale == "")
        locale = english;
    else
        strlower(locale);
    
    string localKey = localizationPath + locale;

    map<string, PSTRBUNDLE>::const_iterator it = languages.find(localKey);
    
    if (it == languages.end()) {
        FILE* f = NULL;
        string fname = localKey;
        f = fopen(fname.c_str(), "r");
        if(f == NULL) {  // assume file doesn't exists
            // requested locale is not supported, default to English
            it = languages.find(localizationPath + english);
            if(it != languages.end())
                return it->second;
            fname = localizationPath + english;
            f = fopen(fname.c_str(), "r");
        }
        PSTRBUNDLE sb = new STRBUNDLE;
        languages[localKey] = sb;
        if(f != NULL) {
            char l[MAX_LOC_LEN + 1];
            for(int lc = 0; fgets(l, MAX_LOC_LEN, f) != NULL; lc++) {
                string line;
                if(lc == 0 && (unsigned char)l[0] == 0xEF)
                    line = trim(string(l + 3)); //Skip UTF8 BOF marker
                else
                    line = trim(string(l));
                if(line.empty() || line.at(0) == '#')
                    continue;
                size_t sep = line.find('=');
                if (sep == string::npos)
                    continue;
                string key = trim(line.substr(0, sep));
                if (key.empty())
                    continue;
                string value = trim(line.substr(sep + 1));
                (*sb)[key] = value;
            }
            fclose(f);
        }
    }
    return languages[localKey];
}

static char* Localize(const char* text_, const char* locale_, int os)
{
    string locale = locale_;
    strlower(locale);
    string text = text_;
    string fontSuffix = (os == 0 ? "Windows" : (os == 1 ? "Macintosh" : "Linux"));


    PSTRBUNDLE sb = GetStringBundle(locale);
    if(sb == NULL)
        return estrdup("");
    size_t len = text.length();
    for(size_t i = 0; i < len; )
    {
        bool fontTag = false;
        size_t pos1 = text.find("__#", i);
        if (pos1 != string::npos)
        {
            size_t pos2 = text.find("#__", pos1 + 3);
            if (pos2 != string::npos)
            {
                string id = text.substr(pos1 + 3, pos2 - pos1 - 3);
                string locStr;
                map<string, string>::const_iterator it = sb->find(id == "@font" || id == "@fontsize" ? id + fontSuffix : id);
                if (it == sb->end())
                    locStr = "";
                else
                    locStr = it->second;
                size_t locLen = locStr.length();

                string begin, end;
                if (pos1 > 0)
                    begin = text.substr(0, pos1);
                else
                    begin = "";
                end = text.substr(pos2 + 3);
                text = begin + locStr + end;

                len = len - 6 - id.length() + locLen;
                i = pos1 + locLen;
            }
            else
                i = len;
        }
        else
            i = len;
    }
    return estrdup(text.c_str());
}

static char* GetLocalizedString(const char* id_, const char* locale_)
{
    PSTRBUNDLE sb = NULL;
    string locale = locale_;
    sb = GetStringBundle(locale);
    if(sb == NULL)
        return estrdup("");
    string id = id_;
    map<string, string>::const_iterator it = sb->find(id);
    if (it == sb->end())
        return estrdup("");
    return estrdup(it->second.c_str());
}
