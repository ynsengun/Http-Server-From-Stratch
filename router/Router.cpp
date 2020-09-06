#include "Router.h"
#include "RouterUtility.h"

string Router::aboutHTML;

void Router::init(){
    aboutHTML = RouterUtility::readFile("public/about.html");
}

string Router::service(string method, string path, map<string, string> params, Session *session){

    /**
     * Add your filter operations here
     */

    if(method == "GET"){
        return Router::httpGet(path, params, session);
    } else if(method == "POST"){
        return Router::httpPost(path, params, session);
    } else if(method == "PUT"){
        return Router::httpPut(path, params, session);
    } else if(method == "DELETE"){
        return Router::httpDelete(path, params, session);
    }
}

string Router::httpGet(string path, map<string, string> params, Session *session){
    cout<<path<<path.length()<<endl;

    if(path == "/"){
        string sessionID = to_string(session->getSessionID());

        // sessionID
        string body = "<h1>Session ID: " + sessionID + "</h1></br>";

        // header
        body += "<h2>Welcome to the home page</h2>";

        // form (about)
        body += "<form method=\"GET\" action=\"http://localhost:8088/about\">";
        body += RouterUtility::includeSessionOnForm(sessionID); // TODO fix here, after initializer thread accept in proper way
        body += "<button>About The Server</button>";
        body += "</form></br>";

        // form (todo)
        body += "<form method=\"GET\" action=\"http://localhost:8088/todo\">";
        body += RouterUtility::includeSessionOnForm(sessionID);
        body += "<button>Open my TODOs</button>";
        body += "</form>";

        return RouterUtility::success(RouterUtility::HTMLBody(body));
    }

    if(path == "/about"){
        return RouterUtility::success(aboutHTML);
    }

    if(path == "/todo"){
        string sessionID = to_string(session->getSessionID());
        vector<string> items = session->getData();

        // home button
        string body = "<form method=\"GET\" action=\"http://localhost:8088\">";
        body += RouterUtility::includeSessionOnForm(sessionID);
        body += "<button>Home</button>";
        body += "</form>";

        // sessionID
        body += "<h1>Session ID: " + sessionID + "</h1></br>";

        // header
        body += "<h2>TODO</h2>";

        // todo list
        body += "<ul>";
        for( string item : items ){
            body += "<li>" + item + "</li>";
        }
        body += "</ul>";

        // form
        body += "<form method=\"POST\" action=\"http://localhost:8088/todo\">";
        body += RouterUtility::includeSessionOnForm(sessionID);
        body += "<input name=\"item\" />";
        body += "<button>Add</button>";
        body += "</form>";

        return RouterUtility::success(RouterUtility::HTMLBody(body));
    }
}

string Router::httpPost(string path, map<string, string> params, Session *session){
    if(path == "/todo"){
        // save the new item to the session
        string item = params["item"];
        session->pushData(item);

        // get items from session
        string sessionID = to_string(session->getSessionID());
        vector<string> items = session->getData();

        // home button
        string body = "<form method=\"GET\" action=\"http://localhost:8088\">";
        body += RouterUtility::includeSessionOnForm(sessionID);
        body += "<button>Home</button>";
        body += "</form>";

        // sessionID
        body += "<h1>Session ID: " + sessionID + "</h1></br>";

        // header
        body += "<h2>TODO</h3>";

        // todo list
        body += "<ul>";
        for( string item : items ){
            body += "<li>" + item + "</li>";
        }
        body += "</ul>";

        // form
        body += "<form method=\"POST\" action=\"http://localhost:8088/todo\">";
        body += RouterUtility::includeSessionOnForm(sessionID);
        body += "<input name=\"item\" />";
        body += "<button>Add</button>";
        body += "</form>";

        // message
        body += "<h5>Item is added succcessfully</h5>";

        return RouterUtility::success(RouterUtility::HTMLBody(body));
    }
}

string Router::httpPut(string path, map<string, string> params, Session *session){
    /**
     * Add your routes here
     */
}

string Router::httpDelete(string path, map<string, string> params, Session *session){
    /**
     * Add your routes here
     */
}
