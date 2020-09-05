#ifndef __ROUTES
#define __ROUTES

#include <iostream>
#include <string>
#include <map>

#include "../server/Session.h"

using namespace std;

// this is an empty map to use as default value
const map<string, string> emptyMap;

namespace Routes{
    // Static pages
    extern string aboutHTML;

    // Initializing static pages at starting phase will make the processes faster.
    void initializeStaticPages();

    string httpGet(string, map<string, string> params = emptyMap, Session *session = NULL);
    string httpPost(string, map<string, string> params = emptyMap, Session *session = NULL);
    string httpPut(string, map<string, string> params = emptyMap, Session *session = NULL);
    string httpDelete(string, map<string, string> params = emptyMap, Session *session = NULL);
}

#endif // __ROUTES