#ifndef __SESSION
#define __SESSION

#include <string>
#include <iostream>
#include <map>
#include <queue>
#include <any>
#include <vector>

using namespace std;

struct Request{
    string method;
    string path;
    int responseSocketID;
    map<string, string> params;
};

class Session{

private:
    static int lastSessionID;

    int sessionID;
    queue<Request> requests;
    vector<string> data;

public:
    static int getNextSessionID();
    Session(int);
    int getSessionID();

    void pushData(string);
    vector<string> getData();
    void setData(vector<string>);

    Request getNextRequest();
    void pushNewRequest(Request);
    bool hasNextRequest();

};

#endif // __SESSION