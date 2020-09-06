#include "RouterUtility.h"

string RouterUtility::htmlOpeningTemplate = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Server</title></head><body>";
string RouterUtility::htmlClosingTemplate = "</body></html>";

/**
 * reads the file placed on the given file path and return it as a string
 */
string RouterUtility::readFile(const string &filePath){
    ifstream ifs(filePath);
    return string( (istreambuf_iterator<char>(ifs) ), (istreambuf_iterator<char>()) );
}

/**
 * converts the given content to a successful http response
 */
string RouterUtility::success(const string &content){
    const int contentLen = content.size();

    return "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: "
                + to_string(contentLen)
                + "\n\n"
                + content;
}

/**
 * creates a full html structure with the given html body
 */
string RouterUtility::HTMLBody(const string &content){
    string ret = RouterUtility::htmlOpeningTemplate;
    ret += content;
    ret += RouterUtility::htmlClosingTemplate;

    return ret;
}

/**
 * return a hidden session info to use in forms
 * makes session operation easy
 */
string RouterUtility::includeSessionOnForm(const string &sessionID){
    return "<input value=\"" + sessionID + "\" name=\"sessionID\" style=\"display:none\" />";
}