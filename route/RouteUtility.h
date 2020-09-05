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
    extern string firstPartIndex;
    extern string secondPartIndex;

    string readFile(const string &);
    string getSessionedIndexHTML(const string &);
    void generateSessionedIndexHTML(const string &);
    string success(const string &);
}

#endif // __ROUTE_UTILITY