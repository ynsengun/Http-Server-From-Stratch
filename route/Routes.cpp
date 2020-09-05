#include "Routes.h"
#include "RouteUtility.h"

string Routes::indexHTML;

void Routes::initializeStaticPages(){
    indexHTML = RouteUtility::readFile("public/index.html");
}

string Routes::httpGet(string path, map<string, string> params, Session *session){

    if(path == "/"){
        string sessionID = to_string(session->getSessionID());

        // header
        string body = "<h1>Welcome to the home page</h1>";

        // form (buttons)
        body += "<form method=\"GET\" action=\"http://localhost:8088/todo\">";
        body += "<input value=\"" + sessionID + "\" name=\"sessionID\" style=\"display:none\" />";
        body += "<button>Open my TODOs</button>";
        body += "</form>";

        return RouteUtility::success(RouteUtility::HTMLBody(body));
    }

    if(path == "/todo"){
        string sessionID = to_string(session->getSessionID());
        vector<string> items = session->getData();

        // header
        string body = "<h1>TODO</h1>";

        // todo list
        body += "<ul>";
        for( string item : items ){
            body += "<li>" + item + "<li>";
        }
        body += "</ul>";

        // form
        body += "<form method=\"POST\" action=\"http://localhost:8088/todo\">";
        body += "<input value=\"" + sessionID + "\" name=\"sessionID\" style=\"display:none\" />";
        body += "<input name=\"item\" />";
        body += "<button>Add</button>";
        body += "</form>";

        return RouteUtility::success(RouteUtility::HTMLBody(body));
    }
}

string Routes::httpPost(string path, map<string, string> params, Session *session){
    if(path == "/todo"){
        // save the new item to the session
        string item = params["item"];
        session->pushData(item);

        // get items from session
        string sessionID = to_string(session->getSessionID());
        vector<string> items = session->getData();

        // header
        string body = "<h1>TODO</h1>";

        // todo list
        body += "<ul>";
        for( string item : items ){
            body += "<li>" + item + "</li>";
        }
        body += "</ul>";

        // form
        body += "<form method=\"POST\" action=\"http://localhost:8088/todo\">";
        body += "<input value=\"" + sessionID + "\" name=\"sessionID\" style=\"display:none\" />";
        body += "<input name=\"item\" />";
        body += "<button>Add</button>";
        body += "</form>";

        // message
        body += "<h5>Item is added succcessfully</h5>";

        return RouteUtility::success(RouteUtility::HTMLBody(body));
    }
}

string Routes::httpPut(string path, map<string, string> params, Session *session){

}

string Routes::httpDelete(string path, map<string, string> params, Session *session){

}
