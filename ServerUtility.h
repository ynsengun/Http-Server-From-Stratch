#ifndef __SERVER_UTILITY
#define __SERVER_UTILITY

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <fstream>

using namespace std;

namespace ServerUtil{
    void createIndexHTML( char *&);
    void createPostResponse( char *&, int);
    void getMethodAndParam( char *, string &, string &);
}

#endif // __SERVER_UTILITY