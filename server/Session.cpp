#include "Session.h"

int Session::lastSessionID = 0;

Session::Session(int _sessionID){
    time(&lastActive);
    sessionID = _sessionID;
}

int Session::getNextSessionID(){
    return lastSessionID++;
}

int Session::getSessionID(){
    return sessionID;
}

void Session::pushData(string new_data){
    time(&lastActive);
    data.push_back(new_data);
}

vector<string> Session::getData(){
    time(&lastActive);
    return data;
}

void Session::setData(vector<string> _data){
    time(&lastActive);
    data = _data;
}

Request Session::getNextRequest(){
    time(&lastActive);
    Request ret = requests.front();
    requests.pop();
    return ret;
}

void Session::pushNewRequest(Request req){
    time(&lastActive);
    requests.push(req);
}

bool Session::hasNextRequest(){
    return !requests.empty();
}

time_t Session::getLastActive(){
    return lastActive;
}