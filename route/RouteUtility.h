#ifndef __ROUTE_UTILITY
#define __ROUTE_UTILITY

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <map>

using namespace std;

namespace RouteUtility{
    extern string htmlOpeningTemplate;
    extern string htmlClosingTemplate;

    string readFile(const string &);
    string success(const string &);
    string HTMLBody(const string &);
}

#endif // __ROUTE_UTILITY