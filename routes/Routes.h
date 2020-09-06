#ifndef __ROUTES
#define __ROUTES

#include <iostream>
#include <string>
#include <map>

#include "../server/Session.h"

using namespace std;

/**
 * all http request are forwarded service method of this, then service forwards again to the related http method
 */
namespace Routes{
    // Static pages
    extern string aboutHTML;

    void init();

    string service(string &, string &, map<string, string> &, Session *);
    string httpGet(string &, map<string, string> &, Session *);
    string httpPost(string &, map<string, string> &, Session *);
    string httpPut(string &, map<string, string> &, Session *);
    string httpDelete(string &, map<string, string> &, Session *);
}

#endif // __ROUTES