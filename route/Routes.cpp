#include "Routes.h"

string Routes::indexHTML;

void Routes::initializeStaticPages(){
    indexHTML = RouteUtility::readFile("public/index.html");
    RouteUtility::generateSessionedIndexHTML(indexHTML);
}

string Routes::httpGet(string path, map<string, string> params){
    if(path == "/"){
        string sessionID = params["sessionID"];
        return RouteUtility::success(RouteUtility::getSessionedIndexHTML(sessionID));
    }
}

string Routes::httpPost(string path, map<string, string> params){

}

string Routes::httpPut(string path, map<string, string> params){

}

string Routes::httpDelete(string path, map<string, string> params){

}
