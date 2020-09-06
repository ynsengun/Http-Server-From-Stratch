#ifndef __ROUTES_UTILITY
#define __ROUTES_UTILITY

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <map>

using namespace std;

/**
 * utility package to create http responses easier
 * can be used in routes
 */
namespace RoutesUtility{
    extern string htmlOpeningTemplate;
    extern string htmlClosingTemplate;

    string readFile(const string &);
    string success(const string &);
    string HTMLBody(const string &);
    string includeSessionOnForm(const string &);
}

#endif // __ROUTE_UTILITY