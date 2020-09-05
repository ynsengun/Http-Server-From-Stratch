#include "RouteUtility.h"

string RouteUtility::firstPartIndex;
string RouteUtility::secondPartIndex;

string RouteUtility::readFile(const string &filePath){
    ifstream ifs(filePath);
    return string( (istreambuf_iterator<char>(ifs) ), (istreambuf_iterator<char>()) );
}

void RouteUtility::generateSessionedIndexHTML(const string &indexHTML){
    int len = indexHTML.length();
    for( int i = 0 ; i < len ; i++ ){
        if(indexHTML.substr(i, 5) == "value"){
            firstPartIndex = indexHTML.substr(0, i + 5);
            secondPartIndex = indexHTML.substr(i + 5, len - i - 5);
            break;
        }
    }
}

string RouteUtility::getSessionedIndexHTML(const string &sessionID){
    return firstPartIndex + "=\"" + sessionID + "\"" + secondPartIndex;
}

string RouteUtility::success(const string &content){
    const int contentLen = content.size();

    return "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: "
                + to_string(contentLen)
                + "\n\n"
                + content;
}