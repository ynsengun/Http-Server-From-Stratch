#ifndef __SERVER_UTILITY
#define __SERVER_UTILITY

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <map>

using namespace std;

/**
 * utility package to use in server
 */
namespace ServerUtil{
    void parseRequest( char *, string &, string &, int &, map<string, string> &);
}

#endif // __SERVER_UTILITY