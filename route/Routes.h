#ifndef __ROUTES
#define __ROUTES

#include <iostream>
#include <string>
#include <map>

#include "RouteUtility.h"

using namespace std;

// this is an empty map to use as default value
const map<string, string> emptyMap;

namespace Routes{
    // Static pages
    extern string indexHTML;

    // Initializing static pages at starting phase will make the processes faster.
    void initializeStaticPages();

    string httpGet(string, map<string, string> params = emptyMap);
    string httpPost(string, map<string, string> params = emptyMap);
    string httpPut(string, map<string, string> params = emptyMap);
    string httpDelete(string, map<string, string> params = emptyMap);
}

#endif // __ROUTES