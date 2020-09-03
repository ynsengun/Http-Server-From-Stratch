#ifndef __SESSION
#define __SESSION

#include <string>
#include <iostream>

using namespace std;

class Session{

private:
    int sessionID;

public:
    Session(int);
    void printSession();

};

#endif // __SESSION