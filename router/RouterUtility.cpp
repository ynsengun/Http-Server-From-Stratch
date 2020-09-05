#include "RouterUtility.h"

string RouteUtility::htmlOpeningTemplate = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Server</title></head><body>";
string RouteUtility::htmlClosingTemplate = "</body></html>";

string RouteUtility::readFile(const string &filePath){
    ifstream ifs(filePath);
    return string( (istreambuf_iterator<char>(ifs) ), (istreambuf_iterator<char>()) );
}

string RouteUtility::success(const string &content){
    const int contentLen = content.size();

    return "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: "
                + to_string(contentLen)
                + "\n\n"
                + content;
}

string RouteUtility::HTMLBody(const string &content){
    string ret = RouteUtility::htmlOpeningTemplate;
    ret += content;
    ret += RouteUtility::htmlClosingTemplate;

    return ret;
}

string RouteUtility::includeSessionOnForm(const string &sessionID){
    return "<input value=\"" + sessionID + "\" name=\"sessionID\" style=\"display:none\" />";
}